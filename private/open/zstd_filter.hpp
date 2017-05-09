#ifndef ZSTD_FILTER_HPP
#define ZSTD_FILTER_HPP

#include <boost/iostreams/filtering_stream.hpp>
#include <zstd.h>
#include <algorithm>

class zstd_compressor : public boost::iostreams::multichar_output_filter{
public:
	typedef char char_type;
	
	zstd_compressor(int compressionLevel):
	cstream(NULL),
	compressionLevel(compressionLevel),
	streamInitialized(false),
	ibuf(NULL),obuf(NULL),
	ibufSize(0),ibufUsed(0)
	{}
	
	//boost::iostreams really likes to copy when it should move. This filter
	//cannot generally be copied, but doing so is possible between construction
	//and first real use, which in practice is the only time it's needed.
	zstd_compressor(const zstd_compressor& other):
	cstream(NULL),
	compressionLevel(other.compressionLevel),
	streamInitialized(other.streamInitialized),
	ibuf(NULL),obuf(NULL),
	ibufSize(other.ibufSize),ibufUsed(other.ibufUsed)
	{
		assert(!other.streamInitialized);
		assert(!other.cstream);
		assert(!other.ibuf);
		assert(!other.obuf);
	}
	
	~zstd_compressor(){
		stream_delete(cstream);
		delete[] ibuf;
		delete[] obuf;
		
	}
	
	private:
	zstd_compressor& operator=(const zstd_compressor&);
	public:
	
	//compress n bytes of input from src to dest
	template<typename Sink>
	std::streamsize write(Sink& dest, const char* src, std::streamsize n){
		if(!streamInitialized)
			initStream();
		
		std::streamsize result=n;
		//Copy as much input into the input buffer as possible,
		//run the compression each time the input buffer is filled,
		//and then insert the remainder.
		//In practice, n tends to be _much_ smaller than ibufSize.
		while(n>0){
			std::streamsize to_copy=std::min(n,std::streamsize(ibufSize-ibufUsed));
			memcpy(ibuf+ibufUsed,src,to_copy);
			n-=to_copy;
			ibufUsed+=to_copy;
			if(ibufUsed<ibufSize)
				break;
			ZSTD_inBuffer input;
			input.src=ibuf;
			input.size=ibufSize;
			input.pos=0;
			while(input.pos<input.size){
				ZSTD_outBuffer output;
				output.dst=obuf;
				output.size=ZSTD_CStreamOutSize();
				output.pos=0;
				size_t result=ZSTD_compressStream(cstream,&output,&input);
				if(ZSTD_isError(result))
					log_fatal_stream("ZSTD_compressStream() error: " << ZSTD_getErrorName(result));
				boost::iostreams::write(dest,obuf,output.pos);
			}
			ibufUsed=0;
		}
		return(result);
	}
	
	//flush buffered input
	template<typename Sink>
	void close(Sink& dest){
		//If no data was compressed initialize the stream anyway, so that we can
		//finalize it and end up with a valid file.
		if(!streamInitialized)
			initStream();
		if(ibufUsed){
			ZSTD_inBuffer input;
			input.src=ibuf;
			input.size=ibufUsed;
			input.pos=0;
			while(input.pos<input.size){
				ZSTD_outBuffer output;
				output.dst=obuf;
				output.size=ZSTD_CStreamOutSize();
				output.pos=0;
				size_t err=ZSTD_compressStream(cstream,&output,&input);
				if(ZSTD_isError(err))
					log_fatal_stream("ZSTD_compressStream error: " << ZSTD_getErrorName(err));
				boost::iostreams::write(dest,obuf,output.pos);
			}
			ibufUsed=0;
		}
		size_t bytes_remaining;
		do{
			ZSTD_outBuffer output;
			output.dst=obuf;
			output.size=ZSTD_CStreamOutSize();
			output.pos=0;
			bytes_remaining=ZSTD_endStream(cstream,&output);
			boost::iostreams::write(dest,obuf,output.pos);
		}while(bytes_remaining>0);
	}
private:
	ZSTD_CStream* cstream;
	int compressionLevel;
	bool streamInitialized;
	char_type* ibuf;
	char_type* obuf;
	std::size_t ibufSize, ibufUsed;
	
	static void stream_delete(ZSTD_CStream* stream){
		if(stream)
			ZSTD_freeCStream(stream);
	}
	
	void initStream(){
		cstream=ZSTD_createCStream();
		ZSTD_initCStream(cstream,compressionLevel);
		ibufSize=ZSTD_CStreamInSize();
		ibufUsed=0;
		ibuf=new char_type[ibufSize];
		obuf=new char_type[ZSTD_CStreamOutSize()];
		streamInitialized=true;
	}
};

class zstd_decompressor : public boost::iostreams::multichar_input_filter{
public:
	typedef char char_type;
	
	zstd_decompressor():
	dstream(NULL),
	streamInitialized(false),
	ibuf(NULL),
	ibufSize(0),
	inputEnd(false)
	{}
	
	//boost::iostreams really likes to copy when it should move. This filter
	//cannot generally be copied, but doing so is possible between construction
	//and first real use, which in practice is the only time it's needed.
	zstd_decompressor(const zstd_decompressor& other):
	dstream(NULL),
	streamInitialized(other.streamInitialized),
	ibuf(NULL),
	ibufSize(other.ibufSize),
	inputEnd(other.inputEnd)
	{
		assert(!other.streamInitialized);
		assert(!other.dstream);
		assert(!other.ibuf);
	}
	
	~zstd_decompressor(){
		stream_delete(dstream);
		delete[] ibuf;
	}
	
	private:
	zstd_decompressor& operator=(const zstd_decompressor&);
	public:
	
	//read as much input from src as necessary to write n bytes of decompressed
	//data to dest
	template <typename Source>
	std::streamsize read(Source& src, char_type* dest, std::streamsize n){
		if(!streamInitialized)
			initStream();
		
		std::streamsize result=0;
		
		//First, see if we have anything sitting in ibuf which still needs to be
		//decompressed
		while(zibuf.pos<zibuf.size && n>0){
			ZSTD_outBuffer zobuf;
			zobuf.dst=dest;
			zobuf.size=(size_t)n;
			zobuf.pos=0;
			size_t err=ZSTD_decompressStream(dstream, &zobuf , &zibuf);
			if(ZSTD_isError(err))
				log_fatal_stream("ZSTD_decompressStream error: " << ZSTD_getErrorName(err));
			std::streamsize output_size=zobuf.pos;
			dest+=output_size;
			n-=output_size;
			result+=output_size;
		}
		
		//If that didn't produce enough data, we need to actually fetch more from
		//src, insert it into ibuf, and decompress.
		while(n>0 && !inputEnd){
			//first, try to grab enough data to fill ibuf
			zibuf.size=0;
			zibuf.pos=0;
			while(zibuf.size<ibufSize){
				int c=boost::iostreams::get(src);
				if(c==EOF){
					inputEnd=true;
					break;
				}
				ibuf[zibuf.size++]=c;
			}
			//then try to decompress data until enough output is produced
			while(zibuf.pos<zibuf.size && n>0){
				ZSTD_outBuffer zobuf;
				zobuf.dst=dest;
				zobuf.size=(size_t)n;
				zobuf.pos=0;
				size_t err=ZSTD_decompressStream(dstream, &zobuf , &zibuf);
				if(ZSTD_isError(err))
					log_fatal_stream("ZSTD_decompressStream error: " << ZSTD_getErrorName(err));
				std::streamsize output_size=zobuf.pos;
				dest+=output_size;
				n-=output_size;
				result+=output_size;
			}
		}
		
		return(result);
	}
	
	template <typename Source>
	void close(Source& src){/*Do nothing*/}
private:
	ZSTD_DStream* dstream;
	bool streamInitialized;
	char_type* ibuf;
	std::size_t ibufSize;
	ZSTD_inBuffer zibuf;
	bool inputEnd;
	
	static void stream_delete(ZSTD_DStream* stream){
		if(stream)
			ZSTD_freeDStream(stream);
	}
	
	void initStream(){
		dstream=ZSTD_createDStream();
		ZSTD_initDStream(dstream);
		ibufSize=ZSTD_DStreamInSize();
		ibuf=new char_type[ibufSize];
		zibuf.src=ibuf;
		zibuf.size=0;
		zibuf.pos=0;
		streamInitialized=true;
	}
};

#endif //ZSTD_FILTER_HPP

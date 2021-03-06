/**
 *  $Id$
 *  
 *  Copyright (C) 2004-8
 *  The IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */

#include <icetray/I3Module.h>
#include <icetray/I3Frame.h>

//
//  Module creates and pushes frames of various types
//
struct ManyStreamsSource : public I3Module
{
  unsigned index;
  std::string frame_types;

  ManyStreamsSource(const I3Context& context)  : I3Module(context)
  {
    index = 0;
    frame_types = "GCDPPXGCXDPP";
  }

  void Process()
  {
    log_trace("%s: %s", GetName().c_str(), __PRETTY_FUNCTION__);
    I3FramePtr frame(new I3Frame(frame_types[index % frame_types.size()]));
    PushFrame(frame);
    index++;
  }
};

I3_MODULE(ManyStreamsSource);


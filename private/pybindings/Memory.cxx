#include <map>
#include <string>
#include <boost/python.hpp>

#include "icetray/python/dataclass_suite.hpp"
#include "icetray/memory.h"

struct Memory{};
void register_Memory(){
  boost::python::scope m = boost::python::class_<Memory>("memory")
    .def("get_extents", &memory::get_extents )
    .staticmethod("get_extents")
    .def("set_label", &memory::set_label )
    .staticmethod("set_label")
    ;

  boost::python::class_<std::map<std::string,size_t> >("MemoryMap")
    .def(boost::python::dataclass_suite<std::map<std::string,size_t> >())
    ;
}

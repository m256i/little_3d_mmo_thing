#ifndef wren_hpp
#define wren_hpp

// This is a convenience header for users that want to compile Wren as C and
// link to it from a C++ application.

extern "C"
{
#include "wren.h"
#include "src/wren/vm/wren_vm.h"
#include "src/wren/vm/wren_debug.h"
}

#endif

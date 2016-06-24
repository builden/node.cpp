// nodecpp.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "nodecpp.h"


// This is an example of an exported variable
NODECPP_API int nnodecpp=0;

// This is an example of an exported function.
NODECPP_API int fnnodecpp(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see nodecpp.h for the class definition
Cnodecpp::Cnodecpp()
{
	return;
}

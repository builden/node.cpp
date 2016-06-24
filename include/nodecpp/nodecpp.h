// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the NODECPP_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// NODECPP_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef NODECPP_EXPORTS
#define NODECPP_API __declspec(dllexport)
#else
#define NODECPP_API __declspec(dllimport)
#endif

// This class is exported from the nodecpp.dll
class NODECPP_API Cnodecpp {
public:
	Cnodecpp(void);
	// TODO: add your methods here.
};

extern NODECPP_API int nnodecpp;

NODECPP_API int fnnodecpp(void);

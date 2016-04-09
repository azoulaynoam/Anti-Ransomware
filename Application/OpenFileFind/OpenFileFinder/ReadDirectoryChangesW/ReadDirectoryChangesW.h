// Inclusion guard
#ifndef _DLLTUT_DLL_H_
#define _DLLTUT_DLL_H_

// Make our life easier, if DLL_EXPORT is defined in a file then DECLDIR will do an export
// If it is not defined DECLDIR will do an import
#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

// Specify "C" linkage to get rid of C++ name mangeling
extern "C"
{
	// Declare 2 functions
	DECLDIR void Files_Change(char* dir_path_char);//, void * variables );
}

// End the inclusion guard
#endif
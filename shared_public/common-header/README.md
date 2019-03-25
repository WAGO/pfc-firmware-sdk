# Common Header Library
## General notes
This file is the library description for the common-header library. The library is header-only, so no compilation is needed for usage.

## Unit tests
In subdirectory "*test-src*" are unit test sources provided. To compile and run unit tests call make target "*check*":  
"*make check*".

## Library usage
To use this library include the header files from "*inc/wc/*" for the functionality you wish to use. Available headers are:
- compiler.h        -> Helper to handle compiler specifics like compiler specific extensions
- api_export.h      -> Defines to publish API functions of shared libraries
- std_includes.h    -> Provides very often used basic headers stdint, stddef, stdbool and limits in a compatible way for C and C++
- preprocessing.h   -> Functionality to handle preprocessor operations
- assertion.h       -> Functionality for static and runtime assertions
- structuring.h     -> Functionality to deal with arrays, structures and other containers

### Usage notes for api_export.h
The *WC_API_EXPORT* macro is **not** intended to be used directly in front of your function/method or class.  
Instead of using it directly declare your project specific define (e. g. *MYLIB_API*) as empty define when not yet defined:

> \#ifndef MYLIB\_API  
> \#define MYLIB\_API  
> \#endif

> MYLIB\_API int MyFunc(int param);

To publish the marked API declare your project specific define to be equal to *WC_API_EXPORT* in front of your build process like the following:

> gcc mylib.c -DMYLIB\_API=WC\_API\_EXPORT

### Usage notes for assertion.h
To use the *WC_ASSERT* and *WC_FAIL* macro from assertion.h you have to provide an implementation for the following C-function with an application specific failure handling:

> void wc_Fail(char const \* const szReason, char const \* const szFile, char const \* const szFunction, int const line);

To disable failure source information (e. g. to shrink binary size and memory footprint) you may declare

> DISABLE\_WC\_FAIL\_SOURCE

in your build process for your library or application in which the *WC_FAIL* or *WC_ASSERT* macros are used.

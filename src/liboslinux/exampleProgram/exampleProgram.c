/*
 ============================================================================
 Name        : exampleProgram.c
 Author      : Jan Sondhauss
 Version     :
 Copyright   : WAGO GmbH & Co.Kg
 Description : Uses shared library to print greeting
               To run the resulting executable the LD_LIBRARY_PATH must be
               set to ${project_loc}/liboslinux/.libs
               Alternatively, libtool creates a wrapper shell script in the
               build directory of this program which can be used to run it.
               Here the script will be called exampleProgram.
 ============================================================================
 */

#include "liboslinux.h"

int main(void) {
  print_hello();
  return 0;
}

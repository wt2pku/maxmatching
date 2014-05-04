//////////////////////////////////////////////////////////////////////
/// @file main.cpp
/// @author Tao Wang@CS401 2014 Spring
/// @main file for testcases
//////////////////////////////////////////////////////////////////////
#include<stdio.h>
#include<string.h>
#include "objects.hpp"
#include "getcloserV.hpp"

/***********************main*******************************************/
/************************ extern variables ************************/
extern char input_file[];
/*****************************************************************/
int main(int argc, char *argv[]) {
	char defaultFile[100] = "./small.dat";
    if (argc==1)  {
		strcpy(input_file, defaultFile);
	} else {
		strcpy(input_file, argv[1]);
	}
	
	//test for getcloser
	getcloserV();
	
	return 0;
}
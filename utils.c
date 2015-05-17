/*
 * =====================================================================================
 *
 *       Filename:  utils.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/16/2015 10:04:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  @gurpinars, 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "utils.h"


const char* get_filename_ext(char* filename) {
    const char *dot = strrchr(filename, '.') + 1;
	if(!dot) 
		return "";
    return dot;
}


const char* lookup(const char* extension){
    if(!strcasecmp(extension, "html"))
		return "text/html";
    else if(!strcasecmp(extension, "jpg"))
		return "image/jpeg";
	else
		return NULL;
}

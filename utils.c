#include "utils.h"
#include <string.h>


const char* get_filename_ext(char* filename) {
    const char *dot = strrchr(filename, '.') + 1;
	if(!dot) 
		return "";
    return dot;
}


const char* lookup(const char* extension){
    if(!strcasecmp(extension, "html"))
		return "text/html";
    else if(!strcasecmp(extension,"jpg"))
		return "image/jpeg";
	else
		return NULL;
}

#include <stdio.h>

FILE* open_file(const char* fname, const char* mode){
    return fopen(fname, mode);
}

void close_file(FILE* const file){
    if(file != NULL){
        fclose(file);
    }
}

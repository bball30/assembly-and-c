#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdio.h>

FILE* open_file(const char* fname, const char* mode);

void close_file(const FILE* file);

#endif

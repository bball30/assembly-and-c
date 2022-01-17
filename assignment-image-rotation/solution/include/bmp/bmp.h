#ifndef BMP_H
#define BMP_H

#include "image/image.h"

#include <stdint.h>
#include <malloc.h>
#include <stdio.h>

struct bmp_header
{
    uint16_t bfType;
    uint32_t bfileSize;
    uint32_t bfReserved;
    uint32_t bOffBits;
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} __attribute__((packed)); // packed заставляет компилятор упаковывать переменные плотно в памяти

enum read_status {
    READ_OK = 0,
    READ_INVALID_SIGNATURE,
    READ_INVALID_BITS,
    READ_INVALID_HEADER,
    READ_FAILED
  };

/*  serializer   */
enum  write_status {
    WRITE_OK = 0,
    WRITE_ERROR
};

enum rotate_status {
  ROTATE_OK = 0,
  ROTATE_FAILED
};

enum read_status from_bmp(FILE* in, struct image* img);

enum write_status to_bmp(FILE* out, struct image const* image);

#endif

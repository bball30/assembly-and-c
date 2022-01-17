#ifndef ROTATE_H
#define ROTATE_H

#include "bmp/bmp.h"
#include "image/image.h"

enum rotate_status rotate(const struct image oldImage, struct image* const newImage);

#endif

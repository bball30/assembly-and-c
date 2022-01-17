#ifndef IMAGE_H
#define IMAGE_H

#include <inttypes.h>
#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"


struct pixel {
    uint8_t b, g, r;
};

struct image {
    uint64_t width, height;
    struct pixel *data;
};

struct image create_image(size_t width, size_t height);

void free_image_memory(struct image* img);

#endif

#include "bmp/bmp.h"
#include "image/image.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static uint32_t get_offset(uint32_t width){
    return width % 4 == 0 ? 0 : (width * 3 / 4 + 1) * 4 - width * 3;
}

static enum read_status read_pixels(FILE* in, struct image* img, struct bmp_header header) {
	const uint32_t width = header.biWidth;
	const uint32_t height = header.biHeight;
	const uint32_t offset = get_offset(width);
	*img = create_image(width, height);
	struct pixel* const pixels = (*img).data;
	for (uint32_t i = 0; i < height; i++) {
		if (fread(pixels + width * i, sizeof(struct pixel), width, in) != width || fseek(in, offset, SEEK_CUR) != 0) return READ_FAILED;
	}
	return READ_OK;
}

enum read_status from_bmp(FILE* in, struct image* img){
	struct bmp_header header = {0};
	enum read_status status = READ_FAILED;
	if (fread(&header, sizeof(struct bmp_header), 1, in)) {
		status = READ_OK;
	}
	if (status != READ_OK) return status;
	return read_pixels(in, img, header);
}

static enum write_status write_header(FILE* const out, const size_t width, const size_t height) {
	uint8_t offset = get_offset(width);
	const size_t image_size = (sizeof(struct pixel) * (width) + offset) * (height);
	const struct bmp_header header = {
		.bfType = 19778,
        .bfileSize = 54 + image_size,
        .bfReserved = 0,
        .bOffBits = 54,
        .biSize = 40,
        .biWidth = width,
        .biHeight = height,
        .biPlanes = 1,
        .biBitCount = 24,
        .biCompression = 0,
        .biSizeImage = image_size,
        .biXPelsPerMeter = 0,
        .biYPelsPerMeter = 0,
        .biClrUsed = 0,
        .biClrImportant = 0,
	};
	if (fwrite(&header, sizeof(struct bmp_header), 1, out) != 1) {
		return WRITE_ERROR;
	}
	return WRITE_OK;
}

static enum write_status write_pixels(struct image const* image, FILE* out) {
	const uint32_t width = image->width;
	const uint32_t height = image->height;
	uint8_t offset = get_offset(width);
	const uint8_t* const offsets[3] = {0};
	for (size_t i = 0; i < height; i++) {
		if (!fwrite((image->data) + i * width, sizeof(struct pixel) * width, 1, out)) return WRITE_ERROR;
		if (!fwrite(offsets, offset, 1, out)) return WRITE_ERROR;
	}
	return WRITE_OK;
}

enum write_status to_bmp(FILE* out, struct image const* image){
    const uint32_t width = image->width;
	const uint32_t height = image->height;
	const enum write_status status = write_header(out, width, height);
	if (status != WRITE_OK) return status;
	return write_pixels(image, out);
}

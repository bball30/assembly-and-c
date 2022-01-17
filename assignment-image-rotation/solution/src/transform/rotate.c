#include "bmp/bmp.h"
#include "image/image.h"
#include "transform/rotate.h"

enum rotate_status rotate(const struct image oldImage, struct image* const newImage){
	size_t new_width = oldImage.height;
	size_t new_height = oldImage.width;
	*newImage = create_image(new_width, new_height);
	for (size_t i = 0; i < new_height; i++) {
		for (size_t j = 0; j < new_width; j++) {
			struct pixel pxl = oldImage.data[j * oldImage.width + i];
			newImage->data[i * (newImage->width) + (newImage->width - j - 1)] = pxl;
		}
	}
	return ROTATE_OK;
}

void calculate(const struct image oldImage, struct image* const newImage) {
	for (size_t i = 0; i < oldImage.width; i++) {
		for (size_t j = 0; j < oldImage.height; j++) {
			struct pixel pxl = oldImage.data[j * oldImage.width + i];
			newImage->data[i * (newImage->width) + (newImage->width - j - 1)] = pxl;
		}
	}
}

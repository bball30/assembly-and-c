#include "bmp/bmp.h"
#include "file_manager/file.h"
#include "image/image.h"
#include "transform/rotate.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

static const char* read_message[] = {
    [READ_OK] = "Данные прочитаны успешно!\n",
    [READ_FAILED] = "Не удалось прочитать данные!\n",
    [READ_INVALID_SIGNATURE] = "Ошибка чтения: Invalid signature\n",
    [READ_INVALID_BITS] = "Ошибка чтения: Invalid pixel data\n",
    [READ_INVALID_HEADER] = "Ошибка чтения: Invalid header\n"
};

static const char* write_message[] = {
    [WRITE_OK] = "Данные успешно записаны!\n",
    [WRITE_ERROR] = "Ошибка при попытке записи в файл!\n"
};

static const char* rotate_message[] = {
    [ROTATE_OK] = "Изображение успешно повернулось!\n",
    [ROTATE_FAILED] = "Произошла ошибка при попытке повернуть изображение!\n"
};

int main( int argc, char** argv ) {
    if (argc < 3) {
        printf("%s\n", "Недостаточное количество параметров!");
        return 0;
    }
    FILE* in = open_file(argv[1], "rb");
    if (errno != 0) {
        printf("%s\n", "Не удалось открыть файл для чтения!");
        return 1;
    }
    FILE* out = open_file(argv[2], "ab");
    if (errno != 0) {
        printf("%s\n", "Не удалось открыть файл для записи!");
        return 1;
    }
    struct image input_image = (struct image){0};
    struct image rotate_input_image = (struct image) {0};
    enum read_status read_status = from_bmp(in, &input_image);
    printf("%s", read_message[read_status]);
    if (read_status != READ_OK) {
        free_image_memory(&input_image);
        free_image_memory(&rotate_input_image);
        close_file(in);
        close_file(out);
        return 0;
    }
    close_file(in);
    if (errno != 0) {
        free_image_memory(&input_image);
        free_image_memory(&rotate_input_image);
        printf("%s\n", "Не удалось закрыть файл!");
        return 1;
    }
    enum rotate_status rotate_status = rotate(input_image, &rotate_input_image);
    printf("%s", rotate_message[rotate_status]);
    enum write_status write_status = to_bmp(out, &rotate_input_image);
    printf("%s", write_message[write_status]);
    close_file(out);
    if (errno != 0) {
        free_image_memory(&input_image);
        free_image_memory(&rotate_input_image);
        printf("%s\n", "Не удалось закрыть файл!");
        return 1;
    }
    free_image_memory(&input_image);
    free_image_memory(&rotate_input_image);
    return 0;
}

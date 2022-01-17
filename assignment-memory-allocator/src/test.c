#include "test.h"

#include <unistd.h>

#include "mem_internals.h"
#include "mem.h"
#include "util.h"
#include <sys/mman.h>
#ifndef MAP_FIXED_NOREPLACE
#define MAP_FIXED_NOREPLACE 0x10000
#endif


void debug(const char* fmt, ...);

static void* test_heap_init();
//static void create_new_heap(struct block_header *last_block);
static struct block_header* get_block_from_contents(void * data);


static void* test_heap_init() {
	debug("***Инициализация кучи***\n");
	void* heap = heap_init(10000);
	if (heap == NULL) {
		err("Ошибка при инициализации кучи.\n");
	}
	debug_heap(stdout, heap);
	debug("Инициализация кучи прошла успешно!");
	return heap;
}
/*
static void create_new_heap(struct block_header *last_block) {
	struct block_header* addr = last_block;
	void* test_addr = (uint8_t*) addr + size_from_capacity(addr->capacity).bytes;
	test_addr = mmap( (uint8_t*) (getpagesize() * ((size_t) test_addr / getpagesize() + (((size_t) test_addr % getpagesize()) > 0))), 1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE,0, 0);
	debug(test_addr);
}
*/
static struct block_header* get_block_from_contents(void* data) {
	return (struct block_header*) ((uint8_t*) data - offsetof(struct block_header, contents));
}

void test1(struct block_header *first_block) {
	debug("\n***Начали тест 1***\n");	
	void* data = _malloc(1000);
	if (data == NULL) {
		err("Ошибка: Тест 1.\n");
	}
	debug_heap(stdout, first_block);
	if (first_block->is_free != false || first_block->capacity.bytes != 1000) {
		err("Ошибка: Тест 1.\n");
	}
	debug("Тест 1 успешно пройден! Обычное успешное выделение памяти\n");
	_free(data);
}

void test2(struct block_header *first_block) {
	debug("\n***Начали тест 2***\n");
	
	void* heap1 = _malloc(1000);
	void* heap2 = _malloc(1000);
	
	if (heap1 == NULL || heap2 == NULL) err("Ошибка: Тест 2.\n");
	
	_free(heap1);
	debug_heap(stdout, first_block);
	
	struct block_header* heap1_block = get_block_from_contents(heap1);
	struct block_header* heap2_block = get_block_from_contents(heap2);
	if (heap1_block->is_free == false || heap2_block->is_free == true) {
		err("Ошибка: Тест 2.\n");
	}
	debug("Тест 2 успешно пройден! Освобождение одного блока из нескольких выделенных.\n");
	_free(heap2);
}

void test3(struct block_header *first_block) {
	debug("\n***Начали тест 3***\n");
	
	void* heap1 = _malloc(1000);
	void* heap2 = _malloc(1000);
	void* heap3 = _malloc(1000);
	
	if (heap1 == NULL || heap2 == NULL || heap3 == NULL) err("Ошибка: Тест 3.\n");
	
	_free(heap1);
	_free(heap3);
	debug_heap(stdout, first_block);
	
	struct block_header* heap1_block = get_block_from_contents(heap1);
	struct block_header* heap2_block = get_block_from_contents(heap2);
	struct block_header* heap3_block = get_block_from_contents(heap3);
	if (heap1_block->is_free == false || heap2_block->is_free == true || heap3_block->is_free == false) {
		err("Ошибка: Тест 2.\n");
	}
	debug("Тест 3 успешно пройден! Освобождение двух блоков из нескольких выделенных.\n");
	_free(heap1);
	_free(heap2);
	_free(heap3);
}

void test4(struct block_header *first_block) {
	debug("\n***Начали тест 4***\n");
	
	debug_heap(stdout, first_block);
	
	void* heap1 = _malloc(10000);
	void* heap2 = _malloc(5000);
	
	if (heap2 == NULL) err("Ошибка: Тест 4.\n");
	
	debug_heap(stdout, first_block);
	debug("Тест 4 успешно пройден! Память закончилась, новый регион памяти расширяет старый.\n");
	_free(heap1);
	_free(heap2);
}

void test5(struct block_header *first_block) {
	debug("\n***Начали тест 5***\n");
	
	void* heap1 = _malloc(10000);
	if (heap1 == NULL) err("Ошибка: Тест 5.\n");
	
	struct block_header* addr = first_block;
	while (addr->next != NULL) addr = addr->next;
	map_pages((uint8_t *) addr + size_from_capacity(addr->capacity).bytes, 1000, MAP_FIXED_NOREPLACE);
	
	void* heap2 = _malloc(30000);
	
	if (get_block_from_contents(heap2) == addr) err("Ошибка: Тест 5.\n");
	
	debug_heap(stdout, first_block);
	debug("Тест 5 успешно пройден! Память закончилась, старый регион памяти не расширить из-за другого выделенного диапазона адресов, новый регион выделяется в другом месте.\n");
	_free(heap1);
	_free(heap2);
}

void run_tester() {
	struct block_header *first_block = (struct block_header*) test_heap_init();
	test1(first_block);
	test2(first_block);
	test3(first_block);
	test4(first_block);
	test5(first_block);
}

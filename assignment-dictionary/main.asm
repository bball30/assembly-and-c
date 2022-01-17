%include "words.inc"
%include "lib.inc"
%define BUFF_SIZE 256
%define DQ_SIZE 8

extern find_word

global _start

section .data

not_found:
	db "Key not found!", 0
buffer_overflow:
	db "ERROR. Buffer read failure.", 0
string_buffer:
	times BUFF_SIZE db 0

section .text

; пытаемся прочитать строку в буффер
_start:
	xor rax, rax
	mov rdi, string_buffer
	mov rsi, BUFF_SIZE
	call read_word
	test rax, rax
	jne .success_read_buffer
	mov rdi, buffer_overflow
	call print_err
	call print_newline
	call exit
; теперь в rax - адрес буфера, в rdx - длина строки
.success_read_buffer:
	mov rdi, rax
	mov rsi, first
	push rdx					; сохраняем длину строки
	call find_word				; ищем такой же элемент в списке
	test rax, rax				; если не нашли rax = 0
	jne .success_key_found
	mov rdi, not_found
	call print_err
	call print_newline
	call exit	
.success_key_found:
	pop rdx						; достаем длину строки обратно
	add rax, DQ_SIZE			; получаем адрес ключа
	add rax, rdx				; добавляем длину строки, чтобы попасть в ее конец
	add rax, 1					; не забываем про нуль-терминатор 
	mov rdi, rax				; и теперь rdi указывает на саму строку 
	call print_string
	call print_newline
	call exit

print_err:
	xor rax, rax
	mov rsi, rdi
	call string_length
	mov rdx, rax
	mov rdi, 2
	mov rax, 1
	syscall
	ret
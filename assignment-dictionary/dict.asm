%define DQ_SIZE 8

global find_word

extern string_length
extern string_equals

find_word:
	xor rax, rax
	mov r8, rdi					; указатель на искомый ключ
	mov r9, rsi					; указатель на адрес начала списка
	.loop:
		add r9, DQ_SIZE			; список начинается с указателя на следующий элемент, поэтому прибавим 8, чтобы получить ключ i-го элемента
		mov rsi, r9 			; i-ый ключ кладем в rsi 
		mov rdi, r8	
		push r8
		push r9
		call string_equals		; сравниваем 2 ключа
		pop r9
		pop r8
		cmp rax, 1
		je .finish
		mov r9, [r9 - DQ_SIZE]	; получаем адрес на следующий элемент списка
		cmp r9, 0				; если указатель на следующий элемент 0, то искомого ключа в списке нет
		je .fail
		jmp .loop
	.finish:	
		sub r9, DQ_SIZE			; получаем сам адрес найденного ключа
		mov rax, r9
		ret
	.fail:
		xor rax, rax
		ret
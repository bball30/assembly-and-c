section .text

global string_length
global print_string
global print_char
global print_newline
global print_uint
global print_int
global string_equals
global read_char
global read_word
global parse_uint
global parse_int
global string_copy
global exit
 
; Принимает код возврата и завершает текущий процесс
exit: 
    mov rax, 60
	syscall 
	ret

; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
    xor rax, rax			; обнуление аккумулятора
	
	.loop:					; цикл для поиска конца строки
		mov rsi, [rdi + rax]; посимвольно читаем строку
		and rsi, 0xff		; оставляем последний байт
		test rsi, rsi		; проверка на 0
		jz .end				
		inc rax				; результат
		jmp .loop
		
    .end:
		ret

; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
	push rdi				; сохраняем указатель на заданную строку
	;push rax
	;push rsi
	;push rdx
	
	.execute:
		call string_length	; количество символов в строке -> rax
		mov rdx, rax		; длина строки -> rdx	
		mov rsi, rdi		; адрес начала строки -> rsi
		mov rax, 1			; номер системного вызова 'write'
		mov rdi, 1			; дискриптор stdout
		syscall				; сам системный вызов
	
	.end:
		;pop rdx
		;pop rsi
		;pop rax
		pop rdi				; достаем значение из стека
		ret

; Переводит строку (выводит символ с кодом 0xA)
print_newline:
    mov rdi, 0x0A			; заданный символ -> rdi

	; Принимает код символа и выводит его в stdout
print_char:
	push rdi				; сохраняем заданный символ в стек
	mov rsi, rsp			; адрес начала строки -> rsi
	mov rax, 1				; количество символов в строке -> rax
	mov rdi, 1				; дискриптор stdout
	mov rdx, 1				; длина строки -> rdx
	syscall					; сам системный вызов
	pop rdi					; достаем значение из стека
	ret

; Выводит беззнаковое 8-байтовое число (rdi) в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
	mov r11, 10				; константа
	mov rax, rdi			; заданное число -> rax
	mov r10, rsp 			; сохраним стек, чтобы потом восстановить
	dec rsp
	mov byte [rsp], 0x0		; нуль-терминатор для корректного вывода строки

	.separete_number:
		xor rdx, rdx		; 0 -> rdx
		div r11				; деление rax(заданное число) на r11(10) -> целая часть в rax, остаток в rdx
		or rdx, 0x30		; получаем ASCII код символа
		dec rsp				; уменьшаем стек
		mov byte [rsp], dl  ; кладем на стек цифру
		cmp rax, 0			; проверка на конец числа
		jne .separete_number; считываем следующую цифру
		
	.out_number:
		mov rdi, rsp		; достаем символ из стека
		push r10
		call print_string
		pop rsp
    
	ret

; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
    .print_sign:
		mov rax, rdi		; заданное число -> rax
		sar rax, 63			; арифметический сдвиг вправо на 63 бита
		and rax, 0x1		; оставляем 0-ой бит 
		cmp rax, 0			; сравниваем с 0
		je .print_number	; если 0, то число неотрицательное
		
	.prepare_neg_number:	; функция для вывода "-" 
		push rdi			; сохраняем заданное число(rdi) в стек
		mov rdi, 0x2d		; ASCII код символа "-" -> rdi
		call print_char		; выводим символ "-"
		pop rdi				; возвращаем заданное число
		neg rdi				; меняем знак заданного числа
		
	.print_number:
		call print_uint		; выводим заданное число
	
	.end:
		ret

; Принимает два указателя(rdi и rsi) на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
string_equals:
	push rbx
	xor rbx, rbx				; счетчик символов
    xor rax, rax				; здесь будем сравнивать
	
	.loop_equal:
		mov cl, byte [rdi + rbx]; символ первой строки
		cmp cl, byte [rsi + rbx]; сравнили с символом второй строки
		jnz .eq_error
		
		cmp byte [rdi + rbx], 0	; сравнили с нуль-терминатором
		jz .eq_ok
		
		inc rbx					; увеличили счетчик символов
		jmp .loop_equal
	
	.eq_error:
		mov rax, 0				; выводим 0
		jmp .end
	
	.eq_ok:
		mov rax, 1				; выводим 1
		jmp .end
    
	.end:
		pop rbx
		ret

; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
	push rdi
	push rsi
	push rdx
	push 0 					; выделяем ячейку под читаемый символ

    xor rax, rax			; номер системного вызова 0 - 'read' 
	mov rdi, 0				; дискриптор stdin
	mov rsi, rsp			; положим читаемый символ на вершину стека
	mov rdx, 1				; длина строки в байтах
	syscall
	
	mov rax, [rsp]			; прочтенный символ -> rax
    .end:
		pop rdx
		pop rdx
		pop rsi
		pop rdi
		ret 

; Принимает: адрес начала буфера(rdi), размер буфера(rsi)
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор

read_word:
    .delete_spaces:
		call read_char			; считываем первый символ
		cmp rax, 0x09			; проверка на табуляцию
		je .delete_spaces		; если да, то считываем следующий
		cmp rax, 0x0A			; проверка на перевод строки
		je .delete_spaces		; если да, то считываем следующий
		cmp rax, 0x20			; проверка на пробел
		je .delete_spaces		; если да, то считываем следующий
		
	xor rdx, rdx				; обнуляем счетчик
	.read_word_loop:
		cmp rdx, rsi			; сравниваем количество считанных символов с размером заданного буфера
		je .error				; выходим из цикла, если буфер меньше
		cmp rax, 0				; проверка на нуль-терминатор
		je .ok					; если да, то выходим из цикла
		cmp rax, 0x09			; проверка на табуляцию
		je .ok					; если да, то выходим из цикла
		cmp rax, 0x0A			; проверка на перевод строки
		je .ok					; если да, то выходим из цикла
		cmp rax, 0x20			; проверка на пробел
		je .ok					; если да, то выходим из цикла
		
		mov byte [rdi + rdx], al; добавляем в буффер символ
		inc rdx					; увеличиваем счетчик считанных символов
		call read_char			; следующий символ -> rax
		jmp .read_word_loop		; в начало цикла
	
	.error:
		xor rax, rax			; возвращаем 0 в rax
		jmp .end
	
	.ok:
		mov byte [rdi + rdx], 0	; добавляем нуль-терминатор
		mov rax, rdi			; возвращаем адрес начала буффера
		jmp .end
	
    .end:
		ret
 

; Принимает указатель на строку(rdi), пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
	push rbx
	xor rdx, rdx				; счетчик количества символов числа
	xor rax, rax				; само число
	xor rbx, rbx				; символ 
	.loop:
		mov bl, byte [rdi + rdx]; считываем первый символ
		cmp bl, 0x30			; проверяем, попадает ли
		jl .end					; считанный символ
		cmp bl, 0x39			; в диапазон чисел
		jg .end					; ASCII таблицы
		
		sub bl, 0x30			; преобразуем в число
		push rdx				; сохраняем в стек количество считанных символов
		mov rdx, 10				; умножаем rax
		mul rdx					; на rdx=10, результат в rax
		pop rdx					; возвращаем количество считанных символов
		add rax, rbx			; добавляем считанный символ
		inc rdx					; увеличиваем счетчик считанных символов
		jmp .loop				; в начало цикла
	
    .end:
		pop rbx
		ret

; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
    push rdi
	
	.check_sign:
		mov rax, rdi			; считываем первый символ
		cmp byte [rax], 0x2D	; проверка на "-"
		jz .minus
		cmp byte [rax], 0x2B	; проверка на "+"
		jz .plus
		call parse_uint			; если знака нет, то используем предыдущую функцию
		jmp .end
		
	.plus:
		inc rdi					; указатель на строку сдвигаем на 1
		call parse_uint			; используем предыдущую функцию
		cmp rdx, 0				; количество считанных символов сравниваем с 0
		jz .end					
		inc rdx					; добавляем 1(знак) к считанным символам
		jmp .end
	
	.minus:
		inc rdi					; указатель на строку сдвигаем на 1
		call parse_uint			; используем предыдущую функцию
		cmp rdx, 0				; количество считанных символов сравниваем с 0
		jz .end
		inc rdx					; добавляем 1(знак) к считанным символам
		neg rax					; само число представляем как отрицательное
		jmp .end
	
    .end:
		pop rdi
		ret

; Принимает указатель на строку(rdi), указатель на буфер(rsi) и длину буфера(rdx)
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
    xor rax, rax			; обнуляем rax
	
	push rdi				; сохраняем в стеке
	push rsi				; исходные данные
	push rdx
	call string_length		; длина исходной строки -> rax
	pop rdx					; достаем из стека
	pop rsi					; исходные данные
	pop rdi
	
	cmp rax, rdx			; если длина строки больше буффера
	jg .bad					; то выводим 0
	
	push rax				; сохраняем длину строки
	push r13				; вспомогательный буффер
	
	.copy:
		cmp rax, 0			; если длина строки меньше нуля
		jl .end				; выходим из цикла
		dec rax				; уменьшаем длину строки
		mov r13, [rdi + rax]; последний символ -> вспомогательный буффер
		mov [rsi + rax], r13; вспомогательный буффер -> окончательный буффер
		cmp rax, 0			; проверка на конец строки
		jz .end				; выход из цикла, если строка полностью скопирована
		jmp .copy			; возврат в начало цикла
		
	.end:
		pop r13				; достаем значения из стека
		pop rax
		
	.bad:
		mov rax, 0
		ret

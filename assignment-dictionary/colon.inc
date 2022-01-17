%define NEXT_NODE 0					; указатель в никуда для последнего элемента списка
%macro colon 2
	
	%ifid %2						; проверка второго аргумента на идентификатор			
		%2: dq NEXT_NODE			; сохраняем указатель на следующий элемент (предыдущий на самом деле, так как добавляем в начало новые элементы)
		%define NEXT_NODE %2		; заменяем define для корректного добавления следующего элемента
	%else
		%fatal "argument 2: label identifier expected"
	%endif

	%ifstr %1						; проверка первого аргумента на строку
		db %1, 0					; создаем нуль-терминированную строку
	%else
		%fatal "argument 1: string expected as a key value"
	%endif

%endmacro 
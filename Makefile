all	: myshell	

nikos	: myshell.o	
	gcc -o myshell myshell.o 			

test.o	: nikos.c	
	gcc -c myshell.c

clean	:
	rm myshell *.o

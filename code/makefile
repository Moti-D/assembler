assembler:      main.o step2.o step1.o additional_methods.o
	  gcc -g -ansi -Wall -pedantic additional_methods.o step1.o step2.o  main.o -o assembler

additional_methods:     additional_methods.c
	        gcc  -g -ansi -Wall -pedantic additional_methods.c -o additional_methods.o
step1:  step1.c   step2.c
	        gcc -g -ansi -Wall -pedantic step1.c step2.c -o step1.o
step2:  step2.c       additional_methods.c
	        gcc   -g -ansi -Wall -pedantic step2.c  additional_methods.c  -o step2.o
main:   main.c step1.c
	        gcc -g -ansi -Wall -pedantic main.c step1.c -o main.o



compileAll: driver.o lexer.o parser.o
	gcc -g driver.o lexer.o parser.o -o stage1exe

driver.o: 	driver.c
	gcc -c driver.c

lexer.o:	lexer.c lexer.h lexerDef.h
	gcc -c lexer.c

parser.o:	parser.c parser.h parserDef.h
	gcc -c parser.c


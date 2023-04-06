all: dynamic realisation1 realisation2

lib:
	gcc -fPIC -c realisation1.c
	gcc -fPIC -c realisation2.c
	gcc -shared -o librealisation1.so realisation1.o
	gcc -shared -o librealisation2.so realisation2.o

dynamic: lib
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
	gcc main_dynamic.c -o main_d -ldl

main: 
	gcc main.c -c

realisation1: lib main
	gcc main.o -L. -lrealisation1 -o realisation1

realisation2: lib main
	gcc main.o -L. -lrealisation2 -o realisation2

clean:
	rm *.o *.so main_d realisation1 realisation2
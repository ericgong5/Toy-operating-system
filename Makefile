mysh: shell.c interpreter.c shellmemory.c pcb.c kernel.c cpu.c harddisk.c
	gcc -D framesize=$(framesize) -D varmemsize=$(varmemsize) -c shell.c interpreter.c shellmemory.c pcb.c kernel.c cpu.c harddisk.c
	gcc -o mysh shell.o interpreter.o shellmemory.o pcb.o cpu.o kernel.o harddisk.o

clean: 
	rm mysh; rm *.o
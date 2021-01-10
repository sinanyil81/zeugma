all:
	nasm -f bin boot.asm
	nasm -f bin initsys.asm
	nasm -f elf start.asm
	nasm -f elf int.asm

	gcc -ffreestanding -c Scheduler.c
	gcc -ffreestanding -c Timer.c
	gcc -ffreestanding -c Kernel.c
	gcc -ffreestanding -c Descriptor.c
	gcc -ffreestanding -c Console.c
	gcc -ffreestanding -c Keyboard.c
	gcc -ffreestanding -c Interrupts.c
	gcc -ffreestanding -c Exceptions.c
	gcc -ffreestanding -c Memory.c
	gcc -ffreestanding -c Process.c
	gcc -ffreestanding -c Z_Api.c
	gcc -ffreestanding -c API_Table.c
	gcc -ffreestanding -c Lib.c
	gcc -ffreestanding -c Shell.c

install:

	ld -Ttext 0x00000 --oformat binary -o KERNEL.bin \
	start.o int.o  Scheduler.o Timer.o Kernel.o \
	Descriptor.o Console.o Keyboard.o Interrupts.o Exceptions.o \
	Memory.o Process.o Z_Api.o API_Table.o Lib.o Shell.o

clean:

	-rm Start.o
	-rm Int.o
	-rm paging.o
	-rm Scheduler.o
	-rm Timer.o
	-rm Descriptor.o
	-rm Console.o
	-rm Keyboard.o
	-rm Interrupts.o
	-rm Exceptions.o
	-rm Memory.o
	-rm Process.o
	-rm Z_API.o
	-rm API_Table.o
	-rm Lib.o
	-rm Shell.o
      

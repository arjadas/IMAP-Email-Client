fetchmail: 
	gcc -Wall -o  -lm

main.o: main.c read.h process.h queue.h task1.h task2.h task3.h task4.h page.h helperfunctions.h
	gcc -Wall -o main.o main.c -c
helperfunctions.o: helperfunctions.c helperfunctions.h
	gcc -Wall -o helperfunctions.o 
clean:
	rm -f *.o fetchmail
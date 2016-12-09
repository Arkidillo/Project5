CC = gcc
CFLAGS = -g -c -Wextra -Wfloat-equal -pedantic -Wall $(DEBUG)
LFLAGS = -g -Wall $(DEBUG)

%.o: %.c Hashtable.h
	$(CC) $(CFLAGS) $< -o $@ -lm 

p5 : project5.o Hashtable.o
	$(CC) $(LFLAGS) project5.o Hashtable.o -o p5 -lm

clean:
	\rm p5 *.0

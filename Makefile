
all: example

example: example.o base64.o
	$(CC) -o $@ $?	

%.o: %.c
	$(CC) -c -o $@ $?

clean:
	rm -f *.o example

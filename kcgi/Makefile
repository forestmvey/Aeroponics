.PHONY: all
all:
	cc -I/usr/local/include -c test.c -o test.o
	cc -static -L/usr/local/lib -o test.cgi test.o -lkcgi -lz

.PHONY: json
json:
	cc -I/usr/local/include -c json.c -o json.o
	cc -static -L/usr/local/lib -o json.cgi json.o -lkcgi -lkcgijson -lz

.PHONY: clean
clean:
	rm -f *.o
	rm -f *.cgi

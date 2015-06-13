CFLAGS += -Wall -Wextra -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -O3 -g

all: pdfcrack

pdfcrack: main.o sha256.o rc4.o md5.o pdfcrack.o pdfparser.o passwords.o common.o \
	benchmark.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $+

pdfreader: pdfparser.o pdfreader.o common.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $+

clean:
	rm -f pdfcrack pdfreader testreader *.o

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -c -o $@ $+

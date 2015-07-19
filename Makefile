CFLAGS += -Wall -Wextra -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -O0 -g -fopenmp

all: pdfcrack

pdfcrack: main.o sha256.o rc4.o md5.o pdfcrack.o pdfparser.o passwords.o common.o pattern.o \
	benchmark.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $+

pdfreader: pdfparser.o pdfreader.o common.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $+

clean:
	rm -f pdfcrack pdfreader testreader savedstate.sav *.o test*.txt

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -c -o $@ $+
	
regtest: pdfcrack
	./pdfcrack -c bark936 -n 8 -m 8 ~/Downloads/100008929366.pdf
	./pdfcrack -t 1 -e [abkr][abkr][abkr][abkr][936][936][936][936] ~/Downloads/100008929366.pdf
	./pdfcrack -t 5 -e [abkr][abkr][abkr][abkr][936][936][936][936] ~/Downloads/100008929366.pdf
	
test: pdfcrack
	./pdfcrack -t 5 -e [abkr][abkr][abkr][abkr][936][936][936][936] ~/Downloads/100008929366.pdf
	
testsingle: pdfcrack
	./pdfcrack -t 1 -e [abkr][abkr][abkr][abkr][936][936][936][936] ~/Downloads/100008929366.pdf
	
debugold: pdfcrack
	gdb --args ./pdfcrack -c bark936 -n 8 -m 8 ~/Downloads/100008929366.pdf
	
debugnew: pdfcrack
	gdb --args ./pdfcrack -t 1 -e [abkr][abkr][abkr][abkr][936][936][936][936] ~/Downloads/100008929366.pdf
	

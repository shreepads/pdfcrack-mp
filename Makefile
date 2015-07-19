CFLAGS += -Wall -Wextra -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -O3 -g -fopenmp

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
		
test3: pdfcrack
	./pdfcrack -c Niy546 -n 6 -m 6 ./testpdfs/TestPDF3.pdf
	./pdfcrack -t 5 -e [Niy546][Niy546][Niy546][Niy546][Niy546][Niy546] ./testpdfs/TestPDF3.pdf
	
perftest3: pdfcrack
	./pdfcrack -c Niy1234567890 -n 6 -m 6 ./testpdfs/TestPDF3.pdf
	./pdfcrack -t 5 -e [Niy1234567890][Niy1234567890][Niy1234567890][Niy1234567890][Niy1234567890][Niy1234567890] ./testpdfs/TestPDF3.pdf
	
debugold: pdfcrack
	gdb --args ./pdfcrack -c Niy546 -n 6 -m 6 ./testpdfs/TestPDF3.pdf
	
debugnew: pdfcrack
	gdb --args ./pdfcrack -t 5 -e [Niy546][Niy546][Niy546][Niy546][Niy546][Niy546] ./testpdfs/TestPDF3.pdf
	

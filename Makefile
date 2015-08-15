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
		
testall: test1 test3 test4

test1: pdfcrack
	./pdfcrack -o -c Jmy831 -n 6 -m 6 ./testpdfs/TestPDF1.pdf
	./pdfcrack -o -t 5 -e [Jmy][Jmy][y][Jmy1234567890][831][123458] ./testpdfs/TestPDF1.pdf

perftest1: pdfcrack
	./pdfcrack -o -c Jmy1234567890 -n 6 -m 6 ./testpdfs/TestPDF1.pdf
	./pdfcrack -o -t 5 -e [Jmy1234567890][Jmy1234567890][Jmy1234567890][Jmy1234567890][Jmy1234567890][Jmy1234567890] ./testpdfs/TestPDF1.pdf

test3: pdfcrack
	./pdfcrack -c Niy546 -n 6 -m 6 ./testpdfs/TestPDF3.pdf
	./pdfcrack -t 5 -e [Niy][Niy][y][Niy1234567890][546][123546] ./testpdfs/TestPDF3.pdf
	
perftest3: pdfcrack
	./pdfcrack -c Niy1234567890 -n 6 -m 6 ./testpdfs/TestPDF3.pdf
	./pdfcrack -t 5 -e [Niy1234567890][Niy1234567890][Niy1234567890][Niy1234567890][Niy1234567890][Niy1234567890] ./testpdfs/TestPDF3.pdf
	
test4: pdfcrack
	./pdfcrack -c Ktw810 -n 6 -m 6 ./testpdfs/TestPDF4.pdf
	./pdfcrack -t 5 -e [Ktw][Ktw][w][Ktw1234567890][810][423810] ./testpdfs/TestPDF4.pdf
	
test6: pdfcrack
	./pdfcrack -c Mnv731 -n 6 -m 6 ./testpdfs/TestPDF6.pdf
	./pdfcrack -t 5 -e [Mnv][Mnv][v][Mnv1234567890][731][731245] ./testpdfs/TestPDF6.pdf	
	
debugold: pdfcrack
	gdb --args ./pdfcrack -c Mnv731 -n 6 -m 6 ./testpdfs/TestPDF6.pdf
	
debugnew: pdfcrack
	gdb --args ./pdfcrack -t 1 -e [Mnv][Mnv][v][Mnv1234567890][731][731245] ./testpdfs/TestPDF6.pdf


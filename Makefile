CFLAGS += -Wall -Wextra -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -O3 -g -fopenmp -std=c99

all: pdfcrack

pdfcrack: main.o sha256.o rc4.o md5.o pdfcrack.o pdfparser.o passwords.o common.o pattern.o \
	benchmark.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $+

pdfreader: pdfparser.o pdfreader.o common.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $+

clean:
	rm -f pdfcrack pdfreader testreader savedstate.sav *.o test*.txt
	rm -f ./testpdfs/testpasswordslist_big.txt ./testpdfs/testpasswordslist_huge.txt

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -c -o $@ $+
		
testall: test perftest

test: test2 test5 test1 test3 test4 test6 test7

perftest: perftest2 perftest5 perftest1 perftest3 perftest4 perftest6 perftest7

unzip:
	@if [ ! -f ./testpdfs/testpasswordslist_big.txt ]; then bunzip2 -k ./testpdfs/testpasswordslist_big.txt.bz2; fi;
	@if [ ! -f ./testpdfs/testpasswordslist_huge.txt ]; then bunzip2 -k ./testpdfs/testpasswordslist_huge.txt.bz2; fi;

test1: pdfcrack
	@echo ""
	@echo "---------------------------------------------"
	@echo "Testing Rev3 owner password"
	@echo "---------------------------------------------"
	@echo ""
	@echo "Generative method"
	@echo "-----------------"
	./pdfcrack -o -c Jmy831 -n 6 -m 6 ./testpdfs/TestPDF1.pdf
	@echo ""
	@echo "Wordlist method"
	@echo "---------------"
	./pdfcrack -o -w ./testpdfs/testpasswordslist_small.txt ./testpdfs/TestPDF1.pdf
	@echo ""
	@echo "Pattern method"
	@echo "--------------"
	./pdfcrack -o -t 5 -e [Jmy][Jmy][y][Jmy1234567890][831][123458] ./testpdfs/TestPDF1.pdf


perftest1: pdfcrack unzip
	@echo ""
	@echo "============================================="
	@echo "Performance testing Rev3 owner password"
	@echo "============================================="
	@echo ""
	@echo "Generative method"
	@echo "================="
	./pdfcrack -o -c Jmy1234567890 -n 6 -m 6 ./testpdfs/TestPDF1.pdf
	@echo ""
	@echo "Wordlist method"
	@echo "================="
	./pdfcrack -o -w ./testpdfs/testpasswordslist_big.txt ./testpdfs/TestPDF1.pdf
	@echo ""
	@echo "Pattern method"
	@echo "=============="
	./pdfcrack -o -t 5 -e [Jmy1234567890][Jmy1234567890][Jmy1234567890][Jmy1234567890][Jmy1234567890][Jmy1234567890] ./testpdfs/TestPDF1.pdf



test2: pdfcrack
	@echo ""
	@echo "---------------------------------------------"
	@echo "Testing Rev2 user password"
	@echo "---------------------------------------------"
	@echo ""
	@echo "Generative method"
	@echo "-----------------"
	./pdfcrack -c Jhe238 -n 6 -m 6 ./testpdfs/TestPDF2.pdf
	@echo ""
	@echo "Wordlist method"
	@echo "---------------"
	./pdfcrack -w ./testpdfs/testpasswordslist_small.txt ./testpdfs/TestPDF2.pdf
	@echo ""
	@echo "Pattern method"
	@echo "--------------"
	./pdfcrack -t 5 -e [Jhe][Jhe][e][Jhe1234567890][123][123458] ./testpdfs/TestPDF2.pdf


perftest2: pdfcrack unzip
	@echo ""
	@echo "================================================"
	@echo "Performance testing Rev2 user password"
	@echo "================================================"
	@echo ""
	@echo "Generative method"
	@echo "================="
	./pdfcrack -c Jhe1234567890zyxwvutsrq -n 6 -m 6 ./testpdfs/TestPDF2.pdf
	@echo ""
	@echo "Wordlist method"
	@echo "================="
	./pdfcrack -w ./testpdfs/testpasswordslist_huge.txt ./testpdfs/TestPDF2.pdf
	@echo ""
	@echo "Pattern method"
	@echo "=============="
	./pdfcrack -t 5 -e [Jhe1234567890zyxwvutsrq][Jhe1234567890zyxwvutsrq][Jhe1234567890zyxwvutsrq][Jhe1234567890zyxwvutsrq][Jhe1234567890zyxwvutsrq][Jhe1234567890zyxwvutsrq] ./testpdfs/TestPDF2.pdf



test3: pdfcrack
	@echo ""
	@echo "---------------------------------------------"
	@echo "Testing Rev3 (AES) user password"
	@echo "---------------------------------------------"
	@echo ""
	@echo "Generative method"
	@echo "-----------------"
	./pdfcrack -c Niy546 -n 6 -m 6 ./testpdfs/TestPDF3.pdf
	@echo ""
	@echo "Wordlist method"
	@echo "---------------"
	./pdfcrack -w ./testpdfs/testpasswordslist_small.txt ./testpdfs/TestPDF3.pdf
	@echo ""
	@echo "Pattern method"
	@echo "--------------"
	./pdfcrack -t 5 -e [Niy][Niy][y][Niy1234567890][546][123546] ./testpdfs/TestPDF3.pdf

	
perftest3: pdfcrack unzip
	@echo ""
	@echo "================================================"
	@echo "Performance testing Rev3 (AES) user password"
	@echo "================================================"
	@echo ""
	@echo "Generative method"
	@echo "================="
	./pdfcrack -c Niy1234567890 -n 6 -m 6 ./testpdfs/TestPDF3.pdf
	@echo ""
	@echo "Wordlist method"
	@echo "================="
	./pdfcrack -w ./testpdfs/testpasswordslist_big.txt ./testpdfs/TestPDF3.pdf
	@echo ""
	@echo "Pattern method"
	@echo "=============="
	./pdfcrack -t 5 -e [Niy1234567890][Niy1234567890][Niy1234567890][Niy1234567890][Niy1234567890][Niy1234567890] ./testpdfs/TestPDF3.pdf


	
test4: pdfcrack
	@echo ""
	@echo "---------------------------------------------"
	@echo "Testing Rev3 (RC4) user password"
	@echo "---------------------------------------------"
	@echo ""
	@echo "Generative method"
	@echo "-----------------"
	./pdfcrack -c Ktw810 -n 6 -m 6 ./testpdfs/TestPDF4.pdf
	@echo ""
	@echo "Wordlist method"
	@echo "---------------"
	./pdfcrack -w ./testpdfs/testpasswordslist_small.txt ./testpdfs/TestPDF4.pdf
	@echo ""
	@echo "Pattern method"
	@echo "--------------"
	./pdfcrack -t 5 -e [Ktw][Ktw][w][Ktw1234567890][810][423810] ./testpdfs/TestPDF4.pdf


perftest4: pdfcrack unzip
	@echo ""
	@echo "================================================"
	@echo "Performance testing Rev3 (RC4) user password"
	@echo "================================================"
	@echo ""
	@echo "Generative method"
	@echo "================="
	./pdfcrack -c Ktw1234567890 -n 6 -m 6 ./testpdfs/TestPDF4.pdf
	@echo ""
	@echo "Wordlist method"
	@echo "================="
	./pdfcrack -w ./testpdfs/testpasswordslist_big.txt ./testpdfs/TestPDF4.pdf
	@echo ""
	@echo "Pattern method"
	@echo "=============="
	./pdfcrack -t 5 -e [Ktw1234567890][Ktw1234567890][Ktw1234567890][Ktw1234567890][Ktw1234567890][Ktw1234567890] ./testpdfs/TestPDF4.pdf



test5: pdfcrack
	@echo ""
	@echo "---------------------------------------------"
	@echo "Testing Rev2 owner password"
	@echo "---------------------------------------------"
	@echo ""
	@echo "Generative method"
	@echo "-----------------"
	./pdfcrack -o -c Rkz573 -n 6 -m 6 ./testpdfs/TestPDF5.pdf
	@echo ""
	@echo "Wordlist method"
	@echo "---------------"
	./pdfcrack -o -w ./testpdfs/testpasswordslist_small.txt ./testpdfs/TestPDF5.pdf
	@echo ""
	@echo "Pattern method"
	@echo "--------------"
	./pdfcrack -o -t 5 -e [Rkz][Rkz][z][Rkz1234567890][127][123456] ./testpdfs/TestPDF5.pdf


perftest5: pdfcrack unzip
	@echo ""
	@echo "================================================"
	@echo "Performance testing Rev2 owner password"
	@echo "================================================"
	@echo ""
	@echo "Generative method"
	@echo "================="
	./pdfcrack -o -c Rkz1234567890abcdefijl -n 6 -m 6 ./testpdfs/TestPDF5.pdf
	@echo ""
	@echo "Wordlist method"
	@echo "================="
	./pdfcrack -o -w ./testpdfs/testpasswordslist_huge.txt ./testpdfs/TestPDF5.pdf
	@echo ""
	@echo "Pattern method"
	@echo "=============="
	./pdfcrack -o -t 5 -e [Rkz1234567890abcdefijl][Rkz1234567890abcdefijl][Rkz1234567890abcdefijl][Rkz1234567890abcdefijl][Rkz1234567890abcdefijl][Rkz1234567890abcdefijl] ./testpdfs/TestPDF5.pdf



test6: pdfcrack
	@echo ""
	@echo "---------------------------------------------"
	@echo "Testing Rev5 (AES 256) user password"
	@echo "---------------------------------------------"
	@echo ""
	@echo "Generative method"
	@echo "-----------------"
	./pdfcrack -c Mnv731 -n 6 -m 6 ./testpdfs/TestPDF6.pdf
	@echo ""
	@echo "Wordlist method"
	@echo "---------------"
	./pdfcrack -w ./testpdfs/testpasswordslist_small.txt ./testpdfs/TestPDF6.pdf
	@echo ""
	@echo "Pattern method"
	@echo "--------------"
	./pdfcrack -t 5 -e [Mnv][Mnv][v][Mnv1234567890][731][731245] ./testpdfs/TestPDF6.pdf	


perftest6: pdfcrack unzip
	@echo ""
	@echo "================================================"
	@echo "Performance testing Rev5 (AES 256) user password"
	@echo "================================================"
	@echo ""
	@echo "Generative method"
	@echo "================="
	./pdfcrack -c mnopqrstuvwxyz1234567890M -n 6 -m 6 ./testpdfs/TestPDF6.pdf
	@echo ""
	@echo "Wordlist method"
	@echo "================="
	./pdfcrack -w ./testpdfs/testpasswordslist_huge.txt ./testpdfs/TestPDF6.pdf
	@echo ""
	@echo "Pattern method"
	@echo "=============="
	./pdfcrack -t 5 -e [mnopqrstuvwxyz1234567890M][mnopqrstuvwxyz1234567890M][mnopqrstuvwxyz1234567890M][mnopqrstuvwxyz1234567890M][mnopqrstuvwxyz1234567890M][mnopqrstuvwxyz1234567890M] ./testpdfs/TestPDF6.pdf	



test7: pdfcrack
	@echo ""
	@echo "---------------------------------------------"
	@echo "Testing Rev5 (AES 256) owner password"
	@echo "---------------------------------------------"
	@echo ""
	@echo "Generative method"
	@echo "-----------------"
	./pdfcrack -o -c pasword -n 8 -m 8 ./testpdfs/TestPDF6.pdf
	@echo ""
	@echo "Wordlist method"
	@echo "---------------"
	./pdfcrack -o -w ./testpdfs/testpasswordslist_small.txt ./testpdfs/TestPDF6.pdf
	@echo ""
	@echo "Pattern method"
	@echo "--------------"
	./pdfcrack -o -t 5 -e [paswo][paswo][pas][s][wor1234567890][sword][sword][d] ./testpdfs/TestPDF6.pdf	


perftest7: pdfcrack unzip
	@echo ""
	@echo "================================================"
	@echo "Performance testing Rev5 (AES 256) owner password"
	@echo "================================================"
	@echo ""
	@echo "Generative method"
	@echo "================="
	./pdfcrack -o -c paswor123d -n 8 -m 8 ./testpdfs/TestPDF6.pdf
	@echo ""
	@echo "Wordlist method"
	@echo "================="
	./pdfcrack -o -w ./testpdfs/testpasswordslist_huge.txt ./testpdfs/TestPDF6.pdf
	@echo ""
	@echo "Pattern method"
	@echo "=============="
	./pdfcrack -o -t 5 -e [paswor123d][paswor123d][paswor123d][paswor123d][paswor123d][paswor123d][paswor123d][paswor123d] ./testpdfs/TestPDF6.pdf	



testrange: pdfcrack
	@echo ""
	@echo "---------------------------------------------"
	@echo "Testing Ranges (using Rev5 user password)"
	@echo "---------------------------------------------"
	@echo ""
	@echo "Test 1: Min, Max"
	@echo "----------------"
	./pdfcrack -t 5 -e "{2,4}[Mnv][Mnv1234567890][731][731245]" ./testpdfs/TestPDF6.pdf	
	@echo ""
	@echo "Test 2: No min, Max"
	@echo "-------------------"
	./pdfcrack -t 5 -e "[Mnv][Mnv]{,5}[Mnv1234567890]" ./testpdfs/TestPDF6.pdf	
	@echo ""
	@echo "Test 3: Min=Max"
	@echo "---------------"
	./pdfcrack -t 5 -e "[Mnv][Mnv]{3,3}[Mnv1234567890][731245]" ./testpdfs/TestPDF6.pdf	
	@echo ""
	@echo "Test 4: Perf test, single range"
	@echo "-------------------------------"
	./pdfcrack -t 5 -e "{4,8}[Mnv1234567890]" ./testpdfs/TestPDF6.pdf	
	

	
testmultirange: pdfcrack
	@echo ""
	@echo "---------------------------------------------"
	@echo "Testing Multiple Ranges (using Rev5 user password)"
	@echo "---------------------------------------------"
	@echo ""
	@echo "Test 1: Multiple ranges"
	@echo "-----------------------"
	./pdfcrack -t 5 -e "{2,4}[Mnv]{1,3}[Mnv1234567890]" ./testpdfs/TestPDF6.pdf	
	@echo ""
	@echo "Test 2: Perf test, multiple ranges"
	@echo "----------------------------------"
	./pdfcrack -t 5 -e "{2,4}[Mnv]{,2}[Mnv1234567890]{3,4}[1234567890]" ./testpdfs/TestPDF6.pdf	
	

testcharclass: pdfcrack
	@echo ""
	@echo "----------------------------------------------------"
	@echo "Testing Character Classes (using Rev2 user password)"
	@echo "----------------------------------------------------"
	@echo ""
	@echo "Test 1: Single class (:any:)"
	@echo "----------------------------"
	./pdfcrack -t 5 -e "[Hdo][:any:][o][25^][25^][25^]" ./testpdfs/TestPDF7.pdf	
	@echo ""
	@echo "Test 2: Multiple classes"
	@echo "------------------------"
	./pdfcrack -t 5 -e "[:upper:][:alpha:][:lower:][:alnum:]][:digit:][:punct:]" ./testpdfs/TestPDF7.pdf	
	@echo ""
	@echo "Test 3: Multiple classes, with ranges"
	@echo "-------------------------------------"
	./pdfcrack -t 5 -e "[:upper:]{1,2}[:lower:]{2,3}[:digit:][:punct:]" ./testpdfs/TestPDF7.pdf	
	


debugold: pdfcrack
	gdb --args ./pdfcrack -c Mnv731 -n 6 -m 6 ./testpdfs/TestPDF6.pdf
	
debugnew: pdfcrack
	gdb --args ./pdfcrack -t 1 -e [Mnv][Mnv][v][Mnv1234567890][731][731245] ./testpdfs/TestPDF6.pdf


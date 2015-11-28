/**
 * Copyright (C) 2006-2014 Henning Norén
 * 
 * Multi-core (OpenMP) and pattern support by 
 *       Shreepad Shukla, Copyright (C) 2015
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, 
 * USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>
#include <time.h>
#include "pdfparser.h"
#include "pdfcrack.h"
#include "benchmark.h"
#include "passwords.h"


#define PRINTERVAL 20 /** Print Progress Interval (seconds) */
#define CRASHFILE "savedstate.sav"
#define VERSION_MAJOR 0
#define VERSION_MINOR 14

#define _FILE_OFFSET_BITS 64

/** alarmInterrupt is used to print out the progress at specific intervals */
static void
alarmInterrupt() {
  if(!printProgress())
    alarm(PRINTERVAL);
}

/** autoSave is used to save down the current state when interrupted */
__attribute__ ((noreturn)) static void
autoSave(int sig) {
  FILE *file;

  if(sig)
    fprintf(stderr,"Caught signal %d!\nTrying to save state...\n",sig);
  if((file = fopen(CRASHFILE, "w")) == 0) {
    fprintf(stderr,"Could not open %s for writing\n", CRASHFILE);
  }
  else{
    saveState(file);
    fclose(file);
    fprintf(stderr,"Successfully saved state to %s!\n",CRASHFILE);
  }
  exit(sig);
}

/** print some help for the user */
static void
printHelp(char *progname) {
  printf("Usage: %s -f filename [OPTIONS]\n"
	 "OPTIONS:\n"
	 "-b, --bench\t\tperform benchmark and exit\n"
	 "-c, --charset=STRING\tUse the characters in STRING as charset\n"
	 "-e, --pattern=STRING\tUse the pattern in STRING\n"
	 "-w, --wordlist=FILE\tUse FILE as source of passwords to try\n"
	 "-n, --minpw=INTEGER\tSkip trying passwords shorter than this\n"
	 "-m, --maxpw=INTEGER\tStop when reaching this passwordlength\n"
	 "-l, --loadState=FILE\tContinue from the state saved in FILENAME\n"
	 "-o, --owner\t\tWork with the ownerpassword\n"
	 "-u, --user\t\tWork with the userpassword (default)\n"
	 "-p, --password=STRING\tGive userpassword to speed up breaking\n"
	 "\t\t\townerpassword (implies -o)\n"
	 "-q, --quiet\t\tRun quietly\n"
	 "-s, --permutate\t\tTry permutating the passwords (currently only\n"
	 "\t\t\tsupports switching first character to uppercase)\n"
	 "-t, --threads=INTEGER\tUse these many threads\n"
	 "-v, --version\t\tPrint version and exit\n",
	 progname);
}

int
main(int argc, char** argv) {
  int ret = 0, minpw = 0, maxpw = 32;
  struct sigaction act1, act2;
  FILE *file = NULL, *wordlist = NULL;
  bool recovery = false, quiet = false, 
    work_with_user = true, permutation = false;
  uint8_t *userpassword = NULL;
  char *charset = NULL, *inputfile = NULL, *wordlistfile = NULL;
  char *pattern = NULL;
  int numthreads = 1;
  EncData *e;

  /** Parse arguments */
  while(true) {
    int c, option_index;
    static struct option long_options[] = {
      {"bench",    no_argument      , 0, 'b'},
      {"charset",  required_argument, 0, 'c'},
      {"pattern",  required_argument, 0, 'e'},
      {"file",     required_argument, 0, 'f'},
      {"loadState",required_argument, 0, 'l'},
      {"maxpw",    required_argument, 0, 'm'},
      {"minpw",    required_argument, 0, 'n'},
      {"owner",    no_argument      , 0, 'o'},
      {"password", required_argument, 0, 'p'},
      {"quiet",    required_argument, 0, 'q'},     
      {"permutate",no_argument,       0, 's'},
      {"threads",  required_argument, 0, 't'},
      {"user",     no_argument      , 0, 'u'},
      {"wordlist", required_argument, 0, 'w'},
      {"version",  no_argument      , 0, 'v'},     
      {0, 0, 0, 0}};
    /* getopt_long stores the option index here. */
    option_index = 0;
     
    c = getopt_long(argc, argv, "bc:e:f:l:m:n:op:qst:uw:v",
			long_options, &option_index);
    
    /* Detect the end of the options. */
    if(c == -1)
      break;
    
    switch(c) {
    case 'b':
      runBenchmark();
      return 0;
    case 'c':
      if(charset)
	fprintf(stderr,"Charset already set\n");
      else
	charset = strdup(optarg);
      break;
      
    case 'e':
      if(pattern)
	fprintf(stderr,"Pattern already set\n");
      else
	pattern = strdup(optarg);
      break;
    
    case 'f':
      if(!recovery)
	inputfile = strdup(optarg);
      break;

    case 'l':
      if(inputfile) {
	free(inputfile);
	inputfile = NULL;
      }
      inputfile = strdup(optarg);
      recovery = true;
      break;
	
    case 'm':
      maxpw = atoi(optarg);
      break;
     
    case 'n':
      minpw = atoi(optarg);
      break;

    case 'o':
      work_with_user = false;
      break;

    case 'p':
      userpassword = (uint8_t*)strdup(optarg);
      work_with_user = false;
      break;

    case 'q':
      quiet = true;
      break;

    case 'u':
      if(!work_with_user) {
	fprintf(stderr, "Cannot work with both user- and owner-password\n");
	exit(1);
      }
      work_with_user = true;
      break;

    case 's':
      permutation = true;
      break;

    case 't':
      numthreads = atoi(optarg);
      if ((numthreads < 1)  ||  (numthreads > omp_get_max_threads()))
      {
      	numthreads = omp_get_max_threads();
      	printf("Invalid thread number, set to %i\n", numthreads);
      }
      break;

    case 'w':
      if(wordlistfile)
	fprintf(stderr, "Wordlist already set\n");
      else
	wordlistfile = strdup(optarg);
      break;

    case 'v':
      printf("pdfcrack version %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
      return 0;

    default:
      printHelp(argv[0]);
      ret = 1;
      goto out3;
    }
  }

  /** If there are any unhandled arguments and the filename and/or wordlist is
      not set: try to match the first as the filename and the second as 
      wordlist.
  */
  {
    int i = optind;
    if(i > 0) {
      if(i < argc && !inputfile)
	inputfile = strdup(argv[i++]);
      if(i < argc && !wordlistfile)
	wordlistfile = strdup(argv[i++]);
      if(i < argc)
	while(i<argc) {
	  fprintf(stderr,"Non-option argument %s\n", argv[i++]);
	}
    }
  }

  if(!inputfile || minpw < 0 || maxpw < 0) {
    printHelp(argv[0]);
    ret = 1;
    goto out3;
  }

  if((file = fopen(inputfile, "rb")) == 0) {
    fprintf(stderr,"Error: file %s not found\n", inputfile);
    ret = 2;
    goto out3;
  }

  e = calloc(1,sizeof(EncData));

  if(recovery) {
    if(wordlistfile) {
      free(wordlistfile);
      wordlistfile = NULL;
    }
    if(!loadState(file, e, &wordlistfile, &work_with_user)) {
      fprintf(stderr, "Error: Not a savefile or savefile is damaged\n");
      ret = 3;
      goto out1;
    }

    if(!quiet)
      printf("Loaded state for %s\n", inputfile);
  }
  else { /** !recovery */
    if(!openPDF(file,e)) {
      fprintf(stderr, "Error: Not a valid PDF\n");
      ret = 3;
      goto out1;
    }

    ret = getEncryptedInfo(file, e);
    if(ret) {
      if(ret == EENCNF) 
	fprintf(stderr, "Error: Could not extract encryption information\n");
      else if(ret == ETRANF || ret == ETRENF || ret == ETRINF) {
	fprintf(stderr, "Error: Encryption not detected (is the document password protected?)\n");
	ret = 4;
	goto out1;
      }
    }
    else if(e->revision < 2 || (strcmp(e->s_handler,"Standard") != 0 || e->revision > 5)) {
      fprintf(stderr, "The specific version is not supported (%s - %d)\n", e->s_handler, e->revision);
      ret = 5;
      goto out1;
      }
  }

  if(fclose(file)) {
    fprintf(stderr, "Error: closing file %s\n", inputfile);
  }

  if(minpw > maxpw) {
    fprintf(stderr, "Warning: minimum pw-length bigger than max\n");
  }

  if(wordlistfile) {
    if((wordlist = fopen(wordlistfile, "r")) == 0) {
      fprintf(stderr,"Error: file %s not found\n", wordlistfile);
      ret = 6;
      goto out2;
    }
  }
  
  /*
  // Check that pattern is not longer than max password length
  
  if (pattern)
  {
  	int patLen = strlen(pattern);
  	if (patLen > PASSLENGTH)
  	{
	      fprintf(stderr,"Error: Pattern %s longer than max password length %i\n", pattern, PASSLENGTH);
	      ret = 8;
	      goto out2;
  	}
  }
  */

  passwordMethod pmethod;
  
  // Pattern method also supports wordlist
  if (pattern)
  	pmethod = Pattern;
  else if (wordlistfile)
  	pmethod = Wordlist;
  else
  	pmethod = Generative;


  act2.sa_handler = autoSave;
  sigfillset(&act2.sa_mask);
  act2.sa_flags = 0;
  sigaction(SIGINT, &act2, 0);


  if(!quiet) {
    printEncData(e);
    printf("Password method %i\n", pmethod);
    
    act1.sa_handler = alarmInterrupt;
    sigemptyset(&act1.sa_mask);
    act1.sa_flags = 0;
    sigaction(SIGALRM, &act1, 0);
    alarm(PRINTERVAL);
  }
  


  /** Try to initialize the cracking-engine */
  if(!initPDFCrack(e, userpassword, work_with_user, wordlistfile,
		   pmethod, wordlist, charset, pattern,
		   (unsigned int)minpw, (unsigned int)maxpw, permutation, quiet, numthreads)) {
    cleanPDFCrack();
    fprintf(stderr, "Wrong userpassword, '%s'\n", userpassword);
    ret = 7;
    goto out2;
  }
  
  // Test the pattern password generator
  /*
  if (pattern)
  {
	  uint8_t password[PASSLENGTH+1];
	  
	  for (unsigned long long int i=0; i<100000L; i++)
	  {
	  	if (getPatternPassword(i, password))
		  	printf("Password %lli: %s\n", i, password);
	  }
  }
  */
  
  printf("On to the crack\n");
  fflush(stdout);
  

  /** Do the actual crunching */
  time_t starttime = time(NULL);
  
  runCrack();
  
  time_t endtime = time(NULL);
  double runtimeinsecs = difftime(endtime, starttime);
  printf("Time taken (secs): %f \n", runtimeinsecs);


  /** cleanup */
  cleanPDFCrack();
  freeEncData(e);

  if(wordlistfile) {
    fclose(wordlist);
    free(wordlistfile);
  }
  if(inputfile)
    free(inputfile);
  if(charset)
    free(charset);
  if(userpassword)
    free(userpassword);
    
  if(pattern)
    free(pattern);

  return 0;

 out1:
  if(fclose(file))
    fprintf(stderr, "Error: closing file %s\n", inputfile);
 out2:
  freeEncData(e);
 out3:
  if(inputfile)
    free(inputfile);
  if(charset)
    free(charset);
  if(userpassword)
    free(userpassword);

  if(pattern)
    free(pattern);

  exit(ret);
}

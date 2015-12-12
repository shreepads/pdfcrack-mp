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

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "passwords.h"
#include "pattern.h"


static FILE *wordList = NULL;
static const char *wordListName;
static bool wlMore;
static bool (*npw)() = NULL;
static unsigned int (*spw)(uint8_t *outbuf) = NULL;
static passwordMethod pwMethod;

static int numberthreads = 1;

bool
nextPassword() { return npw(); }

unsigned int
setPassword(uint8_t *outbuf) { return spw(outbuf); }

static bool
wlNextPassword() { return wlMore; }

static unsigned int 
wlSetPassword(uint8_t *outbuf) {
  int ch;
  unsigned int passlength;

  ch = 0;
  passlength = 0;

  ch = getc(wordList);
  while(ch != '\n' && ch != '\r' && ch != EOF && passlength < 32) {	
    outbuf[passlength++] = ch;
    ch = getc(wordList);
  }

  /** clean up garbage of passwords longed than 32 chars */
  if(unlikely(passlength == 32))
    while(ch != '\n' && ch != '\r' && ch != EOF)
      ch = getc(wordList);

  if(ch == '\r') {
    ch = getc(wordList);
    if(ch != '\n')
      ungetc(ch, wordList);
  }
  if(unlikely(ch == EOF))
    wlMore = false;

  return passlength; 

}

static void
setWordList(FILE *file, const char *wl) {
  wordList = file;
  wordListName = wl;
  npw = &wlNextPassword;
  spw = &wlSetPassword;
  wlMore = true;
}

static const uint8_t
stdchars[] = {"abcdefghijklmnopqrstuvwxyz"
	      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	      "0123456789"};

static const uint8_t *charset;
static unsigned int charsetLen;
static unsigned int maxPasswordLen;
static int password[PASSLENGTH];

static unsigned int
genSetPassword(uint8_t *outbuf) {
  unsigned int i;

  for(i=0;password[i] != -1;i++)
    outbuf[i] = charset[password[i]];

  return i;
}


static bool
genNextPassword() {
  unsigned int i = 0;

  /** this is pretty simple...
      Change the current position (i) to the value in the next position of the
      charset.
      If we have reached the end of the charset, move the current position to 
      the next one and return true unless we have reached the last position we
      want to try.
  */
  while(++password[i] == (int)charsetLen)
    password[i++] = 0;
 
  return (i != maxPasswordLen);
}

static bool recovery = false;

static void
setCharset(const char *cs, const unsigned int minPw, 
	   const unsigned int maxPw) {
  int i;
  unsigned int min;

  npw = &genNextPassword;
  spw = &genSetPassword;

  if(!recovery) {
    /** This should already be set if we are loading from a saved state */
    if(cs)
      charset = (const uint8_t*)cs;
    else
      charset = stdchars;
    charsetLen = strlen((const char*)charset);

    /** Make sure that max- and min-password are smaller than 32 */
    if(maxPw < PASSLENGTH)
      maxPasswordLen = maxPw;
    else
      maxPasswordLen = PASSLENGTH-1;
    if(minPw < PASSLENGTH)
      min = minPw;
    else
      min = PASSLENGTH-1;

    /** Initialize starting position */
    for(i=0;i<(int)maxPasswordLen;i++) {
      if(i<((int)min)-1)
	password[i] = charsetLen-1;
      else
	password[i] = -1;
    }
    while(i < PASSLENGTH-1)
      password[i++] = -1;
  }
  /** Put terminator (-1) at the last position */
  password[PASSLENGTH-1] = -1;
}

// Static variables and functions for patterns - uses pattern.h

static const char *pattern;
static unsigned int patternLen;

static uint8_t* passwordPatternArray[PASSLENGTH];
static unsigned int passwordPatternLengths[PASSLENGTH];
static unsigned long long int passwordPatternDivs[PASSLENGTH];
static unsigned long long int maxPatternPasswords;

static uint8_t patternWordlistCache[PATTWORDLISTCACHESIZE][PASSLENGTH+1];
static unsigned long long int patternWordlistCacheStartIndex;
static unsigned long long int patternWordlistSize;
static unsigned long long int patternWordlistDiv;
static int patternWordClassIndex;

static void setPattern(const char *pat, FILE *file, const char *wl)
{
	// Parse the pattern into basic [] format
	pattern = parsePattern(pat);
	patternLen = patternLength(pattern);
	
	// Set the wordlist and wordlistname
	wordList = file;
	wordListName = wl;
	patternWordlistCacheStartIndex = 0LL;
	patternWordlistSize = 0LL;
	patternWordlistDiv = 1LL;
	
	if (wordList)
		patternWordlistSize = setPatternWordlistCache(patternWordlistCache, wordList, wordListName);
	
	//passwordPatternArray = malloc(patternLen * sizeof(uint8_t*));
	//passwordPatternLengths = malloc(patternLen * sizeof(unsigned int));
	
	// Convert basic format pattern into password pattern array	
	patternWordClassIndex = setPatternArray(pattern, patternLen, passwordPatternArray, passwordPatternLengths, passwordPatternDivs, patternWordlistSize);
	
	if (patternWordlistSize != 0LL)
	{ // There is a :word: pattern and the wordlist is not empty
		if (patternLen != 0)
		{
			patternWordlistDiv = passwordPatternDivs[patternLen-1]*passwordPatternLengths[patternLen-1];

		}
		else
		{ // Single :word: pattern
			patternWordlistDiv = 1;
		}
		
		maxPatternPasswords = patternWordlistSize * patternWordlistDiv;
	}
	else
	{ // There isn't a :word: pattern
		maxPatternPasswords = passwordPatternDivs[patternLen-1]*passwordPatternLengths[patternLen-1];
	}
	
	//maxPatternPasswords = 0LL;
	
	for (unsigned int i=0; i<patternLen; i++)
	{	
		// Calculate max pattern passwords
		// maxPatternPasswords += passwordPatternDivs[i]*passwordPatternLengths[i];
		
		printf("Pattern element %i: Value: %s, Length: %i, Divisor: %lli\n", 
			i, passwordPatternArray[i], passwordPatternLengths[i], passwordPatternDivs[i]);
	}
	
	printf("Wordlist Index: %i, Length: %llu, Divisor: %llu\n", 
		patternWordClassIndex, patternWordlistSize, patternWordlistDiv);	
	
	printf("Max pattern passwords: %lli\n", maxPatternPasswords);
	
	return;
}


// Utility function to get maximum number of password patterns
unsigned long long int getMaxPatternPasswords()
{
	return maxPatternPasswords;
}


// Core function to return the ith password from the password pattern array and its length

int getPatternPassword(long long int n, uint8_t* patPassword)
{
	int patternPasswordLength = 0;
	long long int in_n = n;
	
	if (!patPassword)
		return patternPasswordLength;
		
	if (!pattern)
		return patternPasswordLength;
		
	if (n >= maxPatternPasswords)
		return patternPasswordLength;
	
	// Temp stuff - to be deleted
	for (int i=0; i<PASSLENGTH-1; i++)
		patPassword[i] = 'A' + (i % 26);
	
	patPassword[PASSLENGTH-1] = '\0';
	
	
	int pos = patternLen-1;
	int wordlen = 0;
	uint8_t *word = 0;
	unsigned long long int wordIndex = 0;
	
	// If applicable, get the word index and retrieve from cache, resetting the start index if necessary
	if (patternWordlistSize != 0LL)
	{
		wordIndex = n / patternWordlistDiv;
		
		patternWordlistCacheStartIndex = 
			getWordFromCache(patternWordlistCache, wordList, patternWordlistCacheStartIndex, 
				wordIndex, &word);
				
		n = n % patternWordlistDiv;
	}
	
	// Set default pos for where the word is to be inserted
	int posAtWordIndex = 0;
	
	if (patternWordlistSize != 0LL)
		if (patternLen-1 < patternWordClassIndex)
			posAtWordIndex = patternLen;

	
	// Build the rest of the pattern password
	for (int i=patternLen-1; i>=0; i--)
	{
		
		if (patternWordlistSize != 0LL)
			if (i == patternWordClassIndex)
				posAtWordIndex = pos;

		int index = n / passwordPatternDivs[i];

		if (passwordPatternArray[i][index] != OPTPATCHAR)
		{
			patPassword[pos] = passwordPatternArray[i][index];
			pos--;
		}
		
		n = n % passwordPatternDivs[i];
	
	}
	
	// Munge the word into the built pattern	
	if (word)
	{
		wordlen = strlen((const char*) word);
		
		if (wordlen < 1)
			return patternPasswordLength;
			
		if (wordlen + patternLen > PASSLENGTH)
		{
			printf("Word %s at index %llu is too long\n", 
				word, wordIndex);
			return patternPasswordLength;
		}
		
		// Munge word into patPassword while adjusting pos
		//printf("\nMunging word %s into %s at index %i; patternLen is %i pos is %i, posAtWordIndex is %i\n", 
		//	word, patPassword, patternWordClassIndex, patternLen, pos, posAtWordIndex);
		
		// Move patPassword filled to the right of right to fit in the word
		//memmove(patPassword+pos+1+wordlen+patternWordClassIndex, 
		//	patPassword+pos+1+patternWordClassIndex, patternLen-pos-1);
		
		memmove(patPassword+wordlen+posAtWordIndex, 
			patPassword+posAtWordIndex, patternLen-posAtWordIndex);
		
		//printf("Moved patPassword %s, of length %i\n", patPassword, patternLen-posAtWordIndex);
		
		//memcpy(patPassword+pos+1+patternWordClassIndex, word, wordlen);
		memcpy(patPassword+posAtWordIndex, word, wordlen);
		
		//printf("Copied word into patPassword %s, pos is %i\n", patPassword, pos);

	}
	

	patternPasswordLength = patternLen+wordlen-pos-1;
	
	//printf("prePatPassword:%s; pos:%i; length:%i\n", patPassword, pos, patternPasswordLength);
	
	if (pos != -1)
	{
		// Need to move the pattern password to the left to handle optional char
		memmove(patPassword, patPassword+pos+1, patternPasswordLength);
	}
	
	patPassword[patternPasswordLength] = '\0';
	
	//printf("Returning password %s at %lli of length %i\n", patPassword, in_n, patternPasswordLength);
	
	return patternPasswordLength;
}


void
initPasswords(const passwordMethod pm, FILE *file, const char *wl,
	      const char *cs, const char* pat, const unsigned int minPw,
	      const unsigned int maxPw, const bool qt, int numthreads) {
  if(!recovery)
    pwMethod = pm;

  numberthreads = numthreads;
  
  switch(pwMethod) {
  case Generative:
    setCharset(cs, minPw, maxPw);
    break;
  case Wordlist:
    setWordList(file, wl);
    break;
  
  case Pattern:
    setPattern(pat, file, wl);
    break;
    
  default:
    /** The programmer is a twit!  */
    break;
  }
}

/** Common patterns that is shared between pw_loadState and pw_saveState */
static const char string_PM[] = "\nPM: %d\n";
static const char string_MPCLC[] = "MaxPWL: %d\nCharset(%d): ";

bool
pw_loadState(FILE *file, char **wl) {
  int pm;
  unsigned int i, len;
  char * __restrict string;

  if(fscanf(file, string_PM, &pm) < 1)
    return false;
  if(pm == Generative) {
    if(fscanf(file, string_MPCLC, &maxPasswordLen, &charsetLen) < 2)
      return false;

    /** check for extremely long charsets */
    if(charsetLen > 256)
      return false;

    string = malloc(sizeof(uint8_t)*charsetLen+1);
    for(i=0;i<charsetLen;i++)
      string[i] = getc(file);
    string[i] = '\0';
    charset = (uint8_t*)string;
    
    /** get the linebreak */
    getc(file);

    for(i=0;i<PASSLENGTH-1;i++)
      if(fscanf(file, " %d", &password[i]) < 1) {
	free(string);
	return false;
      }
  }
  else if(pm == Wordlist) {
    if(fscanf(file, "Wordlist(%d): ", &len) < 1)
      return false;
    string = malloc(sizeof(char)*len+1);
    if(fscanf(file, "%[^\n]\n", string) < 1) {
	free(string);
	return false;
    }
    string[len] = '\0';
    *wl = string;
    wordListName = string;
  }
  pwMethod = pm;
  recovery = true;

  return true;
}

void
pw_saveState(FILE *file) {
  unsigned int i;
  fprintf(file, string_PM, pwMethod);
  if(pwMethod == Generative) {
    fprintf(file, string_MPCLC, maxPasswordLen, charsetLen);
    fprintf(file, "%s\n", charset);
    for(i=0;i<PASSLENGTH-1;i++)
      fprintf(file," %d", password[i]);
  }
  else if(pwMethod == Wordlist) {    
    fprintf(file, "Wordlist(%zu): %s", strlen(wordListName), wordListName);
  }
  fprintf(file,"\n");
}

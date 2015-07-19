/**
 * Copyright (C) 2015 Shreepad Shukla
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


// static variables and functions to handle password patterns

/*
static const uint8_t
iso88591chars[] = {"abcdefghijklmnopqrstuvwxyz"
	      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	      "0123456789"};
static const char* anyliteral = ":any:";

static const uint8_t alnumchars[] = {"A-Za-z0-9"};
static const char* alnumliteral = ":alnum:";

static const uint8_t alphachars[] = {"A-Za-z"};
static const char* alphaliteral = ":alpha:";

static const uint8_t digitchars[] = {"0-9"};
static const char* digitliteral = ":digit:";

static const uint8_t lowerchars[] = {"a-z"};
static const char* lowerliteral = ":lower:";

static const uint8_t upperchars[] = {"A-Z"};
static const char* upperliteral = ":upper:";

static const uint8_t punctchars[] = {"][!\"#$%&'()*+,./:;<=>?@\\^_`{|}~-"};
static const char* punctliteral = ":punct:";

static const uint8_t spacechars[] = {" \t\r\n\v\f"};
static const char* spaceliteral = ":space:";

*/

// Validate pattern

int validPattern(const char* pat)
{
	if (!pat)
		return 0;
		
	return 1;
}


// Calculate length of pattern string - only to be used after complete parsing is done

unsigned int patternLength(const char* pat)
{
	if (!pat)
		return 0;

	unsigned int i;
	
	for (i=0; pat[i]; pat[i] == '[' ? i++ : *pat++);
	
	return i;
}


// Take the pattern, validate it and return a string with all possible chars indiviudally listed within []

char* parsePattern(const char* pat)
{
	char* parsedPattern = NULL;
	
	if (!pat)
		return NULL;
				
	// Check the pattern is valid
	if (!validPattern(pat))
		return NULL;
	
	// Replace repeated occurences
	//uint8_t* parsedPattern = 
	
	// Replace types
	
	
	// Replace NOT operator
	
	// Replace ranges
	
	parsedPattern = pat;
	
	return parsedPattern;
}


// Turn a parsed patern string into an array of password chars and associated lengths of each
void setPatternArray(const uint8_t* pat, unsigned int patLen, 
	uint8_t* passwordPatArray[], unsigned int passwordPatLengths[], unsigned long long int passwordPatDivs[])
{
	printf("Parsed pattern: %s, length: %i\n", (const char *)pat, patLen);
	
	for (unsigned long long int i=0; i<patLen; i++)
	{
		/* Simple test
		if (i<4)
			passwordPatArray[i] = "abkr";
		else
			passwordPatArray[i] = "369";
		*/
		
		// Equiv test
		passwordPatArray[i] = "bark936";
		
		 passwordPatLengths[i] = strlen((char *) passwordPatArray[i]);
		 
		 if (i==0)
		 	passwordPatDivs[i]=1LL;
		 else
		 	passwordPatDivs[i]=passwordPatLengths[i]*passwordPatDivs[i-1];
	}
	
	return;
	
}


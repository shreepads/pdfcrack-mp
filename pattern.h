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

#include "passwords.h"

// Char used to indicate optional pattern char value
#define OPTPATCHAR '\t'
 
char* parsePattern(const char* pat);
 
unsigned int patternLength(const char* pattern);

int setPatternArray(const char* pat, unsigned int patLen,  
		uint8_t* passwordPatArray[], unsigned int passwordPatLengths[], unsigned long long int passwordPatDivs[],
		unsigned long long int patternWordlistSize);

unsigned long long int setPatternWordlistCache(uint8_t patternWordlistCache[][PASSLENGTH+1], FILE *wordListFile, const char *wordListName);

unsigned long long int getWordFromCache(uint8_t patternWordlistCache[][PASSLENGTH+1], FILE *wordListFile, unsigned long long int patternWordlistCacheStartIndex, unsigned long long int wordIndex, char **word);


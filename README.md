# pdfcrack-mp

## Introduction

OpenMP update with patterns for pdfcrack.

This is a fork of the pdfcrack project which adds patterns with OpenMP based multi-core support. All user and owner password methods found in pdfcrack are supported.

There are also a number of test PDF files in the `testpdfs` folder and various tests included in the `Makefile`. These tests cover both the original pdfcrack functionality and the new functionality introduced by pdfcrack-mp.

See https://github.com/shreepads/pdfcrack-mp/issues for the list of bugs, enhancements.

## Branches

* master - will always be kept in sync with the latest pdfcrack source.
* baseopenmp - contains the stable version of pdfcrack-mp with all changes over pdfcrack
* other branches - for specific issues found in the issue log

Please note that I have no plans to multi-core enable the standard Generative/ Wordlist methods found in pdfcrack, but aim to cover these as part of the Pattern method.
* Generative method: baseopemp branch contains support for ranges that cover Generative method functionality - see Issue #4
* Wordlist method: Still to be done - see Issue # 7

## How to Use

At the moment there are no ready builds and I assume some level of familiarity with make.

Briefly
* Open the terminal and go to a directory where you want to build pdfcrack-mp
* `git clone https://github.com/shreepads/pdfcrack-mp.git`
* `git checkout baseopenmp`
* `make`
* `make testall` to get an idea of the options supported

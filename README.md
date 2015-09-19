# pdfcrack-mp
OpenMP update with patterns for pdfcrack

This is a fork of the pdfcrack project which adds patterns with OpenMP based multi-core support. All user and owner password methods found in pdfcrack are supported.

Branches
* master - will always be kept in sync with the latest pdfcrack source.
* baseopenmp - contains the stable version of pdfcrack-mp with all changes over pdfcrack
* other branches - for specific issues found in the issue log

Please note that I have no plans to multi-core enable the standard Generative/ Wordlist methods found in pdfcrack, but aim to cover these as part of the Pattern method.
* Generative method covered: Single range in Pattern method done in regexrange branch
* Wordlist method: See Issue # 7

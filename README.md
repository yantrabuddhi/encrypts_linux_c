encrypts_linux_c
================
A command line tool to encrypt characters by sequence shuffling. Does not store password, and can encrypt any type of file text or binary. uses tar and gzip for compression

Building
========
Run make

Install
=======
put folder containing executable in path

encryption
==========
encrypts e password file-or-directory output-encrypted-file

decryption
==========
encrypts d password encrypted-file

limitations
===========
reads and writes a character at a time from file(is very raw and un-optimized), so slow for large files unless ramdisk or ssd is used. 

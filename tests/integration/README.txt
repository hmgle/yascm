SIMPLE INTEGRATION TESTS

Requirement:
Linux, Unix, Mac OS

How to run tests:
cd to this directory and run ./test-all

How to write new tests:
./test-all will search for any file with extenstion ".in" as the input in all subdirectories.
It runs ../../yascm with the input, then compares the output with a file of the same basename 
but with the ".out" extenstion under the same directory.

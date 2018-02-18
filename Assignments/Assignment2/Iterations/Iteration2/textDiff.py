#!/usr/bin/env python

# difflib_test

import difflib
import sys


file1 = open(sys.argv[1], 'r') #this is my output
file2 = open(sys.argv[2], 'r') #this is the actual output

file1lines = file1.readline()
file2lines = file2.readline()

file1.close()
file2.close()

string1 = ""
string2 = ""
print(len(file1lines))


for i in range(len(file1lines)):
	if (i%90 ==0 ):
		string1 += '\n'
		string2 += '\n'
	string1 += file1lines[i]
	string2 += file2lines[i]
	

textfile1 = open('Myoutput.txt', "w")
textfile2 = open('ActualOutput.txt', "w")
textfile1.write(string1)
textfile2.write(string2)

textfile1.close()
textfile2.close()
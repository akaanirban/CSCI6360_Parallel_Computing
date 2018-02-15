#!/usr/bin/env python

# difflib_test

import difflib

file1 = open('op2.txt', 'r')
file2 = open('Output/test_output_7.txt', 'r')

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
	else:
		string1 += file1lines[i]
		string2 += file2lines[i]

textfile1 = open('Comparison/op.txt', "w")
textfile2 = open('Comparison/res.txt', "w")
textfile1.write(string1)
textfile2.write(string2)

textfile1.close()
textfile2.close()
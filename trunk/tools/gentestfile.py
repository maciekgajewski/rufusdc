#!/usr/bin/env python
# genrates test file of any length

import sys

# check input params
if len(sys.argv) < 3:
	print "Usage: gentestfile SIZE FILENAME"
	exit()

size = int(sys.argv[1])
filename = sys.argv[2]

# open file
f = file( filename, 'w' )


for c in xrange(size):
	num = c%100 # 256 would be bad. It could hide missing or diplicate block od size N*256
	f.write( chr( num ) )
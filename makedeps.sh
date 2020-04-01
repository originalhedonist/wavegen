#!/bin/bash

for f in $1/*.cpp; do
	objfile=obj/$1/$(basename $f .cpp).o
	g++ -MM $f -MT $objfile
	echo -e '\tg++ $(CXXFLAGS) '$f -c -o $objfile
	echo ''
done


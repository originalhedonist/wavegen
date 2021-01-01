#!/bin/bash

for f in $1/*.cpp; do
	objfile=obj/$1/$(basename $f .cpp).o
	g++-10 -MM $f -MT $objfile
	echo -e '\t$(CXX) $(CXXFLAGS) '$f -c -o $objfile
	echo ''
done


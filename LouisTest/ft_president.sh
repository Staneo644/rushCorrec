#!/bin/bash

if [ ! -f "ft_president" ]; then
	make || exit
fi

fileIn=$1
fileOut=$2
count=$3

if [ ! -f $1 ]; then
	echo "$1 does not exist."
	exit
fi

if [ $count -le 0 ]; then
	echo "region count cannot be less or equal to 0"
	exit
fi

./ft_president $count < $fileIn > $fileOut

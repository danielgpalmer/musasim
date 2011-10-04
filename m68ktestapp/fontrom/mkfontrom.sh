#!/bin/bash

CHARS="A B C D E F G H I J K L M N O P Q R S T U V W X Y Z";

rm -f rom.bin;

for CHAR in $CHARS; do
	gm convert -size 8x16 -pointsize 12 -draw "text 0,10 $CHAR" null:'rgb(255,255,255)' mono:$CHAR.tmp
	cat $CHAR.tmp >> rom.bin
done;


rm *.tmp;

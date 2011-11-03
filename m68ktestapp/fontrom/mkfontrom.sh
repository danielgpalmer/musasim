#!/bin/bash

#set -x;
set -e;

CHARS="! \" # \$ % & ' ( ) _ + , - . /";
CHARS="${CHARS} 0 1 2 3 4 5 6 7 8 9 \: \; < = > ? @ ";
CHARS="${CHARS} A B C D E F G H I J K L M N O P Q R S T U V W X Y Z";
CHARS="${CHARS} [ \\ ] ^ _ \`";
CHARS="${CHARS} a b c d e f g h i j k l m n o p q r s t u v w x y z";
CHARS="${CHARS} { | } ~";

if [ ! -x ./bin2c ]; then
	gcc -o bin2c bin2c.c;
fi

rm -f fontrom.bin;


BYTES=0;
for CHAR in ${CHARS}; do
	gm convert -size 8x16 -pointsize 12 -draw "text 0,10 \"${CHAR}\"" null:'rgb(255,255,255)' mono:"char.tmp"
	cat "char.tmp" >> "fontrom.bin"
	BYTES=$(($BYTES + 16));
done;

rm *.tmp;

./bin2c fontrom.bin fontrom fontrom

ACTUALBYTES=`stat --printf=%s fontrom.bin`;

if [ $BYTES -ne $ACTUALBYTES ]; then
	echo "!!Result should be $BYTES bytes but is $ACTUALBYTES!!";
fi;

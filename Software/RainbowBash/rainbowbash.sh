#!/bin/sh

stty -F /dev/ttyUSB0 38400

R=$1
G=$2
B=$3

rawframe="\\x1b\\x02"

for c in $R $G $B
do
	if [ $c -eq 27 ]; then
		rawframe="${rawframe}\\x1b"
	fi

	rawframe="${rawframe}\\x$(printf '%02x' $c)"
done

rawframe="${rawframe}\\x00"
rawframe="${rawframe}\\x1b\\x03"

echo -en "$rawframe"

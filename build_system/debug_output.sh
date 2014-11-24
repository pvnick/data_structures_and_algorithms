#!/bin/bash

while read line; do
	echo "$line" | c++filt
	if [[ "$line" == ./main* ]]; then
		ADDRESS=$(echo $line | sed -e 's/.*\[\(.*\)\]/\1/')
		SOURCE_FILE_LINE_NUMBER=$(addr2line -e main $ADDRESS)
		SOURCE_FILE=$(echo "$SOURCE_FILE_LINE_NUMBER" | cut -d":" -f1)
		SOURCE_LINE_NUMBER=$(echo "$SOURCE_FILE_LINE_NUMBER" | cut -d":" -f2 | cut -d" " -f1)
		re='^[0-9]+$'
		if [[ $SOURCE_LINE_NUMBER =~ $re ]] ; then
			echo -n "   "
			echo -ne "\e[31m "
			echo $SOURCE_FILE_LINE_NUMBER
			SOURCE_LINE=$(sed ''$SOURCE_LINE_NUMBER'q;d' $SOURCE_FILE)
			echo -n "   "
			echo -ne "\e[43m\e[30m "
			echo -n "$SOURCE_LINE" | sed -e 's/^[ \t]*//' -e 's/[ \t]*$//'
			echo -ne " \e[0m"
			echo ""
		fi
	fi	
done

#!/bin/bash

invoke_pandoc() {
	INPUT_FILE="$1"
	OUTPUT_FILE="$2"
	echo pandoc --include-in-header $CURR_DIR/pandocheader.tex "$INPUT_FILE" -o "$OUTPUT_FILE"
	pandoc --include-in-header $CURR_DIR/pandocheader.tex "$INPUT_FILE" -o "$OUTPUT_FILE"
}

compile_pdf() {
	MD_FILE="$1"
	OUTPUT_DIR_BASE="$2"
	STRIPPED_EXTENSION="${MD_FILE%.md}"
	MD_FILE_DIR=$(dirname "$MD_FILE")
	OUTPUT_DIR="$OUTPUT_DIR_BASE/$MD_FILE_DIR"
	OUTPUT_PATH="$OUTPUT_DIR_BASE/$STRIPPED_EXTENSION.pdf"
	INPUT_ABSOLUTE=$(readlink -f $MD_FILE)
	echo -n "Compiling $INPUT_ABSOLUTE to $OUTPUT_PATH... "
	mkdir -p $OUTPUT_DIR
	echo $OUTPUT_DIR
	[[ $? != 0 ]] && echo "fail" && echo "Unable to create output directory." && exit
	invoke_pandoc "$INPUT_ABSOLUTE" "$OUTPUT_PATH"
	[[ $? != 0 ]] && echo "fail" && echo "Unable to create pdf." && exit
	echo "done"
}

compile_file() {
	FILE="$1"
	OUTPUT_DIR_BASE="$2"

	case "$FILE" in
	*".md")
		compile_pdf "$FILE" "$OUTPUT_DIR_BASE"
		;;
	*)
		STRIPPED_EXTENSION="${MD_FILE%.md}"
		FILE_DIR=$(dirname "$FILE")
		OUTPUT_DIR="$OUTPUT_DIR_BASE/$FILE_DIR"
		OUTPUT_PATH="$OUTPUT_DIR_BASE/$FILE"
		echo -n "Copying $FILE to $OUTPUT_PATH... "
		mkdir -p "$OUTPUT_DIR"
		[[ $? != 0 ]] && echo "fail" && echo "Unable to create output directory." && exit
		cp $FILE "$OUTPUT_DIR"
		[[ $? != 0 ]] && echo "fail" && echo "Unable to copy file." && exit
		echo "done"
		;;
	esac
}

compile_physical_deliverable() {
	DIST_DIR=$(readlink -f "$1")/
	DIST_DIR_ESCAPED="${DIST_DIR//\//\\/}"
	cp pandocheader.tex /tmp #physical deliverable latex file needs this in its directory
	echo $DIST_DIR_ESCAPED
	cat "$CURR_DIR/physical_deliverable.tex" \
		| sed 's/\\importdeliverable{/\\importdeliverable{'$DIST_DIR_ESCAPED'/g' > /tmp/physical_deliverable_compiled.tex
	echo "Compiling physical deliverable"
	pdflatex "\newcommand\inputincludepath{$DIST_DIR}\input{/tmp/physical_deliverable_compiled.tex}"
	[[ $? != 0 ]] && echo "fail" && echo "Unable to create pdf." && exit
	echo "done"
}

compile_and_run_tests() {
	local SOURCE_DIR="$1"
	echo "Compiling and running tests at $SOURCE_DIR"
	local MY_DIR=$(pwd)
	cd "$SOURCE_DIR"
	./compile.sh
	./unit_tester -s > output.txt
	cd old_tests
	./compile.sh
	./fuzzer | head -n 100000 > output.txt
	cd "$MY_DIR"
}

echo "Compiling project"
CURR_DIR=$(pwd)
DIST_DIR=$CURR_DIR"/dist/project_1"
rm -r $DIST_DIR 2> /dev/null
mkdir -p $DIST_DIR
cd input
#compile_and_run_tests "ssll/source" 
#compile_and_run_tests "cdal/source" 
#compile_and_run_tests "psll/source" 
#compile_and_run_tests "sdal/source" 
FILES="$(find . -name '*.?*')"
IFS=$'\n'
for FILE in $FILES; do
	compile_file "$FILE" "$DIST_DIR"
done
cd $CURR_DIR
compile_physical_deliverable "$DIST_DIR"
echo "Success"

echo "\n\n\n\nWarning: Make sure to reenable test compile/running before turning in assignment"

#this file should be included in the testcase runners using the source built-in command

RESULTS_DIR="./results"
ensure_file_dir_exists() {
	FILENAME="$1"
	DIR=""$(dirname "$FILENAME")""
	mkdir -p "$DIR"
}

function count_keys {
    typeset INPUT_FILE="$1"
    RET=$(cat "$INPUT_FILE" | wc -l)
}
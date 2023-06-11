#!/bin/bash

# Get the directory of the script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Get the parent directory of the script directory
PARENT_DIR="$(dirname "$DIR")"

echo "The script is located in $DIR"
echo "The parent directory is $PARENT_DIR"

docker build -t vya-final-project "$PARENT_DIR"
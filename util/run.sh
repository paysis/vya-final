#!/bin/bash

# Get the directory of the script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Get the parent directory of the script directory
PARENT_DIR="$(dirname "$DIR")"

targetyaml=$1
echo $targetyaml
docker run --rm -it -v "$PARENT_DIR/config":/app/shared vya-final-project "../shared/$targetyaml"
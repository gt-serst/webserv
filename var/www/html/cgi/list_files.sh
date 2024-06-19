#!/bin/bash

UPLOAD_DIR="/Users/geraudtserstevens/Desktop/42cursus/webserv/var/www/html/upload"


if [ ! -d "$UPLOAD_DIR" ]; then
    echo '{"files":[]}'
    exit 0
fi

files=($(ls -1 "$UPLOAD_DIR"))

echo '{"files":['

for i in "${!files[@]}"; do
    if [[ $i -gt 0 ]]; then
        echo ','
    fi
    printf "\"${files[$i]}\""
done

echo ']}'

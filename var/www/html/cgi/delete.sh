#!/bin/bash

# Define the allowed base directory substring
BASE_DIR="/var/www/html/"

# Get the file path from PATH_INFO
FILE_PATH=$PATH_INFO

# Check if the file path is provided
if [ -z "$FILE_PATH" ]; then
  echo "Status: 400 Bad Request"
  echo "Content-Type: text/plain"
  echo
  echo "Error: PATH_INFO is not set."
  exit 1
fi

# Normalize the file path
FILE_PATH=$(realpath -m "$FILE_PATH")

# Check if the file path contains the allowed directory
if [[ "$FILE_PATH" != *"$BASE_DIR"* ]]; then
  echo "Status: 403 Forbidden"
  echo "Content-Type: text/plain"
  echo
  echo "Error: Access to the specified path is not allowed."
  exit 1
fi

# Check if the file exists
if [ ! -f "$FILE_PATH" ]; then
  echo "Status: 404 Not Found"
  echo "Content-Type: text/plain"
  echo
  echo "Error: File not found."
  exit 1
fi

# Check if the file is writable
if [ ! -w "$FILE_PATH" ]; then
  echo "Status: 403 Forbidden"
  echo "Content-Type: text/plain"
  echo
  echo "Error: Permission denied."
  exit 1
fi

# Try to remove the file
if rm "$FILE_PATH"; then
  echo "Status: 200 OK"
  echo "Content-Type: text/plain"
  echo
  echo "Success: File deleted."
else
  echo "Status: 500 Internal Server Error"
  echo "Content-Type: text/plain"
  echo
  echo "Error: Could not delete the file."
fi


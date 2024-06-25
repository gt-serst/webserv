#!/usr/bin/env python3

import os
import json
from urllib.parse import parse_qs

def main():
    # Print the content type header
    print("Content-Type: application/json")
    print()

    try:
        # Parse the query string from the environment variable
        query_string = os.environ.get('QUERY_STRING', '')
        params = parse_qs(query_string)

        # Extract multiplicands from query parameters
        multiplicand1 = float(params.get('multiplicand1', [None])[0])
        multiplicand2 = float(params.get('multiplicand2', [None])[0])
        
        if multiplicand1 is None or multiplicand2 is None:
            raise ValueError("Missing parameters")

        # Perform multiplication
        result = multiplicand1 * multiplicand2

        # Create response
        response = {
            "result": result
        }
        print(json.dumps(response))
    except (TypeError, ValueError) as e:
        # Handle errors in input
        response = {
            "error": "Invalid or missing parameters"
        }
        print(json.dumps(response))

if __name__ == "__main__":
    main()


from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib.parse
import html

class MultiplyHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # Parse query parameters
        parsed_path = urllib.parse.urlparse(self.path)
        query_params = urllib.parse.parse_qs(parsed_path.query)

        try:
            # Extract multiplicands
            multiplicand1 = query_params.get('multiplicand1', [None])[0]
            multiplicand2 = query_params.get('multiplicand2', [None])[0]

            if multiplicand1 is None or multiplicand2 is None:
                raise ValueError("Missing multiplicands")

            # Convert multiplicands to integers
            multiplicand1 = int(multiplicand1)
            multiplicand2 = int(multiplicand2)

            # Perform the multiplication
            result = multiplicand1 * multiplicand2

            # Return the result in an HTML response
            self.send_response(200)
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write(f"""
            <html>
            <head><title>Multiplication Result</title></head>
            <body>
            <h1>Multiplication Result</h1>
            <p>{html.escape(str(multiplicand1))} * {html.escape(str(multiplicand2))} = {html.escape(str(result))}</p>
            </body>
            </html>
            """.encode())

        except OverflowError:
            # Handle overflow errors
            self.send_response(500)
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write("""
            <html>
            <head><title>Error</title></head>
            <body>
            <h1>Overflow Error</h1>
            <p>The multiplication resulted in an overflow error.</p>
            </body>
            </html>
            """.encode())
        
        except ValueError as e:
            # Handle value errors
            self.send_response(400)
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write(f"""
            <html>
            <head><title>Error</title></head>
            <body>
            <h1>Value Error</h1>
            <p>{html.escape(str(e))}</p>
            </body>
            </html>
            """.encode())
        
        except Exception as e:
            # Handle any other errors
            self.send_response(500)
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write(f"""
            <html>
            <head><title>Error</title></head>
            <body>
            <h1>Internal Server Error</h1>
            <p>{html.escape(str(e))}</p>
            </body>
            </html>
            """.encode())

def run(server_class=HTTPServer, handler_class=MultiplyHandler, port=8000):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print(f'Starting httpd server on port {port}')
    httpd.serve_forever()

if __name__ == "__main__":
    run()


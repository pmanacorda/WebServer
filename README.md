# WebServer
From-Scratch implementation of a web server in C++

# Build, Run, Debug
-> Ubuntu 24.04.2 LTS
sudo apt install gcc
sudo apt-get install gdb
sudo apt install libssl-dev
openssl req -x509 -newkey rsa:4096 -keyout server.key -out server.crt -days 365 -nodes
make local
touch credentials.json
echo '{"Username": "Patrick", "Password": "Manacorda"}' | jq > credentials.json
curl -sSL -X POST https://localhost:4430/api/login -H "Content-Type: application/json" -d @credentials.json -k -v 2>&1 | grep -i 'Set-Cookie' | cut -d= -f2 | cut -d';' -f1 > cookie.txt
cat cookie.txt
curl -sSL -X GET https://localhost:4430/api/about -H "Cookie: session_token=$(cat cookie.txt)" -k | jq 


# Add-Listener-Socket
This feature adds IPv4 socket configuration using Linux kernel functions:
- sys/socket.h::socket() to generate the socket file descriptor for this process
- sys/socket.h::setsockopt() to configure the socket options for reuse address and connection keep alive
- sys/socket.h::bind() to bind the socket to a given port
- sys/socket.h::listen() to set the socket in listen state
- sys/socket.h::accept() to accept incoming client connections
sudo lsof -n -i :80 | grep LISTEN
main    3465 patrickmanacorda    3u  IPv4 0xafe05602b6b5b277      0t0  TCP *:http (LISTEN)

# Add-HTTP1.1-Parser
This feature reads and parses HTTP1.1 raw input bytes from the client socket:
- sys/socket.h::read() to get bytes from socket descriptor into a temporary buffer
- continue reading until header termination `\r\n\r\n` is received
- built-in protections against oversized headers (8KB limit)
- parse first line which reveals method, path, query parameters and HTTP version
- parse headers after first line until termination sequence as Key:Value pairs
- if Content-Length is present continue reading from socket until body length matches expected bytes
- supports GET, POST, and DELETE methods with proper error handling
- custom HttpRequest structure stores the parsed result with method, path, headers, parameters, and body

# Add-Flat-Json-Parser
This feature adds parsing for flat JSON objects in HTTP request bodies:
- parseJson() function processes JSON strings into key-value pairs
- supports flat objects: {"key": "value", "key2": 200, "key3": "value3"}
- supports flat arrays of objects: [{"key": "value"}, {"key2": "value2"}, {"key3": "value3"}]
- uses character-by-character parsing with switch statement for tokenization
- automatically triggered when Content-Type header is "application/json"
- integrates seamlessly with existing HTTP parser for POST request bodies
- lightweight implementation without external JSON library dependencies

# Add-Controllers
This feature registers request handlers for matching HTTP paths:
- uses a std::unordered_map to associate path strings with handler functions
- handlers receive HttpRequest and HttpResponse references as parameters
- controller functions populate the response directly with content and status
- after execution, the HttpResponse is serialized and sent back to the client in JSON format
- supports easy extension by adding new routes and handler logic

# Add-SSL
This feature adds HTTPS support using OpenSSL TLS:
Generate self-signed cert and key with
openssl req -x509 -newkey rsa:4096 -keyout server.key -out server.crt -days 365 -nodes
- Initializes SSL context with TLS_server_method()
- Loads cert and private key into context
- Disables insecure TLS/SSL versions and sets secure cipher list
- Performs TLS handshake on client accept
- Uses SSL_read/SSL_write to handle encrypted communication
- Cleans up SSL objects on socket close

# Add-Static-Content
This feature serves static files (HTML, CSS, JavaScript) directly from the filesystem:
- Controller reads files using std::ifstream and serves them based on request path
- Automatic MIME type detection sets appropriate Content-Type headers (text/html, text/css, application/javascript)
- Supports standard web assets including HTML pages, stylesheets, and client-side scripts
- Configurable static file directory path for organized web content structure

# Add-Multithreading
This feature adds concurrent request handling using C++ threading with connection limits:
- Each accepted client connection spawns a dedicated std::thread for parallel processing
- Atomic counter (std::atomic<uint16_t>) tracks active thread count to prevent resource exhaustion
- Configurable maximum thread limit prevents server overload
- Thread count increments before spawn and decrements automatically when request processing completes
- Connections exceeding the thread limit are gracefully rejected by allowing the ClientSocket destructor to close the connection
- Uses std::thread::detach() for fire-and-forget execution, eliminating need for thread cleanup
- Move semantics efficiently transfer ClientSocket ownership to worker threads without copying
- Provides horizontal scaling for handling multiple simultaneous client requests

# Add-Connection-Keep-Alive
This feature adds support for persistent HTTP connections using the Connection: Keep-Alive header as defined in HTTP/1.1:
- Parses and respects the Connection header in incoming client requests
- If Connection: keep-alive is requested, the server maintains the same TCP connection across multiple HTTP requests
- If the client does not request keep-alive, or explicitly sets Connection: close, the server shuts down the connection after the response is sent.
- Implements socket-level timeout protection using setsockopt() with SO_RCVTIMEO, ensuring the server thread doesn't hang if a client goes idle without closing.
- Also enables TCP-level keep-alive using SO_KEEPALIVE to detect broken connections (e.g. client unexpectedly disconnected) and clean up server resources.

# Add-GitHub-Actions-CI/CD
This feature automates the build and deployment process using GitHub Actions self-hosted runners:
- Triggers on push events to specified branches for continuous integration
- Performs automated compilation with g++ using C++17 standard and proper OpenSSL linking
- Safely cleans up previous deployments and replaces the running webserver binary

# Add-AWS-CloudFormation-Infrastructure
This feature provisions secure AWS infrastructure using Infrastructure as Code principles:
- Creates isolated VPC with custom CIDR block (10.0.0.0/16) and dedicated public subnet
- Configures Internet Gateway with proper routing for external web traffic access
- Implements restrictive security groups allowing only SSH from specific IP and HTTP/HTTPS from internet
- Provisions EC2 t2.nano instance with Amazon Linux 2023 AMI for cost-effective hosting
- Includes IAM role configuration for fine-grained permission control and security isolation

# Add-Authentication
This feature adds user authentication:
- Leverages HTTP Cookies to store session tokens in client browser
- Sets HttpOnly and Secure flags to enhance security and prevent spoofing
- Adds automatic redirects upon successful login

# Add-Authorization
This features adds protected routes that require valid cookie:
- Protected routes check for valid cookie and return 401 if unauthorized
- Custom dependency injection resolver adds auth service to login controller
- Cookie session token is validated for both existence and timestamp validity
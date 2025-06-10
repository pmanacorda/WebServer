# WebServer
From-Scratch implementation of a web server in C++
g++ -std=c++17 -Isrc/core/headers -Isrc/controllers/headers main.cpp src/core/implementation/*.cpp src/controllers/implementation/* -o main

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


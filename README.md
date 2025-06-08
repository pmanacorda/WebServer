# WebServer
From-Scratch implementation of a web server in C++

# Add-Listener-Socket
This feature adds IPv4 socket configuration using Linux kernel functions:
- sys/socket.h::socket() to generate the socket file descriptor for this process
- sys/socket.h::setsockopt() to configure the socket options for reuse address and connection keep alive
- sys/socket.h::bind() to bind the socket to a given port
- sys/socket.h::listen() to set the socket in listen state
- sys/socket.h::accept() to accept incoming client connections
sudo lsof -n -i :80 | grep LISTEN
main    3465 patrickmanacorda    3u  IPv4 0xafe05602b6b5b277      0t0  TCP *:http (LISTEN)
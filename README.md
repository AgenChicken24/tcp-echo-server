# tcp-echo-server
A simple linux tcp echo server written in C

# Instructions

1. Build the socket.c file with any C compiler of your choice
2. Run the output.
3. The socket is bind to all interfaces on port 6000.
4. Avaible commands are: `\q` to end the connection and `\s` for statistics for the current connection

You can use the following command `socat STDIO TCP4:127.0.0.1:6000` to connect to it using the socat package in linux.

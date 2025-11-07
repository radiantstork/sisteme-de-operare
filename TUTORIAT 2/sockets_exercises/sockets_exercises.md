# Sockets exercises

1. Create a socket based communication between two processes(P1-client, P2-server) on the same machine, with the following flow.
P1 connects to P2 (TCP connection)
P1 sends a message to P2 and waits for a response
P2 receives the message and sends back the response (any message you want)
P1 receives the response
Useful syscall: socket, bind, connect, listen, accept, send/write, recv/read
**Solution**: in client.c and server.c
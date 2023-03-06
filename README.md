# TCP-Server-Client-Program-for-Prime-Factorization-of-Positive-Integers-in-C

The program is a network client-server application. 

The server listens on a particular port and waits for a client to connect. 

When a client connects, the server sends a message to the client requesting a positive integer. 
The client sends a positive integer back to the server, and the server responds with the next integer in the sequence, starting from the integer that the client sent. 

If the client sends a non-positive integer or sends something other than an integer, the server responds with an error message.

The code represents a client-server communication where the client requests a sequence number from the server. 

The client sends a request to the server with a sequence length and receives a sequence number in response. 
The client is implemented in the Client.c file, while the server is implemented in the Server.c file.

The client code follows the steps below:

    Parse the command-line arguments to obtain the server hostname and the sequence length.
    Call getaddrinfo() to obtain a list of addresses that can be used to connect to the server.
    Walk through the list of addresses and try to connect to the server using each address until a connection succeeds.
    Send the sequence length to the server.
    Read the sequence number returned by the server.

The is_seqnum.h header file contains some definitions used by both the client and server, including the port number used for the communication.

The server code follows the steps below:

    Parse the command-line arguments to obtain the initial sequence number.
    Call getaddrinfo() to obtain a list of addresses that can be used to bind a socket to.
    Walk through the list of addresses and try to bind a socket to each address until a successful bind is achieved.
    Call listen() to listen for incoming connections on the bound socket.
    Accept incoming connections and handle them.
    Generate a sequence number and send it to the client.
    Close the connection with the client.

The is_seqnum.h header file contains some definitions used by both the client and server, including the port number used for the communication.

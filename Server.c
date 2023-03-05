#include <netdb.h>
#include "is_seqnum.h"

#define BACKLOG 50

int main(int argc, char *argv[])
{
    uint32_t seqNum;
    char IntStr[INT_LEN]; /* Length of requested sequence */
    struct sockaddr_storage claddr;
    int lfd, cfd, optval, InputNumber;
    socklen_t addrlen;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
    char addrStr[ADDRSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [init-seq-num]\n", argv[0]);

    seqNum = (argc > 1) ? getInt(argv[1], 0, "init-seq-num") : 0;

    /* Ignore the SIGPIPE signal, so that we find out about broken connection
       errors via a failure from write(). */

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        errExit("signal");

    /* Call getaddrinfo() to obtain a list of addresses that
       we can try binding to */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC; /* Allows IPv4 or IPv6 */
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
    /* Wildcard IP address; service name is numeric */

    if (getaddrinfo(NULL, PORT_NUM, &hints, &result) != 0)
        errExit("getaddrinfo");

    /* Walk through returned list until we find an address structure
       that can be used to successfully create and bind a socket */

    optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (lfd == -1)
            exit(1); /* On error, try next address */

        if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
            errExit("setsockopt");

        if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break; /* Success */

        /* bind() failed: close this socket and try next address */

        close(lfd);
    }

    if (rp == NULL)
        fatal("Could not bind socket to any address");

    if (listen(lfd, BACKLOG) == -1)
        errExit("listen");

    freeaddrinfo(result);

    for (;;)
    { /* Handle clients iteratively */

        /* Accept a client connection, obtaining client's address */

        addrlen = sizeof(struct sockaddr_storage);
        printf("Ready to accept\n");

        cfd = accept(lfd, (struct sockaddr *)&claddr, &addrlen);
        if (cfd == -1)
        {
            errMsg("accept");
            exit(1);
        }

        printf("After accept\n");

        if (getnameinfo((struct sockaddr *)&claddr, addrlen,
                        host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
            snprintf(addrStr, ADDRSTRLEN, "(%s, %s)", host, service);
        else
            snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");
        printf("Connection from %s\n", addrStr);


        if (fork() == 0)
        {
            //child process

            char * first_message = "Give me a positive integer\n";

            if (write(cfd, first_message, strlen(first_message)) != strlen(first_message))
                fprintf(stderr, "Error on write");


            /* Read client request, send sequence number back */

            if (readLine(cfd, IntStr, INT_LEN) <= 0)
            {
                close(cfd);
                exit(1); /* Failed read; skip request */
            }

            InputNumber = atoi(IntStr);
            if (InputNumber <= 0)
            { /* Watch for misbehaving clients */
                char * error_msg = "Invalid number\n";
                if (write(cfd, error_msg, strlen(error_msg)) != strlen(error_msg))
                    fprintf(stderr, "Error on write");
                
                close(cfd);
                exit(1); /* Bad request; skip it */
            }

            
            //Calculate prime factors of reqLen
            
            int p = 2;
            while (InputNumber >= p * p) 
            {
                if (InputNumber % p == 0)
                {
                    printf("Will send %d\n", p);
                    snprintf(IntStr, INT_LEN, "%d,", p);
                    if (write(cfd, IntStr, strlen(IntStr)) != strlen(IntStr))
                       fprintf(stderr, "Error on write");
                    InputNumber = InputNumber / p;
                }
                else
                {
                    p++;
                }
            }


            printf("Will send %d\n", InputNumber);

            snprintf(IntStr, INT_LEN, "%d\n", InputNumber);
            if (write(cfd, IntStr, strlen(IntStr)) != strlen(IntStr))
                fprintf(stderr, "Error on write");

            printf("All ok, %s\n", IntStr);
            seqNum += InputNumber; /* Update sequence number */

            if (close(cfd) == -1) /* Close connection */
                errMsg("close");

            exit(0);

        }

        if (close(cfd) == -1) /* Close connection */
            errMsg("close");       
        
    }

    int status;
    while (wait(status) > 0)
        ;
}


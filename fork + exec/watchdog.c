#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h> // gettimeofday()
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>

#define SERVER_PORT 3000
#define FILE_SIZE 5
#define TIMEOUT 10

int main()
{

    // Open the listening socket , using the IPv4 and TCP protocol
    int listeningsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listeningsocket == -1)
    {
        perror("Could not create socket");
        return -1;
    }

    int enableReuse = 1;
    int ret = setsockopt(listeningsocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        printf("setsockopt() failed with error code : %d", errno);
        return 1;
    }

    //Creating sockaddr_in struct, reset it, and enter important values.
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;

    //INADDR_ANY - means any IP can connect.
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);

    //Bind the socket to the port with any IP at this port
    int bindResult = bind(listeningsocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1)
    {
        perror("Bind failed");
        close(listeningsocket);
        return -1;
    }

    //Make the socket listening, 50 is the size of queue connection requests
    int listenResult = listen(listeningsocket, 50);
    if (listenResult == -1)
    {
        perror("listen didn't work");
        close(listeningsocket);
        return -1;
    }
    printf("Waiting for new ping connection...\n");

    ////////////////////////////////////////////////////////////////////////////////////

    //Build a struct for the client
    struct sockaddr_in clientAddress; //
    socklen_t clientAddressLen = sizeof(clientAddress);
    memset(&clientAddress, 0, sizeof(clientAddress));
    clientAddressLen = sizeof(clientAddress);

    //The receiver will accept requests.
    int client_socket = accept(listeningsocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
    if (client_socket == -1)
    {
        perror("accept didn't work\n");
        close(listeningsocket);
        return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////

    int checknum = 0;

    while (checknum != 100)
    {
        int bytes_received = recv(client_socket, &checknum, sizeof(checknum),MSG_DONTWAIT);
        if(checknum == 100)
        {
            printf("The time has start\n");
        }
    }

    struct timeval start1, end;
    gettimeofday(&start1, 0);
    gettimeofday(&end, 0);
    checknum = 0;
    time_t start = time(NULL);  // Start time
    int elapsed_time;  // Elapsed time
    int x = 1;
    while(x == 1){
        checknum = 0;
        while (checknum != 100)
        {
            int bytes_received = recv(client_socket, &checknum, sizeof(checknum),MSG_DONTWAIT);
            if(checknum == 100){
                printf("The time has reset\n");
                // Reset the start time and elapsed time
                start = time(NULL);
                elapsed_time = 0;
            }
            
            elapsed_time = time(NULL) - start;
            if (elapsed_time > TIMEOUT) {
                printf("server <ip> cannot be reached.\n");
                x = 0;
                break;
            }
        }
    }

    //Terminate watchdog and better_ping
    kill(0, SIGINT);
    return 0;
}
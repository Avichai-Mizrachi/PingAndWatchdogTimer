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

#define IP4_HDRLEN 20  // IPv4 header len without options
#define ICMP_HDRLEN 8  // ICMP header len for echo req

unsigned short calculate_checksum(unsigned short *paddress, int len);

int icmppack(char *packet, int seq)
{
    struct icmp icmphdr; // ICMP-header
    char data[IP_MAXPACKET] = "This is the ping.\n";
    int datalen = strlen(data) + 1;
    icmphdr.icmp_type = ICMP_ECHO; // Message Type (8 bits): ICMP_ECHO_REQUEST
    icmphdr.icmp_code = 0;         // Message Code (8 bits): echo request
    icmphdr.icmp_id = 18;          // Identifier (16 bits): some number to trace the response.
    icmphdr.icmp_seq = seq;        // Sequence Number (16 bits): starts at 0
    icmphdr.icmp_cksum = 0;        // ICMP header checksum (16 bits): set to 0 not to include into checksum calculation
    memcpy((packet), &icmphdr, ICMP_HDRLEN); // add ICMP header to packet
    memcpy(packet + ICMP_HDRLEN, data, datalen); // add ICMP data to packet
    icmphdr.icmp_cksum = calculate_checksum((unsigned short *)(packet), ICMP_HDRLEN + datalen); // calculate checksum
    memcpy((packet), &icmphdr, ICMP_HDRLEN);

    return ICMP_HDRLEN + datalen;
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "Invalid command.\n");
        exit(1);
    }
    
    char packet[IP_MAXPACKET];               // packet to send
    char IP[INET_ADDRSTRLEN];                // Holds IPv4 string
    strcpy(IP, argv[1]);                     // Copy the ip to the string
    struct in_addr addr;                     // IPv4 address
    if (inet_pton(AF_INET, IP, &addr) != 1)  // Convert IPv4 addresses from text to binary
    {
        printf("Invalid ip-address\n");
        exit(1);
    }

    printf("Ping %s:\n", IP);

    struct sockaddr_in dest_in;
    memset(&dest_in, 0, sizeof(struct sockaddr_in));
    dest_in.sin_family = AF_INET;
    dest_in.sin_addr.s_addr = inet_addr(IP);
    int sock = -1; // socket file descriptor
    if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
    {
        fprintf(stderr, "socket() failed with error: %d", errno);
        fprintf(stderr, "To create a raw socket, the process needs to be run by Admin/root user.\n\n");
        exit(1);
    }
    
    int num = 0;      // number of packets to send

    while (true)
    {
        int lenOfPacket= icmppack(packet, num); // create icmp packet
        struct timeval start, end;
        gettimeofday(&start, 0);

        int bytes_sent = sendto(sock, packet, lenOfPacket, 0, (struct sockaddr *)&dest_in, sizeof(dest_in));
        if (bytes_sent == -1)
        {
            fprintf(stderr, "sendto() failed with error: %d", errno);
            return -1;
        }

        // Get the ping response
        bzero(packet, IP_MAXPACKET); // clear packet
        socklen_t len = sizeof(dest_in);
        ssize_t bytes_received = -1;
        struct iphdr *iphdr;
        struct icmphdr *icmphdr;
        while ((bytes_received = recvfrom(sock, packet, sizeof(packet), 0, (struct sockaddr *)&dest_in, &len)))
        {
            if (bytes_received > 0)
            {

                iphdr = (struct iphdr *)packet;
                icmphdr = (struct icmphdr *)(packet + (iphdr->ihl * 4));
                inet_ntop(AF_INET, &(iphdr->saddr), IP, INET_ADDRSTRLEN);
                break;
            }
        }

        gettimeofday(&end, 0);
        char reply[IP_MAXPACKET];
        memcpy(reply, packet + ICMP_HDRLEN + IP4_HDRLEN, lenOfPacket - ICMP_HDRLEN); // get reply data from packet
        float time = (end.tv_sec - start.tv_sec) * 1000.0f + (end.tv_usec - start.tv_usec) / 1000.0f;
        //printf("   response %ld bytes from %s: icmp_seq: %d ttl = %d time: %0.3fms\n", bytes_received, IP, icmphdr->un.echo.sequence,
        //iphdr->ttl,  time);
        printf("%s sent %ld bytes, TTL: %d, ICMP sequence: %d, Time: %.5f ms\n", IP, bytes_received, icmphdr->un.echo.sequence, iphdr->ttl, time);
        num++;
        bzero(packet, IP_MAXPACKET);
        sleep(2);    //  There is 2 seconds delay between each package.
    }
    close(sock);
    return 0;
}

// Compute checksum (RFC 1071).
unsigned short calculate_checksum(unsigned short *paddress, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = paddress;
    unsigned short answer = 0;

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1)
    {
        *((unsigned char *)&answer) = *((unsigned char *)w);
        sum += answer;
    }

    // add back carry outs from top 16 bits to low 16 bits
    sum = (sum >> 16) + (sum & 0xffff); // add hi 16 to low 16
    sum += (sum >> 16);                 // add carry
    answer = ~sum;                      // truncate to 16 bits

    return answer;
}

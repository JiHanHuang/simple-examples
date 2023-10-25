/*具体参考笔记说明*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
#define _PACKED_1_ __attribute__((__packed__))

#define IP "127.0.0.1"
#define PORT 2123

#define MAXDATASIZE 1024

void send_udp(UINT8 *buf, UINT16 buf_len)
{
    int sockfd, num;
    struct sockaddr_in server, peer;

    if((sockfd=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("socket() error\n");
        exit(1);
    }
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    server.sin_addr.s_addr = inet_addr(IP);
    if(connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("connect() error.\n");
        exit(1);
    }

    send(sockfd, buf, buf_len, 0);

    close(sockfd);
}

typedef struct gtpc_msg_header_s {
//#if __BYTE_ORDER == __BIG_ENDIAN
  UINT8 version : 3;
  UINT8 piggyback_flg : 1;
  UINT8 teid_flg : 1;
  UINT8 spare : 3;
// #else
//   UINT8 spare : 3;
//   UINT8 teid_flg : 1;
//   UINT8 piggyback_flg : 1;
//   UINT8 version : 3;
// #endif
  UINT8 msg_type;
  UINT16
      msg_len; /* 不包含header的前4字节固定字段，但是要包含TEID以及以后的长度 */
  UINT32 seq_only; /* seq : 31bit ~ 8bit, spare : 7bit ~ 0bit */
} _PACKED_1_ gtpc_msg_header_t;

static void printX(UINT8 *a, UINT32 len) {
    int i = 0;
    printf("Len:%d\n", len);
    printf("printX: ");
    for (i = 0; i < len; i++) {
        printf("\\x%02x", a[i]);
    }
    printf("\n");
}


typedef     union {
    uint32_t my_int;
    uint8_t  my_bytes[4];
} endian_tester;

int main() {

endian_tester et;
et.my_int = 0x0a0b0c0d;
if(et.my_bytes[0] == 0x0a )
    printf( "I'm on a big-endian system\n" );
else
    printf( "I'm on a little-endian system\n" );

    gtpc_msg_header_t header;
    header.version = 3;
    header.piggyback_flg = 0;
    header.teid_flg = 1;
    header.spare = 2;
    header.msg_type = 9;
    header.msg_len = 10;
    header.seq_only = 6365; //

    printX((UINT8 *)&header, sizeof(header));
    send_udp((UINT8 *)&header, sizeof(header));

  return 0;
}




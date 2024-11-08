#include <string.h>
#include <error.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
int main(int argc, char *argv[])
{
char recvbuff[100];
int sockfd;
struct addrinfo hints, *si;
int rv;
if (argc != 2) {
printf("Usage: %s ip\n", argv[0]);
exit(0);
}
memset(&hints, 0, sizeof(hints));
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;
// 获得服务器地址的 addrinfo 信息
if ((rv = getaddrinfo(argv[1], "5000", &hints, &si)) != 0){
fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
exit(1);
}
if((sockfd = socket(si->ai_family, si->ai_socktype,
si->ai_protocol)) == -1){ // 建立套接字
perror("socket");
exit(1);
}
// 连接服务器
if (connect(sockfd, si->ai_addr, si->ai_addrlen) == -1){
perror("connect");
exit(1);
}
freeaddrinfo(si); // 释放 addrinfo 结构
if (recv (sockfd, recvbuff, sizeof(recvbuff), 0) == -1)
perror("recv");
char buff[1023];
fd_set rset;
struct timeval tv;
do{
tv.tv_sec = 0;
tv.tv_usec = 100; // 最多等待 100 毫秒
FD_ZERO(&rset); // 初始化 rset
FD_SET(0, &rset); // 监视标准输入
FD_SET(sockfd, &rset); // 监视 fd
if (select (sockfd +1, &rset, NULL, NULL, &tv) < 0) {
printf("select error!\n");
return -1;
}
else {
if (FD_ISSET(sockfd, &rset)) { // 如果是 fd 描述符上的事件
int i = read(sockfd, buff, 1023);
if (i ==0) return 0;
buff[i] = 0;
printf("%s", buff);
}
else if(FD_ISSET(0,&rset)){
int i = read(0, buff, 1023);
send(sockfd, buff, i, 0);
}
}
}while(1);

close(sockfd);
}


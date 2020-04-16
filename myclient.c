#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define SERV_PORT 40002

int main(){
	int connfd;
	struct sockaddr_in servaddr;
	int maxfd;
	int ret;
	int n;
	
	char buf[BUFFER_SIZE];
	
	connfd = socket(AF_INET, SOCK_STREAM, 0);
	if(connfd > STDIN_FILENO){
		maxfd = connfd;
	}
	else {
		maxfd = STDIN_FILENO;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(SERV_PORT);	
	inet_pton(AF_INET, "192.168.206.128", &servaddr.sin_addr);

	ret = connect(connfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	if(ret < 0) {
		printf("connection failure!\n");
		printf("errno is %d\n",errno);
		exit(0);
	}

	fd_set allset;
	fd_set rdset;
	FD_ZERO(&allset);
	FD_SET(STDIN_FILENO, &allset);
	FD_SET(connfd, &allset);
	int conn_stat = 1;
	

	for( ; ; ){
		rdset = allset;
		ret = select(maxfd + 1, &rdset, NULL, NULL, NULL);//ret 其实没什么用.
		if( conn_stat && FD_ISSET( STDIN_FILENO, &rdset ) ){//已经建立连接并且有键盘输入
			memset(buf, 0, BUFFER_SIZE);
			n = read( STDIN_FILENO, buf, BUFFER_SIZE - 1);
			printf("has read %d bytes from stdin\n", n);
			n = write( connfd, buf, n);
			if(n <= 0) {
				printf("write error\n");
				continue;			
			}
			printf("%d bytes has been sent\n", n);
			if(--ret <= 0) continue;
		}
		if( conn_stat && FD_ISSET( connfd, &rdset ) ){//客户端有输入
			memset(buf, 0, BUFFER_SIZE);
			n = read( connfd, buf, BUFFER_SIZE - 1);
			n = write( STDOUT_FILENO, buf, (int)n );
			if(--ret <= 0) continue;
		}
	}

	close(connfd);
	return 0;




}	
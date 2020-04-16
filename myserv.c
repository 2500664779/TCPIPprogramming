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
#include <assert.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define SERV_PORT 40002

int main(int argc, char * argv[]){
	int ret = 0;
	struct sockaddr_in addr;
	struct sockaddr_in connaddr;
	int maxfd;
	int connfd = -1;
	int conn_stat = 0;
	int connlen;
	int nread;
	int nwrite;
	char buf[BUFFER_SIZE];
	


	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(SERV_PORT);

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(listenfd >= 0);

	ret = bind( listenfd, (struct sockaddr*)&addr, sizeof(addr));
	assert(ret != -1);

	ret = listen( listenfd, 5);
	assert(ret != -1);

	fd_set allset, rset;
	FD_ZERO(&allset);
	FD_ZERO(&rset);

	FD_SET(0,&allset);
	FD_SET(listenfd,&allset);
		
	if(listenfd > STDIN_FILENO){
		maxfd = listenfd;
		printf("STDIN_FILENO is %d\n", STDIN_FILENO);
		printf("maxfd = %d\n", maxfd);
	}
	else{
		maxfd = STDIN_FILENO;
		printf("maxfd = %d\n", maxfd);
	}
	
		
	for( ; ; ){
		rset = allset;
		ret = select(maxfd + 1, &rset, NULL, NULL, NULL);
		printf("return from select\n");
		if(FD_ISSET(listenfd, &rset)){
			printf("new connection!\n");
			connlen = sizeof(connaddr);
			connfd = accept(listenfd, (struct sockaddr*)&connaddr, &connlen);
			
			FD_SET(connfd,&allset);
			if(connfd >= 0) conn_stat = 1;
			if(connfd > maxfd) maxfd = connfd;
			printf("connfd is %d, maxfd is %d\n", connfd, maxfd);
			if(--ret <= 0) continue;
		}
		if(FD_ISSET(connfd, &rset)){
			nread = read(connfd, buf,BUFFER_SIZE);
			printf("readable from connecton\n");
			if( nread == 0){
				printf("connection %d closed\n", connfd);
				close(connfd);
				break;
			}else if( nread < 0 ){
				printf("errno is %d\n", errno);
			}else{
				write(STDOUT_FILENO, buf, nread);
			}
			if(--ret <= 0) continue;
		}
		if(FD_ISSET(STDIN_FILENO, &rset)){
			nread = read(STDIN_FILENO, buf,BUFFER_SIZE);
			printf("read %d bytes from stdin\n", nread);
			if(conn_stat == 1)
				nwrite = write(connfd, buf, nread);
			printf("%d bytes has been sent\n", nwrite);
			if(--ret <= 0) continue;
		}
	}
	close(listenfd);
}
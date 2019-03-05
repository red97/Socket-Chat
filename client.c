#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<pthread.h>

static int stpr;
void *reader(void *vargp)
{
	int *n_servfd = (int *)vargp;
	char buf[1024] = {0};
	while(stpr == 1)
	{
		recv(*n_servfd, buf, 1024, 0);
		printf("%d %s\n", stpr, buf);
		if(strcmp(buf, "Server>end") == 0)
			stpr = 0;
		bzero(buf, sizeof(buf));
	}
	return NULL;
}
void *writer(void *vargp)
{
	int *n_servfd = (int *)vargp;
	char buf[1024] = {0};
	while(stpr == 1)
	{
		scanf("%s", buf);
		char tmp[1024];
		strcpy(tmp,"Client>");
		strcat(tmp, buf);
		send(*n_servfd, tmp, 1024, 0);
		if(strcmp(tmp, "Client>end") == 0)
			stpr = 0;
		bzero(buf, sizeof(buf));
	}
	return NULL;
}
int main(int argc, char const *argv[])
{

	struct sockaddr_in address;
	int clientfd = 0, valread;
	struct sockaddr_in serv_addr;
	char buf[1024] = {0};
	int PORT = atoi(argv[1]);

	//creation of socket
	if((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Socket creation error!");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	//Convert IPv4/Ipv6 addresses from text to binary format
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		printf("Invalid Address!");
		return -1;
	}

	if(connect(clientfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("Connection error!");
		return -1;
	}

	stpr = 1;
	pthread_t read_id, write_id;
	pthread_create(&read_id, NULL, reader, (void *)&clientfd);
	pthread_create(&write_id, NULL, writer, (void *)&clientfd);
	pthread_join(read_id, NULL);
	pthread_join(write_id, NULL);
	printf("connection ended\n");
	/*while(1)
	{

		char* tmp;
		printf("You>");
		scanf("%s",tmp);
		send(clientfd, tmp, sizeof(tmp), 0);
		recv(clientfd, buf, 1024, 0);
		printf("Server>%s\n", buf);
		bzero(buf, sizeof(buf));

	}*/
	return 0;

}
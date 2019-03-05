#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<pthread.h>

typedef struct data
{
	int sock_fd;
	int stpr;
}data;

void *reader(void *vargp)
{
	data *tmp = (struct data *)vargp;
	int n_servfd = (*tmp).sock_fd;
	char buf[1024] = {0};
	while((*tmp).stpr == 1)
	{
		recv(n_servfd, buf, 1024, 0);
		printf("%s\n", buf);
		if(strcmp(buf, "Client>end") == 0)
			(*tmp).stpr = 0;
		bzero(buf, sizeof(buf));
	}
	return NULL;
}
void *writer(void *vargp)
{
	data *tmp1 = (struct data *)vargp;
	int n_servfd = (*tmp1).sock_fd;
	char buf[1024] = {0};
	while((*tmp1).stpr == 1)
	{
		scanf("%s", buf);
		char tmp[1024];
		strcpy(tmp, "Server>");
		strcat(tmp, buf);
		send(n_servfd, tmp, 1024, 0);
		if(strcmp(tmp, "Server>end") == 0)
			(*tmp1).stpr = 0;
		bzero(buf, sizeof(buf));
	}
	return NULL;
}

void *thread(void *arg)
{
	data *tmp = (struct data *)arg;
	int sock_fd = (*tmp).sock_fd;
	//printf("Sockfd_thread = %d\n",sock_fd);
	send(sock_fd, "Server>Welcome!\n", 20, 0);
	
	pthread_t read_id, write_id;
	pthread_create(&read_id, NULL, reader, arg);
	pthread_create(&write_id, NULL, writer, arg);
	pthread_join(read_id, NULL);
	pthread_join(write_id, NULL);

	return NULL;
}
int main(int argc, char *argv[])
{

	int servfd, n_servfd, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	int PORT = atoi(argv[1]);

	//socket creation : socket(domain(IPv4/IPv6), type(TCP/UDP), protocol(value for Internet Protocol(IP)))
	servfd = socket(AF_INET, SOCK_STREAM, 0);
	if(servfd == 0)
	{
		perror("Socket Failed!");
		exit(0);
	}

	//Helps in reusing address and port (optional)
	if(setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
	{
		perror("setsockopt failed!");
		exit(0);
	}

	//After creation of socket, bind function binds the socket to address and port
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if(bind(servfd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("Binding Failiure!");
		exit(0);
	}

	//makes the server wait for new connections
	if(listen(servfd, 5) < 0)
	{
		perror("Listen Failiure!");
		exit(0);
	}

	while(1)
	{

		data nsoc;
		//printf("Sockfd_init = %d\n",nsoc.sock_fd);
		if((nsoc.sock_fd = accept(servfd, (struct sockaddr *)&address, (socklen_t *)&addrlen))<0)
		{
			printf("Acception error!\n");
			break;
		}
		nsoc.stpr = 1;

		//printf("Sockfd = %d\n",nsoc.sock_fd);
		pthread_t thread_id;
		pthread_create(&thread_id, NULL, thread, (void *)&nsoc);

		/*pthread_create(&nsoc.read_id, NULL, reader, (void *)&nsoc);
		pthread_create(&nsoc.write_id, NULL, writer, (void *)&nsoc);
		pthread_join(nsoc.read_id, NULL);
		pthread_join(nsoc.write_id, NULL);*/

	}

}
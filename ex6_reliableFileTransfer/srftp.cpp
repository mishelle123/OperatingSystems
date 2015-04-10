#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <iostream>
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <limits.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <map>
#include <fstream>      //

#define TRUE   1
#define FALSE  0
#define PORT 8888
#define BUFFER_SIZE 1024
#define INT_SIZE sizeof(int)
#define PREFEX_SIZE(X) 2*INT_SIZE + (X + 1)

struct client
{
	unsigned int fd;
	unsigned int how_many_recieved;
	unsigned int size_of_file, size_of_name;
	char* name_of_file;
	char* client_buffer;
	std::ofstream file;
};

std::map<int, client*> client_map;



void transfer_file(int fd)
{
	//Check if it was for closing , and also read the incoming message

	int valread;
	char buffer[BUFFER_SIZE];
	client* client = client_map[fd];
	if ((valread = recv(fd , buffer, BUFFER_SIZE ,0)) < 0)
	{
		perror("ERROR: recv failed");
		exit(EXIT_FAILURE);
	}
	else
	{
		if(client->how_many_recieved <= PREFEX_SIZE(client->size_of_name))
		{
			memcpy(client->client_buffer + client->how_many_recieved, buffer, valread);

			client->how_many_recieved += valread;
			// checks the file size
			if(client->size_of_file == 0)
			{
				// Initialize the size_of_file
				if(client->how_many_recieved >= INT_SIZE)
				{
					char size_of_file[INT_SIZE + 1];
					memcpy(size_of_file, client->client_buffer,INT_SIZE);
					client->size_of_file = *(reinterpret_cast<int*>(size_of_file));
				}
			}

			// checks the name size
			if(client->size_of_name == 0)
			{
				// Initialize the size_of_name
				if(client->how_many_recieved >= 2*INT_SIZE)
				{
					char size_of_name[INT_SIZE + 1];
					memcpy(size_of_name, client->client_buffer + INT_SIZE, INT_SIZE);
					client->size_of_name = *(reinterpret_cast<int*>(size_of_name));

				}
			}

			// checks the file_name
			if(client->name_of_file == NULL)
			{
				// Initialize the name_of_file
				if(client->how_many_recieved >= 2*INT_SIZE + (client->size_of_name + 1))
				{
					char* name_of_file = new char[client->size_of_name + 1];
					memcpy(name_of_file, client->client_buffer + 2*INT_SIZE, client->size_of_name + 1);

					client->name_of_file = name_of_file;
					client->file.open( client->name_of_file, std::ofstream::trunc | std::ofstream::out | std::ofstream::binary);
				}
			}
			if(client->file.is_open())
			{
				client->file.write(client->client_buffer + PREFEX_SIZE(client->size_of_name),
						client->how_many_recieved - (PREFEX_SIZE(client->size_of_name)));

			}

		}
		else
		{
			client->file.write(buffer, valread);
			client->how_many_recieved += valread;

		}
		// the writing is finish client_map[new_socket]
		if(client->how_many_recieved == client->size_of_file + PREFEX_SIZE(client->size_of_name))
		{
			client->file.close();
			delete(client->name_of_file);
			delete(client->client_buffer);
			close(fd);
			client_map.erase(fd);

		}
	}
}

int main(int argc , char *argv[])
{
	int opt = TRUE;
	int master_socket , addrlen , new_socket  ,  ready_clients, sd;
	int max_socket_fd;

	struct sockaddr_in address;



	//set of socket descriptors
	fd_set readfds;


	//create a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("ERROR: socket failed");
		exit(EXIT_FAILURE);
	}


	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
	{
		perror("ERROR: setsockopt failed");
		exit(EXIT_FAILURE);
	}

	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	//bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("ERROR: bind failed");
		exit(EXIT_FAILURE);
	}

	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, SOMAXCONN) < 0)
	{
		perror("ERROR: listen failed");
		exit(EXIT_FAILURE);
	}

	//accept the incoming connection
	addrlen = sizeof(address);

	while(TRUE)
	{

		//clear the socket set
		FD_ZERO(&readfds);

		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_socket_fd = master_socket;

		//add child sockets to set
		for (std::map<int, client*>::iterator itush = client_map.begin() ; itush != client_map.end() ; itush++)
		{
			sd =  itush->first;
			//add to the set
			if(sd > 0)
			{
				FD_SET(sd , &readfds);
			}

			//update the max socket fd
			if(sd > max_socket_fd)
			{
				max_socket_fd = sd;
			}
		}

		//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
		ready_clients = select( max_socket_fd + 1 , &readfds , NULL , NULL , NULL);

		if ((ready_clients < 0) && (errno!=EINTR))
		{
			perror("ERROR: select failed");
		}

		//If something happened on the master socket , then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("ERROR: accept failed");
				exit(EXIT_FAILURE);
			}



			//add new socket to array of sockets
			std::pair<int, client*> p(new_socket, new client());
			client_map.insert(p);
			client_map[new_socket]->how_many_recieved = 0;
			client_map[new_socket]->size_of_file = 0;
			client_map[new_socket]->size_of_name = 0;
			client_map[new_socket]->name_of_file = NULL;
			client_map[new_socket]->client_buffer = new char[PATH_MAX + 1 + 2*INT_SIZE + BUFFER_SIZE];

		}


		for (std::map<int, client*>::iterator itush = client_map.begin() ; itush != client_map.end() ; itush++)
		{
			sd = itush->first;

			// the fd is still in the map
			if (FD_ISSET(sd, &readfds))
			{
				//send to the method
				transfer_file(sd);
			}
		}


	}

	return 0;
}

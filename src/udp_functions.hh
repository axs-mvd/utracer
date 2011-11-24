#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <sys/time.h>

#include <iostream>

using namespace std;

int create_udp_listen(int listen_port);

int create_udp_send();

bool send_to(const char *ip_address, 
             int connect_port, 
             int socket_fd, 
             const char *data, 
             unsigned int data_length);

bool send_to(const char *ip_address, 
             int connect_port, 
             const char *data, 
             unsigned int data_length);

bool send_to(const char *ip_address, 
             int connect_port, 
             int socket_fd, 
             string data);

bool send_to(const char *ip_address, int connect_port, string data);

int receive_from(int socket_fd, char *data, int data_length, 
                  struct sockaddr_in *si, socklen_t *si_length);

int receive_from(int socket_fd, char *data, int data_length); 

/** returns time in milliseconds */
long tick();

bool ready_to_read(int sockfd);


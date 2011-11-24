#include "udp_functions.hh"

int create_udp_listen(int listen_port) {
    struct sockaddr_in si_me;
 
    int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_fd == -1) {
        cout << "could not create socket" << endl;
        //ought to raise an exception 
        return 0;
    }

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(listen_port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(socket_fd, (struct sockaddr *)&si_me, sizeof(si_me)) == -1) {
        cout << "could not bind socket" << endl;
        //ought to raise an exception 
        return 0;
    }

    return socket_fd;
}

//int create_udp_send(const char *ip_address, int connect_port) {
int create_udp_send() {
 
    int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_fd == -1) {
        cout << "could not create socket" << endl;
        //ought to raise an exception 
        return 0;
    }
  
    return socket_fd;
}

bool send_to(const char *ip_address, 
             int connect_port, 
             int socket_fd, 
             const char *data, 
             unsigned int data_length) {

    struct sockaddr_in si;
    memset(&si, 0, sizeof(sockaddr_in));

    si.sin_family = AF_INET;
    si.sin_port = htons(connect_port);
    if (inet_aton(ip_address, &si.sin_addr) == 0) {
        cout << "could not create address" << endl;
        return false;
    }

    if (sendto(socket_fd, data, data_length, 0, (sockaddr *)&si, sizeof(si)) == -1) {
        cout << "could not send message" << endl;
        return false;
    }

    return true;
}

bool send_to(const char *ip_address, 
             int connect_port, 
             const char *data, 
             unsigned int data_length) {

    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    bool r = send_to(ip_address, connect_port, 
                   fd,
                   data,
                   data_length);
    close(fd);
    return r;
}


bool send_to(const char *ip_address, 
             int connect_port, 
             int socket_fd, 
             string data) {

    return send_to(ip_address, 
                   connect_port, 
                   socket_fd, 
                   data.c_str(), 
                   data.length());
}

bool send_to(const char *ip_address, int connect_port, string data) {
    
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    bool r = send_to(ip_address, 
                   connect_port, 
                   fd,
                   data.c_str(), 
                   data.length());
    close(fd);
    return r;

}

int receive_from(int socket_fd, char *data, int data_length, 
                  struct sockaddr_in *si, socklen_t *si_length) {

    return recvfrom(socket_fd, data, data_length, 0, (sockaddr *)si, si_length);
}

int receive_from(int socket_fd, char *data, int data_length) {
    return receive_from(socket_fd, data, data_length, 0, 0);
}
 
/** returns time in milliseconds */
long tick() {
    struct timeval start;
    gettimeofday(&start, NULL);
    return (long)(start.tv_sec*1000 + (int)start.tv_usec/1000);
}

bool ready_to_read(int sockfd) {
    fd_set read_set;
    struct timeval tv;
    int retval;

    FD_ZERO(&read_set);
    FD_SET(sockfd, &read_set);

    tv.tv_sec  = 1;
    tv.tv_usec = 0;

    retval = select(sockfd + 1, &read_set, 0, 0, &tv);
    
    if (retval == -1) {
        cout << __FILE__ << ":" << __LINE__ << " ready_to_read  fail" << endl;
        return false;
    } if (retval != 0) {
        return true;
    } else {
        return false;
    }
}



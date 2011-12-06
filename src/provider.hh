#pragma once

#include <iostream>
#include <set>
#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

#include <pthread.h>
#include <cstring>

#include "udp_functions.hh"

using namespace std;

class client {
private:
    string _ip;
    int _port;
    unsigned long _last_time_saw;
    static const unsigned long timeout = 500;

public:

    client(string ip, int port) {
        _ip = ip;
        _port = port;
        refresh();
    }    

    client(const client &c) {
        _ip = c._ip;
        _port = c._port;
        _last_time_saw = c._last_time_saw;
    }

    virtual ~client() {
    }

    string ip() {
        return _ip;
    }

    client &operator=(const client &c) {
        if (this != &c) {
            _ip = c._ip;
            _port = c._port;
            _last_time_saw = c._last_time_saw;
        }

        return *this;
    }

    bool operator==(const client &c) const {
        if (this == &c) return true;
        else return (c._ip == _ip) && (c._port == _port);
    }

    bool operator<(const client &c) const {
        if (c._ip == _ip) {

            return c._port < _port;

        } else return c._ip < _ip;
    }
 
    int port() {
        return _port;
    }

    bool expired() const {
        return (tick() - _last_time_saw) > timeout;
    }

    void refresh() {
        _last_time_saw = tick();
    }
    
};


class provider {
private:
    map<string, vector<client> > _enabled_probes;
    set<client> _clients;
    static provider *_instance;
    pthread_t _listen_thread;
    int  _listen_port;
    bool _shutdown_thread;
    static const int _buff_size = 1024;
    char *_buff;

    provider() {
        _listen_port     = get_port();;
        _shutdown_thread = false;
        _buff = new char[_buff_size];
        start();
    }
    
    virtual ~provider() {
        _shutdown_thread = true;
        pthread_cancel(_listen_thread);
        pthread_join(_listen_thread, 0);
        delete [] _buff;
    }

    static void *listen_thread(void *);    

    void start();

    void enable_probe(const string &probe_name, const client &c); 
    void remove_client(const client &c);
    void refresh_client(const client &c);

    void remove_expired();

public:
    
    static provider *instance();
    static void destroy();

    bool enabled(const string &probe) const;
    vector<client> get_subscribers(const string &probe) const;
};

//builds the udp datagram as follows:
// probe_name-timestamp-thread_id-value
char *create_package(string probe_name, uint64_t value, unsigned int &buffer_size);

string deserialize_package(char *buffer, 
                         uint32_t buffer_size, 
                         uint32_t &thread_id,
                         uint64_t &timestamp,
                         uint64_t &value);                        

/*
void probe(const char*name); 
void memory_probe(const char*name);
void cpu_sys_probe(const char*name);
void cpu_user_probe(const char*name);
void cpu_total_probe(const char*name);
void cntx_swch_probe(const char*name);
*/


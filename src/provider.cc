#include "provider.hh"


provider *provider::_instance = 0;

provider *provider::instance() {

    if (_instance == 0) {
        cout << "creating provider" << endl;
        _instance = new provider();
    }

    return _instance;
}

void provider::destroy() {
    if (_instance != 0) {
        cout << "destroying provider" << endl;
        delete _instance;
        _instance = 0;
    }
}

void provider::start() {
    pthread_create(&_listen_thread, 0,
            provider::listen_thread, (void *)this);
}

void *provider::listen_thread(void *arg) {
    provider *i = (provider *) arg;

    int sock;
    
    sock = create_udp_listen(i->_listen_port);

    sockaddr_in foreign;
    socklen_t foreign_len = sizeof(foreign);
    int received_size;

    string probe_to_subscribe;
    string subscriber_ip;
    int subscriber_port;
    
    while (true) {

        if (ready_to_read(sock)) {
            received_size = receive_from(sock, i->_buff, i->_buff_size, 
                                         &foreign, &foreign_len);


            subscriber_port = ntohs(foreign.sin_port);
            subscriber_ip.assign(inet_ntoa(foreign.sin_addr));
            client c(subscriber_ip, subscriber_port);
            
            probe_to_subscribe.assign(i->_buff, received_size);
            
            i->refresh_client(c);
            i->enable_probe(probe_to_subscribe, c);
        } 

        i->remove_expired();
    }

    return 0;
}

void provider::remove_expired() {
    for (set<client>::iterator it = _clients.begin(); 
         it != _clients.end(); it++) {
        if (it->expired()) {
            remove_client(*it);
            _clients.erase(it);
        }
    }
}

void provider::remove_client(const client &c) {
    for (map<string, vector<client> >::iterator it = _enabled_probes.begin();
         it != _enabled_probes.end(); it++) {

        vector<client>::iterator itc = find(it->second.begin(), 
                                            it->second.end(), c);
        if (itc != it->second.end()) {
            it->second.erase(itc);
        }

        if (it->second.empty()) {
            _enabled_probes.erase(it);
        }
    }
}

void provider::enable_probe(const string &probe_name, const client &c) {
    if (_enabled_probes.find(probe_name) == _enabled_probes.end()) {
        vector<client> clients;
        _enabled_probes[probe_name] = clients;
    }

    bool found = find(_enabled_probes[probe_name].begin(),
                      _enabled_probes[probe_name].end(),
                      c) != _enabled_probes[probe_name].end();
    if (!found) {
        _enabled_probes[probe_name].push_back(c);
    }
}

void provider::refresh_client(const client &c) {
    set<client>::iterator it = _clients.find(c);
    if (it != _clients.end()) {
        _clients.erase(it);
    } 
    _clients.insert(c);
}

bool provider::enabled(const string &probe) const {
    map<string, vector<client> >::const_iterator it = _enabled_probes.find(probe);
    return (it != _enabled_probes.end());
}

vector<client> provider::get_subscribers(const string &probe) const {
    map<string, vector<client> >::const_iterator it = _enabled_probes.find(probe);
    if (it != _enabled_probes.end()) {
        return it->second;
    } else {
        vector<client> d;
        return d;
    }
}

//builds the udp datagram as follows:
// probe_name-timestamp-thread_id-value
char *create_package(string probe_name, uint64_t value, unsigned int &buffer_size) {
    uint32_t thread_id = pthread_self();
    buffer_size = probe_name.length() + //size of the probe name
                  sizeof(uint64_t) +    
                  sizeof(uint32_t) + 
                  sizeof(uint64_t);

    char *buffer = new char[buffer_size];
    uint32_t pos = 0;
    uint64_t timestamp = tick();

    memcpy(buffer + pos, probe_name.c_str(), probe_name.length() + 1);
    pos += probe_name.length() + 1;

    memcpy(buffer + pos, &timestamp, sizeof(uint64_t));
    pos += sizeof(uint64_t);

    memcpy(buffer + pos, &thread_id, sizeof(uint32_t));
    pos += sizeof(uint32_t);
 
    memcpy(buffer + pos, &value, sizeof(uint64_t));
    pos += sizeof(uint64_t);

    buffer_size = pos;

    return buffer;
}

string deserialize_package(char *buffer, 
                         uint32_t buffer_size, 
                         uint32_t &thread_id,
                         uint64_t &timestamp,
                         uint64_t &value) {
    string probe_name;
    probe_name.assign(buffer, strlen(buffer));

    uint32_t pos = strlen(buffer) + 1;
    if (pos < buffer_size)  {
        memcpy(&timestamp, buffer + pos, sizeof(uint64_t));
    }

    pos += sizeof(uint64_t);
    if (pos < buffer_size) {
        memcpy(&thread_id, buffer + pos, sizeof(uint32_t));
    }

    pos += sizeof(uint32_t);
    if (pos < buffer_size) {
        memcpy(&value, buffer + pos, sizeof(uint64_t));
    }

    return probe_name;
}
                        


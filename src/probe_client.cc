#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <map>

#include "provider.hh"
#include "probe_processor.hh"


using namespace std;

class thread_status {
private:
    int32_t _last_probe_received;
    vector<measure> _measures;
    
public:
    thread_status() {
        _last_probe_received = -1;
    }
    
    virtual ~thread_status() {}

    int32_t &last_probe_received() {
        return _last_probe_received;
    }

    vector<measure> &measures() {
        return _measures;
    }
};

uint32_t get_probe_pos(vector<string> probes, string probe) {

    for (uint32_t i = 0; i < probes.size(); i++) {
        if (probe == probes[i]) return i;
    }
    cerr << __FILE__ << ":" << __LINE__ << " " << probe << " not found" << endl;
    return probes.size() + 1;
}

//
// receive_port subject_ip subject_port probe1 probe2 .. proben
//
void receive_data(const char* ip_address, 
                  int subject_port, 
                  int sock_fd, 
                  vector<string> probes, 
                  map<uint32_t, thread_status> &status,
                  probe_processor *processor) {

    uint32_t thread_id;
    uint64_t timestamp;
    uint64_t value;

    int buffer_size = 4096;
    char *buffer = new char[buffer_size];

    for (vector<string>::iterator it = probes.begin(); it != probes.end(); it++) {
        string probe = *it;
        send_to(ip_address, subject_port, sock_fd, probe.c_str(), probe.length());
    }

    if (ready_to_read(sock_fd)) {
        int received = receive_from(sock_fd, buffer, buffer_size);
    
        string probe_received = deserialize_package(buffer, received, 
                thread_id, timestamp, value);

//        cout << "received: " << probe_received << " thread_id: " << thread_id 
//             << " timestamp: " << timestamp << " value: " << value << endl;
                
        measure m(timestamp, value);

        int32_t probe_pos = get_probe_pos(probes, probe_received);
        if (probe_pos != -1) {
        
            if (status.find(thread_id) == status.end()) {
                thread_status s;
                status[thread_id] = s;
            } 

            if (status[thread_id].last_probe_received() + 1 == (uint32_t)probe_pos) {
                status[thread_id].last_probe_received() = probe_pos;
                status[thread_id].measures().push_back(m);
            } else {
                status[thread_id].last_probe_received() = -1;
                status[thread_id].measures().clear();
            }
            //cout << "last_probe_received = " << status[thread_id].last_probe_received() << endl;
            if (((uint32_t)status[thread_id].last_probe_received()) + 1 == probes.size()) {

                processor->process(thread_id, status[thread_id].measures());
                status[thread_id].last_probe_received() = -1;
                status[thread_id].measures().clear();
            }
        }
    }
}

void print_usage(const char* command) {
    cout << "#epic_fail use:" << endl;
    cout << command << " <receive port> <subject ip> <subject port> [--notimestamp] [--nothreadid] <probe 1> <probe 2> ... <probe n> " << endl;
}

int main(int argc, char *argv[]) {

    int client_port = -1;
    int subject_port = -1;
    const char * subject_ip;
    vector<string> probes;
    string notimestamp = "--notimestamp";
    string nothreadid  = "--nothreadid";
    bool timestampflag = true;
    bool threadidflag = true;

    if (argc < 5) {
        print_usage(argv[0]);
        exit(1);
    } else {
        client_port  = atoi(argv[1]);
        subject_ip   = argv[2];
        subject_port = atoi(argv[3]);
        
        for (int i = 4; i < argc; i++) {
            if (notimestamp == string(argv[i])) {
                timestampflag = false;
            } else if (nothreadid == string(argv[i])) {
                threadidflag = false;
            } else {
                probes.push_back(argv[i]);
            }
        }
    }

    cerr << "probing: " << subject_ip << ":" << subject_port << endl;
    for (vector<string>::iterator it = probes.begin(); it != probes.end(); it++) {
        cerr << "\t" << *it << endl;
    }
    cerr << endl;

    int sock_fd = create_udp_listen(client_port);
    map<uint32_t, thread_status> status;
    probe_processor *processor = new console_probe_processor(timestampflag, threadidflag);
    while(true) {
        receive_data(subject_ip, subject_port, sock_fd, probes, status, processor);    
    }

    return 0;
}


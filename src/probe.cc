#include "probe.hh"
#include "provider.hh"

#include <sys/time.h>
#include <sys/resource.h>

using namespace std;

void send_packet_to_clients(const string &probe, char *buff, const uint32_t &buffer_size)  {
    vector<client> clients = provider::instance()->get_subscribers(probe);
    for (vector<client>::iterator it = clients.begin(); it != clients.end(); it++) {
        send_to(it->ip().c_str(), it->port(), buff, buffer_size);
    }
}

uint64_t translate(const timeval &t) {
    uint64_t sec = ((uint64_t)t.tv_sec) * 1000;
    uint64_t usec = t.tv_usec/1000;
    return sec + usec;
}

void timestamp_probe(const string &probe) {
    if (provider::instance()->enabled(probe)) {
        integer_probe(probe, (uint64_t)tick());
    }
}

void integer_probe(const string &probe, const int64_t &value) {
    if (provider::instance()->enabled(probe)) {

        char *buff = 0;
        unsigned int buffer_size = 0;

        buff = create_package(probe, value, buffer_size);

        send_packet_to_clients(probe, buff, buffer_size);

        delete [] buff;
    }
}

void integer_probe(const string &probe, const uint64_t &value) {
    if (provider::instance()->enabled(probe)) {

        char *buff = 0;
        unsigned int buffer_size = 0;

        buff = create_package(probe, value, buffer_size);

        send_packet_to_clients(probe, buff, buffer_size);

        delete [] buff;
    }
}

void memory_probe(const std::string &probe) {
    if (provider::instance()->enabled(probe)) {
        struct rusage data;
        getrusage(RUSAGE_SELF, &data);
        integer_probe(probe, (uint64_t)data.ru_ixrss + 
                             (uint64_t)data.ru_idrss + 
                             (uint64_t)data.ru_isrss);
    } 
}
void cpu_usr_probe(const std::string &probe) {
    if (provider::instance()->enabled(probe)) {
        struct rusage data;
        getrusage(RUSAGE_SELF, &data);
 
        integer_probe(probe, translate(data.ru_utime));
    } 
}

void cpu_sys_probe(const std::string &probe) {
    if (provider::instance()->enabled(probe)) {
        struct rusage data;
        getrusage(RUSAGE_SELF, &data);
 
        integer_probe(probe, translate(data.ru_stime));
    } 
}

void cpu_probe(const string &probe) {
    if (provider::instance()->enabled(probe)) {
        struct rusage data;
        getrusage(RUSAGE_SELF, &data);
 
        integer_probe(probe, translate(data.ru_utime) + translate(data.ru_stime));
    } 
}

void cntx_swch_probe(const std::string &probe) {
    if (provider::instance()->enabled(probe)) {
        struct rusage data;
        getrusage(RUSAGE_SELF, &data);
 
        integer_probe(probe, translate(data.ru_utime) + translate(data.ru_stime));
    } 
}

//=============================================================================
__attribute__((constructor)) void __constructor__utracer__(void) {
    provider::instance();
}

__attribute__((destructor)) void __destructor__utracer__(void) {
    provider::destroy(); 
}
//=============================================================================


#include <string>
#include <stdint.h>

void timestamp_probe(const std::string &probe);
void memory_probe(const std::string &probe);
void cpu_usr_probe(const std::string &probe);
void cpu_sys_probe(const std::string &probe);
void cpu_probe(const std::string &probe);
void cntx_swch_probe(const std::string &probe);


void integer_probe(const std::string &probe, const int64_t &value);
void integer_probe(const std::string &probe, const uint64_t &value);

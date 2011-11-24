#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <sys/time.h>

#include "probe.hh"

using namespace std;

long tick() {
    struct timeval start;
    gettimeofday(&start, NULL);
    return (long)(start.tv_sec*1000 + (int)start.tv_usec/1000);
}

int main() {

    uint64_t index = 0;

    while (true) {
        
        timestamp_probe("TEST_PROBE::ONE");
        sleep(1);
        cout << tick() << endl;

        timestamp_probe("TEST_PROBE::TWO");
        sleep(1);
        cout << tick() << endl;

        timestamp_probe("TEST_PROBE::THREE");
        sleep(1);
        cout << tick() << endl;

        timestamp_probe("TEST_PROBE::CHOPEK");
        sleep(1);
        cout << tick() << endl;

        index++;
        integer_probe("TEST_PROBE::INTEGER", index);
        memory_probe("TEST_PROBE::MEMORY");
        cpu_probe("TEST_PROBE::CPU");
    }

    return 0;
}



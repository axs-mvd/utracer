#include <iostream>
#include <vector>

using namespace std;

class measure {
private:

    uint64_t _timestamp;
    uint64_t _value;

public:
    
    measure(uint64_t timestamp, uint64_t value) {
        _timestamp = timestamp;
        _value = value;
    }

    uint64_t &timestamp() {
        return _timestamp;
    }

    uint64_t &value() {
        return _value;
    }
};


class probe_processor {
protected:
    bool _process_timestamp;
    bool _process_threadid;
public:
    probe_processor(bool process_timestamp, bool process_threadid) {
        _process_timestamp = process_timestamp;
        _process_threadid  = process_threadid;
    }

    virtual ~probe_processor() {};
    virtual void process(uint32_t threadid, vector<measure> measures) = 0;
};

class console_probe_processor : public probe_processor {
public:
    console_probe_processor(bool process_timestamp, bool process_threadid): 
        probe_processor(process_timestamp, process_threadid) {};
    
    virtual ~console_probe_processor(){}

    virtual void process(uint32_t threadid, vector<measure> measures) {

        if (_process_threadid) {
            cout << threadid << " ";
        }

        for (vector<measure>::iterator measure = measures.begin(); measure != measures.end(); measure++) {
            if (_process_timestamp) {
                cout << measure->timestamp() << " ";
            }

            cout << measure->value() << " "; 
        }
        cout << endl;
    }
};

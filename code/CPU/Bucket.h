#include "stdio.h"
#include "param.h"
#include "Wave.h"
#include "ostream"
#include "iostream"

class Bucket {
public:
    uint16_t counter[2];
    uint64_t id;
    uint16_t timestamp;
    uint16_t val;
    uint16_t start;
    Window windows;
    char type;

    Bucket() : windows(0, 0, 0) {
        this->counter[0] = 0;
        this->counter[1] = 0;
        this->id = 0;
        this->timestamp = 0;
        this->val = 0;
        this->start = 0;
        this->type = ' ';

    }

    Bucket(uint64_t id, uint16_t cnt1, uint16_t cnt2, uint16_t timestamp, uint16_t val, uint16_t start,
           uint8_t t_i, uint8_t t_l, uint8_t t_d, char type) : windows(t_i, t_l, t_d) {
        this->counter[0] = cnt1;
        this->counter[1] = cnt2;
        this->id = id;
        this->timestamp = timestamp;
        this->val = val;
        this->start = start;
        this->type = type;

    }

    void operator=(const Bucket &b) {
        this->counter[0] = b.counter[0];
        this->counter[1] = b.counter[1];
        this->id = b.id;
        this->timestamp = b.timestamp;
        this->val = b.val;
        this->start = b.start;
        this->type = b.type;
        this->windows.t_i = b.windows.t_i;
        this->windows.t_l = b.windows.t_l;
        this->windows.t_d = b.windows.t_d;
    }

    void clear() {
        this->counter[0] = 0;
        this->counter[1] = 0;
        this->id = 0;
        this->timestamp = 0;
        this->val = 0;
        this->start = 0;
        this->type = ' ';
        this->windows.clear();
    }

    friend std::ostream &operator<<(std::ostream &os, Bucket &b) {
        os << b.id << " " << b.timestamp << "  "<<b.counter[0] << "  "<<b.counter[1] << "  "<< b.start << "  "<< b.val << "  " << unsigned(b.windows.t_i) << "  " << unsigned(b.windows.t_l) << "  " << unsigned(b.windows.t_d)
           << "  " << b.type << "\n";
        return os;
    }

};



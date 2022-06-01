#include "ostream"
#include "iostream"

// define window
class Window {
public:
    uint8_t t_i; // the increase window size
    uint8_t t_l; // the stable window size
    uint8_t t_d; // the decrease window size
    Window() {
        t_i = 0;
        t_l = 0;
        t_d = 0;
    }

    Window(uint8_t i, uint8_t l, uint8_t d) {
        t_i = i;
        t_l = l;
        t_d = d;
    }

    void clear() {
        this->t_i = 0;
        this->t_l = 0;
        this->t_d = 0;
    }

    void operator=(const Window &b) {
        t_i = b.t_i;
        t_l = b.t_l;
        t_d = b.t_d;
        return;
    }

};

class Wave {
public:
    Window window;
    uint64_t flow_id;
    uint16_t start_time;
    uint16_t cnt1, cnt2, cnt3, cnt4;

    Wave() {};

    Wave(uint16_t start_time, uint8_t t_i, uint8_t t_l, uint8_t t_d, uint64_t id, uint16_t c1, uint16_t c2,
          uint16_t c3, uint16_t c4) : window(t_i, t_l, t_d) {

        this->start_time = start_time;
        cnt1 = c1;
        cnt2 = c2;
        cnt3 = c3;
        cnt4 = c4;
        flow_id = id;
    }

    friend std::ostream &operator<<(std::ostream &os, Wave &b) {
        os << b.flow_id << " " << b.start_time << "  " << unsigned(b.window.t_i) << "  " << unsigned(b.window.t_l)
           << "  " << unsigned(b.window.t_d)
           << "  " << b.cnt1
           << "  " << b.cnt2 << "  " << b.cnt3 << "  " << b.cnt4 << "\n";
        return os;
    }

    bool operator==(const Wave &b) const {
        return (flow_id == b.flow_id) && (window.t_i == b.window.t_i) && (window.t_l == b.window.t_l) &&
               (window.t_d == b.window.t_d) && start_time == b.start_time;
        //
    }

};


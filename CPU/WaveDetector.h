#include<map>
# include "Wave.h"
#include "param.h"

fstream file;

class CorrectBurstDetector {
public:
    map<uint64_t, int> F;
    uint32_t w, m;
    uint16_t *counter[2];
    uint16_t *cnt[4];
    Window *windows;
    int *timestamp;
    uint64_t *id;
    uint32_t last_timestamp;
    bool flag;
    uint32_t *counter_flag;
    vector<Wave> Record_p;
    vector<Wave> Record_n;

    CorrectBurstDetector(int _m) {
        F.clear();
        w = 0;
        m = _m;
        flag = 0;
        Record_p.clear();
        Record_n.clear();
        counter[0] = new uint16_t[packet_num];
        counter[1] = new uint16_t[packet_num];
        cnt[0] = new uint16_t[packet_num];
        cnt[1] = new uint16_t[packet_num];
        cnt[2] = new uint16_t[packet_num];
        cnt[3] = new uint16_t[packet_num];

        id = new uint64_t[packet_num];
        timestamp = new int[packet_num];
        windows = new Window[packet_num];
        counter_flag = new uint32_t[packet_num];
        for (long long i = 0; i < packet_num; i++) {
            counter[0][i] = 0;
            counter[1][i] = 0;
            timestamp[i] = -1;
            counter_flag[i] = 0;
            id[i] = 0;
            cnt[0][i] = 0;
            cnt[1][i] = 0;
            cnt[2][i] = 0;
            cnt[3][i] = 0;
        }
    }

    void update(uint32_t time) {
        for (int i = 0; i < w; i++) {
            if (id[i] == 0)
                continue;
            switch (counter_flag[i]) {
                // stage 1
                case 0:
                    // increase
                    if (counter[1][i] >= k * counter[0][i] && counter[1][i] >= threshold) {
                        counter_flag[i] = 2;
                        windows[i].t_i = 1;
                        cnt[0][i] = counter[0][i];
                        cnt[1][i] = counter[1][i];
                        counter[0][i] = counter[1][i];
                        timestamp[i] = time - 1;
                    } else if (counter[1][i] <= 1 / k * counter[0][i] && counter[0][i] >= threshold) {
                        counter_flag[i] = 2;
                        windows[i].t_d = 1;
                        cnt[0][i] = counter[0][i];
                        cnt[1][i] = counter[1][i];
                        counter[0][i] = counter[1][i];
                        timestamp[i] = time - 1;
                    } else if (counter[1][i] > ((k - 1) / (T_i) + 1) * counter[0][i] &&
                               counter[1][i] >= threshold / T_i) {
                        counter_flag[i] = 1;
                        timestamp[i] = time - 1;
                        windows[i].t_i++;
                        cnt[0][i] = counter[0][i];
                        cnt[1][i] = counter[1][i];
                        // decrease
                    } else if (counter[1][i] < (1 - (1 - 1 / k) / T_d) * counter[0][i] && counter[0][i] >= threshold) {
                        counter_flag[i] = 1;
                        timestamp[i] = time - 1;
                        windows[i].t_d++;
                        cnt[0][i] = counter[0][i];
                        cnt[1][i] = counter[1][i];
                    } else {
                        // clear the counter for next time window
                        counter[0][i] = counter[1][i];
                    }
                    counter[1][i] = 0;
                    break;

                    // stage 2
                case 1:
                    // exceed the increase threshold
                    if (counter[1][i] >= threshold && counter[1][i] >= k * counter[0][i]) {
                        counter_flag[i] = 2;
                        if (windows[i].t_i) {
                            windows[i].t_i++;
                            if (windows[i].t_i > T_i) {
                                clear(i);
//                                break;
                            } else {
                                cnt[1][i] = counter[1][i];
                            }

                        } else {

                            windows[i].clear();
                            windows[i].t_i = 1;
                            timestamp[i] = time - 1;
                            cnt[0][i] = cnt[1][i];
                            cnt[1][i] = counter[1][i];

                        }

                        counter[0][i] = counter[1][i];

                        // the end of decrease
                    } else if (counter[1][i] <= 1 / k * counter[0][i] && counter[0][i] >= threshold) {
                        counter_flag[i] = 2;
                        if (windows[i].t_d) {
                            windows[i].t_d++;
                            if (windows[i].t_d > T_d) {
                                clear(i);
                            } else {
                                cnt[1][i] = counter[1][i];
                            }
                        } else {

                            windows[i].clear();
                            windows[i].t_d = 1;
                            timestamp[i] = time - 1;
                            cnt[0][i] = cnt[1][i];
                            cnt[1][i] = counter[1][i];


                        }

                        counter[0][i] = counter[1][i];
                        // increase
                    } else if (counter[1][i] > ((1) * (k - 1) / (T_i) + 1) * counter[0][i] && windows[i].t_i
                               && counter[1][i] >= threshold / T_i) {

                        windows[i].t_i++;
                        cnt[1][i] = counter[1][i];
                        // exceed the increase time threshold
                        if (windows[i].t_i > T_i) {
                            counter[0][i] = counter[1][i];
                            clear(i);
                        }

                        // decrease
                    } else if (counter[1][i] <
                               (1 - (1 - 1 / k) * (1) / T_d) * counter[0][i] && windows[i].t_d) {
                        // windows[i].t_d +


                        windows[i].t_d++;
                        cnt[1][i] = counter[1][i];
                        // exceed the decrease time threshold
                        if (windows[i].t_d > T_d) {
                            counter[0][i] = counter[1][i];
                            clear(i);
                        }

                        // the end of increase
                    } else {
                        clear(i);
                        counter[0][i] = counter[1][i];
                    }

                    counter[1][i] = 0;
                    break;

                    // stage 3
                case 2:
                    if (counter[1][i] <= (1 / k) * counter[0][i] && counter[0][i] >= threshold) {
                        if (windows[i].t_i) {
                            Wave b(timestamp[i], windows[i].t_i, windows[i].t_l, 1, id[i], cnt[0][i], cnt[1][i],
                                    counter[0][i], counter[1][i]);
                            Record_p.push_back(b);

                            fout << "p   " << b << "    " << endl;
                            clear(i);

                            windows[i].t_d = 1;
                            timestamp[i] = time - 1;
                            counter_flag[i] = 2;
                            cnt[0][i] = counter[0][i];
                            cnt[1][i] = counter[1][i];
                            counter[0][i] = counter[1][i];

                        } else {
                            if (windows[i].t_l) {
                                counter_flag[i] = 2;
                                windows[i].clear();
                                windows[i].t_d = 1;
                                timestamp[i] = time - 1;
                                cnt[0][i] = counter[0][i];
                                cnt[1][i] = counter[1][i];
                                counter[0][i] = counter[1][i];
                            } else {
                                windows[i].t_d++;
                                counter[0][i] = counter[1][i];
                                cnt[1][i] = counter[1][i];
                                if (windows[i].t_d > T_d) {

                                    clear(i);
                                }
                            }

                        }


                    } else if (counter[1][i] >= k * counter[0][i] && counter[1][i] >= threshold) {
                        if (windows[i].t_d) {
                            // output decrease-increase burst
                            Wave b(timestamp[i], 1, windows[i].t_l, windows[i].t_d, id[i], cnt[0][i], cnt[1][i],
                                    counter[0][i], counter[1][i]);
                            Record_n.push_back(b);
                            fout << "n   " << b << "    " << endl;
                            clear(i);

                            windows[i].t_i = 1;
                            timestamp[i] = time - 1;
                            counter_flag[i] = 2;
                            cnt[0][i] = counter[0][i];
                            cnt[1][i] = counter[1][i];
                            counter[0][i] = counter[1][i];
                        } else {
                            if (windows[i].t_l) {
                                counter_flag[i] = 2;
                                windows[i].clear();
                                windows[i].t_i = 1;
                                timestamp[i] = time - 1;
                                cnt[0][i] = counter[0][i];
                                cnt[1][i] = counter[1][i];
                                counter[0][i] = counter[1][i];
                            } else {
                                windows[i].t_i++;
                                counter[0][i] = counter[1][i];
                                cnt[1][i] = counter[1][i];
                                if (windows[i].t_i > T_i) {
//                                    counter[0][i] = counter[1][i];
                                    clear(i);
                                }

                            }

                        }
                    } else if (counter[1][i] < (1 - (1 - 1 / k) / T_d) * counter[0][i] && counter[0][i] >= threshold) {
                        if (windows[i].t_i) {
                            counter_flag[i] = 3;
                            windows[i].t_d++;
                            cnt[2][i] = counter[0][i];
                            cnt[3][i] = counter[1][i];
                        } else if (windows[i].t_d && !windows[i].t_l) {
                            windows[i].t_d++;
                            counter[0][i] = counter[1][i];
                            cnt[1][i] = counter[1][i];
                            if (windows[i].t_d > T_d) {
//                                counter[0][i] = counter[1][i];
                                clear(i);
                            }
                        } else {
                            counter[0][i] = counter[1][i];
                            clear(i);
                        }


                    } else if (counter[1][i] > (1 * (k - 1) / (T_i) + 1) * counter[0][i] &&
                               counter[1][i] >= threshold / T_i) {
                        if (windows[i].t_d) {
                            counter_flag[i] = 4;
                            windows[i].t_i++;
                            cnt[2][i] = counter[0][i];
                            cnt[3][i] = counter[1][i];
                        } else if (windows[i].t_i && !windows[i].t_l) {
                            windows[i].t_i++;
                            counter[0][i] = counter[1][i];
                            cnt[1][i] = counter[1][i];
                            if (windows[i].t_i > T_i) {
                                clear(i);
                            }
//                            counter[0][i] = counter[1][i];
                        } else {
                            counter[0][i] = counter[1][i];
                            clear(i);
                        }


                    } else {
                        windows[i].t_l++;
                        counter[0][i] = counter[1][i];
                        if (windows[i].t_l > T) {
                            clear(i);
                        }
                    }
                    counter[1][i] = 0;
                    break;
                    // stage 4
                case 3:
                    // increase-decrease burst

                    if (counter[1][i] <= (1 / k) * counter[0][i] && counter[0][i] >= threshold) {
                        // output increase-decrease burst
                        windows[i].t_d++;
                        Wave b(timestamp[i], windows[i].t_i, windows[i].t_l, windows[i].t_d, id[i], cnt[0][i],
                                cnt[1][i],
                                counter[0][i], counter[1][i]);
                        Record_p.push_back(b);

                        fout << "p   " << b << "    " << endl;
//                        clear(i);

                        uint8_t tmp = windows[i].t_d;
                        clear(i);

                        windows[i].t_d = tmp;
                        timestamp[i] = time - tmp;
                        counter_flag[i] = 2;
                        cnt[0][i] = counter[0][i];
                        cnt[1][i] = counter[1][i];

                        counter[0][i] = counter[1][i];

                        // decrease-increase burst
                    }

                    else if (counter[1][i] <
                             (1 - (1 - 1 / k) * (1) / T_d) * counter[0][i] &&
                             counter[0][i] >= threshold) { // windows[i].t_d +
                        windows[i].t_d++;
//                        cnt[3][i] = counter[1][i];
                        // exceed the decrease time threshold
                        if (windows[i].t_d >= T_d) {
                            counter[0][i] = counter[1][i];
                            clear(i);
                        }

                    } else {
                        clear(i);
                        counter[0][i] = counter[1][i];
                    }
                    counter[1][i] = 0;
                    break;
                case 4:
                    if (counter[1][i] >= k * counter[0][i] && counter[1][i] >= threshold) {

                        // output decrease-increase burst
                        windows[i].t_i++;
                        Wave b(timestamp[i], windows[i].t_i, windows[i].t_l, windows[i].t_d, id[i], cnt[0][i],
                                cnt[1][i],
                                cnt[2][i], counter[1][i]);
                        Record_n.push_back(b);

                        fout << "n   " << b << "    " << endl;

                        uint8_t tmp = windows[i].t_i;
                        clear(i);

                        windows[i].t_i = tmp;
                        timestamp[i] = time - tmp;
                        counter_flag[i] = 2;
                        cnt[0][i] = counter[0][i];
                        cnt[1][i] = counter[1][i];

                        counter[0][i] = counter[1][i];
//                        counter[1][i] = 0;
                    }

                    else if (counter[1][i] >
                             ((1) * (k - 1) / (T_i) + 1) * counter[0][i] &&
                             counter[1][i] >= threshold / T_i) {
                        windows[i].t_i++;
                        // exceed the increase time threshold
                        if (windows[i].t_i >= T_i) {
                            counter[0][i] = counter[1][i];
                            clear(i);
                        }
                    } else {
                        clear(i);
                        counter[0][i] = counter[1][i];
//                        counter[1][i] = 0;
                    }
                    counter[1][i] = 0;
                    break;
            }
        }

    }

    void insert(uint64_t flow_id, int i) {
        if (F.find(flow_id) == F.end()) {
            id[w] = flow_id;
            F[flow_id] = w++;
        }
        counter[i][F[flow_id]]++;
    }

    void clear(uint32_t i) {
        counter_flag[i] = 0;
        windows[i].clear();
        timestamp[i] = -1;
    }
//    file.close();
};

#endif



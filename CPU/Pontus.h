#include "BOBHash32.h"
#include "param.h"
#include "Bucket.h"
#include <iomanip>

class Stage1 {
public:
    uint16_t *counter[2];
    uint64_t *id[2];
    BOBHash32 *bobhash[hash_num];
    int bucket_num;
    bool flag_count;

    Stage1() {}

    Stage1(int _size) {
        flag_count = 0;
        bucket_num = _size;
        counter[1] = new uint16_t[bucket_num];
        counter[0] = new uint16_t[bucket_num];
        id[0] = new uint64_t[bucket_num];
        id[1] = new uint64_t[bucket_num];

        for (int i = 0; i < hash_num; i++)
            bobhash[i] = new BOBHash32(i + 103);
    }

    void insert(uint64_t flow_id, int v = 1) {
        bool flag = true;
        int min_z = oo, min_val = oo, zero_z = oo, zero_j = oo;
        for (int i = 0; i < hash_num; i++) {
            int z = bobhash[i]->run((char *) &flow_id, 8) % bucket_num;
            if (id[flag_count][z] == flow_id) {
                counter[flag_count][z] += v;
                return;
            } else if (id[flag_count][z] == 0 && flag) {
                zero_z = z;
                flag = false;

            } else {
                if (counter[flag_count][z] < min_val) {
                    min_z = z;
                    min_val = counter[flag_count][z];
                }

            }
        }
        if (!flag) {
            id[flag_count][zero_z] = flow_id;
            counter[flag_count][zero_z] = v;

            return;
        }

        int q = rand(), a = q % (min_val + 1);

        if (a < v) {
            id[flag_count][min_z] = flow_id;
            counter[flag_count][min_z] = v;
        }
        return;
    }

    void clear(int i, int m = 1) {
        id[m][i] = 0;
        counter[m][i] = 0;
    }


    uint16_t get_value(uint64_t flow_id, int m) {
        for (int i = 0; i < hash_num; i++) {
            int z = bobhash[i]->run((char *) &flow_id, 8) % bucket_num;
            if (id[m][z] == flow_id) {
                return counter[m][z];
            }
        }
        return 0;
    }

    int get_position(uint64_t flow_id, int m) {
        for (int i = 0; i < hash_num; i++) {
            int z = bobhash[i]->run((char *) &flow_id, 8) % bucket_num;
            if (id[m][z] == flow_id) {
                return z;
            }
        }
        return -1;
    }

};

class MidStage {
public:
    int bucket_num;
    BOBHash32 *bobhash[hat_hashnum];
    Bucket *buckets;

    MidStage() {}

    MidStage(int _size) {
        bucket_num = _size;
        buckets = new Bucket[bucket_num];
        for (int i = 0; i < hat_hashnum; i++)
            bobhash[i] = new BOBHash32(i + 103);
    }

    int isInBucket(uint64_t flow_id) {
        for (int i = 0; i < bucket_num; i++)
            if (flow_id == buckets[i].id)
                return i;

        return -1;
    }

    int get_empty() {
        for (int i = 0; i < bucket_num; i++)
            if (buckets[i].id == 0)
                return i;
        return -1;
    }

    bool insert(uint64_t flow_id, int v = 1) {
        for (int i = 0; i < hat_hashnum; i++) {
            int z = bobhash[i]->run((char *) &flow_id, 8) % bucket_num;
            if (buckets[z].id == flow_id) {
                buckets[z].counter[1] += v;
                return true;
            }
        }
        return false;

    }

    bool insert(Bucket b) {
        bool flag = true;
        int min_z = oo, min_val = b.val, zero_z = oo;
        for (int i = 0; i < hat_hashnum; i++) {
            int z = bobhash[i]->run((char *) &b.id, 8) % bucket_num;

            if (buckets[z].id == 0) {
                zero_z = z;
                flag = false;
                break;
            }
            if (b.type == buckets[z].type) {
                if (buckets[z].val < min_val) {
                    min_z = z;
                    min_val = buckets[z].val;
                }
            }
        }

        if (!flag) {
            buckets[zero_z] = b;
            return true;
        }
        // evit
        if (min_z != oo) {
            buckets[min_z] = b;
            return true;
        }

        return false;
    }

    void clear(int i) {
        buckets[i].clear();
    }


};

class Pontus {
public:
    Stage1 s1;
    MidStage s2;
    MidStage s3;
    MidStage s4;
    vector<Wave> Record_p;
    vector<Wave> Record_n;

    Pontus() {}

    Pontus(int s1_size, int s2_size, int s3_size, int s4_size) : s1(s1_size), s2(s2_size), s3(s3_size), s4(s4_size) {}

    void insert(uint64_t flow_id) {

        if (s4.insert(flow_id))
            return;
        if (s3.insert(flow_id))
            return;
        if (s2.insert(flow_id))
            return;
        s1.insert(flow_id);
        return;
    }


    void update(uint16_t time) {
        vector<pair<uint64_t, uint16_t>> s;
        if (time == 2) {
            s1.flag_count ^= 1;
            return;
        }
//        cout << "--------------update----------------" << endl;
//        cout << time << endl;
        // update stage 4
        for (int i = 0; i < s4.bucket_num; i++) {
            if (s4.buckets[i].id == 0)
                continue;
            if (s4.buckets[i].counter[1] <= (1 / k) * s4.buckets[i].counter[0] &&
                s4.buckets[i].counter[0] >= threshold) {
//                cout<<"s4.buckets[i].counter[0]: "<<s4.buckets[i].counter[0]<<endl;
                s4.buckets[i].windows.t_d++;
                if (s4.buckets[i].type == 'p') {
                    Wave b(s4.buckets[i].timestamp, s4.buckets[i].windows.t_i, s4.buckets[i].windows.t_l,
                            s4.buckets[i].windows.t_d,
                            s4.buckets[i].id,
                            s4.buckets[i].start,
                            s4.buckets[i].start + s4.buckets[i].val, s4.buckets[i].counter[0],
                            s4.buckets[i].counter[1]);
                    Record_p.push_back(b);
                    fout_my << "p   " << b << "    " << endl;
                }

//                if (s3.insert(
//                        Bucket(s4.buckets[i].id, s4.buckets[i].counter[1], 20000, time - s4.buckets[i].windows.t_d,
//                               s4.buckets[i].counter[0] - s4.buckets[i].counter[1], s4.buckets[i].counter[0], 0, 0,
//                               s4.buckets[i].windows.t_d, 'n')))
//                    s4.clear(i);
//                else
//                    s.emplace_back(make_pair(s4.buckets[i].id, s4.buckets[i].counter[1]));

                if (!s3.insert(
                        Bucket(s4.buckets[i].id, s4.buckets[i].counter[1], 20000, time - s4.buckets[i].windows.t_d,
                               s4.buckets[i].counter[0] - s4.buckets[i].counter[1], s4.buckets[i].counter[0], 0, 0,
                               s4.buckets[i].windows.t_d, 'n')))
                    s.emplace_back(make_pair(s4.buckets[i].id, s4.buckets[i].counter[1]));
                s4.clear(i);




                // decrease-increase burst
            } else if (s4.buckets[i].counter[1] >= k * s4.buckets[i].counter[0] && s4.buckets[i].counter[1] >= threshold
                    ) {
                s4.buckets[i].windows.t_i++;

                if (s4.buckets[i].type == 'n') {
                    Wave b(s4.buckets[i].timestamp, s4.buckets[i].windows.t_i, s4.buckets[i].windows.t_l,
                            s4.buckets[i].windows.t_d,
                            s4.buckets[i].id,
                            s4.buckets[i].start,
                            s4.buckets[i].start - s4.buckets[i].val, s4.buckets[i].counter[0],
                            s4.buckets[i].counter[1]);
                    // output decrease-increase burst
                    Record_n.push_back(b);

                    fout_my << "n   " << b << "    " << endl;
                }

//                if (s3.insert(
//                        Bucket(s4.buckets[i].id, s4.buckets[i].counter[1], 20000, time - s4.buckets[i].windows.t_i,
//                               s4.buckets[i].counter[1] - s4.buckets[i].counter[0], s4.buckets[i].counter[0],
//                               s4.buckets[i].windows.t_i, 0,
//                               0, 'p')))
//                    s4.clear(i);
//                else
//                    s.emplace_back(make_pair(s4.buckets[i].id, s4.buckets[i].counter[1]));

                if (!s3.insert(
                        Bucket(s4.buckets[i].id, s4.buckets[i].counter[1], 20000, time - s4.buckets[i].windows.t_i,
                               s4.buckets[i].counter[1] - s4.buckets[i].counter[0], s4.buckets[i].counter[0],
                               s4.buckets[i].windows.t_i, 0,
                               0, 'p')))
                    s.emplace_back(make_pair(s4.buckets[i].id, s4.buckets[i].counter[1]));
                s4.clear(i);
            } else if (s4.buckets[i].counter[1] <
                       (1 - (1 - 1 / k) * (1) / T_d) * s4.buckets[i].counter[0] &&
                       s4.buckets[i].type == 'p' && s4.buckets[i].counter[0] >= threshold) { // windows[i].t_d +
                s4.buckets[i].windows.t_d++;
                // exceed the decrease time threshold
                if (s4.buckets[i].windows.t_d >= T_d) {
                    s.emplace_back(make_pair(s4.buckets[i].id, s4.buckets[i].counter[1]));
                    s4.clear(i);
                }

            } else if (s4.buckets[i].counter[1] >
                       ((1) * (k - 1) / (T_i) + 1) * s4.buckets[i].counter[0] &&
                       s4.buckets[i].type == 'n' && s4.buckets[i].counter[1] >= threshold / T_i) { // windows[i].t_i +
                s4.buckets[i].windows.t_i++;
                // exceed the increase time threshold
                if (s4.buckets[i].windows.t_i >= T_i) {
                    s.emplace_back(make_pair(s4.buckets[i].id, s4.buckets[i].counter[1]));
                    s4.clear(i);
                }
            } else {
                s.emplace_back(make_pair(s4.buckets[i].id, s4.buckets[i].counter[1]));
                s4.clear(i);
            }
            s4.buckets[i].counter[1] = 0;
        }

        // update stage 3
        for (int i = 0; i < s3.bucket_num; i++) {
            if (s3.buckets[i].id == 0 || s3.buckets[i].counter[1] == 20000) {
                s3.buckets[i].counter[1] = 0;
                continue;
            }

            if (s3.buckets[i].counter[1] <= (1 / k) * s3.buckets[i].counter[0] &&
                s3.buckets[i].counter[0] >= threshold) {

                if (s3.buckets[i].type == 'p') {
                    s3.buckets[i].windows.t_d = 1;
                    Wave b(s3.buckets[i].timestamp, s3.buckets[i].windows.t_i, s3.buckets[i].windows.t_l,
                            s3.buckets[i].windows.t_d,
                            s3.buckets[i].id,
                            s3.buckets[i].start,
                            s3.buckets[i].start + s3.buckets[i].val, s3.buckets[i].counter[0],
                            s3.buckets[i].counter[1]);
                    Record_p.push_back(b);
                    fout_my << "p   " << b << "    " << endl;
//                    cout << "p   " << b << "    " << endl;

                    s3.buckets[i].timestamp = time - 1;
                    s3.buckets[i].windows.t_l = 0;
                    s3.buckets[i].windows.t_i = 0;
                    s3.buckets[i].start = s3.buckets[i].counter[0];
                    s3.buckets[i].val = s3.buckets[i].counter[0] - s3.buckets[i].counter[1];
                    s3.buckets[i].type = 'n';

                    s3.buckets[i].counter[0] = s3.buckets[i].counter[1];
                } else {
                    if (s3.buckets[i].windows.t_l) {
                        s3.buckets[i].timestamp = time - 1;
                        s3.buckets[i].windows.t_d = 1;
                        s3.buckets[i].windows.t_l = 0;
                        s3.buckets[i].windows.t_i = 0;
                        s3.buckets[i].start = s3.buckets[i].counter[0];
                        s3.buckets[i].val = s3.buckets[i].counter[0] - s3.buckets[i].counter[1];
                        s3.buckets[i].type = 'n';

                        s3.buckets[i].counter[0] = s3.buckets[i].counter[1];
                    } else {
                        s3.buckets[i].windows.t_d++;
                        s3.buckets[i].val += s3.buckets[i].counter[0] - s3.buckets[i].counter[1];
                        s3.buckets[i].counter[0] = s3.buckets[i].counter[1];
                        if (s3.buckets[i].windows.t_d > T_d) {
                            s.emplace_back(make_pair(s3.buckets[i].id, s3.buckets[i].counter[1]));
                            s3.clear(i);
                        }


                    }
                }

            } else if (s3.buckets[i].counter[1] >= k * s3.buckets[i].counter[0] &&
                       s3.buckets[i].counter[1] >= threshold) {

                if (s3.buckets[i].type == 'n') {
//                    if (s3.buckets[i].start < threshold)
//                        cnt_n++;
                    s3.buckets[i].windows.t_i = 1;
                    Wave b(s3.buckets[i].timestamp, s3.buckets[i].windows.t_i, s3.buckets[i].windows.t_l,
                            s3.buckets[i].windows.t_d,
                            s3.buckets[i].id,
                            s3.buckets[i].start,
                            s3.buckets[i].start - s3.buckets[i].val, s3.buckets[i].counter[0],
                            s3.buckets[i].counter[1]);
                    // output decrease-increase burst
                    Record_n.push_back(b);

                    fout_my << "n   " << b << "    " << endl;

                    s3.buckets[i].timestamp = time - 1;
                    s3.buckets[i].windows.t_d = 0;
                    s3.buckets[i].windows.t_l = 0;
                    s3.buckets[i].start = s3.buckets[i].counter[0];
                    s3.buckets[i].val = s3.buckets[i].counter[1] - s3.buckets[i].counter[0];
                    s3.buckets[i].type = 'p';

                    s3.buckets[i].counter[0] = s3.buckets[i].counter[1];
                } else {
                    if (s3.buckets[i].windows.t_l) {
                        s3.buckets[i].timestamp = time - 1;
                        s3.buckets[i].windows.t_d = 0;
                        s3.buckets[i].windows.t_l = 0;
                        s3.buckets[i].windows.t_i = 1;
                        s3.buckets[i].start = s3.buckets[i].counter[0];
                        s3.buckets[i].val = s3.buckets[i].counter[1] - s3.buckets[i].counter[0];
                        s3.buckets[i].type = 'p';

                        s3.buckets[i].counter[0] = s3.buckets[i].counter[1];
                    } else {
                        s3.buckets[i].windows.t_i++;
                        s3.buckets[i].val += s3.buckets[i].counter[1] - s3.buckets[i].counter[0];
                        s3.buckets[i].counter[0] = s3.buckets[i].counter[1];
                        if (s3.buckets[i].windows.t_i > T_i) {
                            s.emplace_back(make_pair(s3.buckets[i].id, s3.buckets[i].counter[1]));
                            s3.clear(i);
                        }

                    }

                }

            } else if (s3.buckets[i].counter[1] < (1 - (1 - 1 / k) / T_d) * s3.buckets[i].counter[0] &&
                       s3.buckets[i].counter[0] >= threshold) {
                if (s3.buckets[i].type == 'p') {
                    s3.buckets[i].windows.t_d++;
//                    s3.buckets[i].counter[0] = s3.buckets[i].counter[1];
                    s3.buckets[i].counter[1] = 0;
//                    if (s4.insert(s3.buckets[i]))
//                        s3.clear(i);
//                    else
//                        s.emplace_back(make_pair(s3.buckets[i].id, s3.buckets[i].counter[1]));
                    if (!s4.insert(s3.buckets[i]))
                        s.emplace_back(make_pair(s3.buckets[i].id, s3.buckets[i].counter[1]));
                    s3.clear(i);

                } else if (!s3.buckets[i].windows.t_l) {
                    s3.buckets[i].windows.t_d++;
                    s3.buckets[i].val += s3.buckets[i].counter[0] - s3.buckets[i].counter[1];
                    s3.buckets[i].counter[0] = s3.buckets[i].counter[1];
                    if (s3.buckets[i].windows.t_d > T_d) {
                        s.emplace_back(make_pair(s3.buckets[i].id, s3.buckets[i].counter[1]));
                        s3.clear(i);
                    }
                } else {
                    s.emplace_back(make_pair(s3.buckets[i].id, s3.buckets[i].counter[1]));
                    s3.clear(i);
                }

            } else if (s3.buckets[i].counter[1] > (1 * (k - 1) / (T_i) + 1) * s3.buckets[i].counter[0] &&
                       s3.buckets[i].counter[1] >= threshold / T_i) {

                if (s3.buckets[i].type == 'n') {
                    s3.buckets[i].windows.t_i++;
//                    s3.buckets[i].counter[0] = s3.buckets[i].counter[1];
                    s3.buckets[i].counter[1] = 0;
                    if (!s4.insert(s3.buckets[i]))
                        s.emplace_back(make_pair(s3.buckets[i].id, s3.buckets[i].counter[1]));
                    s3.clear(i);
//                    if (s4.insert(s3.buckets[i]))
//                        s3.clear(i);
//                    else
//                        s.emplace_back(make_pair(s3.buckets[i].id, s3.buckets[i].counter[1]));

                } else if (!s3.buckets[i].windows.t_l) {
                    s3.buckets[i].windows.t_i++;
                    s3.buckets[i].val += s3.buckets[i].counter[1] - s3.buckets[i].counter[0];
                    s3.buckets[i].counter[0] = s3.buckets[i].counter[1];
                    if (s3.buckets[i].windows.t_i > T_i) {
                        s.emplace_back(make_pair(s3.buckets[i].id, s3.buckets[i].counter[1]));
                        s3.clear(i);
                    }
                } else {
                    s.emplace_back(make_pair(s3.buckets[i].id, s3.buckets[i].counter[1]));
                    s3.clear(i);
                }

            } else {
                s3.buckets[i].windows.t_l++;

                if (s3.buckets[i].windows.t_l > T) {
                    s.emplace_back(make_pair(s3.buckets[i].id, s3.buckets[i].counter[1]));
                    s3.clear(i);
                } else {
                    s3.buckets[i].counter[0] = s3.buckets[i].counter[1];
                }
            }
            s3.buckets[i].counter[1] = 0;
        }

        // update stage 2
        for (int i = 0; i < s2.bucket_num; i++) {
            if (s2.buckets[i].id == 0)
                continue;
            // exceed the increase threshold
            if (s2.buckets[i].counter[1] >= threshold && s2.buckets[i].counter[1] >= k * s2.buckets[i].counter[0]) {
//                cout << "s2->s3" << endl;
                if (s2.buckets[i].type == 'p') {
                    s2.buckets[i].windows.t_i++;
                    s2.buckets[i].val = s2.buckets[i].counter[1] - s2.buckets[i].counter[0];
                    s2.buckets[i].counter[0] = s2.buckets[i].counter[1];
                    s2.buckets[i].counter[1] = 0;
//                    if (s3.insert(s2.buckets[i]))
//                        s2.clear(i);
//                    else
//                        s.emplace_back(make_pair(s2.buckets[i].id, s2.buckets[i].counter[1]));
                    if (!s3.insert(s2.buckets[i]))
                        s.emplace_back(make_pair(s2.buckets[i].id, s2.buckets[i].counter[1]));
                    s2.clear(i);

                } else {
//                    if (s3.insert(Bucket(s2.buckets[i].id, s2.buckets[i].counter[1],
//                                         0, time - 1,
//                                         s2.buckets[i].counter[1] - s2.buckets[i].start + s2.buckets[i].val,
//                                         s2.buckets[i].counter[0] + s2.buckets[i].val, 1, 0, 0, 'p')))
//                        s2.clear(i);
//                    else
//                        s.emplace_back(make_pair(s2.buckets[i].id, s2.buckets[i].counter[1]));
                    if (!s3.insert(Bucket(s2.buckets[i].id, s2.buckets[i].counter[1],
                                          0, time - 1,
                                          s2.buckets[i].counter[1] - s2.buckets[i].start + s2.buckets[i].val,
                                          s2.buckets[i].counter[0] + s2.buckets[i].val, 1, 0, 0, 'p')))
                        s.emplace_back(make_pair(s2.buckets[i].id, s2.buckets[i].counter[1]));
                    s2.clear(i);


                }
                // the end of decrease
            } else if (s2.buckets[i].counter[1] <= 1 / k * s2.buckets[i].counter[0] &&
                       s2.buckets[i].counter[0] >= threshold) {
                if (s2.buckets[i].type == 'n') {
                    s2.buckets[i].windows.t_d++;
                    s2.buckets[i].val = s2.buckets[i].counter[0] - s2.buckets[i].counter[1];
                    s2.buckets[i].counter[0] = s2.buckets[i].counter[1];
                    s2.buckets[i].counter[1] = 0;
//                    if (s3.insert(s2.buckets[i]))
//                        s2.clear(i);
//                    else
//                        s.emplace_back(make_pair(s2.buckets[i].id, s2.buckets[i].counter[1]));
                    if (!s3.insert(s2.buckets[i]))
                        s.emplace_back(make_pair(s2.buckets[i].id, s2.buckets[i].counter[1]));
                    s2.clear(i);


                } else {

//                    if (s3.insert(Bucket(s2.buckets[i].id, s2.buckets[i].counter[1],
//                                         0, time - 1,
//                                         s2.buckets[i].start + s2.buckets[i].val - s2.buckets[i].counter[1],
//                                         s2.buckets[i].counter[0] + s2.buckets[i].val, 0, 0, 1, 'n')))
//                        s2.clear(i);
//                    else
//                        s.emplace_back(make_pair(s2.buckets[i].id, s2.buckets[i].counter[1]));
                    if (!s3.insert(Bucket(s2.buckets[i].id, s2.buckets[i].counter[1],
                                          0, time - 1,
                                          s2.buckets[i].start + s2.buckets[i].val - s2.buckets[i].counter[1],
                                          s2.buckets[i].counter[0] + s2.buckets[i].val, 0, 0, 1, 'n')))
                        s.emplace_back(make_pair(s2.buckets[i].id, s2.buckets[i].counter[1]));
                    s2.clear(i);

                }
                // increase
            } else if (s2.buckets[i].counter[1] > ((1) * (k - 1) / (T_i) + 1) * s2.buckets[i].counter[0] &&
                       s2.buckets[i].type == 'p' && s2.buckets[i].counter[1] >= threshold / T_i
                    ) { // windows[i].t_i +
                s2.buckets[i].windows.t_i++;
                s2.buckets[i].val = s2.buckets[i].counter[1] - s2.buckets[i].counter[0];
                // exceed the increase time threshold
                if (s2.buckets[i].windows.t_i >= T_i) {
                    s.emplace_back(make_pair(s2.buckets[i].id, s2.buckets[i].counter[1]));
                    s2.clear(i);
                }
                // decrease
            } else if (s2.buckets[i].counter[1] <
                       (1 - (1 - 1 / k) * (1) / T_d) * s2.buckets[i].counter[0] && s2.buckets[i].type == 'n') {
                // windows[i].t_d + ||
                s2.buckets[i].windows.t_d++;
                s2.buckets[i].val = s2.buckets[i].counter[0] - s2.buckets[i].counter[1];
                // exceed the decrease time threshold
                if (s2.buckets[i].windows.t_d >= T_d) {
                    s.emplace_back(make_pair(s2.buckets[i].id, s2.buckets[i].counter[1]));
                    s2.clear(i);
                }

                // the end of increase
            } else {
                s.emplace_back(make_pair(s2.buckets[i].id, s2.buckets[i].counter[1]));
                s2.clear(i);
            }

            s2.buckets[i].counter[1] = 0;
        }

        // update stage 1
        for (int i = 0; i < s1.bucket_num; i++) {
            if (s1.id[s1.flag_count][i] == 0)
                continue;
            // increase
            uint16_t pre_cnt;
            int z = s1.get_position(s1.id[s1.flag_count][i], s1.flag_count ^ 1);
            if (z == -1)
                pre_cnt = 0;
            else
                pre_cnt = s1.counter[s1.flag_count ^ 1][z];
            if (s1.counter[s1.flag_count][i] >= k * pre_cnt && s1.counter[s1.flag_count][i] >= threshold) {
//                cout << "p   insert s3" << endl;
                if (s3.insert(Bucket(s1.id[s1.flag_count][i], s1.counter[s1.flag_count][i],
                                     0, time - 1, s1.counter[s1.flag_count][i] - pre_cnt,
                                     pre_cnt, 1, 0, 0, 'p')))
                    s1.clear(i, s1.flag_count);

            } else if (s1.counter[s1.flag_count][i] <= 1 / k * pre_cnt && pre_cnt >= threshold) {
//                cout << "n  insert s3" << endl;
//                cout<<pre_cnt<<endl;
                if (s3.insert(Bucket(s1.id[s1.flag_count][i], s1.counter[s1.flag_count][i],
                                     0, time - 1, pre_cnt - s1.counter[s1.flag_count][i],
                                     pre_cnt, 0, 0, 1, 'n')))
                    s1.clear(i, s1.flag_count);
                //
            } else if (s1.counter[s1.flag_count][i] > ((k - 1) / (T_i) + 1) * pre_cnt &&
                       s1.counter[s1.flag_count][i] >= threshold / T_i) {
//                cout << "p  insert s2" << endl;
                if (s2.insert(Bucket(s1.id[s1.flag_count][i], pre_cnt,
                                     0, time - 1, s1.counter[s1.flag_count][i] - pre_cnt,
                                     pre_cnt, 1, 0, 0, 'p')))
                    s1.clear(i, s1.flag_count);
                // decrease
            } else if (s1.counter[s1.flag_count][i] < (1 - (1 - 1 / k) / T_d) * pre_cnt &&
                       pre_cnt >= threshold) {
//                cout << "n  insert s2" << endl;
//                cout<<pre_cnt<<endl;
                if (s2.insert(Bucket(s1.id[s1.flag_count][i], pre_cnt,
                                     0, time - 1, pre_cnt - s1.counter[s1.flag_count][i],
                                     pre_cnt, 0, 0, 1, 'n')))
                    s1.clear(i, s1.flag_count);

            }

            if (z != -1)
                s1.clear(z, s1.flag_count ^ 1);
        }

        for (int i = 0; i < s1.bucket_num; i++) {
            if (s1.id[s1.flag_count ^ 1][i] == 0)
                continue;
            if (s1.counter[s1.flag_count ^ 1][i] >= threshold) {
//                cout << "n  insert s3" << endl;
//                cout << s1.counter[s1.flag_count ^ 1][i] << endl;
                s3.insert(Bucket(s1.id[s1.flag_count ^ 1][i], 0,
                                 0, time - 1, s1.counter[s1.flag_count ^ 1][i] - 0,
                                 s1.counter[s1.flag_count ^ 1][i], 0, 0, 1, 'n'));

            }
            s1.clear(i, s1.flag_count ^ 1);
        }

        for (int i = 0, len = s.size(); i < len; i++) {
            s1.insert(s[i].first, s[i].second);
        }

        s1.flag_count ^= 1;

    }

    void print_sketch() {
        cout << "---------------------s1---------------------------" << endl;
        for (int i = 0; i < s1.bucket_num; i++) {
            cout << setw(32) << s1.id[s1.flag_count ^ 1][i] << "  " << setw(32) << s1.counter[s1.flag_count ^ 1][i]
                 << "  "
                 << setw(32) << s1.id[s1.flag_count][i] << "  " << setw(32) << s1.counter[s1.flag_count][i] << endl;
        }
        cout << "---------------------s2---------------------------" << endl;
        for (int i = 0; i < s2.bucket_num; i++) {
            cout << s2.buckets[i] << endl;
        }
        cout << "---------------------s3---------------------------" << endl;
        for (int i = 0; i < s3.bucket_num; i++) {
            cout << s3.buckets[i] << endl;
        }
        cout << "---------------------s4---------------------------" << endl;
        for (int i = 0; i < s4.bucket_num; i++) {
            cout << s4.buckets[i] << endl;
        }
        cout << "--------------------end----------------------------" << endl;
    }


};





#include <queue>
#include "BOBHash32.h"
#include "param.h"
#include "Wave.h"
#include <cstdio>
#include <string.h>
#include "Bucket.h"

class CMsketch {
public:
    uint32_t *v;
    int size;
    BOBHash32 *bobhash[hash_num];

    CMsketch() {};

    CMsketch(int _size) {
        size = _size;
        v = new uint32_t[size];
        memset(v, 0, size * sizeof(uint32_t));
        for (int i = 0; i < hash_num; i++)
            bobhash[i] = new BOBHash32(101 + i);
    }

    void clear() {
        memset(v, 0, size * sizeof(uint32_t));
    }

    void insert(uint64_t flow_id) {
        for (int i = 0; i < hash_num; i++)
            v[bobhash[i]->run((char *) &flow_id, 8) % size]++;
    }

    void insert(char *flow_id) {
        for (int i = 0; i < hash_num; i++)
            v[bobhash[i]->run(flow_id, 8) % size]++;
    }

    uint32_t query(uint64_t flow_id) {
        uint32_t ret = oo;
        for (int i = 0; i < hash_num; i++)
            ret = min(ret, v[bobhash[i]->run((char *) &flow_id, 8) % size]);
        return ret;
    }
};

class Strawman {
public:
    int size, now, bucket_num;
    CMsketch *cm[lens];
    vector<Wave> Record_p;
    vector<Wave> Record_n;
    Bucket *buckets;

    Strawman() {};

    Strawman(int _size, int bucket_num) {
        now = 0;
        size = _size;
        this->bucket_num = bucket_num;
        for (int i = 0; i < (lens); i++) {
            cm[i] = new CMsketch(size);
        }
        buckets = new Bucket[bucket_num];
        Record_p.clear();
        Record_n.clear();
    }

    int detect(uint64_t id, uint32_t val, char &t, int &pre) {
        // check positive wave
        for (int i = 1; i <= T_i; i++) {
            uint32_t pre_cnt = cm[(now + lens - i) % (lens)]->query(id);
            if (val >= k * pre_cnt) {
                t = 'p';
                pre = pre_cnt;
                return i;
            }

        }

        // negative wave
        t = 'n';
        pre = 0;
        return 0;

    }

    void insert(uint64_t id, uint32_t timestamp) {
        bool flag = true;
        for (int i = 0; i < bucket_num; i++)
            if (buckets[i].id == id) {
                flag = false;
                if (buckets[i].timestamp == timestamp) {
                    buckets[i].counter[1]++;
                    return;
                }
                break;
            }

        cm[now]->insert(id);
        if (!flag)
            return;
        int q = cm[now]->query(id);
        if (q >= threshold) {
            char type;
            int pre_cnt, t_in = detect(id, q, type, pre_cnt);
            if (type == 'p') {
//                positive_insert++;
                if (timestamp - t_in > 0)
                    insert(Bucket(id, 3, q, timestamp - t_in, q - pre_cnt, pre_cnt, t_in, 0, 0, type), type);
            } else {
//                negative_insert++;
                insert(Bucket(id, 1, q, timestamp, 0, q, 0, 0, 0, type), type);
            }
        }
    }

    int insert(Bucket b, char type) {
        int s, e;
        if (type == 'p') {
            s = 0;
            e = bucket_num / 2;
        } else {
            s = bucket_num / 2;
            e = bucket_num;
        }
        for (int i = s; i < e; i++) {
            if (buckets[i].id == 0) {
                buckets[i] = b;
                return 1;
            }
        }
        return 0;
    }

    void update(uint32_t timestamp) {
        for (int i = 0; i < bucket_num; i++) {
            int q = cm[now]->query(buckets[i].id);
            if (buckets[i].type == 'p') {
                if (q <= 1 / k * buckets[i].counter[1]) {
                    buckets[i].windows.t_d++;
                    Wave b(buckets[i].timestamp, buckets[i].windows.t_i, buckets[i].windows.t_l,
                            buckets[i].windows.t_d, buckets[i].id, buckets[i].start,
                            buckets[i].start + buckets[i].val, buckets[i].counter[1], q);
                    Record_p.push_back(b);
                    fout_my << "p   " << b << "    " << endl;
//                    buckets[i].clear();
                    buckets[i].timestamp = timestamp - 1;
                    buckets[i].start = buckets[i].counter[1];
                    buckets[i].val = buckets[i].counter[1] - q;
                    buckets[i].counter[1] = q;
                    buckets[i].windows.t_i = 0;
                    buckets[i].windows.t_l = 0;
                    buckets[i].type = 'n';
                } else if (q < (1 - (1 - 1 / k) * (1) / T_d) * buckets[i].counter[1]) {
                    buckets[i].windows.t_d++;
                    if (buckets[i].windows.t_d >= T_d)
                        buckets[i].clear();
                } else if (q > ((1) * (k - 1) / (T_i) + 1) * buckets[i].counter[1] || q >= k * buckets[i].counter[1]) {
                    buckets[i].clear();
                } else if (!buckets[i].windows.t_d) {
                    buckets[i].windows.t_l++;
                    buckets[i].counter[1] = q;
                    if (buckets[i].windows.t_l > T)
                        buckets[i].clear();
                } else {
                    buckets[i].clear();
                }
            } else {
                switch (buckets[i].counter[0]) {
                    case 1:
                        if (q <= 1 / k * buckets[i].counter[1]) {
                            buckets[i].counter[0] = 3;
                            buckets[i].windows.t_d++;
                            buckets[i].start = buckets[i].counter[1];
                            buckets[i].val = buckets[i].counter[1] - q;
                            buckets[i].counter[1] = q;
                        } else if (q < (1 - (1 - 1 / k) * (1) / T_d) * buckets[i].counter[1]) {
                            buckets[i].windows.t_d++;
                            if (buckets[i].windows.t_d >= T_d)
                                buckets[i].clear();
                        } else {
                            buckets[i].clear();
                        }
                        break;

                    case 3:
                        if (q >= k * buckets[i].counter[1] && q >= threshold) {
                            buckets[i].windows.t_i++;
                            Wave b(buckets[i].timestamp, buckets[i].windows.t_i, buckets[i].windows.t_l,
                                    buckets[i].windows.t_d, buckets[i].id, buckets[i].start,
                                    buckets[i].start - buckets[i].val, buckets[i].counter[1], q);
                            Record_n.push_back(b);
                            fout_my << "n   " << b << "    " << endl;
//                            buckets[i].clear();
                            buckets[i].timestamp = timestamp - 1;
                            buckets[i].start = buckets[i].counter[1];
                            buckets[i].val = q - buckets[i].counter[1];
                            buckets[i].counter[1] = q;
                            buckets[i].windows.t_l = 0;
                            buckets[i].windows.t_d = 0;
                            buckets[i].type = 'p';

                        } else if (q > ((1) * (k - 1) / (T_i) + 1) * buckets[i].counter[1]) {
                            buckets[i].windows.t_i++;
                            if (buckets[i].windows.t_i >= T_i)
                                buckets[i].clear();
                        } else if (q < (1 - (1 - 1 / k) * (1) / T_d) * buckets[i].counter[1] ||
                                   q <= 1 / k * buckets[i].counter[1]) {
                            buckets[i].clear();

                        } else if (!buckets[i].windows.t_i) {
                            buckets[i].windows.t_l++;
                            buckets[i].counter[1] = q;
                            if (buckets[i].windows.t_l > T)
                                buckets[i].clear();
                        } else {
                            buckets[i].clear();
                        }
                        break;

                }
            }


        }

        now = (now + 1) % (lens);
        cm[now]->clear();

    }

    void print_bucket(uint64_t id) {
        cout << "cm query: " << cm[now]->query(id);
        cout << "------------------start-------------------" << endl;
        for (int i = 0; i < bucket_num; i++) {
            cout << buckets[i] << endl;
        }
        cout << "-----------------finish-------------------" << endl;
    }

};

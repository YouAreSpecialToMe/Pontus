#include "WaveDetector.h"
#include "Pontus.h"
#include "Strawman.h"
#include "ctime"
#include <cmath>


#include "iostream"

using namespace std;
map<uint64_t, int> D;
vector<pair<int, int> > S[packet_num];
int w = 0;
int main() {

    CorrectBurstDetector B(threshold);
    Pontus ms(1800, 100, 200, 100);

    cout << "Pontus memory cost: "
         << (ms.s1.bucket_num * 98 + (ms.s2.bucket_num + ms.s3.bucket_num + ms.s4.bucket_num) * 121) / 1024 << endl;
    long long int window = 1, cnt = 0;
    for (long long i = 0; i < packet_num; i++) {
        uint64_t tim, id;
        fin.read((char *) &id, sizeof(char) * 8);
        cnt++;
        if (cnt > window_num) {
            cnt = 0;
            window++;
            ms.update(window);
            if (window > 2) {
                B.update(window);
            }

        }

        ms.insert(id);
        if (window == 1) {
            B.insert(id, 0);
        } else
            B.insert(id, 1);
    }

    printf("There exists %lld positive waves.\n", B.Record_p.size());
    printf("There exists %lld negitive waves.\n", B.Record_n.size());
    printf("Actually there exist %d waves!\n", B.Record_n.size() + B.Record_p.size());

    printf("There exists %lld positive waves in Pontus.\n", ms.Record_p.size());
    printf("There exists %lld negitive waves in Pontus.\n", ms.Record_n.size());
    printf("Actually there exist %d waves in Pontus!\n", ms.Record_n.size() + ms.Record_p.size());


    int Pontus_p = 0;
    double are_pontus_p = 0;
    int Pontus_size_p = ms.Record_p.size(), Pontus_size_n = ms.Record_n.size();
    for (int i = 0, len = B.Record_p.size(); i < len; i++) {
        for (auto it = ms.Record_p.begin(); it != ms.Record_p.end();) {
            if (*it == B.Record_p[i]) {
                Pontus_p++;
                int a = B.Record_p[i].cnt1, b = B.Record_p[i].cnt2, c = B.Record_p[i].cnt3, d = B.Record_p[i].cnt4;
                if (!B.Record_p[i].cnt1)
                    a = 1;
                if (!B.Record_p[i].cnt2)
                    b = 1;
                if (!B.Record_p[i].cnt3)
                    c = 1;
                if (!B.Record_p[i].cnt4)
                    d = 1;
                are_pontus_p = are_pontus_p + (abs(it->cnt1 - B.Record_p[i].cnt1) / a +
                                               abs(it->cnt2 - B.Record_p[i].cnt2) / b +
                                               abs(it->cnt3 - B.Record_p[i].cnt3) / c +
                                               abs(it->cnt4 - B.Record_p[i].cnt4) / d);
                it = ms.Record_p.erase(it);

                break;
            } else {
                ++it;
            }
        }

    }

    are_pontus_p += (B.Record_p.size() - Pontus_p) * 4;
    are_pontus_p /= B.Record_p.size();
    int Pontus_n = 0;
    double are_pontus_n = 0;
//    cout << are_pontus_n << endl;
    for (int i = 0, len = B.Record_n.size(); i < len; i++) {
        for (auto it = ms.Record_n.begin(); it != ms.Record_n.end();) {
            if (*it == B.Record_n[i]) {
                int a = B.Record_n[i].cnt1, b = B.Record_n[i].cnt2, c = B.Record_n[i].cnt3, d = B.Record_n[i].cnt4;
                if (!B.Record_n[i].cnt1)
                    a = 1;
                if (!B.Record_n[i].cnt2)
                    b = 1;
                if (!B.Record_n[i].cnt3)
                    c = 1;
                if (!B.Record_n[i].cnt4)
                    d = 1;
                are_pontus_n += (abs(it->cnt1 - B.Record_n[i].cnt1) / a +
                                 abs(it->cnt2 - B.Record_n[i].cnt2) / b +
                                 abs(it->cnt3 - B.Record_n[i].cnt3) / c +
                                 abs(it->cnt4 - B.Record_n[i].cnt4) / d);
                Pontus_n++;
                it = ms.Record_n.erase(it);
                break;
            } else {
//                are_pontus_n += 4;
                ++it;
            }
        }
    }
    are_pontus_n += (B.Record_n.size() - Pontus_n) * 4;
    are_pontus_n /= B.Record_n.size();

    // RR PR of Pontus
    printf("Postive wave precision: %.5lf\n", 100.0 * (Pontus_p) / Pontus_size_p);
    printf("Postive wave recall: %.5lf\n", 100.0 * (Pontus_p) / B.Record_p.size());
    printf("Negative wave precision: %.5lf\n", 100.0 * (Pontus_n) / Pontus_size_n);
    printf("Negative wave recall: %.5lf\n", 100.0 * (Pontus_n) / B.Record_n.size());
    printf("Total wave precision: %.5lf\n", 100.0 * (Pontus_n + Pontus_p) / (Pontus_size_p + Pontus_size_n));
    printf("Total wave recall: %.5lf\n", 100.0 * (Pontus_n + Pontus_p) / (B.Record_n.size() + B.Record_p.size()));

    printf("Postive wave ARE: %.10lf\n", are_pontus_p);
    printf("Negative wave ARE: %.10lf\n", are_pontus_n);

    return 0;
}

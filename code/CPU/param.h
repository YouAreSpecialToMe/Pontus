#include <fstream>
#include"BOBHash32.h"

#define hash_num 3 // number of hash functions in S1
#define hat_hashnum 5 // number of hash functions in S2 and S3
#define window_num 10000
#define oo 1000000000
#define T 10
#define threshold 10  // the wave thresold
// caida_b 85000000 univ 20000000 caida_a 36000000
#define packet_num 20000000
#define T_i 3
#define T_d 3
#define k 2.0
#define lens T_i + T + T_d


ifstream fin("Your Input data path", ios::binary);
fstream fout("Wave Detector output waves path", ios::out | ios::app);
fstream fout_my("Pontus output waves path", ios::out | ios::app);


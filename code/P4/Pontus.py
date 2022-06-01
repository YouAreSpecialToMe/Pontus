from Firtst_Stage import *
from Mid_Stage import *
from Include import *


class Pontus(object):
    """Pontus P4"""

    def __init__(self, s1_bucket_num, s2_bucket_num, s3_bucket_num, s4_bucket_num, hash_num=HASH_NUM):
        super(Pontus, self).__init__()
        self.Stage1 = First_Stage(s1_bucket_num, hash_num)
        self.Stage2 = Mid_Stage(s2_bucket_num)
        self.Stage3 = Mid_Stage(s3_bucket_num)
        self.Stage4 = Mid_Stage(s4_bucket_num)
        self.Record_p = []
        self.Record_n = []

    def insert(self, flow_id):
        if self.Stage4.insert_flow(flow_id):
            return
        if self.Stage3.insert_flow(flow_id):
            return
        if self.Stage2.insert_flow(flow_id):
            return
        self.Stage1.insert(flow_id)
        return

    def update(self, reg_data, time):
        """"""
        "Insert in Stage1"
        s1_insert = []

        """Update Stage4"""
        s4_id = reg_data['s4_id']
        s4_counter = reg_data['s4_counter']
        for i in range(self.Stage4.bucket_num):
            if self.Stage4.buckets[i].key == 0:
                continue

            pre_cnt = self.Stage4.buckets[i].val
            cur_cnt = s4_counter[i]

            "Four Situations"
            if cur_cnt >= k * pre_cnt and cur_cnt >= THRESHOLD:
                self.Stage4.buckets[i].window.t_i += 1
                if self.Stage4.buckets[i].type == 'n':
                    self.Record_n.append(Burst(self.Stage4.buckets[i].timestamp, self.Stage4.buckets[i].window.t_i,
                                               self.Stage4.buckets[i].window.t_l, self.Stage4.buckets[i].window.t_d,
                                               self.Stage4.buckets[i].key, self.Stage4.buckets[i].start,
                                               self.Stage4.buckets[i].start - self.Stage4.buckets[i].var, pre_cnt,
                                               cur_cnt))

                if not self.Stage3.insert(
                        Bucket(self.Stage4.buckets[i].key, cur_cnt, time - self.Stage4.buckets[i].window.t_i,
                               cur_cnt - pre_cnt, pre_cnt, self.Stage4.buckets[i].window.t_i, -1, 0, 'p')):
                    s1_insert.append(CounterWithKey(self.Stage4.buckets[i].key, cur_cnt))
                self.Stage4.buckets[i].clear()

            elif cur_cnt <= 1 / k * pre_cnt and pre_cnt >= THRESHOLD:
                self.Stage4.buckets[i].window.t_d += 1
                if self.Stage4.buckets[i].type == 'p':
                    self.Record_p.append(Burst(self.Stage4.buckets[i].timestamp, self.Stage4.buckets[i].window.t_i,
                                               self.Stage4.buckets[i].window.t_l, self.Stage4.buckets[i].window.t_d,
                                               self.Stage4.buckets[i].key, self.Stage4.buckets[i].start,
                                               self.Stage4.buckets[i].start + self.Stage4.buckets[i].var, pre_cnt,
                                               cur_cnt))

                if not self.Stage3.insert(
                        Bucket(self.Stage4.buckets[i].key, cur_cnt, time - self.Stage4.buckets[i].window.t_d,
                               pre_cnt - cur_cnt, pre_cnt, 0, -1, self.Stage4.buckets[i].window.t_d
                            , 'n')):
                    s1_insert.append(CounterWithKey(self.Stage4.buckets[i].key, cur_cnt))
                self.Stage4.buckets[i].clear()

            elif cur_cnt < (1 - (1 - 1 / k) * (1) / T_d) * pre_cnt and self.Stage4.buckets[
                i].type == 'p' and pre_cnt >= THRESHOLD:
                self.Stage4.buckets[i].window.t_d += 1
                if self.Stage4.buckets[i].window.t_d >= T_d:
                    s1_insert.append(CounterWithKey(self.Stage4.buckets[i].key, cur_cnt))
                    self.Stage4.buckets[i].clear()

            elif cur_cnt > ((1) * (k - 1) / (T_i) + 1) * pre_cnt and self.Stage4.buckets[
                i].type == 'n' and cur_cnt >= THRESHOLD / T_i:
                if self.Stage4.buckets[i].window.t_i >= T_i:
                    s1_insert.append(CounterWithKey(self.Stage4.buckets[i].key, cur_cnt))
                    self.Stage4.buckets[i].clear()
            else:
                s1_insert.append(CounterWithKey(self.Stage4.buckets[i].key, cur_cnt))
                self.Stage4.buckets[i].clear()

        """"Update Stage3"""
        s3_id = reg_data['s3_id']
        s3_counter = reg_data['s3_counter']
        for i in range(self.Stage3.bucket_num):

            if self.Stage3.buckets[i].window.t_l == -1 or self.Stage3.buckets[i].key == 0:
                self.Stage3.buckets[i].window.t_l = 0
                continue

            pre_cnt = self.Stage3.buckets[i].val
            cur_cnt = s3_counter[i]

            "Four Situations"
            if cur_cnt >= k * pre_cnt and cur_cnt >= THRESHOLD:
                if self.Stage3.buckets[i].type == 'n':
                    self.Stage3.buckets[i].window.t_i = 1
                    self.Record_n.append(Burst(self.Stage3.buckets[i].timestamp, self.Stage3.buckets[i].window.t_i,
                                               self.Stage3.buckets[i].window.t_l, self.Stage3.buckets[i].window.t_d,
                                               self.Stage3.buckets[i].key, self.Stage3.buckets[i].start,
                                               self.Stage3.buckets[i].start - self.Stage3.buckets[i].var, pre_cnt,
                                               cur_cnt))
                    self.Stage3.buckets[i].timestamp = time - 1
                    self.Stage3.buckets[i].window.t_d = 0
                    self.Stage3.buckets[i].window.t_l = 0
                    self.Stage3.buckets[i].start = pre_cnt
                    self.Stage3.buckets[i].var = cur_cnt - pre_cnt
                    self.Stage3.buckets[i].type == 'p'
                    self.Stage3.buckets[i].val = cur_cnt
                else:
                    if self.Stage3.buckets[i].window.t_l:
                        self.Stage3.buckets[i].timestamp = time - 1
                        self.Stage3.buckets[i].window.t_d = 0
                        self.Stage3.buckets[i].window.t_l = 0
                        self.Stage3.buckets[i].window.t_i = 1
                        self.Stage3.buckets[i].start = pre_cnt
                        self.Stage3.buckets[i].var = cur_cnt - pre_cnt
                        self.Stage3.buckets[i].type == 'p'
                        self.Stage3.buckets[i].val = cur_cnt
                    else:
                        self.Stage3.buckets[i].window.t_i += 1
                        self.Stage3.buckets[i].var += cur_cnt - pre_cnt
                        self.Stage3.buckets[i].val = cur_cnt
                        if self.Stage3.buckets[i].window.t_i > T_i:
                            s1_insert.append(CounterWithKey(self.Stage3.buckets[i].key, cur_cnt))
                            self.Stage3.buckets[i].clear()

            elif cur_cnt <= 1 / k * pre_cnt and pre_cnt >= THRESHOLD:
                if self.Stage3.buckets[i].type == 'p':
                    self.Stage3.buckets[i].window.t_d = 1
                    self.Record_p.append(Burst(self.Stage3.buckets[i].timestamp, self.Stage3.buckets[i].window.t_i,
                                               self.Stage3.buckets[i].window.t_l, self.Stage3.buckets[i].window.t_d,
                                               self.Stage3.buckets[i].key, self.Stage3.buckets[i].start,
                                               self.Stage3.buckets[i].start + self.Stage3.buckets[i].val, pre_cnt,
                                               cur_cnt))
                    self.Stage3.buckets[i].timestamp = time - 1
                    self.Stage3.buckets[i].window.t_i = 0
                    self.Stage3.buckets[i].window.t_l = 0
                    self.Stage3.buckets[i].start = pre_cnt
                    self.Stage3.buckets[i].var = pre_cnt - cur_cnt
                    self.Stage3.buckets[i].type == 'n'
                    self.Stage3.buckets[i].val = cur_cnt
                else:
                    if self.Stage3.buckets[i].window.t_l:
                        self.Stage3.buckets[i].timestamp = time - 1
                        self.Stage3.buckets[i].window.t_i = 0
                        self.Stage3.buckets[i].window.t_l = 0
                        self.Stage3.buckets[i].window.t_d = 1
                        self.Stage3.buckets[i].start = pre_cnt
                        self.Stage3.buckets[i].var = pre_cnt - cur_cnt
                        self.Stage3.buckets[i].type == 'n'
                        self.Stage3.buckets[i].val = cur_cnt
                    else:
                        self.Stage3.buckets[i].window.t_d += 1
                        self.Stage3.buckets[i].var += pre_cnt - cur_cnt
                        self.Stage3.buckets[i].val = cur_cnt
                        if self.Stage3.buckets[i].window.t_d > T_d:
                            s1_insert.append(CounterWithKey(self.Stage3.buckets[i].key, cur_cnt))
                            self.Stage3.buckets[i].clear()

            elif cur_cnt < (1 - (1 - 1 / k) / T_d) * pre_cnt and pre_cnt >= THRESHOLD:
                if self.Stage3.buckets[i].type == 'p':
                    self.Stage3.buckets[i].window.t_d += 1
                    if not self.Stage4.insert(self.Stage3.buckets[i]):
                        s1_insert.append(CounterWithKey(self.Stage3.buckets[i].key, cur_cnt))
                    self.Stage3.buckets[i].clear()
                elif not self.Stage3.buckets[i].window.t_l:
                    self.Stage3.buckets[i].window.t_d += 1
                    self.Stage3.buckets[i].var += pre_cnt - cur_cnt
                    self.Stage3.buckets[i].val = cur_cnt
                    if self.Stage3.buckets[i].window.t_d > T_d:
                        s1_insert.append(CounterWithKey(self.Stage3.buckets[i].key, cur_cnt))
                        self.Stage3.buckets[i].clear()
                else:
                    s1_insert.append(CounterWithKey(self.Stage3.buckets[i].key, cur_cnt))
                    self.Stage3.buckets[i].clear()

            elif cur_cnt > (1 * (k - 1) / (T_i) + 1) * pre_cnt and cur_cnt >= THRESHOLD / T_i:
                if self.Stage3.buckets[i].type == 'n':
                    self.Stage3.buckets[i].window.t_i += 1
                    if not self.Stage4.insert(self.Stage3.buckets[i]):
                        s1_insert.append(CounterWithKey(self.Stage3.buckets[i].key, cur_cnt))
                    self.Stage3.buckets[i].clear()
                elif not self.Stage3.buckets[i].window.t_l:
                    self.Stage3.buckets[i].window.t_i += 1
                    self.Stage3.buckets[i].var += cur_cnt - pre_cnt
                    self.Stage3.buckets[i].val = cur_cnt
                    if self.Stage3.buckets[i].window.t_i > T_i:
                        s1_insert.append(CounterWithKey(self.Stage3.buckets[i].key, cur_cnt))
                        self.Stage3.buckets[i].clear()
                else:
                    s1_insert.append(CounterWithKey(self.Stage3.buckets[i].key, cur_cnt))
                    self.Stage3.buckets[i].clear()

            else:
                self.Stage3.buckets[i].window.t_l += 1
                if self.Stage3.buckets[i].window.t_l > T:
                    s1_insert.append(CounterWithKey(self.Stage3.buckets[i].key, cur_cnt))
                    self.Stage3.buckets[i].clear()
                else:
                    self.Stage3.buckets[i].val = cur_cnt

        """update Stage2"""
        s2_id = reg_data['s2_id']
        s2_counter = reg_data['s2_counter']
        for i in range(self.Stage2.bucket_num):

            if self.Stage2.buckets[i].key == 0:
                continue

            pre_cnt = self.Stage2.buckets[i].val
            cur_cnt = s2_counter[i]

            "Four Situations"
            if cur_cnt >= k * pre_cnt and cur_cnt >= THRESHOLD:
                if self.Stage2.buckets[i].type == 'p':
                    self.Stage2.buckets[i].window.t_i += 1
                    self.Stage2.buckets[i].var = cur_cnt - pre_cnt
                    self.Stage2.buckets[i].val = cur_cnt

                    if not self.Stage3.insert(self.Stage2.buckets[i]):
                        s1_insert.append(CounterWithKey(self.Stage2.buckets[i].key, cur_cnt))
                    self.Stage2.buckets[i].clear()

                else:
                    if not self.Stage3.insert(
                            Bucket(self.Stage2.buckets[i].key, cur_cnt, time - 1,
                                   cur_cnt - self.Stage2.buckets[i].start + self.Stage2.buckets[i].var,
                                   pre_cnt + self.Stage2.buckets[i].var, 1, 0, 0,
                                   'p')):
                        s1_insert.append(CounterWithKey(self.Stage2.buckets[i].key, cur_cnt))
                    self.Stage2.buckets[i].clear()

            elif cur_cnt <= 1 / k * pre_cnt and pre_cnt >= THRESHOLD:
                if self.Stage2.buckets[i].type == 'n':
                    self.Stage2.buckets[i].window.t_d += 1
                    self.Stage2.buckets[i].var = pre_cnt - cur_cnt
                    self.Stage2.buckets[i].val = cur_cnt

                    if not self.Stage3.insert(self.Stage2.buckets[i]):
                        s1_insert.append(CounterWithKey(self.Stage2.buckets[i].key, cur_cnt))
                    self.Stage2.buckets[i].clear()

                else:

                    if not self.Stage3.insert(
                            Bucket(self.Stage2.buckets[i].key, cur_cnt, time - 1,
                                   self.Stage2.buckets[i].start + self.Stage2.buckets[i].var - cur_cnt,
                                   pre_cnt + self.Stage2.buckets[i].var, 0, 0, 1,
                                   'n')):
                        s1_insert.append(CounterWithKey(self.Stage2.buckets[i].key, cur_cnt))
                    self.Stage2.buckets[i].clear()

            elif cur_cnt > ((1) * (k - 1) / (T_i) + 1) * pre_cnt and self.Stage2.buckets[
                i].type == 'p' and cur_cnt >= THRESHOLD / T_i:
                self.Stage2.buckets[i].window.t_i += 1
                self.Stage2.buckets[i].var = cur_cnt - pre_cnt
                if self.Stage2.buckets[i].window.t_i >= T_i:
                    s1_insert.append(CounterWithKey(self.Stage2.buckets[i].key, cur_cnt))
                    self.Stage2.buckets[i].clear()

            elif cur_cnt < (1 - (1 - 1 / k) * (1) / T_d) * pre_cnt and self.Stage2.buckets[
                i].type == 'n':
                self.Stage2.buckets[i].window.t_d += 1
                self.Stage2.buckets[i].var = pre_cnt - cur_cnt
                if self.Stage2.buckets[i].window.t_d >= T_d:
                    s1_insert.append(CounterWithKey(self.Stage2.buckets[i].key, cur_cnt))
                    self.Stage2.buckets[i].clear()

            else:
                s1_insert.append(CounterWithKey(self.Stage2.buckets[i].key, cur_cnt))
                self.Stage2.buckets[i].clear()

        """update Stage1"""
        s1_d1_id = reg_data['s1_d1_id']
        s1_d1_counter = reg_data['s1_d1_counter']
        s1_d2_id = reg_data['s1_d2_id']
        s1_d2_counter = reg_data['s1_d2_counter']
        s1_d3_id = reg_data['s1_d3_id']
        s1_d3_counter = reg_data['s1_d3_counter']
        s1_id = []
        for tmp_list in [s1_d1_id, s1_d2_id, s1_d3_id]:
            s1_id.append(tmp_list)
        s1_counters = []
        for tmp_list in [s1_d1_counter, s1_d2_counter, s1_d3_counter]:
            s1_counters.append(tmp_list)

        tmp_s1 = []
        for l in range(self.Stage1.hash_num):
            tmp_s1.append([])

        for i in range(self.Stage1.hash_num):
            for j in range(self.Stage1.bucket_num):
                if s1_id[i][j] == 0:
                    tmp_s1[i].append(CounterWithKey())
                    continue
                idx, z = self.Stage1.get_position(s1_id[i][j])
                if z == -1:
                    pre_cnt = 0
                else:
                    pre_cnt = self.Stage1.buckets[idx][z].val
                cur_cnt = s1_counters[i][j]

                "Four Situations"
                if cur_cnt >= k * pre_cnt and cur_cnt >= THRESHOLD:
                    if self.Stage3.insert(
                            Bucket(s1_id[i][j], cur_cnt, time - 1, cur_cnt - pre_cnt, pre_cnt, 1, 0, 0, 'p')):
                        s1_id[i][j] = 0
                        tmp_s1[i].append(CounterWithKey())
                    else:
                        tmp_s1[i].append(CounterWithKey(s1_id[i][j], s1_counters[i][j]))

                elif cur_cnt <= 1 / k * pre_cnt and pre_cnt >= THRESHOLD:
                    if self.Stage3.insert(
                            Bucket(s1_id[i][j], cur_cnt, time - 1, pre_cnt - cur_cnt, pre_cnt, 0, 0, 1, 'n')):
                        s1_id[i][j] = 0
                        tmp_s1[i].append(CounterWithKey())
                    else:
                        tmp_s1[i].append(CounterWithKey(s1_id[i][j], s1_counters[i][j]))

                elif cur_cnt > ((k - 1) / (T_i) + 1) * pre_cnt and cur_cnt >= THRESHOLD / T_i:
                    if self.Stage2.insert(
                            Bucket(s1_id[i][j], cur_cnt, time - 1, cur_cnt - pre_cnt, pre_cnt, 1, 0, 0, 'p')):
                        s1_id[i][j] = 0
                        tmp_s1[i].append(CounterWithKey())
                    else:
                        tmp_s1[i].append(CounterWithKey(s1_id[i][j], s1_counters[i][j]))
                elif cur_cnt < (1 - (1 - 1 / k) / T_d) * pre_cnt and pre_cnt >= THRESHOLD:
                    if self.Stage2.insert(
                            Bucket(s1_id[i][j], cur_cnt, time - 1, pre_cnt - cur_cnt, pre_cnt, 0, 0, 1, 'n')):
                        s1_id[i][j] = 0
                        tmp_s1[i].append(CounterWithKey())
                    else:
                        tmp_s1[i].append(CounterWithKey(s1_id[i][j], s1_counters[i][j]))
                else:
                    tmp_s1[i].append(CounterWithKey(s1_id[i][j], s1_counters[i][j]))
                if z != -1:
                    self.Stage1.buckets[idx][z].key = 0

            # print(tmp_s1[i])

        for i in range(self.Stage1.hash_num):
            for j in range(self.Stage1.bucket_num):
                if self.Stage1.buckets[i][j].key == 0:
                    continue

                if self.Stage1.buckets[i][j].val >= THRESHOLD:
                    self.Stage3.insert(Bucket(self.Stage1.buckets[i][j].key, 0, time - 1,
                                              self.Stage1.buckets[i][j].val - 0, self.Stage1.buckets[i][j].val, 0, 0, 1,
                                              'n'))

        self.Stage1.buckets = tmp_s1
        for i in range(len(s1_insert)):
            self.Stage1.insert(s1_insert[i].key, s1_insert[i].val)
        # print( self.Stage1.buckets)

        return

    def get_data(self):
        """return reg_data"""

        s1_d1_id = []
        s1_d1_counter = []
        # print(len(s1_d1_id))
        # for i in range(self.Stage1.bucket_num):
        #     print(self.Stage1.buckets[i])
        for item in range(self.Stage1.bucket_num):
            # print(item)
            s1_d1_id.append(self.Stage1.buckets[0][item].key)
            s1_d1_counter.append(self.Stage1.buckets[0][item].val)

        s1_d2_id = []
        s1_d2_counter = []
        for item in range(self.Stage1.bucket_num):
            s1_d2_id.append(self.Stage1.buckets[1][item].key)
            s1_d2_counter.append(self.Stage1.buckets[1][item].val)

        s1_d3_id = []
        s1_d3_counter = []
        for item in range(self.Stage1.bucket_num):
            s1_d3_id.append(self.Stage1.buckets[2][item].key)
            s1_d3_counter.append(self.Stage1.buckets[2][item].val)

        s2_id = []
        s2_counter = []
        for item in range(self.Stage2.bucket_num):
            s2_id.append(self.Stage2.buckets[item].key)
            s2_counter.append(self.Stage2.buckets[item].val)

        s3_id = []
        s3_counter = []
        for item in range(self.Stage3.bucket_num):
            s3_id.append(self.Stage3.buckets[item].key)
            s3_counter.append(self.Stage3.buckets[item].val)

        s4_id = []
        s4_counter = []
        for item in range(self.Stage4.bucket_num):
            s4_id.append(self.Stage4.buckets[item].key)
            s4_counter.append(self.Stage4.buckets[item].val)

        reg_data = {
            "s1_d1_id": s1_d1_id,
            "s1_d1_counter": s1_d1_counter,
            "s1_d2_id": s1_d2_id,
            "s1_d2_counter": s1_d2_counter,
            "s1_d3_id": s1_d3_id,
            "s1_d3_counter": s1_d3_counter,
            "s2_id": s2_id,
            "s2_counter": s2_counter,
            "s3_id": s3_id,
            "s3_counter": s3_counter,
            "s4_id": s4_id,
            "s4_counter": s4_counter
        }

        return reg_data

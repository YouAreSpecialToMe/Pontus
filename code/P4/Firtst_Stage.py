import random

from Include import *
from Hash import *


class First_Stage(object):
    """Stage 1"""

    def __init__(self, bucket_num, hash_num=HASH_NUM):
        super(First_Stage, self).__init__()
        self.hash_num = hash_num
        self.bucket_num = bucket_num
        self.buckets = []

        # for _ in range(hash_num):
        #     self.buckets.append([])

        for i in range(self.hash_num):
            self.buckets.append([])
            for _ in range(bucket_num):
                self.buckets[i].append(CounterWithKey())

        self.hashes = [Hash().hash for _ in range(self.hash_num)]

    def insert(self, flow_id, v=1):
        """ insert operation"""
        flag = True
        min_z = oo
        min_i = oo
        min_val = oo
        zero_z = oo
        zero_i = oo
        for i in range(self.hash_num):
            z = self.hashes[i](str(flow_id)) % self.bucket_num
            # print(self.buckets[i])
            if self.buckets[i][z].key == flow_id:
                self.buckets[i][z].val += v
                return
            elif self.buckets[i][z].key == 0 and flag:
                zero_z = z
                zero_i = i
                flag = False
            else:
                if self.buckets[i][z].val < min_val:
                    min_z = z
                    min_i = i
                    min_val = self.buckets[i][z].val

        if not flag:
            self.buckets[zero_i][zero_z].key = flow_id
            self.buckets[zero_i][zero_z].val = v
            return

        """Hash collision"""
        if random.randint(0, min_val) < v:
            self.buckets[min_i][min_z].key = flow_id
            self.buckets[min_i][min_z].val = v

        return

    def get_position(self, flow_id):
        for i in range(self.hash_num):
            z = self.hashes[i](str(flow_id)) % self.bucket_num
            if self.buckets[i][z].key == flow_id:
                return i, z
        return -1, -1


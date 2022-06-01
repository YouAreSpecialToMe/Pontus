from Include import *
from Hash import *


class Mid_Stage(object):
    """Stage 2&3&4"""
    def __init__(self, bucket_num):
        self.bucket_num = bucket_num
        self.buckets = []
        for _ in range(self.bucket_num):
            self.buckets.append(Bucket())

    def insert_flow(self, flow_id, v=1):
        for i in range(self.bucket_num):
            if self.buckets[i].key == flow_id:
                self.buckets[i].val += v
                return True
        return False


    def insert(self, b):
        flag = True
        min_z = oo
        min_val = b.val
        zero_z = oo
        for i in range(self.bucket_num):
            if self.buckets[i].key == 0:
                zero_z = i
                flag = False
                break
            if self.buckets[i].type == b.type and self.buckets[i].val < min_val:
                min_z = i
                min_val = self.buckets[i].val

        if not flag:
            self.buckets[zero_z].setBucket(b)
            return True

        if min_z != oo:
            self.buckets[min_z].setBucket(b)
            return True

        return False


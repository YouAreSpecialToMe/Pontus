# -*- coding: utf-8 -*-

# para
HASH_NUM = 3
WINDOW_NUM = 10
THRESHOLD = 10
T_i = 1
T_d = 1
T = 10
oo = 1000000
k = 2
WINDOW_SIZE = 10000


class Window(object):
    """wave windows"""

    def __init__(self, t_i=0, t_l=0, t_d=0):
        super(Window, self).__init__()
        self.t_i = t_i
        self.t_d = t_d
        self.t_l = t_l

    def clear(self):
        self.t_i = 0
        self.t_d = 0
        self.t_l = 0


class CounterWithKey(object):
    """docstring for CounterWithKey"""

    def __init__(self, key=0, val=0):
        super(CounterWithKey, self).__init__()
        self.key = key
        self.val = val

    def compare(self, b):
        return self.val > b.val


class Bucket(object):
    """docstring for Bucket"""

    def __init__(self, key=0, val=0, timestamp=0, var=0, start=0, t_i=0, t_l=0, t_d=0,
                 type=''):
        super(Bucket, self).__init__()
        self.key = key
        self.val = val
        self.timestamp = timestamp
        self.start = start
        self.var = var
        self.type = type
        self.window = Window(t_i, t_l, t_d)

    def clear(self):
        self.key = 0
        self.val = 0
        self.timestamp = 0
        self.start = 0
        self.var = 0
        self.type = ''
        self.window.clear()

    def setBucket(self, b):
        self.key = b.key
        self.val = b.val
        self.timestamp = b.timstamp
        self.start = b.start
        self.var = b.var
        self.type = b.type
        self.window.t_i = b.window.t_i
        self.window.t_d = b.window.t_d
        self.window.t_l = b.window.t_l


class Burst(object):
    def __init__(self, timestamp, t_i, t_l, t_d, key, c1, c2, c3, c4):
        self.timesmp = timestamp
        self.window = Window(t_i, t_l, t_d)
        self.key = key
        self.c1 = c1
        self.c2 = c2
        self.c3 = c3
        self.c4 = c4

    def __eq__(self, other):
        return self.key == other.key and self.type == other.type and self.window.t_i == other.window.t_i and self.window.t_l == other.window.t_l and self.window.t_d == other.window.t_d

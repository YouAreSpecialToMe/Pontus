# -*- coding: utf-8 -*-

import mmh3
from random import randint


# 32-bits hash
class Hash(object):
    """docstring for Hash"""

    def __init__(self):
        self.seed = randint(1, 0xFFFFFFFF)

    def hash(self, key):
        return mmh3.hash(key, self.seed)

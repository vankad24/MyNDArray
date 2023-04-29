from functools import reduce
from collections import Counter
from time import time
import random
import math

def bin_hash(s: str):
	return reduce(lambda x,y: x^ord(y), s[1:], ord(s[0]))
# print(bin_hash("asdqwerty"))
# with open("endict.txt") as f:
# 	counter = Counter(map(lambda s: bin_hash(s[:-1]), f.readlines()))
# 	print(sum(filter(lambda x: x>1, counter.values())))


def crc_hash(data, seed=0):
    h = seed
    for ki in data:
        highorder = h & 0xF8000000 #обрезаем хэш маской
        h = (h << 5) & 0xFFFFFFFF #образаем те биты, которые уехали налево
        h = h ^ (highorder >> 27)
        h = h ^ ki
    return h

def pjw_hash(data, seed=0):
    h = seed
    for ki in data:
        h = (h << 4) + ki
        g = h & 0xF0000000
        if g != 0:
            h = h ^ (g >> 24)
            h = h ^ g
    return h

def buz_hash(data, seed=2):
    h = seed
    random.seed(seed)
    R = dict()
    for ki in data:
        highorder = h & 0x80000000
        h = h << 1
        h = h ^ (highorder >> 31)
        if ki not in R:
            R[ki] = random.randint(0, 0xFFFFFFFF)
        h = h ^ R[ki]
    return h


def find_duplicates(files, hash_function):
    hashes = set()
    dup = []
    for file in files:
        h = hash_function(file)
        if h in hashes:
            dup.append(h)
        else:
            hashes.add(h)
    return dup


files = []
for i in range(500):
    with open(f"out/{i}.txt","rb") as f:
        files.append(f.read())
        
funs = [crc_hash, pjw_hash, buz_hash, hash]

print("-----Hash tests-----")
for fun in funs:
    start = time()
    dups = find_duplicates(files, fun)
    end = time()
    print(fun.__name__+" function:")
    print(f"{len(dups)} duplicates")
    print(f"{math.floor((end-start)*1000)} ms")
    print("")
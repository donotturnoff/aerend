#!/usr/bin/python3
import sys
import statistics

paths = sys.argv[1:]
for path in paths:
    stats = {}
    with open(path) as f:
        for line in f.readlines():
            parts = line.split(" ")
            k = " ".join(parts[:-1])
            v = float(parts[-1])
            if k in stats:
                stats[k].append(v)
            else:
                stats[k] = []
    for k, v in stats.items():
        print(path, k, statistics.mean(v), statistics.stdev(v))

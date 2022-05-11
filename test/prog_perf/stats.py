#!/usr/bin/python3
import sys
import statistics

paths = sys.argv[1:]
for path in paths:
    with open(path) as f:
        vals = [float(x.strip("\n")) for x in f.readlines()]
    print(path, statistics.mean(vals), statistics.stdev(vals))

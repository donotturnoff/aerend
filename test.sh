#!/bin/sh

./button &
PID=$!
perf stat -p $PID -e instructions &
#perf record -p $PID sleep 1 &
sleep 1
kill -TERM $PID

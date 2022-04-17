#!/bin/zsh

#./instr_test > test/widget_tests/instr
#./mem_test > test/widget_tests/mem

bws=("100kbit" "1mbit" "10mbit" "100mbit" "1gbit")
latencies=("0ms" "10ms" "100ms")

tc qdisc del dev enp1s0 root

for latency in $latencies ; do
    for bw in $bws ; do
        tc qdisc add dev enp1s0 root netem rate $bw delay $latency
        ./pcap_test > test/widget_tests/pcap-bw$bw-lt$latency
        tc qdisc del dev enp1s0 root
    done
done

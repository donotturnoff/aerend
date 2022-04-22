#!/bin/zsh

#bin/instr_test > test/widget_tests/instr
#bin/mem_test > test/widget_tests/mem
bin/instr_test > instr
bin/mem_test > mem

bws=("1mbit" "10mbit" "100mbit" "1gbit" "100kbit" "10kbit")
latencies=("0ms" "10ms" "100ms")

tc qdisc del dev enp1s0 root

for bw in $bws ; do
    for latency in $latencies ; do
        tc qdisc add dev enp1s0 root netem rate $bw delay $latency
        #bin/pcap_test > test/widget_tests/pcap-bw$bw-lt$latency
        bin/pcap_test > pcap-bw$bw-lt$latency
        tc qdisc del dev enp1s0 root
    done
done

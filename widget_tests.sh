#!/bin/sh

sudo ./instr_test > test/widget_tests/instr
./mem_test > test/widget_tests/mem

bws=("10kbit" "100kbit" "1mbit" "10mbit" "100mbit" "1gbit")

for $bw in $bws ; do
    sudo tc qdisc add dev enp1s0 root tbf rate $bw
    sudo ./pcap_test > test/widget_tests/pcap_bw$bw
    sudo tc qdisc del dev enp1s0 root
done

#!/bin/bash
USER="custom"
USER1="gshare"
USER2="tournament"
LENGTH="13"
LENGTH1="13"
LENGTH2="9:10:10"


echo "fp_1.bz2"
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --$USER1:$LENGTH1
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --$USER2:$LENGTH2
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --$USER:$LENGTH
echo "fp_1.bz2"
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --$USER1:$LENGTH1
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --$USER2:$LENGTH2
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --$USER:$LENGTH
echo "int_1.bz2"
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --$USER1:$LENGTH1
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --$USER2:$LENGTH2
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --$USER:$LENGTH
echo "int_2.bz2"
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --$USER1:$LENGTH1
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --$USER2:$LENGTH2
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --$USER:$LENGTH
echo "mm_1.bz2"
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --$USER1:$LENGTH1
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --$USER2:$LENGTH2
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --$USER:$LENGTH
echo "mm_2.bz2 "
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --$USER1:$LENGTH1
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --$USER2:$LENGTH2
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --$USER:$LENGTH
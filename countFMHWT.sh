#!/bin/bash
make
for f in dna200 english200 proteins200 sources200 xml200; do
  for type in basic bch cf mpe1 mpe2 mpe3; do
    for q in 1000000; do
      for m in 10 20 50; do
        sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
        taskset -c 0 ./rank-select/test/countFMHWT $type 32 $f $q $m
      done
    done
  done
done
for f in english proteins; do
  for type in basic bch cf mpe1 mpe2 mpe3; do
    for q in 1000000; do
      for m in 10 20 50; do
        sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
        taskset -c 0 ./rank-select/test/countFMHWT $type 64 $f $q $m
      done
    done
  done
done
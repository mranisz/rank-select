#!/bin/bash
make
for rank in basic bch cf mpe1 mpe2 mpe3; do
  for type in balanced huff; do
    for f in dna200 english200 proteins200 sources200 xml200; do
      sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
      taskset -c 0 ./test/testRank $rank 32 WT-$type-$f.idx 100000000
    done
  done
  for d in 5 10 15 20 25 30 35 40 45 50 55 60 65 70 75 80 85 90 95; do
    sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
    taskset -c 0 ./test/testRank $rank 32 random-$d.dat 100000000
  done
done
for rank in basic bch cf mpe1 mpe2 mpe3; do
  for type in balanced huff; do
    for f in dna200 english200 proteins200 sources200 xml200; do
      sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
      taskset -c 0 ./test/testRank $rank 64 WT-$type-$f.idx 100000000
    done
  done
  for d in 5 10 15 20 25 30 35 40 45 50 55 60 65 70 75 80 85 90 95; do
    sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
    taskset -c 0 ./test/testRank $rank 64 random-$d.dat 100000000
  done
done
for rank in basic bch cf mpe1 mpe2 mpe3; do
  for type in balanced huff; do
    for f in english proteins; do
      sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
      taskset -c 0 ./test/testRank $rank 64 WT-$type-$f.idx 100000000
    done
  done
done
for rank in basic bch cf mpe1 mpe2 mpe3; do
  for d in eb en li sh sw uw; do
    sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
    taskset -c 0 ./test/testRank $rank 32 $d.dat 1000000
  done
done
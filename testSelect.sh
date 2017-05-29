#!/bin/bash
make
for select in basic-128-4096 basic-512-8192 bch-128-4096 bch-512-8192 mpe1-128-4096 mpe2-128-4096 mpe3-128-4096; do
  for type in balanced huff; do
    for f in dna200 english200 proteins200 sources200 xml200; do
      sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
      taskset -c 0 ./test/testSelect $select 32 WT-$type-$f.idx 100000000
    done
  done
  for d in 5 20 50; do
    sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
    taskset -c 0 ./test/testSelect $select 32 random-$d.dat 100000000
  done
done
for select in basic-128-4096 basic-512-8192 bch-128-4096 bch-512-8192 mpe1-128-4096 mpe2-128-4096 mpe3-128-4096; do
  for type in balanced huff; do
    for f in dna200 english200 proteins200 sources200 xml200; do
      sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
      taskset -c 0 ./test/testSelect $select 64 WT-$type-$f.idx 100000000
    done
  done
  for d in 5 20 50; do
    sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
    taskset -c 0 ./test/testSelect $select 64 random-$d.dat 100000000
  done
done
#!/bin/bash
make
for p in 10 30 50 70; do
  for rs in bch mpe2; do
    for type in balanced huff; do
      for f in dna200 english200 proteins200 sources200 xml200; do
        sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
        taskset -c 0 ./test/testMixedRankSelect $rs WT-$type-$f.idx data-$p.dat 100000000
      done
    done
    for d in 5 10 15 20 25 30 35 40 45 50 55 60 65 70 75 80 85 90 95; do
      sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
      taskset -c 0 ./test/testMixedRankSelect $rs random-$d.dat data-$p.dat 100000000
    done
  done
  for rs in bch mpe2; do
    for d in eb en li sh sw uw; do
      sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
      taskset -c 0 ./test/testMixedRankSelect $rs $d.dat data-$p.dat 1000000
    done
  done
done
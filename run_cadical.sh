#!/bin/sh

if [ "$#" -ne 2 ]; then
  echo "Illegal number of parameters"
  exit 2
fi

./build/cadical -color $1 ../dimacs_graphs/$2.col --phase=1 --forcephase=1 --no-binary --elim=0 --block=0 --chronoreusetrail=0 $2.drat -l > $2_$1.log
#./build/cadical -color $1 ../dimacs_graphs/$2.col --phase=1 --forcephase=1 --no-binary --elim=0 --block=0 --chronoreusetrail=0 $2.drat

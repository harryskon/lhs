#!/bin/bash
make > a.out.tmp
./game >> a.out.tmp
cat a.out.tmp > a.out
make clean >> a.out.tmp
rm -rf a.out.tmp
exit 0
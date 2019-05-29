#!/bin/bash -e

fn1=$(tempfile)
fn2=$(tempfile)

jq '.' $1 > $fn1
./load_save_roundtrip.m $1 | jq '.' > $fn2

diff $fn1 $fn2

rm -f $fn1 $fn2

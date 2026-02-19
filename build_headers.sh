#!/usr/bin/env bash
set -e

echo "Fetching sched-ext headers..."

rm -rf include
mkdir -p include

wget -qO scx.tar.gz https://github.com/sched-ext/scx/archive/refs/heads/main.tar.gz
tar -xzf scx.tar.gz

mv scx-*/scheds/include/scx include/

rm -rf scx-* scx.tar.gz

echo "Headers installed in include/scx"

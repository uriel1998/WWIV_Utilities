#!/bin/bash

files=$(fdfind -e ans)

for p in $files;do
    if [ "$p" != ".ans" ];then 
        echo "Processing ${p%.*}..."
        outname=$(echo "${p%.*}.msg")
        escape-to-ascii-and-pipe.sh < $p > $outname
    fi
done

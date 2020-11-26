#!/bin/bash

# Modified from https://unix.stackexchange.com/a/607428/409984
# Permits file in or stdin from pipe
# - er nevermind, gotta < it in....
if [ "$#" -ne 0 ]; then
    if [ -f "$1" ];then
        echo "reading from file"
        cat "$1"
    else 
        exec <<< "$1"
    fi
fi

OLDIFS=$IFS
IFS=$'\n'
while read line; do
    echo $line | sed 's@\x1B\[30m@|00@g' | \
    sed 's@\x1B\[34m@|01@g' | \
    sed 's@\x1B\[32m@|02@g' | \
    sed 's@\x1B\[36m@|03@g' | \
    sed 's@\x1B\[31m@|04@g' | \
    sed 's@\x1B\[35m@|05@g' | \
    sed 's@\x1B\[33;2m@|06@g' | \
    sed 's@\x1B\[37;2m@|07@g' | \
    sed 's@\x1B\[30;1m@|08@g' | \
    sed 's@\x1B\[34;1m@|09@g' | \
    sed 's@\x1B\[32;1m@|10@g' | \
    sed 's@\x1B\[36;1m@|11@g' | \
    sed 's@\x1B\[31;1m@|12@g' | \
    sed 's@\x1B\[35;1m@|13@g' | \
    sed 's@\x1B\[33m@|14@g' | \
    sed 's@\x1B\[37m@|15@g' | \
    sed 's@\x1B\[40m@|16@g' | \
    sed 's@\x1B\[44m@|17@g' | \
    sed 's@\x1B\[42m@|18@g' | \
    sed 's@\x1B\[46m@|19@g' | \
    sed 's@\x1B\[41m@|20@g' | \
    sed 's@\x1B\[45m@|21@g' | \
    sed 's@\x1B\[43;2m@|22@g' | \
    sed 's@\x1B\[47;2m@|23@g' | \
    sed 's@\x1B\[2;33m@|06@g' | \
    sed 's@\x1B\[2;37m@|07@g' | \
    sed 's@\x1B\[1;30m@|08@g' | \
    sed 's@\x1B\[1;34m@|09@g' | \
    sed 's@\x1B\[1;32m@|10@g' | \
    sed 's@\x1B\[1;36m@|11@g' | \
    sed 's@\x1B\[1;31m@|12@g' | \
    sed 's@\x1B\[1;35m@|13@g' | \
    sed 's@\x1B\[2;43m@|22@g' | \
    sed 's@\x1B\[2;47m@|23@g' | \
    sed 's@\x1B\[0;30m@|00@g' | \
    sed 's@\x1B\[0;34m@|01@g' | \
    sed 's@\x1B\[0;32m@|02@g' | \
    sed 's@\x1B\[0;36m@|03@g' | \
    sed 's@\x1B\[0;31m@|04@g' | \
    sed 's@\x1B\[0;35m@|05@g' | \
    sed 's@\x1B\[0;33m@|14@g' | \
    sed 's@\x1B\[0;37m@|15@g' | \
    sed 's@\x1B\[0;40m@|16@g' | \
    sed 's@\x1B\[0;44m@|17@g' | \
    sed 's@\x1B\[0;42m@|18@g' | \
    sed 's@\x1B\[0;46m@|19@g' | \
    sed 's@\x1B\[0;41m@|20@g' | \
    sed 's@\x1B\[0;45m@|21@g' | \
    sed 's@\x1B\[30;0m@|00@g' | \
    sed 's@\x1B\[34;0m@|01@g' | \
    sed 's@\x1B\[32;0m@|02@g' | \
    sed 's@\x1B\[36;0m@|03@g' | \
    sed 's@\x1B\[31;0m@|04@g' | \
    sed 's@\x1B\[35;0m@|05@g' | \
    sed 's@\x1B\[33;0m@|14@g' | \
    sed 's@\x1B\[37;0m@|15@g' | \
    sed 's@\x1B\[40;0m@|16@g' | \
    sed 's@\x1B\[44;0m@|17@g' | \
    sed 's@\x1B\[42;0m@|18@g' | \
    sed 's@\x1B\[46;0m@|19@g' | \
    sed 's@\x1B\[41;0m@|20@g' | \
    sed 's@\x1B\[45;0m@|21@g' 
done
IFS=$OLDIFS



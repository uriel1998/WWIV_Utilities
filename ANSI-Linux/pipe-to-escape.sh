#!/bin/bash


# Modified from https://unix.stackexchange.com/a/607428/409984
# Permits file in or stdin from pipe
if [ "$#" -ne 0 ]; then
    if [ -f "$1" ];then
        cat "$1"
    else 
        exec <<< "$1"
    fi
fi

OLDIFS=$IFS
IFS=$'\n'

while read line; do
    echo $line | sed 's@|00@'$(echo -e "\033")'[30m@g' | \
    sed 's@|01@'$(echo -e "\033")'[34m@g' | \
    sed 's@|02@'$(echo -e "\033")'[32m@g' | \
    sed 's@|03@'$(echo -e "\033")'[36m@g' | \
    sed 's@|04@'$(echo -e "\033")'[31m@g' | \
    sed 's@|05@'$(echo -e "\033")'[35m@g' | \
    sed 's@|06@'$(echo -e "\033")'[33;2m@g' | \
    sed 's@|07@'$(echo -e "\033")'[37;2m@g' | \
    sed 's@|08@'$(echo -e "\033")'[30;1m@g' | \
    sed 's@|09@'$(echo -e "\033")'[34;1m@g' | \
    sed 's@|10@'$(echo -e "\033")'[32;1m@g' | \
    sed 's@|11@'$(echo -e "\033")'[36;1m@g' | \
    sed 's@|12@'$(echo -e "\033")'[31;1m@g' | \
    sed 's@|13@'$(echo -e "\033")'[35;1m@g' | \
    sed 's@|14@'$(echo -e "\033")'[33m@g' | \
    sed 's@|15@'$(echo -e "\033")'[37m@g' | \
    sed 's@|16@'$(echo -e "\033")'[40m@g' | \
    sed 's@|17@'$(echo -e "\033")'[44m@g' | \
    sed 's@|18@'$(echo -e "\033")'[42m@g' | \
    sed 's@|19@'$(echo -e "\033")'[46m@g' | \
    sed 's@|20@'$(echo -e "\033")'[41m@g' | \
    sed 's@|21@'$(echo -e "\033")'[45m@g' | \
    sed 's@|22@'$(echo -e "\033")'[43;2m@g' | \
    sed 's@|23@'$(echo -e "\033")'[47;2m@g' | \
    sed 's@|00@'$(echo -e "\033")'[30m@g' | \
    sed 's@|#1@'$(echo -e "\033")'[34m@g' | \
    sed 's@|#2@'$(echo -e "\033")'[32m@g' | \
    sed 's@|#3@'$(echo -e "\033")'[36m@g' | \
    sed 's@|#4@'$(echo -e "\033")'[31m@g' | \
    sed 's@|#5@'$(echo -e "\033")'[35m@g' | \
    sed 's@|#6@'$(echo -e "\033")'[33;2m@g' | \
    sed 's@|#7@'$(echo -e "\033")'[37;2m@g' | \
    sed 's@|#8@'$(echo -e "\033")'[30;1m@g' | \
    sed 's@|#9@'$(echo -e "\033")'[34;1m@g' | \
    sed 's@|#10@'$(echo -e "\033")'[32;1m@g' | \
    sed 's@|#11@'$(echo -e "\033")'[36;1m@g' | \
    sed 's@|#12@'$(echo -e "\033")'[31;1m@g' | \
    sed 's@|#13@'$(echo -e "\033")'[35;1m@g' | \
    sed 's@|#14@'$(echo -e "\033")'[33m@g' | \
    sed 's@|#15@'$(echo -e "\033")'[37m@g' | \
    sed 's@|#16@'$(echo -e "\033")'[40m@g' | \
    sed 's@|#17@'$(echo -e "\033")'[44m@g' | \
    sed 's@|#18@'$(echo -e "\033")'[42m@g' | \
    sed 's@|#19@'$(echo -e "\033")'[46m@g' | \
    sed 's@|#20@'$(echo -e "\033")'[41m@g' | \
    sed 's@|#21@'$(echo -e "\033")'[45m@g' | \
    sed 's@|#22@'$(echo -e "\033")'[43;2m@g' | \
    sed 's@|#23@'$(echo -e "\033")'[47;2m@g'     
done 

IFS=$OLDIFS

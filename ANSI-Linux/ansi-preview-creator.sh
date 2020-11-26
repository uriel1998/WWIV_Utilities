#! /bin/bash

# A simple utility to create png of all ansi art in a directory

FD_FIND=$(which fdfind)


if [[ -d "$1" ]];then
    dirname=${1%/}

    if [ -f "$FD_FIND" ];then
        artworks=$(fdfind -a -e ans -e asc -e diz -e nfo -e txt . ${dirname})
    else
        artworks=$(find -H ${dirname} -type f -iname "*.ans" -or -iname "*.asc" -or -iname "*.diz" -or -iname "*.txt")
    fi

    for infile in $artworks;do
        ansilove "${infile}"
    done
fi

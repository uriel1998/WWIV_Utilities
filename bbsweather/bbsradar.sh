#!/bin/bash

# only call this every three or six hours.  The map is good for a day, so 
# we are caching it here *separately* from the door being run.

scriptpath=$(readlink -f "${0}" | xargs dirname)
datapath="$scriptpath/data"

# cleanup of files older than 6hrs
find $scriptpath/data -maxdepth 1 -mmin +360 -type f -iname "*.png" -iname "*.jpg" -iname "*.gif" -exec rm -f {} \;

if [ ! -f "$datapath"/noaa.gif ];then 
    #NOAA should only be pulled once a day!!!
    wget -O "$datapath"/noaa.gif https://www.wpc.ncep.noaa.gov/noaa/noaa.gif

    cd "$datapath"
    echo "splitting"
    convert noaa.gif -resize 1280x400! noaa.png
    nice -n 19 convert -crop 640x200+0+0 noaa.png northwest.jpg
    nice -n 19 convert -crop 640x200+0+200 noaa.png southwest.jpg
    nice -n 19 convert -crop 640x200+640+0 noaa.png northeast.jpg
    nice -n 19 convert -crop 640x200+640+200 noaa.png southeast.jpg
fi

# display commands.
#chafa -s 80x25 ./northwest.jpg && chafa -s 80x25 ./southwest.jpg 
#chafa -s 80x25 ./northeast.jpg && chafa -s 80x25 ./southeast.jpg 

# get weather map from NOAA
# check if updated (see xplanet setup)
#https://www.wpc.ncep.noaa.gov/noaa/noaa.gif
# use chafa -s 80x25 to get a decent one, or rotate with exiftran
# DO NOT ROTATE, UGH
#exiftran -a -9  noaa.jpg -o noaa_r.jpg

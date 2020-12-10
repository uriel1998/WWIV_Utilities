#!/bin/bash

# Need to specify temp/cache path so that the cache persists and you won't
# have your API rate hammered by a jerk
# TODO - load asynchronously 
# TODO - actually check for cache, duh

scriptpath=$(readlink -f "${0}" | xargs dirname)
chafa_bin=$(which chafa)

##############################################################################
# Cleanup of pending forecasts > 119 minutes old
##############################################################################

find $scriptpath/data -maxdepth 1 -mmin +119 -type f ! -iname "*.png" ! -iname "*.jpg" ! -iname "*.gif" -exec rm -f {} \;


if [ -f "$PWD/bashcolors" ];then
    source "$PWD/bashcolors"
    colors="True"
fi

if [ "$colors" = "True" ];then
    echo "${BLUE}###############################################${RESTORE}"        
    echo "${LGRAY}Enter your City and state and country code, like so:${RESTORE}"
    echo -e "${YELLOW}\nDayton,OH,US\n${RESTORE}"
    echo "${LGRAY}or just hit enter to see the weather at the BBS.${RESTORE}"
    echo "${BLUE}###############################################${RESTORE}"
else
    echo "###############################################"        
    echo "Enter your City and state and country code, like so:"
    echo -e "\nDayton,OH,US\n"
    echo "or just hit enter to see the weather at the BBS."
    echo "###############################################"
fi

IFS= read -r my_city
#echo "${my_city}"

if [ -z "$my_city" ];then
    Location="4509884"
else
    commas=$(echo "${my_city}" | awk -F "," '{print NF-1}') 
    if [ $commas -lt 2 ];then
        echo "Badly formatted city string" && sleep 5
        exit 99
    else
        Location="$my_city"
    fi
fi

echo "Your current conditions will be with you in just a moment..."

current=$($scriptpath/weather.sh -l "$Location" -n -p $scriptpath/data)
if [ "$?" -gt 0 ];then
    echo "There was an error finding the city. Please try again with"
    echo "the proper format, thanks!"
    sleep 5
    exit 99
fi

echo "$current" | sed 's@\xB0@ degrees @g' | sed 's@\xC2@@g' | sed 's@$@\x1B\[37m@' | sed -ne 's/.*/\x1B\[37m &/p' > $scriptpath/weather.txt

boxes -i box -s 60 -d boxquote $scriptpath/weather.txt

echo "Your forecast will be with you in just a moment..."

forecast=$($scriptpath/forecast.sh -l "$Location" -n -p $scriptpath/data )
if [ "$?" -gt 0 ];then
    echo "There was an error finding the city. Please try again with"
    echo "the proper format, thanks!"
    sleep 5
    exit 99
fi

echo "$forecast" | sed 's@\xB0@ degrees @g' | sed 's@\xC2@@g' | sed 's@$@\x1B\[37m@' | sed -ne 's/.*/\x1B\[37m &/p' | cut -c -95 > $scriptpath/forecast.txt

boxes -i box -s 60 -d boxquote $scriptpath/forecast.txt && sleep 5

# is there a weather map file?
if [ -f "$datapath"/noaa.gif ] && [ -f "$chafa_bin" ];then
    $chafa_bin -s 80x25 ./northwest.jpg && chafa -s 80x25 ./southwest.jpg 
    $chafa_bin -s 80x25 ./northeast.jpg && chafa -s 80x25 ./southeast.jpg 
    sleep 5
fi

echo "Press any key to continue..."

exit

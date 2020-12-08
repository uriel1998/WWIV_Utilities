#! /bin/bash

##############################################################################
# Doorparty Script for WWIV 5+
#
# By Steven Saus (c) 2020
#
# Under the MIT license
#
# Uses Chain.txt or username passed as a variable
# If chain.txt isn't being passed properly,then use door.sys line 36
# TODO - if there's a way to pass the door directly, that'd be fabulous
# Add throttle to splash screen | throttle -k 14.4  - why isn't it echoing?
##############################################################################

##############################################################################
# Init Variables
##############################################################################
username=""
doorfile=""
directdoor=""
systemtag=""
splashscreen=""

scriptpath=$(readlink -f "${0}" | xargs dirname)

##############################################################################
# Get command-line parameters
##############################################################################

while [ $# -gt 0 ]; do
    option="$1"
    case $option in
        -h) display_help
            exit
            shift ;;      
        -u) shift
            username="${1}"
            shift;;
        -f) shift
            doorfile="${1}"
            shift;;      
        -d) shift 
            directdoor="${1}"
            shift;;
        -a) shift
            splashscreen="${1}"
            shift;;            
        -s) shift
            systemtag="${1}"
            shift;;
        *)  shift;;
    esac
done    

if [ -f "$scriptpath/bashcolors" ];then
    source "$scriptpath/bashcolors"
    colors="True"
fi

if [ -z $splashscreen ];then
    if [ -f "${scriptpath}/doorparty.ans" ];then
        splashscreen="${scriptpath}/doorparty.ans"
    fi
fi

if [ ! -z $doorfile ];then
    if [ -z $username ];then
        if [ ! -f ${doorfile} ];then
            echo "CHAIN.TXT not passed properly! Exiting!"
            exit 99
        else
            username=$(sed -n '2p' ${doorfile})
        fi
    fi
fi


if [ -z $username ];then
    echo "No username! Exiting!"
    exit 99
fi

##############################################################################
# Show help
##############################################################################

function show_help() {
    echo "Usage: doorparty-link.sh [-u $var] [-s $var] [-f $var] [-a $var]"
    echo "  -h = show this help"
    echo "  -u = manually specify an username, overrides any in DOOR.SYS"
    echo "  -s = Your SystemTag from DoorParty. Do NOT use brackets."
    echo "  -f = The path to CHAIN.TXT"
    echo "  -a = The path to optional ANSI file splash screen"
}

if [ -z $splashscreen ];then
    if [ -z $splashscreen ] || [ ! -f ${splashscreen} ];then
        if [ "$colors" = "True" ];then
            echo "${BLUE}###############################################${RESTORE}"        
            echo "${LGRAY}You are entering${RESTORE}"
            echo -e "${YELLOW}\nDoor Party\n${RESTORE}"
            echo "${LGRAY}You will be able to choose one of MANY doors.${RESTORE}"
            echo "${BLUE}###############################################${RESTORE}"
        else
            echo "###############################################"        
            echo "You are entering"
            echo -e "\nDoorParty\n"
            echo "You will be able to choose one of MANY doors."
            echo "###############################################"
        fi
    else
        if [ -z $throttle ];then
            cat ${splashscreen}
        else
            cat ${splashscreen} | throttle -k 14.4  
        fi
    fi
fi

sleep 3

#TODO - this is returning [][fc]username

commandstring=$(printf "rlogin -E -K -8 -l \[%s\]%s -p 9999 localhost" "${systemtag}" "${username}") # uncertain how to pass a specific door yet
eval ${commandstring}
read


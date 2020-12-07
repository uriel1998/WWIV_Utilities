#! /bin/bash

##############################################################################
# BBSLink Script for WWIV 5+
#
# By Steven Saus (c) 2020
#
# Crypto/Auth bits from the BBSlink.net InterBBS Door Server Connection Script
# by Dennis Ayala 
#
# Under the MIT license
#
# Uses Chain.txt
##############################################################################


##############################################################################
# Init Variables
#
# BBSLink variables (syscode, etc) should be stored in bbslink.ini in the 
# same directory as this script.
#
##############################################################################

version="1.0.4"
screenrows="24"
scripttype="bash"
scriptver="$version"
usernumber=""
doorfile=""
doorcode=""
splashscreen=""

scriptpath=$(readlink -f "${0}" | xargs dirname)
throttle=$(which throttle)

source "$scriptpath/bbslink.ini"

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
            usernumber="${1}"
            shift ;;
        -f) shift
            doorfile="${1}"
            shift;;      
        -d) shift 
            doorcode="${1}"
            shift;;
        -a) shift
            splashscreen="${1}"
            shift;;            
        *)  shift;;
    esac
done    

if [ -f "$scriptpath/bashcolors" ];then
    source "$scriptpath/bashcolors"
    colors="True"
fi

if [ ! -z $doorfile ];then
    if [ -z $usernumber ];then
        if [ ! -f ${doorfile} ];then
            echo "CHAIN.TXT not passed properly! Exiting!"
            exit 99
        else
            usernumber=$(sed -n '1p' ${doorfile})
        fi
    fi
fi

if [ -z $splashscreen ];then
    if [ -f "${scriptpath}/bbslink.ans" ];then
        splashscreen="${scriptpath}/bbslink.ans"
    fi
fi

if [ -z $doorcode ];then
    doorcode="menu"
fi

##############################################################################
# Show help
##############################################################################

function show_help() {
    echo "Usage: bbslink.sh [-d $var] [-u $var] [-f $var] [-a $var]"
    echo "  -h = show this help"
    echo "  -u = manually specify an usernumber, overrides any in CHAIN.TXT"
    echo "  -f = The path to CHAIN.TXT"
    echo "  -a = The path to optional ANSI file splash screen"
    echo "  -d = The door ID desired (defaults to menu)"
}

##############################################################################
# Auth/Crypto portion
##############################################################################

##############################################################################
# Generate random 32 character alphanumeric string 	
# (upper and lowercase)				
##############################################################################
xkey=$(head -n 64 /dev/urandom  | tr -dc 'a-zA-Z0-9' | fold -w 6 | head -n 1)
##############################################################################
# Get Token from BBSLink				
##############################################################################
token=$(curl -s "http://$host/token.php?key=$xkey")
##############################################################################
# HASH AUTHCODE & SCHEME CODE		                
##############################################################################
xauth=$(echo -n "$authcode$token" | md5sum | awk '{print $1}')
xcode=$(echo -n "$schemecode$token" | md5sum | awk '{print $1}')
##############################################################################
# AUTHENTICATE ON BBSLINK	                        
##############################################################################
result=$(curl -s -H "X-User: $usernumber" -H "X-System: $syscode" -H "X-Auth: $xauth" -H "X-Code: $xcode" -H "X-Rows: $screenrows" -H "X-Key: $xkey" -H "X-Door: $doorcode" -H "X-Token: $token" -H "X-Type: $scripttype" -H "X-Version: $scriptver" "http://$host/auth.php?key=$xkey")

##############################################################################
# Launch BBSLink Sessino
##############################################################################
if [ "$result" == "complete" ]; then
    if [ -z $splashscreen ] || [ ! -f ${splashscreen} ];then
        if [ "$colors" = "True" ];then
            echo "${BLUE}###############################################${RESTORE}"        
            echo "${LGRAY}You are entering${RESTORE}"
            echo -e "${YELLOW}\nBBSLink\n${RESTORE}"
            echo "${LGRAY}Connecting to BBSlink.net, please wait...${RESTORE}"
            echo "${BLUE}###############################################${RESTORE}"
        else
            echo "###############################################"        
            echo "You are entering"
            echo -e "\nBBSLink\n"
            echo -e "Connecting to BBSlink.net, please wait..."
            echo "###############################################"
        fi
    else
        if [ -z $throttle ];then
            cat ${splashscreen}
        else
            cat ${splashscreen} | throttle -k 14.4  
        fi
    fi
    sleep 2
    telnet -E -K -8 $host
else
    ##############################################################################
    # AUTHENTICATION FAILED                               
    ##############################################################################
	echo "Error: $result"
	exit 1
fi

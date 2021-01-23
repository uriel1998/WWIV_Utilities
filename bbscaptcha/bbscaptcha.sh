#!/bin/bash

#TODO - testing is clearly fubared
#TODO - USE TOILET/FIGLET INSTEAD OF PYTHON
# script/shadow/slant/small/smslant/standard/block/lean/big/smmono9/smmono12/smblock
# pagga/emboss/future/smbraille  < - these are part of the standard fonts with 
# sudo apt install figlet toilet toilet-fonts
# boxes
# columns/diamonds/scroll/twisted/xes/whirly
# options for concat wav files for audio

## wwivutil asv --key=VALUE USERNUMBER
##############################################################################
# BBS Captcha / Autovalidator Script for WWIV 5+
#
# By Steven Saus (c) 2020
#
# Under the MIT license
#
# Uses Chain.txt or usernumber passed as a variable
# If chain.txt isn't being passed properly,then use door.sys line 36
#
##############################################################################


#ONE PERSON IN DOOR AT A TIME, SET MAX TIME IN DOOR TO 5m or less
#TODO - error when code exists when picking font or somesuch
#TODO - re - presentation of captcha

##############################################################################
# Init Variables
##############################################################################
usernumber=""
username=""
doorfile=""
userSL=""
scriptpath=$(readlink -f "${0}" | xargs dirname)
source "$scriptpath/bbscaptcha.ini"
if [ ! -f "$scriptpath/data/email_log.txt" ];then
    touch "$scriptpath/data/email_log.txt"
fi
email_logfile="$scriptpath/data/email_log.txt"
VERIFYCODE=""
toilet_bin=$(which toilet)
box_styles="columns@diamonds@scroll@twisted@xes@whirly"
toilet_fonts="script@shadow@slant@small@smslant@standard@block@lean@big@smmono9@smmono12@smblock@pagga@emboss@future@smbraille"
throttle_bin=$(which throttle)
mutt_bin=$(which mutt)
SHOWANSI=""

##############################################################################
# Get command-line parameters
##############################################################################

while [ $# -gt 0 ]; do
    option="$1"
    case $option in
        -h) display_help
            exit
            shift ;;      
        -f) shift
            doorfile="${1}"
            shift;;      
        *)  shift;;
    esac
done    

##############################################################################
# Verifying that setup is correct
##############################################################################

# If CHAIN.TXT is not passed, then this program doesn't know what their SL is
if [ ! -f ${doorfile} ];then
    userSL=""
else
    userSL=$(sed -n '11p' ${doorfile})
    usernumber=$(sed -n '1p' ${doorfile})
    username=$(sed -n '2p' ${doorfile})
fi

if [ -z $usernumber ];then
    echo "No usernumber! Exiting!"
    exit 99
fi
echo "$userSL" 
echo "$valuserSL"
read
# either it's not read, or their SL is too high to auto-validate
if [ -z $userSL ];then 
    if [ -f "${scriptpath}/SL-error.ans" ];then
        splashscreen="${scriptpath}/SL-error.ans"
    else
        echo "Error - User SL not available"
    fi
    exit 99
fi

if [ $userSL -ge $valuserSL ];then    
    if [ -f "${scriptpath}/SL-error.ans" ];then
        splashscreen="${scriptpath}/SL-error.ans"
    else
        echo "Error - User SL is greater than validation SL"
    fi
    exit 99
fi

if [ -f "$scriptpath/bashcolors" ];then
    source "$scriptpath/bashcolors"
    colors="True"
fi

if [ -f "${scriptpath}/captcha-welcome.ans" ];then
    splashscreen="${scriptpath}/captcha-welcome.ans"
fi

##############################################################################
# Cleanup of pending verifications > 59 minutes old
##############################################################################

find $scriptpath/data/pending -maxdepth 1 -mmin +59 -type f -exec rm -f {} \;

##############################################################################
# Show help
##############################################################################

function show_help() {
    echo "Usage: bbscaptcha.sh [-f $var] "
    echo "  -h = show this help"
    echo "  -f = The path to CHAIN.TXT"
}

function show_ansi() {

    if [ -z $throttle_bin ];then
        cat ${SHOWANSI}
    else
        cat ${SHOWANSI} | ${throttle_bin} 14.4
    fi
}

##############################################################################
# Show Splashscreen for CAPTCHA
##############################################################################

function captcha_splashscreen() {

    if [ ! -f ${splashscreen} ] || [ -z ${splashscreen} ];then
        if [ "$colors" = "True" ];then
            echo "${BLUE}###############################################${RESTORE}"        
            echo "${LGRAY}Users used to auto-validate by having ${RESTORE}"
            echo "${LGRAY}the BBS call back. Now, we use CAPTCHAs. ${RESTORE}"
            echo -e "${LGRAY}\nYou will momentarily be presented with${RESTORE}"
            echo "${LGRAY}an ANSI/ASCII CAPTCHA with several${RESTORE}"      
            echo "${LGRAY}numbers on it.  You know the drill${RESTORE}"      
            echo "${LGRAY}from there, right?${RESTORE}"                      
            echo "${BLUE}###############################################${RESTORE}"
        else
            echo "###############################################"        
            echo " Users used to auto-validate by having"     
            echo "  the BBS call back. Now, we use CAPTCHAs."
            echo -e "\n You will momentarily be presented with"  
            echo "  an ANSI/ASCII CAPTCHA with several "       
            echo "  numbers on it.  You know the drill"        
            echo "  from there, right?"
            echo "###############################################"
        fi
    else
        SHOWANSI=${splashscreen}
        show_ansi
    fi
}

function create_solva () {

    #check for existant verifyfile
    if [ ! -f $scriptpath/data/pending/$usernumber ];then

        VERIFYCODE=$(printf "%s%s%s%s%s" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))")
        echo "$VERIFYCODE" > $scriptpath/data/pending/$usernumber
        bob=$(($RANDOM % 5))
        
        right_code="first@second@third@fourth@fifth"
        code_position=$(echo "$right_code" | awk -F '@' -v bob="$bob2" '{ print $bob }')
        
        for ((count = 0; count < 6; count++));do
            if [ $count = $bob ];then
                printf "%s" "$VERIFYCODE"
            else
                printf "%s%s%s%s%s" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))"
            fi
        done
        printf "Type in the %s code please.\n" "code_position"
        verify_code
    else 
        echo "Verification already pending"
        exit 98
    fi
    
}


function create_captcha () {
    
    #check for existant verifyfile
    if [ ! -f $scriptpath/data/pending/$usernumber ];then
        VERIFYCODE=$(printf "%s%s%s%s%s" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))")
        echo "$VERIFYCODE" > $scriptpath/data/pending/$usernumber
        bob=$(($RANDOM % 10))
        tfont=$(echo "$toilet_fonts" | awk -F '@' -v bob="$bob" '{ print $bob }')
        bob2=$(($RANDOM % 6))
        box_style=$(echo "$box_styles" | awk -F '@' -v bob="$bob2" '{ print $bob }')
        toilet -f "${tfont}" ${VERIFYCODE} > $scriptpath/data/buildcaptcha.txt
        boxes -d ${box_style} -i none -p a1 $scriptpath/data/buildcaptcha.txt

        # TODO - OFFER REGENERATING IT IF PROBLEMS HAPPEN
        verify_code
    else 
        echo "Verification already pending"
        exit 98
    fi
}

function upgrade_user () {
    #pseudo-code at the moment
    #wwivutil asv --key=VALUE USERNUMBER
    echo "Worked!" >> /home/wwiv/captcha-text.txt
}

function verify_code () {
    
    if [ -f $scriptpath/data/pending/$usernumber ];then
        VERIFYCODE=$(head -n 1 $scriptpath/data/pending/$usernumber)
        if [ -f "${scriptpath}/captcha-code-enter.ans" ];then
            SHOWANSI="${scriptpath}/captcha-code-enter.ans"
            show_ansi
        else
            if [ "$colors" = "True" ];then
                echo "${BLUE}###############################################${RESTORE}"        
                echo "${LGRAY}A verification code has been found for${RESTORE}"
                echo -e "${YELLOW}\n${username}\n${RESTORE}"
                echo "${LGRAY}Please enter your verification code below${RESTORE}"
                echo "${LGRAY}Codes are reset after one hour.${RESTORE}"
                echo "${BLUE}###############################################${RESTORE}"
            else
                echo "###############################################"        
                echo "A verification code has been found for"
                echo -e "\n${username}\n"
                echo "Please enter your verification code below."
                echo "Codes are reset after one hour."                
                echo "###############################################"
            fi
        fi
        IFS= read -r entered_code        

        if [[ $entered_code = $VERIFYCODE ]]; then
            if [ -f "${scriptpath}/captcha-success.ans" ];then
                SHOWANSI="${scriptpath}/captcha-success.ans"
                show_ansi
            else
                upgrade_user
                if [ "$colors" = "True" ];then
                    echo "${BLUE}###############################################${RESTORE}"        
                    echo "${LGRAY}Congratulations,${RESTORE}"
                    echo -e "${YELLOW}\n${username}\n${RESTORE}"
                    echo "${LGRAY}You are now verified!${RESTORE}"
                    echo "${BLUE}###############################################${RESTORE}"
                else
                    echo "###############################################"        
                    echo "Congratulations,"
                    echo -e "\n${username}\n"
                    echo "You are now verified!"
                    echo "###############################################"
                fi
            fi
            rm -f $scriptpath/data/pending/$usernumber
            exit
        else
            if [ -f "${scriptpath}/captcha-error.ans" ];then
                SHOWANSI="${scriptpath}/captcha-error.ans"
                show_ansi
            else 
                if [ "$colors" = "True" ];then
                    echo "${RED}###############################################${RESTORE}"        
                    echo -e "${YELLOW}\nINCORRECT CODE\n${RESTORE}"
                    echo "${RED}###############################################${RESTORE}"        
                else
                    echo "###############################################"
                    echo -e "\n$INCORRECT CODE\n"
                    echo "###############################################"
                fi
            fi
            # We want the captcha to persist until cleaned up by the script
            #rm -f $scriptpath/data/pending/$usernumber
            # yes, it dumps them out.
            sleep 3
            exit 77
        fi
    else
        if [ -f "${scriptpath}/captcha-error.ans" ];then
            SHOWANSI="${scriptpath}/captcha-error.ans"
            show_ansi
        else
            if [ "$colors" = "True" ];then
                echo "${RED}###############################################${RESTORE}"        
                echo "${LGRAY}No verification code found for${RESTORE}"
                echo -e "${YELLOW}\n${usernumber} ${username}\n${RESTORE}"
                echo "${RED}###############################################${RESTORE}"        
            else
                echo "###############################################"
                echo "No verification code found for"
                echo -e "\n${usernumber} ${username}\n"
                echo "###############################################"
            fi
        fi
        sleep 3
        exit 88
    fi
}

function main () {
    
    captcha_splashscreen
    
    if [ -f $scriptpath/data/pending/$usernumber ];then
        # if there's already a code, go straight to verification
        verify_code
    else
        if [ ! -z $from_email ];then
            echo "Do you want [s]olva or [c]aptcha?"
            # will insert email verification here
            IFS= read -r result
            if [[ "$result" =~ "s" ]];then
                create_solva
                read
            fi
        fi
        create_captcha
    fi

}

main

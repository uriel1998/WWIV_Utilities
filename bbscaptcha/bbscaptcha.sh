#!/bin/bash

#TODO: make file to check use the username as well so it's a little harder to guess?

## wwivutil asv --key=VALUE USERNUMBER
##############################################################################
# BBS Captcha / Autovalidator Script for WWIV 5+
#
# By Steven Saus (c) 2020
#
# Under the MIT license
#
# Uses Chain.txt passed as a variable ( -f /path/to/file )
#
##############################################################################

##############################################################################
# Init Variables
##############################################################################
usernumber=""
username=""
doorfile=""
userSL=""
scriptpath=$(readlink -f "${0}" | xargs dirname)
source "$scriptpath/bbscaptcha.ini"
VERIFYCODE=""
toilet_bin=$(which toilet)
box_styles="columns@diamonds@scroll@twisted@xes@whirly"
toilet_fonts="script@shadow@slant@small@smslant@standard@block@lean@big@smmono9@smmono12@smblock@pagga@emboss@future@smbraille"
SHOWANSI=""
HOME=$scriptpath

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

if [ ! -d "$scriptpath/data/pending" ];then
    mkdir -p "$scriptpath/data/pending"
fi

# If CHAIN.TXT is not passed, then this program doesn't know what their SL is
if [ ! -f ${doorfile} ];then
    userSL=""
    echo "Not properly configured! Notify SYSOP."
    exit 99
else
    userSL=$(sed -n '11p' ${doorfile})
    usernumber=$(sed -n '1p' ${doorfile})
    username=$(sed -n '2p' ${doorfile})
    userANSI=$(sed -n '14p' ${doorfile})
fi

if [ "$userANSI" == "1" ];then
    if [ -f "$scriptpath/bashcolors" ];then
        source "$scriptpath/bashcolors"
        colors="True"
    fi
fi

if [ -z $usernumber ];then
    echo "No usernumber! Exiting!"
    exit 99
fi

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
    #exit 99
fi


if [ -f "${scriptpath}/captcha-welcome.ans" ];then
    splashscreen="${scriptpath}/captcha-welcome.ans"
fi

##############################################################################
# Cleanup of pending verifications > 10 minutes old
##############################################################################

find $scriptpath/data/pending -maxdepth 1 -mmin +10 -type f -exec rm -f {} \;

##############################################################################
# Show help
##############################################################################

function show_help() {
    echo "Usage: bbscaptcha.sh [-f $var] "
    echo "  -h = show this help"
    echo "  -f = The path to CHAIN.TXT"
    exit 99
}

function show_ansi() {
    if [ "$colors" = "True" ];then
        cat ${SHOWANSI}
        echo ""
        sleep 3
    fi
}

##############################################################################
# Show Splashscreen for CAPTCHA
##############################################################################

function captcha_splashscreen() {
    if [ "$colors" = "True" ];then
        if [ -f ${splashscreen} ];then 
            SHOWANSI=${splashscreen}
            show_ansi
        else
            echo "${BLUE}###############################################${RESTORE}"        
            echo "${LGRAY} Users used to auto-validate by having ${RESTORE}"
            echo "${LGRAY} the BBS call back. Now, we use CAPTCHAs. ${RESTORE}"
            echo "${LGRAY} You may choose an ASCII art CAPTCHA "  
            echo "${LGRAY} or a SOLVA where you must choose the "       
            echo "${LGRAY} correct code. If you are using a screen "        
            echo "${LGRAY} reader, choose the SOLVA."
            echo "${BLUE}###############################################${RESTORE}"
        fi
    else
        echo "5"
        echo " Users used to auto-validate by having"     
        echo " the BBS call back. Now, we use CAPTCHAs."
        echo " You may choose an ASCII art CAPTCHA "  
        echo " or a SOLVA where you must choose the "       
        echo " correct code. If you are using a screen "        
        echo " reader, choose the SOLVA."
    fi
}

function create_solva () {

    #check for existant verifyfile
    if [ ! -f $scriptpath/data/pending/$usernumber ];then

        VERIFYCODE=$(printf "%s%s%s%s%s" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))")
        echo "$VERIFYCODE" > $scriptpath/data/pending/$usernumber
        bob=$((1+$RANDOM % 5))
        right_code="first@second@third@fourth@fifth"
        code_position=$(echo "$right_code" | awk -F '@' -v bob="$bob" '{ print $bob }')
        if [ "$colors" = "True" ];then
            echo "${BLUE}###############################################${RESTORE}"        
        fi
        for ((count = 1; count < 6; count++));do
            if [ $count = $bob ];then
                printf "%s\n" "$VERIFYCODE"
            else
                printf "%s%s%s%s%s\n" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))"
            fi
        done
        printf "\nTo verify, on the next screen type in the %s code please.\n" "$code_position"
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
        bob=$((1 + $RANDOM % 10))
        tfont=$(echo "$toilet_fonts" | awk -F '@' -v bob="$bob" '{ print $bob }')
        bob2=$((1 + $RANDOM % 6))
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
    ${WWIVutil_bin} users asv --user="${usernumber}" --asv="${ASVlevel}"
}

function verify_code () {
    
    if [ -f $scriptpath/data/pending/$usernumber ];then
        VERIFYCODE=$(head -n 1 $scriptpath/data/pending/$usernumber)
        if [ "$colors" = "True" ];then
            if [ -f "${scriptpath}/captcha-code-enter.ans" ];then
                SHOWANSI="${scriptpath}/captcha-code-enter.ans"
                show_ansi
            else
                echo "${BLUE}###############################################${RESTORE}"        
                echo "${LGRAY}A verification code has been found for${RESTORE}"
                echo -e "${YELLOW}\n${username}\n${RESTORE}"
                echo "${LGRAY}Please enter your verification code below${RESTORE}"
                echo "${LGRAY}Codes are reset after ten minutes.${RESTORE}"
                echo "${BLUE}###############################################${RESTORE}"
            fi
        else
            echo "A verification code has been found for"
            echo -e "\n${username}\n"
            echo "Please enter your verification code below."
            echo "Codes are reset after ten minutes."                
        fi
        IFS= read -r entered_code        

        if [[ $entered_code = $VERIFYCODE ]]; then
            upgrade_user
            if [ "$colors" = "True" ];then
                if [ -f "${scriptpath}/captcha-success.ans" ];then
                    SHOWANSI="${scriptpath}/captcha-success.ans"
                    show_ansi
                else
                    echo "${BLUE}###############################################${RESTORE}"        
                    echo "${LGRAY}Congratulations,${RESTORE}"
                    echo -e "${YELLOW}\n${username}\n${RESTORE}"
                    echo "${LGRAY}You are now verified!${RESTORE}"
                    echo "${BLUE}###############################################${RESTORE}"
                fi
            else
                echo "Congratulations,"
                echo -e "\n${username}\n"
                echo "You are now verified!"
            fi
            rm -f $scriptpath/data/pending/$usernumber
            exit
        else
            if [ "$colors" = "True" ];then
                if [ -f "${scriptpath}/captcha-error.ans" ];then
                    SHOWANSI="${scriptpath}/captcha-error.ans"
                    show_ansi
                else 
                    echo "${RED}###############################################${RESTORE}"        
                    echo -e "${YELLOW}\nINCORRECT CODE\n${RESTORE}"
                    echo "${RED}###############################################${RESTORE}"        
                fi
            else
                echo -e "\n$INCORRECT CODE\n"
            fi
            # We want the captcha to persist until cleaned up by the script
            #rm -f $scriptpath/data/pending/$usernumber
            # yes, it dumps them out.
            sleep 3
            exit 77
        fi
    else
        if [ "$colors" = "True" ];then
            if [ -f "${scriptpath}/captcha-error.ans" ];then
                SHOWANSI="${scriptpath}/captcha-error.ans"
                show_ansi
            else
                echo "${RED}###############################################${RESTORE}"        
                echo "${LGRAY}No verification code found for${RESTORE}"
                echo -e "${YELLOW}\n${usernumber} ${username}\n${RESTORE}"
                echo "${RED}###############################################${RESTORE}"        
            fi
        else
            echo "No verification code found for"
            echo -e "\n${usernumber} ${username}\n"
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
        echo "Do you want a [s]olva or [c]aptcha?"
        IFS= read -r result
        if [[ "$result" =~ "s" ]];then
            create_solva
        else
            create_captcha
        fi
    fi
}

main

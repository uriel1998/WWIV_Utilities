#!/bin/bash

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

#https://github.com/lepture/captcha

#TODO - check htaccess
#TODO - check nginx


##############################################################################
# Init Variables
##############################################################################
usernumber=""
username=""
doorfile=""
userSL=""
scriptpath=$(readlink -f "${0}" | xargs dirname)
source "$scriptpath/bbscaptcha.ini"
email-logfile="$scriptdir/data/email-log.txt"
VERIFYCODE=""
chafa_bin==$(which chafa)
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
if [ ! -z $doorfile ] || [ ! -f ${doorfile} ];then
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

# either it's not read, or their SL is too high to auto-validate
if [ -z $userSL ] || [ $userSL -ge $valuserSL ];then    
    if [ -f "${scriptpath}/SL-error.ans" ];then
        splashscreen="${scriptpath}/SL-error.ans"
        exit 99
    fi
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

    if [ -z $throttle ];then
        cat ${SHOWANSI}
    else
        cat ${SHOWANSI} | throttle -k 14.4  
    fi
 
    
}

##############################################################################
# Show Splashscreen for CAPTCHA
##############################################################################

function captcha_splashscreen() {

    if [ -z $splashscreen ];then
        if [ -z $splashscreen ] || [ ! -f ${splashscreen} ];then
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
    fi

    sleep 3
}


function create_email () {
    
    if [ ! -f $scriptdir/data/pending/$usernumber ];then
        if [ -f "${scriptpath}/captcha-email.ans" ];then
            SHOWANSI="${scriptpath}/captcha-email.ans"
            show_ansi
        else
            if [ "$colors" = "True" ];then
                echo "${BLUE}###############################################${RESTORE}"        
                echo "${LGRAY}Please enter the email address to verify.${RESTORE}"
                echo "${BLUE}###############################################${RESTORE}"
            else
                echo "###############################################"
                echo "Please enter the email address to verify."
                echo "###############################################"
            fi
        fi
        echo "Please enter the email address to send the verification code to."
        IFS= read -r raw-email
        entered-email=$(echo "raw-email" | awk '{print tolower($0)}')  
        
        # VERY basic validation of format
        if [[ $entered-email =~ '(.+)@(.+)' ]] ; then     
            UsedEmail=$(grep -c "^${entered-email}" "${email-logfile}")
            if [ $UsedEmail -ge 1 ];then
                # They have already tried sending email with this address.
                exit 98
            else
                VERIFYCODE=$($scriptdir/bbscaptcha.py)
                echo "$VERIFYCODE" > $scriptdir/data/pending/$usernumber
                rm $scriptdir/out.wav
                
                BBSName=$(sed -n '22p' ${doorfile})
                Sysop=$(sed -n '23p' ${doorfile})
                
                echo "Hi! Someone entered this email address to verify on $BBSName ." > $scriptdir\data\building_email.txt
                echo " " >> $scriptdir\data\building_email.txt
                echo "If you did not request this validation, our apologies. You do not need to" >> $scriptdir\data\building_email.txt 
                echo "do anything. " >> $scriptdir\data\building_email.txt
                echo " " >> $scriptdir\data\building_email.txt
                echo "If you *did* request validation, your code is at the bottom of this email." >> $scriptdir\data\building_email.txt 
                echo "Log back into the BBS and enter the code into the verification area.  " >> $scriptdir\data\building_email.txt
                echo "It expires one hour after it was issued, so don't delay!" >> $scriptdir\data\building_email.txt
                echo " " >> $scriptdir\data\building_email.txt
                echo "Your number is $VERIFYCODE" >> $scriptdir\data\building_email.txt
                echo " " >> $scriptdir\data\building_email.txt
                echo "Thanks, $Sysop, Sysop of $BBSName " >> $scriptdir\data\building_email.txt
                
                echo "${entered-email}" >> ${email-logfile}
                "$mutt_bin" -s User-Validation -i ${scriptdir}\data\building_email.txt -a ${scriptdir}\out.png ${entered-email}
                rm ${scriptdir}\out.png    
                rm ${scriptdir}\data\building_email.txt
            fi
        fi
    fi
    echo "###############################################"  
    echo " Email sent! Re-enter this program to enter the"
    echo " validation code!"
    echo "###############################################"  
    exit 0
}


function create_captcha () {
    
    #check for existant verifyfile
    if [ ! -f $scriptdir/data/pending/$usernumber ];then
        VERIFYCODE=$($scriptdir/bbscaptcha.py)
        echo "$VERIFYCODE" > $scriptdir/data/pending/$usernumber
        if [ -d $webaudiopath ];then   # if you don't want to offer audio, do not define a webaudio path
            cp $scriptdir/out.wav $webaudiopath/$usernumber.wav
            echo "If you need or would prefer audio verification,"
            echo "point a web browser to"
            echo "$webaudiourl/$usernumber.wav"
            echo "and listen to that file."
            echo "Press any key to see the CAPTCHA."
        fi
        rm $scriptdir/out.wav
        if [ -z $throttle ];then
            $chafa_bin --colors=none -s 70x20  --clear --fill -all-stipple-braille-ascii-space-extra-inverted --invert --symbols -all-stipple-braille+ascii+space-extra-inverted $scriptdir/out.png | sed 's/ /./g' 
        else
            $chafa_bin --colors=none -s 70x20  --clear --fill -all-stipple-braille-ascii-space-extra-inverted --invert --symbols -all-stipple-braille+ascii+space-extra-inverted $scriptdir/out.png | sed 's/ /./g' | $throttle_bin -k 14.4  
        fi
        rm $scriptdir/out.png
        verify_code
    else 
        echo "Verification already pending"
        exit 98
    fi
}

function upgrade_user () {
    #pseudo-code at the moment
    wwivutil users set --user_num=22 --sl=20 --dsl=20
    wwivutil autoval --user_num=20 ALT-F1
}

function verify_code () {
    
    if [ -f $scriptdir/data/pending/$usernumber ];then
        VERIFYCODE=$(head -n 1 $scriptdir/data/pending/$usernumber)
        if [ -f "${scriptpath}/captcha-code-enter.ans" ];then
            SHOWANSI="${scriptpath}/captcha-code-enter.ans"
            show_ansi
        else
            if [ "$colors" = "True" ];then
                echo "${BLUE}###############################################${RESTORE}"        
                echo "${LGRAY}A verification code has been found for${RESTORE}"
                echo -e "${YELLOW}\n${username}\n${RESTORE}"
                echo "${LGRAY}Please enter your verification code below.${RESTORE}"
                echo "${BLUE}###############################################${RESTORE}"
            else
                echo "###############################################"        
                echo "A verification code has been found for"
                echo -e "\n${username}\n"
                echo "Please enter your verification code below."
                echo "###############################################"
            fi
        fi
        IFS= read -r entered_code        
        if [ $entered_code = $VERIFYCODE]; then
            if [ -f "${scriptpath}/captcha-success.ans" ];then
                SHOWANSI="${scriptpath}/captcha-success.ans"
                show_ansi
            else
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
            upgrade_user
            rm -f $scriptdir/data/pending/$usernumber
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
            rm -f $scriptdir/data/pending/$usernumber
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
    
    if [ -f $scriptdir/data/pending/$usernumber ];then
        # if there's already a code, go straight to verification
        verify_code
    else
        if [ ! -z $from-email ];then
            echo "Do you want [e]mail or [c]aptcha?"
            # will insert email verification here
            IFS= read -r result
            if [[ "$result" =~ "^e" ]];then
                create_email
            fi
        fi
        create_captcha
    fi

}

main

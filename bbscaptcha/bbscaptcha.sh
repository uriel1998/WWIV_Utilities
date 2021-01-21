#!/bin/bash

#TODO - testing is clearly fubared
#TODO - USE TOILET/FIGLET INSTEAD OF PYTHON
# script/shadow/slant/small/smslant/standard/block/lean/big/smmono9/smmono12/smblock
# pagga/emboss/future/smbraille  < - these are part of the standard fonts with 
# sudo apt install figlet toilet toilet-fonts

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
if [ ! -f "$scriptpath/data/email_log.txt" ];then
    touch "$scriptpath/data/email_log.txt"
fi
email_logfile="$scriptpath/data/email_log.txt"
VERIFYCODE=""
chafa_bin=$(which chafa)
toilet_bin=$(which toilet)
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

echo "${doorfile}"
sed -n '11p' ${doorfile}
sed -n '1p' ${doorfile}
sed -n '2p' ${doorfile}

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
    
    if [ ! -f $scriptpath/data/pending/$usernumber ];then
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
            UsedEmail=$(grep -c "^${entered-email}" "${email_logfile}")
            if [ $UsedEmail -ge 1 ];then
                # They have already tried sending email with this address.
                exit 98
            else
                VERIFYCODE=$($scriptpath/bbscaptcha.py)
                echo "$VERIFYCODE" > $scriptpath/data/pending/$usernumber
                rm $scriptpath/out.wav
                
                BBSName=$(sed -n '22p' ${doorfile})
                Sysop=$(sed -n '23p' ${doorfile})
                
                echo "Hi! Someone entered this email address to verify on $BBSName ." > $scriptpath\data\building_email.txt
                echo " " >> $scriptpath\data\building_email.txt
                echo "If you did not request this validation, our apologies. You do not need to" >> $scriptpath\data\building_email.txt 
                echo "do anything. " >> $scriptpath\data\building_email.txt
                echo " " >> $scriptpath\data\building_email.txt
                echo "If you *did* request validation, your code is at the bottom of this email." >> $scriptpath\data\building_email.txt 
                echo "Log back into the BBS and enter the code into the verification area.  " >> $scriptpath\data\building_email.txt
                echo "It expires one hour after it was issued, so don't delay!" >> $scriptpath\data\building_email.txt
                echo " " >> $scriptpath\data\building_email.txt
                echo "Your number is $VERIFYCODE" >> $scriptpath\data\building_email.txt
                echo " " >> $scriptpath\data\building_email.txt
                echo "Thanks, $Sysop, Sysop of $BBSName " >> $scriptpath\data\building_email.txt
                
                echo "${entered-email}" >> ${email_logfile}
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
    if [ ! -f $scriptpath/data/pending/$usernumber ];then
        VERIFYCODE=$(printf "%s%s%s%s%s" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))" "$(($RANDOM % 10))")
        echo "$VERIFYCODE" > $scriptpath/data/pending/$usernumber
        ###DISABLED FOR THE MOMENT
        if [ -d $webaudiopath ] && [ 2 == 1 ] ;then   # if you don't want to offer audio, do not define a webaudio path
            #https://superuser.com/a/587553
            #ffmpeg -i input1.wav -i input2.wav -i input3.wav -i input4.wav -i input5.wav -filter_complex '[0:0][1:0][2:0][3:0]concat=n=5:v=0:a=1[out]' -map '[out]' output.wav
            cp $scriptpath/out.wav $webaudiopath/$usernumber.wav
            echo "If you need or would prefer audio verification,"
            echo "point a web browser to"
            echo "$webaudiourl/$usernumber.wav"
            echo "and listen to that file."
            echo "Press any key to see the CAPTCHA."
        fi
        rm $scriptpath/out.wav
        bob=$(($RANDOM % 10))
        tfont=$(echo "$toilet_fonts" | awk -F '@' -v bob="$bob" '{ print $bob }')

        toilet -f "${tfont}" ${VERIFYCODE}
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
            rm -f $scriptpath/data/pending/$usernumber
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

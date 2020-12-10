# BBS Weather

This is designed for WWIV, but can be adapted for other BBSes **IF** there 
is a way for you to externally modify the security values of the user.  

**UNTESTED CODE**  

**NO GUARANTEES AT ALL**


## Contents
 1. [About](#1-about)
 2. [License](#2-license)
 3. [Prerequisites](#3-prerequisites)
 4. [Installation](#4-installation)
 5. [TODO](#5-todo)

***

## 1. About


## 2. License

This project is licensed under the MIT license. For the full license, see `LICENSE`.

## 3. Prerequisites

* chafa
* python3
* pip
* https://uriel1998.github.io/weather.sh/ (slightly modified; included in this repo)
* boxes
 * OpenWeatherMap API key ([http://openweathermap.org/appid](http://openweathermap.org/appid)).
 * Bash shell ≥ 4.2.
 * `bc` basic calculator for floating point arithmetic. Can be found in the 
 `bc` package on major Linux distributions.
 * `curl` command-line tool for getting data using HTTP protocol. cURL can be 
 found in the `curl` package on major Linux distributions.
 * `grep` command-line tool used for parsing downloaded XML data. `grep` can 
 be found in the `grep` package on major Linux distributions.
 * `jq` command-line tool for parsing JSON data. `jq` can be found in the `jq` 
 package on major Linux distributions.
 * `tr` command-line tool for parsing JSON data. `tr` can be found in the `tr` 
 package on major Linux distributions.
 * `awk` command-line tool for parsing JSON data. `awk` can be found in the 
 `awk` package on major Linux distributions. 
 
Optional:


* [throttle](https://linux.die.net/man/1/throttle)

If you wish to use `throttle` to emulate connection speeds, just ensure that 
it is present in your `$PATH`. If it is not present, it will simply be skipped.

### weather_sh.rc

Copy (and edit, as appropriate) the `weather_sh.rc` file to `data\weather_sh.rc`.   
* The first line is the OpenWeatherMap API key  
* The second line is your default location. (See note below)  
* The third line is your default degree character (either `c` or `f`)  
* The fourth line is True or False depending on whether or not you want icons displayed for the weather.

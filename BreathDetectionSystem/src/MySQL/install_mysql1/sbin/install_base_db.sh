#!/bin/sh
echo "********Setup base_db***********"

echo "Connecting to mysql server......"

local_passwd="dcs.1234"

read -p "Enter Mysql root password(default ${local_passwd}): " REPLAY

local_passwd=$REPLAY

if [ "$local_passwd" = "" ];
then
	local_passwd="dcs.1234"
fi

echo "connect mysql .............................................."

basedb="mmt_simple_db"

read -p "Enter basedb name(default $basedb): " REPLAY

basedb=$REPLAY

if [ "$basedb" = "" ];
then
        basedb="mmt_simple_db"
fi
 
mysql -uroot -p$local_passwd -e "drop database if exists $basedb" &>/dev/null
if [ "$?" != "0" ];
then 
	echo :
	echo "password error"
	exit 1
fi

is_creat=y;

mysql -uroot -p$local_passwd -e "use $basedb" &> /dev/null

if [ "$?" -eq "0" ];
then
    read -p "$basedb exists,recreate ?(y or n,default y):" REPLAY
    is_creat=$REPLAY
    if [ "$is_creat" = "n" ];
    then
        exit 0
    else
        mysql -uroot -p$local_passwd -e "drop database $basedb" &> /dev/null
    fi
else
    read -p "$basedb not exists,create ?(y or n,default y):" REPLAY
    is_creat=$REPLAY
    if [ "$is_creat" = "n" ];
    then
        exit 0
    fi
fi

echo "Install $basedb ......"
mysql -u root -p$local_passwd -e "create database $basedb default character set utf8"

if [ "$?" -eq "0" ];
then
    echo "Install database $basedb successful"
else
    echo "Install database $basedb failed"
    exit 1
fi

echo "Import ${basedb}_sql.sql......"
mysql -uroot -p$local_passwd $basedb < ../DBflie/${basedb}_sql.sql

if [ "$?" -eq "0" ];
then 
	echo "Import successful"
else
	echo "Import failed"
	exit 1
fi

DBCommServ="CenterDBCommServ"

echo "Create mysql user for $DBCommServ ......"

ip_addr=`ifconfig eth0|grep Mask|cut -d: -f2|cut -d' ' -f1`

read -p "Enter $DBCommServ IP (default $ip_addr):" REPLAY
if [ ! -z $REPLAY ];
then
	ip_addr=$REPLAY
fi

username="mmtsimple"

read -p "Enter $DBCommServ username connecting to mysql(default $username):" REPLAY
if [ ! -z $REPLAY ];
then 
	username=$REPLAY
fi

passwd="simple.dbcomm"
read -p "Enter $DBCommServ password connecting to mysql(default $passwd):" REPLAY
if [ ! -z $REPLAY ]
then 
	passwd=$REPLAY
fi

mysql -uroot mysql -p$local_passwd -e "revoke all on *.* from $username" &> /dev/null
mysql -uroot mysql -p$local_passwd -e "delete from user where user='$username'" &> /dev/null
mysql -uroot mysql -p$local_passwd -e "grant all privileges on $basedb.* to $username@'%' identified by '$passwd' with grant option" &> /dev/null
mysql -uroot mysql -p$local_passwd -e "grant all on $basedb.* to $username@'%' identified by '$passwd' with grant option" &> /dev/null
mysql -uroot mysql -p$local_passwd -e "flush privileges" &> /dev/null
if [ "$?" -eq "0" ];
then
	echo "Update $basedb successful"
else
	exit 1
fi
exit 0

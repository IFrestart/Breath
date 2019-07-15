#!/bin/sh

backupopt_g="--complete-insert --add-drop-table --default-character-set=utf8"

#just structure & no data export
backupopt_s="$backupopt_g --no-data "

#just export date & no structure
backupopt_d="$backupopt_g --no-create-db --no-create-info"


#targetDir
randomFile='dbbackup'
targetDir=/tmp/$randomFile
passwd="dcs.1234"

read -ep "Enter password for root@localhost(default dcs.1234): " REPLY
passwd=$REPLY

if [ "$passwd" = "" ]
then
   passwd="dcs.1234"
fi

mkdir -p $targetDir

mysql -uroot -p$passwd -e "show databases"
read -ep "Please input backup database name: " REPLY
database_name=$REPLY

randomFile=$targetDir/${database_name}_`date -u +%F`-`date -u +%H-%M-%S`'.sql'

mysqldump -u root -p$passwd --databases $database_name $backupopt_g >$randomFile

if [ "$?" -eq "0" ]
then
        echo "Backup $database_name successful!"
else
        echo "$database_name is not existed!"
        exit 1
fi

cd $targetDir
ls -l

exit 0

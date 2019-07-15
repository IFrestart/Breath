#!/bin/sh

read -ep "Please input password of root@localhost(default dcs.1234): " REPLY
passwd=$REPLY

if [ "$passwd" = "" ]
then
   passwd="dcs.1234"
fi

read -ep "Please input the complete path for backup pieces file name: " REPLY
pieces_name=$REPLY

mysql -u root -p$passwd  <$pieces_name --default-character-set=gbk

if [ "$?" -eq "0" ]
then
        echo "Restore successful! " 
else
        echo "Restore fail!"
        exit 1
fi

exit 0

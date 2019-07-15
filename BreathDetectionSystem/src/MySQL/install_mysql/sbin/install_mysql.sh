#!/bin/sh

#mysql install and initialize shell
#file:install_mysql.sh
#author:shizc 2015-8-03
#version:5.5.29
#if you find some bugs of this shell script,please mail to seabiscuit@mmt.shedi.cn

#you must run install shell with root user

if [ "$USER" != "root" ]
then
  echo "Run MySQL install shell must be root user"
  echo "Please relogin this host again with root user"
  exit 0
fi

echo "########################################################################"
#if mysqld is running,we will stop it!!!!!!
echo "Checking whether your mysql server is runing..."

if test -n "`ps acx|grep mysqld`"
then
echo "MySQL is running,try to stop it..."
killall -9 mysqld_safe &>/dev/null
killall -9 mysqld  &>/dev/null
fi

echo "#########################################################################"
echo "Delete mysqld service from your host..."
chkconfig --del mysqld &>/dev/null
mv -f /etc/my.cnf /etc/old.my.cnf &>/dev/null

echo "########################################################################"
#if you installed mysql server or client RPM,we will uninstall them!
echo "If you installed mysql server or client RPM,we will uninstall them!"
echo "We are checking RPM of installed,please waiting........................."
rpm -qa|grep MySQL >mysql_rpm.test

test -s mysql_rpm.test
if [ "$?" -eq "0" ]
then
  echo "############################################################################"
  echo "RPM MySQL:..................................."
  more mysql_rpm.test
  
  echo "Uninstall RPM MySQL......................................"
  rpm -e MySQL-server-5.5.29-1.linux2.6.i386 &>/dev/null
  rpm -e MySQL-client-5.5.29-1.linux2.6.i386 &>/dev/null
  mv -f /var/lib/mysql /var/lib/mysql.$date &>/dev/null
  
fi

rm -f mysql_rpm.test
rm -rf /etc/my.cnf
rm -rf /var/lib/mysql
  
#install new version mysql server and client
echo "##########################################################################"
echo "Install MySQL-5.5.29 Server & Client.........."

rpm -ivh MySQL-server-5.5.29-1.linux2.6.i386.rpm 

if [ "$?" -eq "0" ]
then
	echo "Create MySQL Server successful........"
else	
	echo "Create MySQL Server failed........"
	exit 1
fi

rpm -ivh MySQL-client-5.5.29-1.linux2.6.i386.rpm

if [ "$?" -eq "0" ]
then
	echo "Create MySQL client successful........"
else	
	echo "Create MySQL client failed........"
	exit 1
fi

rm -rf /etc/my.cnf
cp /usr/share/mysql/my-medium.cnf /etc/my.cnf

echo "##########################################################################"
echo "Modify password of root@localhost and set security"
passwd="dcs.1234"
read -ep "Please input new password of root@localhost (default dcs.1234): " REPLY
passwd=$REPLY

if [ "$passwd" = "" ]
then
   passwd="dcs.1234"
fi

sed -i 's/skip-external-locking$/&\n\skip-name-resolve/' /etc/my.cnf

#Add by wuwc
mysql_path=/etc/my.cnf
if [ -f $mysql_path ];
then
        line=`sed -n '/^\[mysqld\]/=' $mysql_path`
        if [ ! -z $line ];
        then
                echo $line
                sed -i "${line}a\event_scheduler\t= ON" $mysql_path
        else
                echo "[\033[01;31mWarning\033[0m]Set configuration failed, please append [\033[01;32mevent_scheduler=ON\033[0m] in $mysql_path manually."
        fi
else        
        echo "$mysql_path - file does not exist."
        echo "[\033[01;31mWarning\033[0m]Set configuration failed, please append [\033[01;32mevent_scheduler=ON\033[0m] in my.cnf manually."
fi
#EndAdd

echo "Start MySQL ........"
service mysql restart

echo "Initializing mysql server,please wait....."
mysql -uroot mysql -e "drop database test" &> /dev/null
mysql -uroot mysql -e "delete from user where user!='root' or host!='localhost'"
mysql -uroot mysql -e "grant all on *.* to root@'localhost' identified by '$passwd' with grant option"
mysql -uroot -p$passwd mysql -e "flush privileges"

echo "##########################################################################"
if [ "$?" -eq "0" ]
then
        echo "Change password of root@localhost and set security successful"
else
        echo "Change password of root@localhost failed!"
        exit 1
fi

exit 0

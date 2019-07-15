#!/bin/bash

#author : shizc 2015-08-04

#version :1.0.0

print_menu()
{
    echo "Select your operation :"
    echo
    echo "	m : print this menu"
    echo "	1 : install_mysql"
    echo "	2 : install_base_simple_db"
    echo "	3 : dbbackup"
    echo "	4 : dbrestore"
    echo "	q : quit"
    echo
}

print_menu

install_error=0

while [ true ]
do
    read -ep "You choice : " REPLY
case $REPLY in
        h*|m*)
        print_menu
        ;;

	1)
	./install_mysql.sh
	install_error=$?
	if [ "$install_error" -eq "0" ]
	then
	    echo "You successfully install mysql server & client"
	else
	    echo "Install mysql failed"
	    exit 1
	fi
	;;

	2)
	./install_base_db.sh
	install_error=$?
	if [ "$install_error" -eq "0" ]
	then
	    echo "You successfully install mmt_simple_db database and tables"
	else
	    echo "Install mmt_simple_db database failed"
	    exit 1
	fi
	;;

	3)
	./dbbackup.sh
	install_error=$?
	if [ "$install_error" -eq "0" ]
	then
	    echo "You successfully backup data of mysql"
	else
	    echo "Install backup data of mysql failed"
	    exit 1
	fi
	;;

	4)
	./dbrestore.sh
	install_error=$?
	if [ "$install_error" -eq "0" ]
	then
	    echo "You successfully restore data of mysql"
	else
	    echo "Install restore data of mysql failed"
	    exit 1
	fi
	;;

	q*)
	    break
	;;

	*)
	    echo Invalid choice
	;;
	esac
	    print_menu
	done

if [ "$install_error" -eq "0" ]
then
    echo "install databases successful !"
else
    echo "install databases failed !"
    exit 1
fi

echo "###########################################################"

exit 0

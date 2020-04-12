#! /bin/sh

if [ -z "${1}" ] || [ -z "${2}" ];
then
	echo "usage: ./startServer <username> <hostname>";
	exit;
fi;

scp server.c ${1}@${2}:~

ssh ${1}@${2} "gcc server.c -o server && ./server"

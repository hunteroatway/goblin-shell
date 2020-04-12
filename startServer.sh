#! /bin/sh

scp server.c ${1}@${2}:~

ssh ${1}@${2} "gcc server.c -o server && ./server"

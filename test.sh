#!/bin/bash

echo "TEST 1: connect/disconnect"
./procchat > ./tests/connect_dis.out &
./client Bob room1 < ./tests/disconnect.in
sleep 1
d=$(diff ./tests/connect_dis.out ./tests/connect_dis.exp)
if [ "$d" ]
then
    echo failed
else
    echo success
fi
rm ./tests/connect_dis.out
rm -r ./room1
pkill -f procchat
pkill -f client
sleep 1

echo "TEST 2: two client"
./procchat > ./tests/two_client.out &
./client Amy room1 < ./tests/disconnect.in
sleep 1
./client Bob room1 < ./tests/disconnect.in
sleep 1
d=$(diff ./tests/two_client.out ./tests/two_client.exp)
if [ "$d" ]
then
    echo failed
else
    echo success
fi
rm ./tests/two_client.out
rm -r ./room1
sleep 1
pkill -f procchat
pkill -f client
sleep 1

echo "TEST 3: two domain"
./procchat > ./tests/two_domain.out &
./client Amy room1 < ./tests/disconnect.in
sleep 1
./client Amy room2 < ./tests/disconnect.in
sleep 1
d=$(diff ./tests/two_domain.out ./tests/two_domain.exp)
if [ "$d" ]
then
    echo failed
else
    echo success
fi
sleep 1
rm ./tests/two_domain.out
rm -r ./room1/
rm -r ./room2
sleep 1
pkill -f procchat
pkill -f client
sleep 2


echo "TEST 4 SAY/REC"
./procchat > ./tests/say.out &
./client Amy room1 < ./tests/say.in
sleep 2
d=$(diff ./tests/say.out ./tests/say.exp)
if [ "$d" ]
then
    echo failed
    #echo $d
else
    echo success
fi
rm ./tests/say.out
sleep 1
rm -r ./room1
pkill -f procchat
pkill -f client
sleep 2

echo "TEST 5 SAY/REC MORE"
./procchat > ./tests/say_more.out &
./client Amy room1 < ./tests/say_more.in
sleep 4
d=$(diff ./tests/say_more.out ./tests/say_more.exp)
if [ "$d" ]
then
    echo failed
    #echo $d
else
    echo success
fi
rm ./tests/say_more.out
sleep 2
rm -r ./room1
pkill -f procchat
pkill -f client
sleep 2

echo "TEST 6 SAYCONT/RECCONT"
./procchat > ./tests/saycont.out &
./client Amy room1 < ./tests/saycont.in
sleep 3
d=$(diff ./tests/saycont.out ./tests/saycont.exp)
if [ "$d" ]
then
    echo failed
    #echo $d
else
    echo success
fi
rm ./tests/saycont.out
rm -r ./room1
pkill -f procchat
pkill -f client
sleep 2

echo "TEST 7 SAYCONT/RECCONT MORE"
./procchat > ./tests/mix.out &
./client Amy room1 < ./tests/mix.in
sleep 3
d=$(diff ./tests/mix.out ./tests/mix.exp)
if [ "$d" ]
then
    echo failed
    #echo $d
else
    echo success
fi
rm ./tests/mix.out
sleep 1
rm -r ./room1
pkill -f procchat
pkill -f client
sleep 2

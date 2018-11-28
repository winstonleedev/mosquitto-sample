# Modern mosquitto C sample

How to write a C MQTT client using Mosquitto The 2018 version.

Read the full article [on my blog](https://thanhphu.net/2018/11/28/how-to-write-a-c-mqtt-client-using-mosquitto/)

#Requirements
(MacOS / Brew)
```
brew install libssl-dev
brew install c-ares
```

#Running
Compile and run the code with
```
gcc -Wall -o mypub mypub.c -lmosquitto -lssl -lcrypto -pthread -lcares && ./mypub
```
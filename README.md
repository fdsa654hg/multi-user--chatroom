# multiprocess-chatroom

## Prerequisites
Ubuntu 20.04  
c++14 or later  

## Compile to build Client&Server
```shell
$ cd src
$ make
$ ./Server (or ./client)
```

## Introduce
1. c++ socket TCP溝通的聊天室  
2. std::thread使server能支援多個client聊天  
3. state pattern維護private的密語模式和public的公開模式  

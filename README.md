# cwmp_grpc

gRPC server and client implementations for CWMP. 

https://www.broadband-forum.org provides CWMP xml files.

    tr-069-1-0-0-full.xml
    tr-098-1-8-0-full.xml
    tr-104-2-0-0-full.xml
    tr-106-1-2-0-full.xml
    tr-135-1-4-0-full.xml
    tr-140-1-3-0-full.xml
    tr-196-2-1-0-full.xml

[cwmp_xml_to_proto](https://github.com/jaemannyeh/cwmp_xml_to_proto) generates CWMP proto files using CWMP xml files. 

    tr-140-1-3-0-full.proto
    tr-140-1-3-0-full.methods

Have a look at the files, and use the terminal to build them:

    $ make clean
    $ make tr-140

Run Server:

    $ ./tr-140_server &
    [1] 3693
    tr-140_server.dat: File not found. BOOSTRAP
    D0826 15:37:12.685537848    3693 tr-140_server.cc:219]       Server listening on 0.0.0.0:50051

Run client:

    $ ./tr-140_client 1
    D0826 15:38:38.981401326    3699 tr-140_server.cc:78]        SetStorageService device_.enable 1 
    D0826 15:38:38.981823385    3723 tr-140_client.cc:116]       uptime 86 alias  # A server-side streaming RPC
    D0826 15:38:39.983058987    3723 tr-140_client.cc:116]       uptime 87 alias  # A server-side streaming RPC

Shutdown server:

    $ ./tr-140_client 0
    D0826 15:40:13.717173145    3700 tr-140_server.cc:78]        SetStorageService device_.enable 0 
    [1]+  Done                    ./tr-140_server
 
jaemannyeh

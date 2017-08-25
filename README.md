# cwmp_grpc

Have a look at the files, and use the terminal to build them:

    $ make 
    protoc -I ./ --cpp_out=. tr-140.proto
    g++ -std=c++11 -I/usr/local/include -pthread  -c -o tr-140.pb.o tr-140.pb.cc
    protoc -I ./ --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` tr-140.proto
    g++ -std=c++11 -I/usr/local/include -pthread  -c -o tr-140.grpc.pb.o tr-140.grpc.pb.cc
    g++ -std=c++11 -I/usr/local/include -pthread  -c -o storage_server.o storage_server.cc
    g++ tr-140.pb.o tr-140.grpc.pb.o storage_server.o -L/usr/local/lib `pkg-config --libs grpc++` -Wl,--no-as-needed -lgrpc -lgrpc++_reflection -Wl,--as-needed -lprotobuf -lpthread -ldl -o storage_server
    g++ -std=c++11 -I/usr/local/include -pthread  -c -o storage_client.o storage_client.cc
    g++ tr-140.pb.o tr-140.grpc.pb.o storage_client.o -L/usr/local/lib `pkg-config --libs grpc++` -Wl,--no-as-needed -lgrpc -lgrpc++_reflection -Wl,--as-needed -lprotobuf -lpthread -ldl -o storage_client

Run Server:

    $ ./storage_server &
    [1] 12415
    storage_server.dat: File not found. BOOSTRAP
    I0825 02:32:52.994846785   12415 server_builder.cc:262]      Synchronous server. Num CQs: 8, Min pollers: 1, Max Pollers: 2, CQ timeout (msec): 10000
    I0825 02:32:52.995022740   12415 ev_epollsig_linux.c:90]     epoll engine will be using signal: 40
    D0825 02:32:52.995035994   12415 ev_posix.c:121]             Using polling engine: epollsig
    D0825 02:32:52.995074585   12415 dns_resolver.c:316]         Using native dns resolver
    D0825 02:32:53.154354115   12415 storage_server.cc:171]      Server listening on 0.0.0.0:50051

Run client:

    $ ./storage_client 1
    I0825 02:33:06.874127628   12433 ev_epollsig_linux.c:90]     epoll engine will be using signal: 40
    D0825 02:33:06.874216003   12433 ev_posix.c:121]             Using polling engine: epollsig
    D0825 02:33:06.874256487   12433 dns_resolver.c:316]         Using native dns resolver
    D0825 02:33:06.887626634   12433 storage_client.cc:63]       enable 1 alias StorageDevice
    D0825 02:33:06.888132175   12433 storage_client.cc:87]       uptime 13 alias 
    D0825 02:33:07.889488311   12433 storage_client.cc:87]       uptime 14 alias 

Shutdown server:

    $ ./storage_client 0
    I0825 02:33:11.447027989   12440 ev_epollsig_linux.c:90]     epoll engine will be using signal: 40
    D0825 02:33:11.447138159   12440 ev_posix.c:121]             Using polling engine: epollsig
    D0825 02:33:11.447189166   12440 dns_resolver.c:316]         Using native dns resolver
    $ 
    [1]+  Done                    ./storage_server
 
jaemannyeh

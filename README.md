# cwmp_grpc

$ make
protoc -I ./ --cpp_out=. tr-098.proto
g++ -std=c++11 -I/usr/local/include -pthread  -c -o tr-098.pb.o tr-098.pb.cc
protoc -I ./ --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` tr-098.proto
g++ -std=c++11 -I/usr/local/include -pthread  -c -o tr-098.grpc.pb.o tr-098.grpc.pb.cc



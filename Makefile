
CXXFLAGS += -std=c++11
CPPFLAGS += -I/usr/local/include -pthread

LDFLAGS += -L/usr/local/lib `pkg-config --libs grpc++`
LDFLAGS += -Wl,--no-as-needed -lgrpc -lgrpc++_reflection -Wl,--as-needed
LDFLAGS += -lprotobuf -lpthread -ldl
           
PROTO_FILES := $(wildcard ./*.proto) 
PB_CC_FILES := $(patsubst %.proto, %.pb.cc, $(notdir $(PROTO_FILES)))
GRPC_PB_CC_FILES := $(patsubst %.proto, %.grpc.pb.cc, $(notdir $(PROTO_FILES)))
PB_O_FILES := $(patsubst %.proto, %.pb.o, $(notdir $(PROTO_FILES)))
GRPC_PB_O_FILES := $(patsubst %.proto, %.grpc.pb.o, $(notdir $(PROTO_FILES)))

.PHONY: default clean show

default:

tr-069: tr-069_server tr-069_client

tr-098: tr-098_server tr-098_client

tr-104: tr-104_server tr-104_client

tr-106: tr-106_topics tr-106_server tr-106_client

tr-106_topics:
	@echo topics : ClientAsyncResponseReader, .proto type bytes 
	@echo
	
tr-135: tr-135_server tr-135_client

tr-140: tr-140_topics tr-140_server tr-140_client

tr-140_topics:
	@echo topics : ServerReaderWriter
	@echo

tr-196: tr-196_server tr-196_client

%_server: %.pb.o %.grpc.pb.o %_server.o
	$(CXX) $^ $(LDFLAGS) -o $@
	
%_client: %.pb.o %.grpc.pb.o %_client.o
	$(CXX) $^ $(LDFLAGS) -o $@

pb_cc: $(PB_CC_FILES) $(GRPC_PB_CC_FILES)

pb_obj: $(PB_O_FILES) $(GRPC_PB_O_FILES)

%.pb.cc: %.proto
	protoc -I ./ --cpp_out=. $<

%.grpc.pb.cc: %.proto
	protoc -I ./ --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $<

clean:
	-rm *.pb.cc *.pb.h *.o *.dat tr-???_client tr-???_server
	
show:
	@echo PROTO_FILES = $(PROTO_FILES)
	@echo PB_CC_FILES = $(PB_CC_FILES)
	@echo GRPC_PB_CC_FILES = $(GRPC_PB_CC_FILES)
	@echo PB_O_FILES = $(PB_O_FILES)
	@echo GRPC_PB_O_FILES = $(GRPC_PB_O_FILES)	



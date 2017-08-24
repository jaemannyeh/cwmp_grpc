
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

pb_cc: $(PB_CC_FILES) $(GRPC_PB_CC_FILES)

pb_obj: $(PB_O_FILES) $(GRPC_PB_O_FILES)

%.pb.cc: %.proto
	protoc -I ./ --cpp_out=. $<

%.grpc.pb.cc: %.proto
	protoc -I ./ --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $<

storage_server: $(PB_O_FILES) $(GRPC_PB_O_FILES) storage_server.o
	$(CXX) $^ $(LDFLAGS) -o $@
	
clean:
	-rm *.pb.cc *.pb.h *.o
	
show:
	@echo PROTO_FILES = $(PROTO_FILES)
	@echo PB_CC_FILES = $(PB_CC_FILES)
	@echo GRPC_PB_CC_FILES = $(GRPC_PB_CC_FILES)
	@echo PB_O_FILES = $(PB_O_FILES)
	@echo GRPC_PB_O_FILES = $(GRPC_PB_O_FILES)	



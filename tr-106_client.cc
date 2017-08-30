// See examples/cpp/route_guide
//
// jaemannyeh

#include <chrono>
#include <iostream>
#include <iomanip>      // std::setw
#include <memory>
#include <random>
#include <string>
#include <thread>

#include <grpc/support/log.h>
#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>

#include "tr-106.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

using grpc::CompletionQueue; // using grpc::ServerCompletionQueue;
using grpc::ClientAsyncResponseReader; // using grpc::ServerAsyncResponseWriter;

using std::chrono::system_clock;

using tr106::Device; // package tr106; // message Device {}
using tr106::BoardRequest;
using tr106::BoardReply;
using tr106::Board; // service Board {}

static std::string get_localtime() {
  gpr_timespec now = gpr_now(GPR_CLOCK_REALTIME); // see grpc/src/core/lib/support/log_linux.c
  time_t timer = (time_t)now.tv_sec; // int64_t tv_sec; int32_t tv_nsec
  struct tm tm;
  char time_buffer[64];
  localtime_r(&timer, &tm);
  strftime(time_buffer, sizeof(time_buffer), "%m%d %H:%M:%S.", &tm);
  sprintf(&time_buffer[14],"%09d",now.tv_nsec);
  std::string time_string(time_buffer);
  return time_string;
}

class BoardClient {
public:
  BoardClient(std::shared_ptr<Channel> channel) : stub_(Board::NewStub(channel)) {
    gpr_log(GPR_DEBUG, "%s", __FUNCTION__);         
  }

  void SetDevice(bool shutdown=false) {
    gpr_log(GPR_DEBUG, "%s %d", __FUNCTION__,shutdown);         
    ClientContext context;
    tr106::Device device;
    BoardReply reply;
    if (shutdown) {
      device.set_device_summary("shutdown");
    } else {
      device.set_device_summary(__FILE__);
    }
    Status status = stub_->SetDevice(&context, device, &reply);
    if (!status.ok()) {
      gpr_log(GPR_DEBUG, "rpc failed"); 
      return;
    }     
  }
  
  void GetDeviceInfo() {
    gpr_log(GPR_DEBUG, "%s", __FUNCTION__);  
    
    struct AsyncClientCall {
        std::string request_time;
        tr106::Device::DeviceInfo info; // reply from the server
        ClientContext context;
        Status status; // Storage for the status of the RPC upon completion.
        std::unique_ptr<ClientAsyncResponseReader<tr106::Device::DeviceInfo>> response_reader;
    };
    
    CompletionQueue queue;
    CompletionQueue *que = &queue;
    
    std::thread transmitter([que]() {
        void* tag;
        bool ok = false;
        int loop=0;
        
        while (que->Next(&tag, &ok)) { // NextStatus AsyncNext (void **tag, bool *ok, const T &deadline)
          AsyncClientCall* call = static_cast<AsyncClientCall*>(tag);
          if (call->status.ok()) {
            //gpr_log(GPR_DEBUG, "up_time %d request_time %s reply_time %s # ClientAsyncResponseReader", call->info.up_time(), call->request_time.c_str(), get_localtime().c_str());
            gpr_log(GPR_DEBUG, "%s", call->info.device_log().c_str());
          } else {
            gpr_log(GPR_DEBUG, "rpc failed");             
          }
          delete call;
          
          if (++loop==4) {
            break;
          }
        }
    }); 
    
    for (int loop=0; loop<4; loop++) {
      //ClientContext context;
      BoardRequest request;
      AsyncClientCall* call = new AsyncClientCall;
      call->request_time = get_localtime();
      call->response_reader = stub_->AsyncGetDeviceInfo(&call->context, request, &queue);
      call->response_reader->Finish(&call->info, &call->status, (void*)call); // Finish (R *msg, Status *status, void *tag)
      //std::this_thread::sleep_for(std::chrono::milliseconds(1100)); // To test an asynchronous client to call a remote method
    }
    
    transmitter.join();
  }
  
  void GetTime() {
    gpr_log(GPR_DEBUG, "%s", __FUNCTION__);         
    ClientContext context;
    BoardRequest request;
    tr106::Device::Time device_time;
    Status status = stub_->GetTime(&context, request, &device_time);
    if (!status.ok()) {
      gpr_log(GPR_DEBUG, "rpc failed"); 
      return;
    }     
    gpr_log(GPR_DEBUG, "time %s", device_time.current_local_time().c_str());
  }
  
  void GetUserInterface() {
    gpr_log(GPR_DEBUG, "%s", __FUNCTION__);         
    ClientContext context;
    BoardRequest request;
    tr106::Device::UserInterface interface;
    Status status = stub_->GetUserInterface(&context, request, &interface);
    if (!status.ok()) {
      gpr_log(GPR_DEBUG, "rpc failed"); 
      return;
    }  
    gpr_log(GPR_DEBUG, "isplogo.size() %d isplogo().length() %d ", (int)interface.isplogo().size(), (int)interface.isplogo().length());         
    
    if (interface.isplogo().length()==0) { // size_t is unsigned.
      return;
    }
    
    unsigned int b = (unsigned int)(unsigned char)interface.isplogo()[interface.isplogo().length()-1];    
    unsigned int c = (unsigned int)(unsigned char)interface.isplogo().c_str()[interface.isplogo().length()-1];

    unsigned char *bin = new unsigned char[interface.isplogo().length()];
    memcpy(bin,interface.isplogo().c_str(),interface.isplogo().length());
    unsigned int d = (unsigned int)bin[interface.isplogo().length()-1];    
    delete bin;
    
    std::vector<unsigned char> raw(interface.isplogo().begin(),interface.isplogo().end());
    unsigned int e = (unsigned int)*(raw.end()-1);

    gpr_log(GPR_DEBUG, "b %d c %d d %d e %d", b, c, d, e);         
  }
  
private:  
  std::unique_ptr<Board::Stub> stub_;  
};

static int RunClient(int mode) {
  BoardClient client(grpc::CreateChannel("localhost:50051",grpc::InsecureChannelCredentials()));
  
  if (mode==0) {
    client.SetDevice(true);
    return 0;
  }
  
  client.GetDeviceInfo(); // ClientAsyncResponseReader  

  client.GetTime();
  
  client.GetUserInterface(); // .proto type bytes
  
  return 0;
}

int main(int argc, char** argv) {
  assert(argc==2);
  
  gpr_set_log_verbosity(GPR_LOG_SEVERITY_DEBUG); // GPR_LOG_SEVERITY_DEBUG GPR_LOG_SEVERITY_INFO GPR_LOG_SEVERITY_ERROR  
  
  RunClient(atoi(argv[1]));
  
  return 0;
}
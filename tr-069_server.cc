// See examples/cpp/route_guide
//
// jaemannyeh

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <iomanip>      // std::setw
#include <fstream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/support/log.h>
#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>

#include "tr-069.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using grpc::ServerAsyncResponseWriter; // using grpc::ClientAsyncResponseReader;
using grpc::ServerCompletionQueue; // using grpc::CompletionQueue;

using std::chrono::system_clock;

using tr069::InternetGatewayDevice; // package tr069; // message InternetGatewayDevice {}
using tr069::GatewayRequest;
using tr069::GatewayReply;
using tr069::Gateway; // service Gateway {}

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

class GatewayImpl final : public Gateway::AsyncService { // public Gateway::Service { // service Gateway {}
public:
  explicit GatewayImpl(tr069::InternetGatewayDevice &device) : device_(device) {
    start_time_ = system_clock::now();
  }
  
private:
  tr069::InternetGatewayDevice &device_; // message InternetGatewayDevice {}
  std::chrono::system_clock::time_point start_time_;  
};

static int BuildAndRun(tr069::InternetGatewayDevice &device) {
  std::string server_address("0.0.0.0:50051");

  GatewayImpl service(device); // service Gateway {}
  
  ServerBuilder builder;
  
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials()); // InsecureChannelCredentials

  builder.RegisterService(&service); // builder.RegisterAsyncService(&service); // builder.RegisterAsyncGenericService(&service);
   
  auto queue = builder.AddCompletionQueue();
   
  std::unique_ptr<grpc::Server> server = builder.BuildAndStart(); // std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  gpr_log(GPR_DEBUG, "Server listening on %s", server_address.c_str());     

  void* tag;  // uniquely identifies a request.
  bool ok;
  while (true) {
    queue->Next(&tag, &ok);
    //static_cast<CallData*>(tag)->Proceed();
  }
    
  //server->Wait();
  //device.clear_device_summary();
  //while (device.device_summary().compare("shutdown")!=0) {
    //std::this_thread::sleep_for(std::chrono::seconds(2));
  //}

  server->Shutdown(); //server->GetHealthCheckService();   

  queue->Shutdown();

  return 0;
}

static int RunServer() {
  std::string data_file_name(__FILE__);
  data_file_name.erase(data_file_name.rfind("."));  
  data_file_name += ".dat";
  
  tr069::InternetGatewayDevice device; // package tr069; // message InternetGatewayDevice {}

  std::fstream input(data_file_name.c_str(), std::ios::in | std::ios::binary); // % hexdump -C Board.dat 
  if (!input) {
    std::cout << data_file_name << ": File not found. BOOSTRAP" << std::endl;
    //vice.mutable_device_config()->set_persistent_data("BOOSTRAP");
  } else if (!device.ParseFromIstream(&input)) {
    std::cerr << "Failed to parse device data. BOOSTRAP" << std::endl;
    //vice.mutable_device_config()->set_persistent_data("BOOSTRAP");    
    return -1;
  } else {
    // BOOT, not BOOTSTRAP
  }
  
  BuildAndRun(device);
  
  std::fstream output(data_file_name.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
  if (!device.SerializeToOstream(&output)) {
    std::cerr << "Failed to write device data." << std::endl;
    return -1;
  }   
  
  return 0;
}

int main(int argc, char** argv) {
  gpr_set_log_verbosity(GPR_LOG_SEVERITY_DEBUG); // GPR_LOG_SEVERITY_DEBUG GPR_LOG_SEVERITY_INFO GPR_LOG_SEVERITY_ERROR  
  RunServer();
  return 0;
}


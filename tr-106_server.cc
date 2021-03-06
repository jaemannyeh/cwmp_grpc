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

#include "tr-106.grpc.pb.h"

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

class BoardImpl final : public Board::Service { // service Board {}
public:
  explicit BoardImpl(tr106::Device &device) : device_(device) {
    start_time_ = system_clock::now();
  }

  Status GetDevice(ServerContext* context, const BoardRequest* request, tr106::Device* reply) override {
    gpr_log(GPR_DEBUG, "%s", __FUNCTION__);     
    reply->set_device_summary(__FILE__);
    return Status::OK;
  }
  
  Status SetDevice(ServerContext* context, const tr106::Device* request, BoardReply* reply) override {
    device_.set_device_summary(request->device_summary());
    return Status::OK;
  }

  Status GetServices(ServerContext* context, const BoardRequest* request, tr106::Device::Services* reply) override { return Status::OK; }
  Status SetServices(ServerContext* context, const tr106::Device::Services* request, BoardReply* reply) override { return Status::OK; }

  Status GetCapabilities(ServerContext* context, const BoardRequest* request, tr106::Device::Capabilities* reply) override { return Status::OK; }
  Status SetCapabilities(ServerContext* context, const tr106::Device::Capabilities* request, BoardReply* reply) override { return Status::OK; }

  Status GetPerformanceDiagnostic(ServerContext* context, const BoardRequest* request, tr106::Device::Capabilities::PerformanceDiagnostic* reply) override { return Status::OK; }
  Status SetPerformanceDiagnostic(ServerContext* context, const tr106::Device::Capabilities::PerformanceDiagnostic* request, BoardReply* reply) override { return Status::OK; }

  Status GetDeviceInfo(ServerContext* context, const BoardRequest* request, tr106::Device::DeviceInfo* reply) override {
    gpr_log(GPR_DEBUG, "%s", __FUNCTION__);    
    std::string request_time = get_localtime();
    static int static_latency = 5;
    if (--static_latency == 0) {
      static_latency = 5;
    }
    int latency = static_latency;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000*latency));// std::this_thread::sleep_for(std::chrono::seconds(latency)); // To test an asynchronous client to call a remote method   
    
    reply->set_up_time(std::chrono::duration_cast<std::chrono::seconds>(system_clock::now()-start_time_).count()); // elapsed time
    std::string reply_time = get_localtime();    
    reply->set_device_log("request_time " + request_time + " reply_time " + reply_time);
    return Status::OK;
  }
  
  Status SetDeviceInfo(ServerContext* context, const tr106::Device::DeviceInfo* request, BoardReply* reply) override { return Status::OK; }

  Status GetManagementServer(ServerContext* context, const BoardRequest* request, tr106::Device::ManagementServer* reply) override { return Status::OK; }
  Status SetManagementServer(ServerContext* context, const tr106::Device::ManagementServer* request, BoardReply* reply) override { return Status::OK; }

  Status GetGatewayInfo(ServerContext* context, const BoardRequest* request, tr106::Device::GatewayInfo* reply) override { return Status::OK; }
  Status SetGatewayInfo(ServerContext* context, const tr106::Device::GatewayInfo* request, BoardReply* reply) override { return Status::OK; }

  Status GetConfig(ServerContext* context, const BoardRequest* request, tr106::Device::Config* reply) override { return Status::OK; }
  Status SetConfig(ServerContext* context, const tr106::Device::Config* request, BoardReply* reply) override { return Status::OK; }

  Status GetTime(ServerContext* context, const BoardRequest* request, tr106::Device::Time* reply) override {
    reply->set_current_local_time(get_localtime());    
    return Status::OK;
  }
    
  Status SetTime(ServerContext* context, const tr106::Device::Time* request, BoardReply* reply) override { return Status::OK; }

  Status GetUserInterface(ServerContext* context, const BoardRequest* request, tr106::Device::UserInterface* reply) override {
    gpr_log(GPR_DEBUG, "%s", __FUNCTION__);   
    unsigned char raw[256]; // do not try a number bigger than 256.
    memset(raw,0,sizeof(raw));
    raw[sizeof(raw)/2-1] = sizeof(raw)/2-1;
    raw[sizeof(raw)-1] = sizeof(raw)-1;
    reply->set_isplogo(raw,sizeof(raw)); // bytes
    //unsigned int b = (unsigned int)(unsigned char)reply->isplogo()[reply->isplogo().length()-1];    
    //unsigned int c = (unsigned int)(unsigned char)reply->isplogo().c_str()[reply->isplogo().length()-1];    
    return Status::OK;
  }
  Status SetUserInterface(ServerContext* context, const tr106::Device::UserInterface* request, BoardReply* reply) override { return Status::OK; }

  Status GetLAN(ServerContext* context, const BoardRequest* request, tr106::Device::LAN* reply) override { return Status::OK; }
  Status SetLAN(ServerContext* context, const tr106::Device::LAN* request, BoardReply* reply) override { return Status::OK; }

  Status GetDHCPOption(ServerContext* context, const BoardRequest* request, ServerWriter<tr106::Device::LAN::DHCPOption>* reply) override { return Status::OK; }
  Status SetDHCPOption(ServerContext* context, ServerReader<tr106::Device::LAN::DHCPOption>* request, BoardReply* reply) override { return Status::OK; }

  Status GetStats(ServerContext* context, const BoardRequest* request, tr106::Device::LAN::Stats* reply) override { return Status::OK; }
  Status SetStats(ServerContext* context, const tr106::Device::LAN::Stats* request, BoardReply* reply) override { return Status::OK; }

  Status GetIPPingDiagnostics(ServerContext* context, const BoardRequest* request, tr106::Device::LAN::IPPingDiagnostics* reply) override { return Status::OK; }
  Status SetIPPingDiagnostics(ServerContext* context, const tr106::Device::LAN::IPPingDiagnostics* request, BoardReply* reply) override { return Status::OK; }

  Status GetTraceRouteDiagnostics(ServerContext* context, const BoardRequest* request, tr106::Device::LAN::TraceRouteDiagnostics* reply) override { return Status::OK; }
  Status SetTraceRouteDiagnostics(ServerContext* context, const tr106::Device::LAN::TraceRouteDiagnostics* request, BoardReply* reply) override { return Status::OK; }

  Status GetRouteHops(ServerContext* context, const BoardRequest* request, ServerWriter<tr106::Device::LAN::TraceRouteDiagnostics::RouteHops>* reply) override { return Status::OK; }
  Status SetRouteHops(ServerContext* context, ServerReader<tr106::Device::LAN::TraceRouteDiagnostics::RouteHops>* request, BoardReply* reply) override { return Status::OK; }

  Status GetDownloadDiagnostics(ServerContext* context, const BoardRequest* request, tr106::Device::DownloadDiagnostics* reply) override { return Status::OK; }
  Status SetDownloadDiagnostics(ServerContext* context, const tr106::Device::DownloadDiagnostics* request, BoardReply* reply) override { return Status::OK; }

  Status GetUploadDiagnostics(ServerContext* context, const BoardRequest* request, tr106::Device::UploadDiagnostics* reply) override { return Status::OK; }
  Status SetUploadDiagnostics(ServerContext* context, const tr106::Device::UploadDiagnostics* request, BoardReply* reply) override { return Status::OK; }

  Status GetUDPEchoConfig(ServerContext* context, const BoardRequest* request, tr106::Device::UDPEchoConfig* reply) override { return Status::OK; }
  Status SetUDPEchoConfig(ServerContext* context, const tr106::Device::UDPEchoConfig* request, BoardReply* reply) override { return Status::OK; }

private:
  tr106::Device &device_; // message Device {}
  std::chrono::system_clock::time_point start_time_;
};

static int BuildAndRun(tr106::Device &device) {
  std::string server_address("0.0.0.0:50051");

  BoardImpl service(device); // service Board {}
  
  ServerBuilder builder;
  
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials()); // InsecureChannelCredentials

  builder.RegisterService(&service);
  
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  gpr_log(GPR_DEBUG, "Server listening on %s", server_address.c_str());     

  //server->Wait();

  device.clear_device_summary();
  while (device.device_summary().compare("shutdown")!=0) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  server->Shutdown(); //server->GetHealthCheckService();   

  return 0;
}

static int RunServer() {
  std::string data_file_name(__FILE__);
  data_file_name.erase(data_file_name.rfind("."));  
  data_file_name += ".dat";

  tr106::Device device; // package tr106; // message Device {}

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

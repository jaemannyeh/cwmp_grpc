// See examples/cpp/route_guide
//
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

#include "tr-140.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

using std::chrono::system_clock;

using tr140::StorageService; // package tr140; // message StorageService {}
using tr140::StorageRequest;
using tr140::StorageReply;
using tr140::Storage; // service Storage {}

static std::string get_localtime() {
  gpr_timespec now = gpr_now(GPR_CLOCK_REALTIME); // see grpc/src/core/lib/support/log_linux.c
  time_t timer = (time_t)now.tv_sec;
  struct tm tm;
  char time_buffer[64];
  localtime_r(&timer, &tm);
  strftime(time_buffer, sizeof(time_buffer), "%m%d %H:%M:%S", &tm);
  std::string time_string(time_buffer);
  return time_string;
}

class StorageClient {
public:
  StorageClient(std::shared_ptr<Channel> channel) : stub_(Storage::NewStub(channel)) {
  }
  
  void GetStorageService() {
    ClientContext context;
    StorageRequest request;
    tr140::StorageService device; // reply
    
    Status status = stub_->GetStorageService(&context, request, &device);
    if (!status.ok()) {
      gpr_log(GPR_DEBUG, "rpc failed"); 
      return;
    } 
    gpr_log(GPR_DEBUG, "enable %d alias %s", device.enable(), device.alias().c_str()); 
  }

  void SetStorageService(bool shutdown=false) {
    ClientContext context;
    tr140::StorageService device; // request
    StorageReply reply;

    device.set_enable((shutdown) ? false : true);
    device.set_alias("StorageBox");
    Status status = stub_->SetStorageService(&context, device, &reply);
    if (!status.ok()) {
      gpr_log(GPR_DEBUG, "rpc failed"); 
      return;
    } 
  }

  void SetUserAccount() {
    // A client-side streaming RPC
    ClientContext context;
    tr140::StorageService::UserAccount account; // request
    StorageReply reply;    
    
    std::unique_ptr<ClientWriter<tr140::StorageService::UserAccount>> stream_tx(stub_->SetUserAccount(&context, &reply));
    
    account.set_username("Peter"); account.set_user_group_participation("McKinney");
    stream_tx->Write(account);
    account.set_username("Matthew"); account.set_user_group_participation("McKinney");
    stream_tx->Write(account);
    account.set_username("Philip"); account.set_user_group_participation("Dallas");
    stream_tx->Write(account);
    account.set_username("Andrew"); account.set_user_group_participation("Dallas");
    stream_tx->Write(account);
    account.set_username("Nathanael"); account.set_user_group_participation("Dallas");
    stream_tx->Write(account);
    
    stream_tx->WritesDone();
    Status status = stream_tx->Finish();
    if (!status.ok()) {
      gpr_log(GPR_DEBUG, "rpc failed"); 
      return;
    }    
  }

  void GetPhysicalMedium() {
    ClientContext context;
    StorageRequest request; 
    tr140::StorageService::PhysicalMedium medium; // reply

    std::unique_ptr<ClientReader<tr140::StorageService::PhysicalMedium>> stream_rx(stub_->GetPhysicalMedium(&context, request));  
    while (stream_rx->Read(&medium)) {
      gpr_log(GPR_DEBUG, "uptime %d alias %s # A server-side streaming RPC", medium.uptime(), medium.alias().c_str());               
    }

    Status status = stream_rx->Finish();
    if (!status.ok()) {
      gpr_log(GPR_DEBUG, "rpc failed"); 
      return;
    }    
  }  
  
  void SetPhysicalMedium() {
    // A client-side streaming RPC
    ClientContext context;
    tr140::StorageService::PhysicalMedium medium; // request
    StorageReply reply;
    
    std::unique_ptr<ClientWriter<tr140::StorageService::PhysicalMedium>> stream_tx(stub_->SetPhysicalMedium(&context, &reply));

    medium.set_alias("McKinney");
    stream_tx->Write(medium);
    medium.set_alias("Dallas");
    stream_tx->Write(medium);
    medium.set_alias("Houston");
    stream_tx->Write(medium);

    stream_tx->WritesDone();
    Status status = stream_tx->Finish();
    if (!status.ok()) {
      gpr_log(GPR_DEBUG, "rpc failed"); 
      return;
    }       
  }
  
  void GetMatchedUserAccout() {
    // A bidirectional streaming RPC
    // rpc GetMatchedUserAccout(stream tr140.StorageService.UserGroup) returns (stream tr140.StorageService.UserAccount) {} // .{i}.
    // grpc::ServerReaderWriter< W, R > Class Template 
    // grpc::ClientReaderWriter< W, R > Class Template
    // ServerReaderWriter<tr140::StorageService::UserAccount,tr140::StorageService::UserGroup>* stream
  
    ClientContext context;
    std::shared_ptr<ClientReaderWriter<tr140::StorageService::UserGroup,tr140::StorageService::UserAccount>> stream_tx_rx(stub_->GetMatchedUserAccout(&context));  

    std::thread transmitter(
      [stream_tx_rx]() { // lambda expression // wow! __FUNCTION__ is "operator()" in lambda expression.    
        tr140::StorageService::UserGroup group;    
  
        group.set_group_name("McKinney");
        stream_tx_rx->Write(group);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
        group.set_group_name("Dallas");
        stream_tx_rx->Write(group);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
        group.set_group_name("Houston");
        stream_tx_rx->Write(group);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
  
        stream_tx_rx->WritesDone();
      }
    );
    
    tr140::StorageService::UserAccount account;
    while (stream_tx_rx->Read(&account)) {
      gpr_log(GPR_DEBUG, "%s", account.username().c_str());               
    }
    
    transmitter.join();
    
    Status status = stream_tx_rx->Finish();
    if (!status.ok()) {
      gpr_log(GPR_DEBUG, "rpc failed"); 
    }         
  }
  
private:  
  std::unique_ptr<Storage::Stub> stub_;
};

int RunClient(int mode) {
  
  StorageClient client(grpc::CreateChannel("localhost:50051",grpc::InsecureChannelCredentials()));

  if (mode == 0) {
    client.SetStorageService(true); // true will shutdown the server.
    return 0;
  } 

  client.GetStorageService();
  
  client.SetStorageService();
  
  client.GetPhysicalMedium();
  
  client.SetPhysicalMedium();

  client.SetUserAccount();
  
  client.GetMatchedUserAccout();
  
  return 0;
}

int main(int argc, char** argv) {
  assert(argc==2);
  
  gpr_set_log_verbosity(GPR_LOG_SEVERITY_DEBUG); // GPR_LOG_SEVERITY_DEBUG GPR_LOG_SEVERITY_INFO GPR_LOG_SEVERITY_ERROR  
  
  RunClient(atoi(argv[1]));
  
  return 0;
}
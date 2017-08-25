// See examples/cpp/route_guide
//
// Note: tr140::StorageService is a message. // package tr140; // message StorageService {}
//       tr140::Storage is a service. // service Storage {}
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

#include "tr-140.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
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

class StorageImpl final : public Storage::Service { // service Storage {}
public:
  explicit StorageImpl(tr140::StorageService &device) : device_(device) {
    start_time_ = system_clock::now();
    
    device_.set_enable(true); // bool
    
    if (device.alias().empty()) {
      device_.set_alias("StorageDevice");
    }
    
    if (device_.physical_medium_size()==0) {
      device_.add_physical_medium(); // StorageService.{i}.PhysicalMedium.{i}.
      device_.add_physical_medium(); // StorageService.{i}.PhysicalMedium.{i}.
    }
  }

  Status GetStorageService(ServerContext* context, const StorageRequest* request, tr140::StorageService* reply) override {
    reply->set_enable(device_.enable());
    reply->set_alias(device_.alias());
    return Status::OK;
  }
  
  Status SetStorageService(ServerContext* context, const tr140::StorageService* request, StorageReply* reply) override {
    device_.set_enable(request->enable());
    gpr_log(GPR_DEBUG, "%s device_.enable %d ", __FUNCTION__, device_.enable()); 
    device_.set_alias(request->alias());
    return Status::OK;
  }

  Status GetCapabilities(ServerContext* context, const StorageRequest* request, tr140::StorageService::Capabilities* reply) override { return Status::OK; }
  Status SetCapabilities(ServerContext* context, const tr140::StorageService::Capabilities* request, StorageReply* reply) override { return Status::OK; }

  Status GetNetInfo(ServerContext* context, const StorageRequest* request, tr140::StorageService::NetInfo* reply) override { return Status::OK; }
  Status SetNetInfo(ServerContext* context, const tr140::StorageService::NetInfo* request, StorageReply* reply) override { return Status::OK; }

  Status GetUserGroup(ServerContext* context, const StorageRequest* request, ServerWriter<tr140::StorageService::UserGroup>* reply) override { return Status::OK; }
  Status SetUserGroup(ServerContext* context, ServerReader<tr140::StorageService::UserGroup>* request, StorageReply* reply) override {
    return Status::OK;
  }

  Status GetUserAccount(ServerContext* context, const StorageRequest* request, ServerWriter<tr140::StorageService::UserAccount>* reply) override { return Status::OK; }
  Status SetUserAccount(ServerContext* context, ServerReader<tr140::StorageService::UserAccount>* request, StorageReply* reply) override {
    // A client-side streaming RPC
    // ServerReader is not const. why?
    
    tr140::StorageService::UserAccount account;    
    while (request->Read(&account)) {
      int i;
      for (i=0; i<device_.user_account_size(); i++) {
        if (account.username() == device_.user_account(i).username()) {
          break; // found an existing user.
        }
      }
      if (i==device_.user_account_size()) {
        *device_.add_user_account() = account;
      }
      gpr_log(GPR_DEBUG, "%d name %s user_account_size %d", i, account.username().c_str(), device_.user_account_size());
    }
    return Status::OK;
  }

  Status GetNetworkServer(ServerContext* context, const StorageRequest* request, tr140::StorageService::NetworkServer* reply) override { return Status::OK; }
  Status SetNetworkServer(ServerContext* context, const tr140::StorageService::NetworkServer* request, StorageReply* reply) override { return Status::OK; }

  Status GetFTPServer(ServerContext* context, const StorageRequest* request, tr140::StorageService::FTPServer* reply) override { return Status::OK; }
  Status SetFTPServer(ServerContext* context, const tr140::StorageService::FTPServer* request, StorageReply* reply) override { return Status::OK; }

  Status GetAnonymousUser(ServerContext* context, const StorageRequest* request, tr140::StorageService::FTPServer::AnonymousUser* reply) override { return Status::OK; }
  Status SetAnonymousUser(ServerContext* context, const tr140::StorageService::FTPServer::AnonymousUser* request, StorageReply* reply) override { return Status::OK; }

  Status GetSFTPServer(ServerContext* context, const StorageRequest* request, tr140::StorageService::SFTPServer* reply) override { return Status::OK; }
  Status SetSFTPServer(ServerContext* context, const tr140::StorageService::SFTPServer* request, StorageReply* reply) override { return Status::OK; }

  Status GetHTTPServer(ServerContext* context, const StorageRequest* request, tr140::StorageService::HTTPServer* reply) override { return Status::OK; }
  Status SetHTTPServer(ServerContext* context, const tr140::StorageService::HTTPServer* request, StorageReply* reply) override { return Status::OK; }

  Status GetHTTPSServer(ServerContext* context, const StorageRequest* request, tr140::StorageService::HTTPSServer* reply) override { return Status::OK; }
  Status SetHTTPSServer(ServerContext* context, const tr140::StorageService::HTTPSServer* request, StorageReply* reply) override { return Status::OK; }

  Status GetPhysicalMedium(ServerContext* context, const StorageRequest* request, ServerWriter<tr140::StorageService::PhysicalMedium>* reply) override {
    //A server-side streaming RPC
    
    for (int i=0; i<device_.physical_medium_size(); i++) {
      device_.mutable_physical_medium(i)->set_uptime(std::chrono::duration_cast<std::chrono::seconds>(system_clock::now()-start_time_).count());

      reply->Write(device_.physical_medium(i));
      
      std::this_thread::sleep_for(std::chrono::milliseconds(1001));      
    }    
    return Status::OK;
  }
  
  Status SetPhysicalMedium(ServerContext* context, ServerReader<tr140::StorageService::PhysicalMedium>* request, StorageReply* reply) override {
    // A client-side streaming RPC
    // ServerReader is not const. why?
    
    tr140::StorageService::PhysicalMedium medium;
    int i = 0;
    while (request->Read(&medium)) {
      if (i>=device_.physical_medium_size()) {
        device_.add_physical_medium();
      }
      device_.mutable_physical_medium(i++)->set_alias(medium.alias());
    }
    return Status::OK;
  }

  Status GetSMART(ServerContext* context, const StorageRequest* request, tr140::StorageService::PhysicalMedium::SMART* reply) override { return Status::OK; }
  Status SetSMART(ServerContext* context, const tr140::StorageService::PhysicalMedium::SMART* request, StorageReply* reply) override { return Status::OK; }

  Status GetStorageArray(ServerContext* context, const StorageRequest* request, ServerWriter<tr140::StorageService::StorageArray>* reply) override { return Status::OK; }
  Status SetStorageArray(ServerContext* context, ServerReader<tr140::StorageService::StorageArray>* request, StorageReply* reply) override { return Status::OK; }

  Status GetLogicalVolume(ServerContext* context, const StorageRequest* request, ServerWriter<tr140::StorageService::LogicalVolume>* reply) override { return Status::OK; }
  Status SetLogicalVolume(ServerContext* context, ServerReader<tr140::StorageService::LogicalVolume>* request, StorageReply* reply) override { return Status::OK; }

  Status GetFolder(ServerContext* context, const StorageRequest* request, ServerWriter<tr140::StorageService::LogicalVolume::Folder>* reply) override { return Status::OK; }
  Status SetFolder(ServerContext* context, ServerReader<tr140::StorageService::LogicalVolume::Folder>* request, StorageReply* reply) override { return Status::OK; }

  Status GetUserAccess(ServerContext* context, const StorageRequest* request, ServerWriter<tr140::StorageService::LogicalVolume::Folder::UserAccess>* reply) override { return Status::OK; }
  Status SetUserAccess(ServerContext* context, ServerReader<tr140::StorageService::LogicalVolume::Folder::UserAccess>* request, StorageReply* reply) override { return Status::OK; }

  Status GetGroupAccess(ServerContext* context, const StorageRequest* request, ServerWriter<tr140::StorageService::LogicalVolume::Folder::GroupAccess>* reply) override { return Status::OK; }
  Status SetGroupAccess(ServerContext* context, ServerReader<tr140::StorageService::LogicalVolume::Folder::GroupAccess>* request, StorageReply* reply) override { return Status::OK; }

  Status GetQuota(ServerContext* context, const StorageRequest* request, tr140::StorageService::LogicalVolume::Folder::Quota* reply) override { return Status::OK; }
  Status SetQuota(ServerContext* context, const tr140::StorageService::LogicalVolume::Folder::Quota* request, StorageReply* reply) override { return Status::OK; }

  Status GetMatchedUserAccout(ServerContext* context, ServerReaderWriter<tr140::StorageService::UserAccount,tr140::StorageService::UserGroup>* stream) override {
    // A bidirectional streaming RPC
    // rpc GetMatchedUserAccout(stream tr140.StorageService.UserGroup) returns (stream tr140.StorageService.UserAccount) {} // .{i}.
    // grpc::ServerReaderWriter< W, R > Class Template 
    // grpc::ClientReaderWriter< W, R > Class Template
    // ServerReaderWriter<tr140::StorageService::UserAccount,tr140::StorageService::UserGroup>* stream
    
    tr140::StorageService::UserGroup group;
    while (stream->Read(&group)) { 
      gpr_log(GPR_DEBUG, "group %s user_account_size %d", group.group_name().c_str(),device_.user_account_size());
      for (int i=0; i<device_.user_account_size(); i++) {
        if (device_.user_account(i).user_group_participation() == group.group_name()) {
          stream->Write(device_.user_account(i));
        }
      }
      //std::this_thread::sleep_for(std::chrono::milliseconds(1001)); 
    }
    return Status::OK;
  }

private:
  tr140::StorageService &device_; // message StorageService {}
  std::chrono::system_clock::time_point start_time_;
};

static int BuildAndRun(tr140::StorageService &device) {
  std::string server_address("0.0.0.0:50051");

  StorageImpl service(device); // service Storage {}
  
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  gpr_log(GPR_DEBUG, "Server listening on %s", server_address.c_str());     

  while (device.enable()==true) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  std::this_thread::sleep_for(std::chrono::seconds(2));
  
  server->Shutdown(); // //server->GetHealthCheckService();   //server->Wait();

  return 0;
}

static int RunServer() {
  std::string data_file_name(__FILE__);
  data_file_name.erase(data_file_name.rfind("."));  
  data_file_name += ".dat";

  tr140::StorageService device; // package tr140; // message StorageService {}

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
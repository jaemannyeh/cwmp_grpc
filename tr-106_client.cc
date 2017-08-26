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

using std::chrono::system_clock;

using tr106::Device; // package tr106; // message Device {}
using tr106::BoardRequest;
using tr106::BoardReply;
using tr106::Board; // service Board {}

int main(int argc, char** argv) {
  assert(argc==2);
  
  gpr_set_log_verbosity(GPR_LOG_SEVERITY_DEBUG); // GPR_LOG_SEVERITY_DEBUG GPR_LOG_SEVERITY_INFO GPR_LOG_SEVERITY_ERROR  
  
  //TBD;
  
  return 0;
}
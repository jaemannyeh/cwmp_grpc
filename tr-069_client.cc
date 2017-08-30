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

static int RunClient(int mode) {
  return 0;
}

int main(int argc, char** argv) {
  assert(argc==2);
  
  gpr_set_log_verbosity(GPR_LOG_SEVERITY_DEBUG); // GPR_LOG_SEVERITY_DEBUG GPR_LOG_SEVERITY_INFO GPR_LOG_SEVERITY_ERROR  
  
  //RunClient(atoi(argv[1]));
  
  return 0;
}
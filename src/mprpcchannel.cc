#include "mprpcapplication.h"
#include <arpa/inet.h>
#include <cstring>
#include <mprpcchannel.h>
#include <netinet/in.h>
#include <rpcHeader.pb.h>
#include <string>
#include <sys/socket.h>
#include "zookeeperutil.h"
#include <sys/types.h>
#include <unistd.h>

/*
rpc_send_str=rpcHeader+request.tostring
rpcHeader=uint32 header_size + service_name + method_name + argv_size
header_size = (service_name + method_name + argv_size).size()
*/

void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              ::google::protobuf::RpcController *controller,
                              const ::google::protobuf::Message *request,
                              ::google::protobuf::Message *response,
                              ::google::protobuf::Closure *done) {
  const ::google::protobuf::ServiceDescriptor *serviceDesc = method->service();
  const std::string serviceName = serviceDesc->name(); // 获取服务名字
  const std::string methodName = method->name();

  std::string requestStr;
  std::string errorStr;
  if (!request->SerializeToString(&requestStr)) {
    errorStr = "MprpcChannel::CallMethod request Serialize To String ERROR";
    controller->SetFailed(errorStr);
    return;
  }

  uint32_t argv_size = requestStr.size();
  mprpc::RpcHeader rpcHeader;
  rpcHeader.set_argv_size(argv_size);
  rpcHeader.set_method_name(methodName);
  rpcHeader.set_service_name(serviceName);

  std::string rpcHeaderStr;
  if (!rpcHeader.SerializeToString(&rpcHeaderStr)) {
    errorStr = "MprpcChannel::CallMethod rpcHeader Serialize To String ERROR";
    controller->SetFailed(errorStr);
    return;
  }
  uint32_t header_size = rpcHeaderStr.size();

  // 组织待发送的rpc请求字符串
  std::string rpcSendStr;
  rpcSendStr.insert(0, std::string((char *)&header_size, 4));
  rpcSendStr += rpcHeaderStr;
  rpcSendStr += requestStr;

  // 打印调试信息
  std::cout << "=================================================" << std::endl;
  std::cout << "serviceName:" << serviceName << std::endl;
  std::cout << "methodName:" << methodName << std::endl;
  std::cout << "argv_size:" << argv_size << std::endl;
  std::cout << "requestStr:" << requestStr << std::endl;
  std::cout << "rpcSendstr:" << rpcSendStr << std::endl;
  std::cout << "=================================================" << std::endl;

  // 使用tcp变成，完成rpc方法的远程调用
  int clientfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == clientfd) {
    errorStr = "Creat Socket Error! errno:" + std::to_string(errno) +
               "  Error Message:" + strerror(errno);
    controller->SetFailed(errorStr);
    return;
  }

  // 读取配置文件rpcserver的信息
  // std::string ip = MprpcApplication::getConfig().value("rpcserver_ip");
  // uint16_t port =
  //     std::stoi(MprpcApplication::getConfig().value("rpcserver_port").c_str());
  
  ZkClient Zkc;
  Zkc.Start();
  std::string path="/"+serviceName+"/"+methodName;
  std::string host=Zkc.GetZnodeData(path.c_str());
  if(host.empty())
  {
    errorStr = "CallMethod Get Znode Data Error!";
    controller->SetFailed(errorStr);
    return;
  }
  size_t index=host.find(':');
  std::string ip = host.substr(0,index);
  uint16_t port =std::stoi(host.substr(index+1).c_str());

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

  // 连接rpc节点
  if (-1 ==
      connect(clientfd, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
    errorStr = "Connect error!errno: " + std::to_string(errno) +
               "  Error Message:" + strerror(errno);
    controller->SetFailed(errorStr);
    close(clientfd);
    return;
  }

  // 发送rpc请求
  if (-1 == send(clientfd, rpcSendStr.c_str(), rpcSendStr.size(), 0)) {
    errorStr = "Send error!errno: " + std::to_string(errno) +
               "  Error Message:" + strerror(errno);
    controller->SetFailed(errorStr);
    close(clientfd);
    return;
  }

  // 接收rpc请求的响应值
  char recv_buf[1024] = {0};
  int recv_size = 0;
  if (-1 == (recv_size = recv(clientfd, recv_buf, 1024, 0))) {
    errorStr = "Recv error!errno: " + std::to_string(errno) +
               "  Error Message:" + strerror(errno);
    controller->SetFailed(errorStr);
    close(clientfd);
    return;
  }

  // 反序列化rpc调用的请求数据
  std::string response_str(recv_buf, recv_size);
  if (!response->ParseFromString(response_str)) {
    errorStr = "ParseFromString Error! response_str: " + response_str;
    controller->SetFailed(errorStr);
    close(clientfd);
    return;
  }
  close(clientfd);
}
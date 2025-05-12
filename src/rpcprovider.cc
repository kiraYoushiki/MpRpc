#include "rpcprovider.h"
#include "mprpcapplication.h"
#include "zookeeperutil.h"
#include <cstdio>
#include <cstring>
#include <functional>
#include <google/protobuf/descriptor.h>
#include <string>

void RpcProvider::NotifyService(google::protobuf::Service *service) {
  // 获取服务对象的描述信息
  const google::protobuf::ServiceDescriptor *pserverDesc =
      service->GetDescriptor();
  // 获取服务名字
  std::string service_name = pserverDesc->name();
  // 获取服务对象service的方法数量
  int methodCnt = pserverDesc->method_count();

  std::cout << "service name:" << service_name << std::endl;

  for (int i = 0; i < methodCnt; ++i) {
    // 获取服务对象指定下标方法的描述
    const google::protobuf::MethodDescriptor *pmethodDesc =
        pserverDesc->method(i);
    std::string method_name = pmethodDesc->name();

    service_info.m_methodMap[method_name] = pmethodDesc;
  }
  service_info.m_service = service;
  m_serviceMap[service_name] = service_info;
}

void RpcProvider::run() {
  std::string ip = MprpcApplication::getConfig().value("rpcserver_ip");
  uint16_t port =
      std::stoi(MprpcApplication::getConfig().value("rpcserver_port").c_str());
  muduo::net::InetAddress address(ip, port);

  // 创建TcpServer对象
  muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");
  // 绑定连接回调和消息读写回调
  server.setConnectionCallback(
      std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
  server.setMessageCallback(
      std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

  // 设置muduo库的线程数量
  server.setThreadNum(4);

  ZkClient Zkc;
  Zkc.Start();
  for (const auto &it : m_serviceMap) {
    std::string service_path = "/" + it.first;
    Zkc.CreatZnode(service_path.c_str(), nullptr, 0);
    service_path += '/';
    for (const auto &it2 : it.second.m_methodMap) {
      service_path += it2.first;
      char host[128];
      sprintf(host, "%s:%d", ip.c_str(), port);
      Zkc.CreatZnode(service_path.c_str(), host, strlen(host), 1);
    }
  }

  // 启动网络服务
  server.start();
  m_eventLoop.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn) {
  // 和rpc client连接断开
  if (!conn->connected()) {
    conn->shutdown();
  }
}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn,
                            muduo::net::Buffer *buffer, muduo::Timestamp) {
  // 从网络上接收rpc请求的字节流
  std::string recv_buf = buffer->retrieveAllAsString();
  // 从字节流中读取前4个字节的内容
  uint32_t header_size = 0;
  recv_buf.copy((char *)&header_size, 4, 0);

  // 根据header_size 的长度读取数据头的原始字节流，得到rpc请求的详细信息
  std::string rpc_header_str = recv_buf.substr(4, header_size);
  mprpc::RpcHeader rpc_header;
  std::string service_name, method_name;
  uint32_t argv_size;

  if (rpc_header.ParseFromString(rpc_header_str)) {
    // 数据头序列化成功
    service_name = rpc_header.service_name();
    method_name = rpc_header.method_name();
    argv_size = rpc_header.argv_size();
  } else {
    // 数据头序列化失败
    std::cout << "Parse From String Failed!" << std::endl;
    return;
  }
  std::string argv = recv_buf.substr(4 + header_size, argv_size);
  std::cout << "=================================================" << std::endl;
  std::cout << "service_name:      " << service_name << std::endl;
  std::cout << "method_name:       " << method_name << std::endl;
  std::cout << "argv_size:         " << argv_size << std::endl;
  std::cout << "argv:              " << argv << std::endl;
  std::cout << "=================================================" << std::endl;

  auto it = m_serviceMap.find(service_name);
  if (it == m_serviceMap.end()) {
    std::cout << "Service Map Doesn't has this Service!" << std::endl;
    return;
  }
  google::protobuf::Service *service = it->second.m_service;
  auto mit = it->second.m_methodMap.find(method_name);
  if (mit == it->second.m_methodMap.end()) {
    std::cout << "This Service Dosen't has this Method!" << std::endl;
    return;
  }

  google::protobuf::Message *request =
      service->GetRequestPrototype(mit->second).New();
  google::protobuf::Message *response =
      service->GetResponsePrototype(mit->second).New();
  if (!request->ParseFromString(argv)) {
    std::cout << "Failed to parse argv into request message!" << std::endl;
    delete request;
    delete response;
    return;
  }
  google::protobuf::Closure *done =
      google::protobuf::NewCallback<RpcProvider,
                                    const muduo::net::TcpConnectionPtr &,
                                    google::protobuf::Message *>(
          this, &RpcProvider::SendRpcResponse, conn, response);

  service->CallMethod(mit->second, nullptr, request, response, done);
  delete request;
  delete response;
}

// closure回调操作，用于序列化rpc响应和网络发送
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn,
                                  google::protobuf::Message *response) {
  std::string strResonse;
  if (response->SerializeToString(&strResonse)) {
    conn->send(strResonse);
  } else
    std::cout << "SendRpcResponse:Serialize To String ERROR!" << std::endl;
  //模拟http短连接请求，给其他更多的客户端的请求腾出资源
  conn->shutdown();
}
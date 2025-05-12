#pragma once
#include "google/protobuf/service.h"
#include "rpcHeader.pb.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>
#include <unordered_map>


//框架提供的，专门用于发布rpc服务的网络对象类
class RpcProvider{
    public:
    //框架给外部提供的用于发布rpc方法的接口
    void NotifyService(google::protobuf::Service* service);
    
    void run();
    private:
    //组合EventLoop
    muduo::net::EventLoop m_eventLoop;

    //服务类型信息
    struct ServiceInfo
    {
        google::protobuf::Service* m_service;//保存服务对象
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> m_methodMap;//保存服务方法
    }service_info;
    //存储注册成功的服务及其方法的所有信息
    std::unordered_map<std::string,ServiceInfo> m_serviceMap;

    //新的socket连接回调
    void OnConnection(const muduo::net::TcpConnectionPtr& conn);
    //新的已建立连接的读写时间回调
    void OnMessage(const muduo::net::TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp);
    //closure回调操作，用于序列化rpc响应和网络发送
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&,google::protobuf::Message*);
};
# MpRpc - 高性能 C++ RPC 框架

## 🚀 项目简介
基于 Muduo、Protobuf 和 Zookeeper 实现的轻量级 RPC 框架，支持服务注册、发现、序列化和高并发处理。

## 🧱 技术栈
- Muduo（网络通信）
- Protobuf（序列化）
- Zookeeper（服务注册/发现）

## ✨ 特性亮点
- 自定义协议封装，支持多种消息类型
- 基于事件驱动的非阻塞网络通信
- 动态服务注册与发现机制（Zookeeper）
- 自动代码生成（.proto 文件）

## 🧪 示例运行
```bash
./autobuild.sh
cd bin
./server_example
./client_example

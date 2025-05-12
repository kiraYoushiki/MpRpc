#pragma once_Nonnull
#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>

//封装的zk客户端类
class ZkClient
{
    public:
        ZkClient();
        ~ZkClient();
        //zkclient开始连接zkserver
        void Start();
        //根据指定的路径创建Znode
        void CreatZnode(const char* path,const char* data,int datalen,int state=0);
        //获取指定路径Znode的值
        std::string GetZnodeData(const char* path);
    private:
    //zk的客户端连接句柄
    zhandle_t* m_zhandle;
};
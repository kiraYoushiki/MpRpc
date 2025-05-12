#include "zookeeperutil.h"
#include "mprpcapplication.h"
#include <cstdlib>
#include <semaphore.h>
#include <iostream>
#include <string>
#include <zookeeper/zookeeper.h>

//全局的观察器 zkserver给zkclient的通知
void global_watcher(zhandle_t* zh,int type,int state,const char* path,void* watcherCtx)
{
    if(type==ZOO_SESSION_EVENT)//回调的消息类型
    {
        if(state==ZOO_CONNECTED_STATE)
        {
            sem_t* sem=(sem_t*)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}
ZkClient::ZkClient():m_zhandle(nullptr)
{

}
ZkClient::~ZkClient()
{
    //关闭句柄，释放资源
    if(m_zhandle)zookeeper_close(m_zhandle);
}
// zkclient开始连接zkserver
void ZkClient::Start()
{
    std::string ip=MprpcApplication::getInstance().getConfig().value("zookeeper_ip");
    std::string port=MprpcApplication::getInstance().getConfig().value("zookeeper_port");
    std::string host=ip+':'+port;
    /*zookeeper多线程版本
    client提供三个线程
    1.主线程api调用线程
    2.网络io线程
    3.watcher回调线程
    */
    m_zhandle=zookeeper_init(host.c_str(), global_watcher, 30000, nullptr, nullptr, 0);

    if(!m_zhandle)
    {
        std::cout<<"ZooKeeper Init error!"<<std::endl;
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem,0,0);
    zoo_set_context(m_zhandle, &sem);
    
    sem_wait(&sem);
    std::cout<<"zookeeper_init success!"<<std::endl;
}
//根据指定的路径创建Znode
void ZkClient::CreatZnode(const char *path, const char *data, int datalen, int state)
{
    char path_buffer[128];
    size_t buf=sizeof(path_buffer);
    int flag;
    //先判断指定路径是不是已经有了
    flag=zoo_exists(m_zhandle, path, 0, nullptr);
    if(flag==ZNONODE)
    {
        //开始创建
        flag =zoo_create(m_zhandle, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, buf);
        if(flag==ZOK)
        {
            std::cout<<"Znode "<<path<<" Creat Successfully!"<<std::endl;
        }
        else
        {
             std::cout<<"Znode "<<path<<" Creat Error"<<std::endl;
             std::cout<<"flag: "<<flag<<std::endl;
        }
    }
}
//获取指定路径Znode的值
std::string ZkClient::GetZnodeData(const char *path)
{
    char buffer[64];
    int len=sizeof(buffer);
    int flag=zoo_get(m_zhandle, path, 0, buffer, &len, nullptr);
    if(flag==ZOK)
        return buffer;

    std::cout<<"Get "<<path<<" Znode Data Error!"<<std::endl;
    return "";
}
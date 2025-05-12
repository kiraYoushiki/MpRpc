#include "mprpcapplication.h"
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <string>

MprpcConfig MprpcApplication::m_config;

MprpcConfig& MprpcApplication::getConfig(){
    return m_config;
}

void ShowArgsHelp(){
    std::cout<<"format : command -i <configfile>"<<std::endl;
}

MprpcApplication& MprpcApplication::getInstance()
{
    static MprpcApplication app;
    return app;
}    

void MprpcApplication::Init(int argc,char** argv)
{
    if(argc<2){
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c=0;
    std::string config_file;
    while((c=getopt(argc,argv,"i:"))!=-1)
    {
        switch (c)
        {
        case 'i':
            config_file=optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    //开始加载配置文件 rpcserver_ip= rpcserver_port= zookeeper_ip= zookeeper_port= 
    m_config.loadConfig(config_file.c_str());

    std::cout<<"rpcserver_ip:"<<m_config.value("rpcserver_ip")<<std::endl;
    std::cout<<"rpcserver_port:"<<m_config.value("rpcserver_port")<<std::endl;
    std::cout<<"zookeeper_ip:"<<m_config.value("zookeeper_ip")<<std::endl;
    std::cout<<"zookeeper_port:"<<m_config.value("zookeeper_port")<<std::endl;
}
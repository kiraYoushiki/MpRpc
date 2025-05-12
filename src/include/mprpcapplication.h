#pragma once

#include "mprpcconfig.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"
#include "logger.h"
// 负责框架的初始化操作
class MprpcApplication
{
public:
    // 调用框架的初始化操作
    static void Init(int argc, char **argv);
    static MprpcApplication &getInstance();
    static MprpcConfig& getConfig();
private:
    static MprpcConfig m_config;
    MprpcApplication() {}
    MprpcApplication(const MprpcApplication &) = delete;
    MprpcApplication(MprpcApplication &&) = delete;
};
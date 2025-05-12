#include "logger.h"
#include "mprpcapplication.h"
#include <user.pb.h>

int main(int argc, char **argv)
{
    LOG_INFO("Fisrst logger message!");
    LOG_INFO("%s:%s:%d",__FILE__,__FUNCTION__,__LINE__);

    MprpcApplication::Init(argc, argv);
    fixBug::UserServiceRpc_Stub stub(new MprpcChannel());

    fixBug::LoginRequest request;
    request.set_name("张三");
    request.set_pwd("12345678w");
    fixBug::LoginResponse response;

    MprpcController rpccontroller;
    stub.Login(&rpccontroller, &request, &response, nullptr);
    if (rpccontroller.Failed())
    {
        std::cout<<"Rpc Method Login() called Error with msg: "<<rpccontroller.ErrorText()<<std::endl;
    }
    else
    {
        if (response.success())
        {
            std::cout << "Login success!" << std::endl;
        }
        else
        {
            std::cout << "Login failed with message:" << response.result().errmsg() << std::endl;
            std::cout << "Fail Code:" << response.result().errcode() << std::endl;
        }
    }
    return 0;
}
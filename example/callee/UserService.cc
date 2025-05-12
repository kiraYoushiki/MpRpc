#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "user.pb.h"
#include <string>
#include <iostream>

class UserService:public fixBug::UserServiceRpc{
    public:
        bool Login(const std::string& name,const std::string& pwd){
            std::cout<<"doing: Login"<<std::endl;
            std::cout<<"name: "<<name<<std::endl;
            std::cout<<"pwd: "<<pwd<<std::endl;

            return false;
        }
        void Login(::google::protobuf::RpcController* controller,
                       const ::fixBug::LoginRequest* request,
                       ::fixBug::LoginResponse* response,
                       ::google::protobuf::Closure* done)
        {
            const std::string name=request->name();
            const std::string pwd=request->pwd();

            bool result=Login(name,pwd);

            response->set_success(result);
            fixBug::ResultCode* code=response->mutable_result();
            code->set_errcode(1);
            code->set_errmsg("账号或密码错误!");

            done->Run();
        }

    private:
};

int main(int argc,char** argv){
    //调用框架的初始化操作
    MprpcApplication::Init(argc,argv);

    //provider是一个rpc网络服务对象，把UserService对象发布到rpc节点
    RpcProvider provider;
    provider.NotifyService(new UserService());
    
    //启动一个rpc服务发布节点 Run以后，进入阻塞状态，等待远程的rpc调用
    provider.run();

    return 0;
}
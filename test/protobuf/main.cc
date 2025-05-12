#include "test.pb.h"
#include <iostream>
#include <string>

using namespace fixbug;

int main() {
    LoginRequest req;
    req.set_name("test");
    req.set_pwd("123456");

    std::string send_str;   
    if(req.SerializeToString(&send_str)){
        std::cout << "send_str7787654: " << send_str << std::endl;
    }

    LoginRequest req2;
    if(req2.ParseFromString(send_str)){
        std::cout<<req2.name()<<std::endl;
        std::cout<<"反序列化成功"<<std::endl;
        std::cout<<req2.pwd()<<std::endl;
    }
    else std::cout<<"没有反序列化成功"<<std::endl;

    return 0;
}
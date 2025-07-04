#pragma once
#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>


class MprpcChannel : public google::protobuf::RpcChannel
{
public:
    void CallMethod(const google::protobuf::MethodDescriptor *method,
                    google::protobuf::RpcController *controller, const google::protobuf::Message *request,
                    google::protobuf::Message *response, google::protobuf::Closure *done);

private:
};
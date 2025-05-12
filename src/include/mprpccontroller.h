#pragma once

#include "google/protobuf/service.h"
#include "google/protobuf/stubs/callback.h"

class MprpcController : public google::protobuf::RpcController
{
public:
    void Reset();
    bool Failed()const;
    std::string ErrorText()const;
    void SetFailed(const std::string &reason);

    
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(::google::protobuf::Closure *callback);

private:
    bool fail=false;
    std::string errortext;
};
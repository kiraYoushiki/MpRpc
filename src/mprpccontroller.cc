#include "mprpccontroller.h"

void MprpcController::Reset()
{
    errortext = "";
    fail = false;
}

bool MprpcController::Failed()const
{
    return fail;
}

std::string MprpcController::ErrorText()const
{
    return errortext;
}

void MprpcController::SetFailed(const std::string &reason)
{
    fail = true;
    errortext = reason;
}
void MprpcController::StartCancel() {}
bool MprpcController::IsCanceled() const 
{
    return true;
}
void MprpcController::NotifyOnCancel(::google::protobuf::Closure *callback) {}

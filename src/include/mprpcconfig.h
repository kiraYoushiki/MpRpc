#pragma once

#include <string>
#include <unordered_map>

class MprpcConfig
{
public:
    //加载配置文件
    void loadConfig(const char *configFile);
    //查询配置文件
    std::string value(const std::string &key);

private:
    std::unordered_map<std::string, std::string> m_configMap;
    void remSpa(std::string& strbuf);
};

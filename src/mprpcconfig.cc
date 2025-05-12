#include "mprpcconfig.h"
#include <iostream>

void MprpcConfig::loadConfig(const char* configFile)
{
    FILE *fp = fopen(configFile, "r");
    if (!fp)
    {
        std::cout << "Config File Does Not Exist!" << std::endl;
        exit(EXIT_FAILURE);
    }

    char buffer[512];
    while (fgets(buffer, 512, fp))
    {
        std::string strbuf(buffer);
        remSpa(strbuf);
        size_t index = strbuf.find('=');
        if (index != std::string::npos)
        {
            std::string key = strbuf.substr(0, index);
            std::string value = strbuf.substr(index + 1, strbuf.size() - index - 1);
            remSpa(key);
            remSpa(value);
            m_configMap[key] = value.substr(0,value.size()-1);
        }
    }
}
void MprpcConfig::remSpa(std::string &strbuf)
{
    size_t index = strbuf.find_first_not_of(' ');
    strbuf = strbuf.substr(index, strbuf.size() - index);
    index = strbuf.find_last_not_of(' ');
    strbuf = strbuf.substr(0, index + 1);
}
std::string MprpcConfig::value(const std::string &key)
{
    if (m_configMap.count(key))
        return m_configMap[key];
    return "";
}
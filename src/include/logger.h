#pragma once

#include "lockqueue.h"
#include <cstdio>

enum LogLevel{
    INFO,//普通信息
    ERROR,//错误信息
};


class Logger{
    public: 
        //获取日志单例
        static Logger& GetInstance();
        //设置日志级别
        void SetLogLevel(LogLevel level);
        //写日志
        void Log(std::string data);
        ~Logger();
    private:
    int m_loglevel;//记录日志级别
    LockQueue<std::string> lQ;//日志缓冲队列
    Logger();
    Logger(const Logger&)=delete;
    Logger(Logger&&)=delete;
    Logger& operator=(const Logger&)=delete;
    Logger& operator=(Logger&&)=delete;
};

#define LOG_INFO(logmsgformat,...)\
    do\
    {\
        Logger& logger=Logger::GetInstance();\
        logger.SetLogLevel(INFO);\
        char c[1024] = {0};\
        snprintf(c,1024,logmsgformat,##__VA_ARGS__);\
        logger.Log(c);\
    }while(0)

#define LOG_ERRO(logmsgformat,...)\
    do\
    {\
        Logger& logger=Logger::GetInstance();\
        logger.SetLogLevel(ERROR);\
        char c[1024] = {0};\
        snprintf(c,1024,logmsgformat,##__VA_ARGS__);\
        logger.Log(c);\
    }while(0)

   
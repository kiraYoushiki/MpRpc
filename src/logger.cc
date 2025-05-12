#include "logger.h"
#include "lockqueue.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>

Logger &Logger::GetInstance() {
  static Logger logger;
  return logger;
}

Logger::Logger() {
  std::thread writeLogTask([&]() {
    for (;;) {
      //获取当天的日期,然后取日志信息,写入相应的日志
      time_t now = time(nullptr);
      tm *nowtm = localtime(&now);

      char file_name[128];
      sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year + 1900,
              nowtm->tm_mon + 1, nowtm->tm_mday);

      FILE *fp = fopen(file_name, "a+");
      if (!fp) {
        std::cout << "logger file: " << file_name << " open error!"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      std::string msg = lQ.pop();
      if(msg.empty())return;

      char time_buf[128] = {0};
      sprintf(time_buf, "%d:%d:%d ===> ", nowtm->tm_hour, nowtm->tm_min,
              nowtm->tm_sec);

      msg.insert(0, time_buf);
      msg.append("\n");
      fputs(msg.c_str(), fp);
      fclose(fp);
    }
  });
  //设置分离线程
  writeLogTask.detach();
}

void Logger::SetLogLevel(LogLevel level) { m_loglevel = level; }

//把日志信息谢日缓冲区
void Logger::Log(std::string data) { lQ.push(data); }

Logger::~Logger()
{
  Log("");
}
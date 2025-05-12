#pragma once
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

template<typename T>
class LockQueue{
    public:
        inline void push(const T data)
        {
            std::lock_guard<std::mutex> lock(queMtex);
            lockQue.push(data);
            conVar.notify_one();
        }
        inline T pop()
        {
            std::unique_lock<std::mutex> lock(queMtex);
            conVar.wait(lock,[this]{return !lockQue.empty();});
            T data=lockQue.front();
            lockQue.pop();
            return data;
        }
    private:
        std::queue<T> lockQue;
        std::mutex queMtex;
        std::condition_variable conVar;
};
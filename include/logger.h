#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <vector>
#include <string>
#include <thread>
#include <fstream>
#include <mutex>
#include <future>
#include <condition_variable>

namespace nvrec {


enum CommitStatus {
    kSucces,
    kFail
};
typedef std::pair<std::string,std::string>  KVPair;

class Logger {
public:
    Logger(const std::string &file_name, size_t frequency, size_t transactions);
    ~Logger();
    CommitStatus Commit(const std::string &table, const std::string &key,
                        const std::vector<KVPair> &values);
private:
    void WriteToBuffer(const std::string &table, const std::string &key,
                       const std::vector<KVPair> &values, std::promise<CommitStatus> &&commit_promise);
    size_t frequency_, transactions_, tx_counter_ = 0;
    std::ofstream file_;
    std::mutex m_;
    std::vector<std::promise<CommitStatus>> pending_promises_;
    std::condition_variable commit_cv_;
    bool notified_ = false;
    std::thread logger_;

};

}

#endif // LOGGER_H

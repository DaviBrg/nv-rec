#include "logger.h"

#include <string_view>

using namespace nvrec;

constexpr std::string_view kUpdate = "UPDATE";

Logger::Logger(const std::string &file_name, size_t frequency, size_t transactions):
    frequency_(frequency),
    transactions_(transactions),
    file_(file_name, std::ofstream::out | std::ofstream::app),
    logger_([&](){
        std::unique_lock<std::mutex> lock(m_);
            for(;;) {
                while (!notified_) {
                commit_cv_.wait(lock);
                }
            }
        }) {}

Logger::~Logger() {
    file_.close();
}

CommitStatus Logger::Commit(const std::string &table, const std::string &key,
                            const std::vector<KVPair> &values) {
    std::promise<CommitStatus> commit_promise;
    auto commit_future = commit_promise.get_future();
    WriteToBuffer(table, key, values, std::move(commit_promise));
    return commit_future.get();
}

void Logger::WriteToBuffer(const std::string &table, const std::string &key,
                           const std::vector<KVPair> &values, std::promise<CommitStatus> &&commit_promise) {
    {
        std::lock_guard lg(m_);
        file_ << kUpdate << " ";
        file_ << table << " ";
        file_ << key << " ";
        for (auto &value : values) {
            file_ << value.first << " ";
            file_ << value.second << " ";
        }
        file_ << "\n";
        tx_counter_++;
        pending_promises_.emplace_back(std::move(commit_promise));
    }
    if (tx_counter_ >= transactions_) {
        commit_cv_.notify_one();
    }
}

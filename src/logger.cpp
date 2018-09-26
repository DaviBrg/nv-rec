#include "logger.h"

#include <string_view>
#include <algorithm>

using namespace nvrec;

constexpr std::string_view kUpdate = "UPDATE";

Logger::Logger(const std::string &file_name, size_t tx_timeout, size_t tx_limit):
    tx_timeout_(tx_timeout),
    txs_limit_(tx_limit),
    file_(file_name, std::ofstream::out | std::ofstream::app) {
    logger_ = std::thread([&](){
        for(;;) {
            std::unique_lock lk(commit_m_);
            empty_cv.wait(lk, [this](){
                return !commit_list_.empty()
                        ;});
            group_commit_cv_.wait_for(lk,
                                      std::chrono::milliseconds(tx_timeout),
                                      [this](){
                return commit_list_.front().jobs.size() >= txs_limit_;
            });

            AtomicFlush();
            KeepPromises(commit_list_.front());
            commit_list_.pop_front();
            if (!commit_list_.empty()) {
                ExecuteJobs(commit_list_.front());
            }

        }
    });
    logger_.detach();
}

Logger::~Logger() {
    try {
        file_.close();
    }
    catch (std::exception&) {
        std::abort();
    }
}

inline void Logger::AtomicFlush() {
    std::lock_guard lg{file_m_};
    file_.flush();
}

void Logger::ExecuteJobs(Logger::CommitGroup &cg) {
    for (auto& job : cg.jobs) {
        AtomicWriteToFile(job.table(), job.key(), job.values());
    }
}

void Logger::KeepPromises(Logger::CommitGroup &cg) {
    for (auto& promise : cg.promises) {
        promise.set_value(kSucces);
    }
}



CommitStatus Logger::Commit(const std::string &table, const std::string &key,
                            const std::vector<KVPair> &values) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::promise<CommitStatus> result_promise;
    TxJob job(std::move(table), std::move(key), std::move(values));
    auto result = result_promise.get_future();
    {
        std::lock_guard lg(commit_m_);
        auto it = std::find_if(std::begin(commit_list_),
                               std::end(commit_list_),
                               [this](CommitGroup &cg){
            return cg.jobs.size() < txs_limit_;
        });
        if (it != std::end(commit_list_)) {
            it->promises.emplace_back(std::move(result_promise));
            it->jobs.emplace_back(std::move(job));
            if (it->jobs.size() >= txs_limit_) {
                group_commit_cv_.notify_one();
            }
        } else {
            CommitPromises promises;
            promises.emplace_back(std::move(result_promise));
            CommitJobs jobs;
            jobs.emplace_back(std::move(job));
            commit_list_.emplace_back(std::move(promises), std::move(jobs));
            empty_cv.notify_one();
        }
    }
    return result.get();
}

void Logger::AtomicWriteToFile(const std::string &table, const std::string &key,
                           const std::vector<KVPair> &values) {
    std::lock_guard lg{file_m_};
    file_ << kUpdate << " ";
    file_ << table << " ";
    file_ << key << " ";
    for (auto &value : values) {
        file_ << value.first << " ";
        file_ << value.second << " ";
    }
    file_ << "\n";
}



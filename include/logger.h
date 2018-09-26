#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <list>
#include <vector>
#include <string>
#include <thread>
#include <fstream>
#include <mutex>
#include <future>
#include <condition_variable>
#include <atomic>
#include <memory>

namespace nvrec {


enum CommitStatus {
    kSucces,
    kFail
};
typedef std::pair<std::string,std::string>  KVPair;

class TxJob {
public:
    TxJob(std::string table, std::string key, std::vector<KVPair> values):
        table_(std::move(table)),
        key_(std::move(key)),
        values_(std::move(values)) {}
    std::string& table() {return table_;}
    std::string& key() {return key_;}
    std::vector<KVPair>& values() {return values_;}
private:
    std::string table_;
    std::string key_;
    std::vector<KVPair> values_;
};


class Logger {
public:
    Logger(const std::string &file_name, size_t tx_timeout, size_t tx_limit);
    ~Logger();
    CommitStatus Commit(const std::string &table, const std::string &key,
                        const std::vector<KVPair> &values);
private:
    typedef std::vector<std::promise<CommitStatus>> CommitPromises;
    typedef std::vector<TxJob> CommitJobs;
    class CommitGroup {
    public:
        CommitGroup(CommitPromises in_promises, CommitJobs in_jobs):
            promises(std::move(in_promises)),
            jobs(std::move(in_jobs)){}
        CommitPromises promises;
        CommitJobs jobs;
    };
    void AtomicWriteToFile(const std::string &table, const std::string &key,
                       const std::vector<KVPair> &values);
    void AtomicFlush();
    void ExecuteJobs(CommitGroup &cg);
    void KeepPromises(CommitGroup &cg);
    size_t tx_timeout_, txs_limit_;
    std::ofstream file_;
    std::mutex commit_m_, file_m_;
    std::condition_variable group_commit_cv_, empty_cv;
    std::list<CommitGroup> commit_list_;
    std::thread logger_;
};

}

#endif // LOGGER_H

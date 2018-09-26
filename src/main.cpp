#include "logger.h"

#include <iostream>

int main(int argc, char *argv[]) {
    nvrec::Logger lg("test.txt", 3000, 5);
//    lg.Commit("tabela", "chave", {{"nome","davi"},{"idade","28"},{"profissao","dev"}});

    std::vector<std::thread> threads;

    for (size_t i=0; i<1; i++) {
        std::vector<nvrec::KVPair> pairs = {{"nome","davi"},{"idade","28"},{"profissao","dev"}};
        threads.emplace_back(std::thread(&nvrec::Logger::Commit, &lg, "tabela", "chave", std::move(pairs)));
    }

    for (auto &th : threads) {
        th.join();
    }

    std::cout << "Finished" << std::endl;

    return 0;
}

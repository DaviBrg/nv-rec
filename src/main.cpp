#include "logger.h"

int main(int argc, char *argv[]) {
    nvrec::Logger lg("test.txt", 3, 100);
    lg.Commit("tabela", "chave", {{"nome","davi"},{"idade","28"},{"profissao","dev"}});
    return 0;
}

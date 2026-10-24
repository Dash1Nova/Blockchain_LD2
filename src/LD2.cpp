#include <random>
#include <chrono>

#include "hash.h"

struct User {
    std::string name;
    std::string public_key;
    long long balance;
};

std::vector<User> generateUsers() {
    std::vector<User> users;
    unsigned int seed = 100; 
    std::mt19937 generator(seed);
    std::uniform_int_distribution<long long> distribution(100, 1000000);

    for (int i = 1; i <= 1000; i++) {
        User newUser;
        newUser.name = "user" + std::to_string(i);
        newUser.public_key = hash(convertTextTB(newUser.name));
        newUser.balance = distribution(generator);
        users.push_back(newUser);
    }

    return users;
}

struct Transaction {
    std::string sender;
    std::string receiver;
    long long amount;
    int sequence;
    std::string transaction_id;
};

std::vector<Transaction> generateTransactions(const std::vector<User>& users) {
    std::vector<Transaction> transactions;
    unsigned int seed = 111; 
    std::mt19937 generator(seed);
    std::uniform_int_distribution<size_t> userDist(0, users.size() - 1);
    std::uniform_int_distribution<long long> amountDist(1, 1000);

    for (int i = 1; i <= 10000; i++) {
        Transaction trnsctn;
        size_t sIndex = userDist(generator);
        size_t rIndex = userDist(generator);

        while (sIndex == rIndex) {
            rIndex = userDist(generator);
        }

        trnsctn.sender = users[sIndex].public_key;
        trnsctn.receiver = users[rIndex].public_key;
        trnsctn.amount = amountDist(generator);
        trnsctn.sequence = i;
        std::string converting = trnsctn.sender + "|" + trnsctn.receiver + "|" + std::to_string(trnsctn.amount) + "|" + std::to_string(trnsctn.sequence);
        trnsctn.transaction_id = hash(convertTextTB(converting));
        transactions.push_back(trnsctn);
    }
    return transactions;
}

struct Block {
    std::string prev_hash;
    long long timestamp;
    int version;
    std::string tx_root;
    long long nonce;
    int difficulty;
    std::vector<Transaction> transactions;
};

Block genesisBlock() {
    Block genesis;
    genesis.prev_hash = "0000000000000000000000000000000000000000000000000000000000000000";
    genesis.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    genesis.version = 1;
    genesis.tx_root = hash(convertTextTB(""));
    genesis.nonce = 0;        
    genesis.difficulty = 3;   
    genesis.transactions = std::vector<Transaction>();
    return genesis;
}
#include <iostream>
#include <random>
#include <chrono>
#include <map>

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

std::vector<Block> runLoop(std::vector<User>& users, std::vector<Transaction>& transactions, std::vector<Block>& blockchain) {
    unsigned int seed = 210;
    std::mt19937 generator(seed);
    std::string lastHash = blockchain.back().prev_hash;

    while (!transactions.empty()) {
        std::vector<Transaction> selectedTxs;
        std::vector<Transaction> tempTxs = transactions;

        while (selectedTxs.size() < 100 && !tempTxs.empty()) {
            std::uniform_int_distribution<size_t> distribution(0, tempTxs.size() - 1);
            size_t randomIndex = distribution(generator);
            selectedTxs.push_back(tempTxs[randomIndex]);
            tempTxs.erase(tempTxs.begin() + randomIndex);
        }

        std::map<std::string, long long> copyBalances;

        for (size_t i = 0; i < users.size(); i++) {
            copyBalances[users[i].public_key] = users[i].balance;
        }

        std::vector<Transaction> validTransactions;
        long long totalBefore = 0;

        for (size_t i = 0; i < users.size(); i++) {
            totalBefore += users[i].balance;
        }

        for (size_t i = 0; i < selectedTxs.size(); i++) {
            Transaction transaction = selectedTxs[i];

            if (copyBalances[transaction.sender] >= transaction.amount) {
                copyBalances[transaction.sender] -= transaction.amount;
                copyBalances[transaction.receiver] += transaction.amount;
                validTransactions.push_back(transaction);
            }
        }

        long long totalAfter = 0;

        for (size_t i = 0; i < users.size(); i++) {
            totalAfter += copyBalances[users[i].public_key];
        }

        if (totalBefore != totalAfter) {
            std::cout << "Bendra lesu suma pasikeite!" << std::endl;
            break;
        }

        Block candidateBlock;
        candidateBlock.prev_hash = lastHash;
        candidateBlock.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        candidateBlock.version = 1;
        candidateBlock.nonce = 0;
        candidateBlock.difficulty = 3;
        candidateBlock.transactions = validTransactions;
        std::string joinedID = "";

        for (size_t i = 0; i < validTransactions.size(); i++) {
            joinedID += validTransactions[i].transaction_id;
        }

        candidateBlock.tx_root = hash(convertTextTB(joinedID));
        std::string blockHash = "";
        long long attempts = 0;

        auto miningStart = std::chrono::steady_clock::now();

        while (true) {
            std::string header = candidateBlock.prev_hash + "|" + std::to_string(candidateBlock.timestamp) + "|" + std::to_string(candidateBlock.version) + "|" + candidateBlock.tx_root + "|" + std::to_string(candidateBlock.nonce) + "|" + std::to_string(candidateBlock.difficulty);
            blockHash = hash(convertTextTB(header));
            attempts++;

            if (blockHash.substr(0, 3) == "000") {
                break;
            }
            candidateBlock.nonce++;
        }

        auto miningEnd = std::chrono::steady_clock::now();
        auto miningTime = std::chrono::duration<double>(miningEnd - miningStart).count();
        std::cout << "Kasimo laikas: " << miningTime << " s" << std::endl;

        std::string header = candidateBlock.prev_hash + "|" + std::to_string(candidateBlock.timestamp) + "|" + std::to_string(candidateBlock.version) + "|" + candidateBlock.tx_root + "|" + std::to_string(candidateBlock.nonce) + "|" + std::to_string(candidateBlock.difficulty);
        std::string checkedHash = hash(convertTextTB(header));

        if (checkedHash != blockHash || checkedHash.substr(0, 3) != "000" || candidateBlock.prev_hash != lastHash) {
            std::cout << "Blokas atmestas." << std::endl;
            break;
        }

        blockchain.push_back(candidateBlock);

        for (size_t i = 0; i < users.size(); i++) {
            users[i].balance = copyBalances[users[i].public_key];
        }

        for (size_t i = 0; i < validTransactions.size(); i++) {
            for (size_t j = 0; j < transactions.size(); j++) {
                if (transactions[j].transaction_id == validTransactions[i].transaction_id) {
                    transactions.erase(transactions.begin() + j);
                    break;
                }
            }
        }
        lastHash = checkedHash;
        std::cout << "Bloko aukstis: " << blockchain.size() - 1 << std::endl;
        std::cout << "Transakciju skaicius: " << validTransactions.size() << std::endl;
        std::cout << "Prev hash: " << candidateBlock.prev_hash << std::endl;
        std::cout << "Tx root: " << candidateBlock.tx_root << std::endl;
        std::cout << "Bloko hash: " << checkedHash << std::endl;
        std::cout << "Difficulty: " << candidateBlock.difficulty << std::endl;
        std::cout << "Nonce: " << candidateBlock.nonce << std::endl;
        std::cout << "Bandymai: " << attempts << std::endl;
    }
    return blockchain;
}


int main() {
    auto startTime = std::chrono::steady_clock::now();
    std::vector<User> users = generateUsers();
    std::vector<Transaction> transactions = generateTransactions(users);
    long long initialBalance = 0;

    for (const User& user : users) {
        initialBalance += user.balance;
    }

    size_t initialTransactionCount = transactions.size();
    std::vector<Block> blockchain;
    blockchain.push_back(genesisBlock());
    runLoop(users, transactions, blockchain);
    long long finalBalance = 0;

    for (const User& user : users) {
        finalBalance += user.balance;
    }

    auto endTime = std::chrono::steady_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();

    std::cout << "\n--- Galutine suvestine ---\n";
    std::cout << "Vartotoju skaicius: " << users.size() << '\n';
    std::cout << "Pradinis transakciju skaicius: " << initialTransactionCount << '\n';
    std::cout << "Priimtu bloku skaicius: " << blockchain.size() - 1 << '\n';
    std::cout << "Itrauktu transakciju skaicius: " << initialTransactionCount - transactions.size() << '\n';
    std::cout << "Likusių transakciju skaicius: " << transactions.size() << '\n';
    std::cout << "Pradine lesu suma: " << initialBalance << '\n';
    std::cout << "Galutine lesu suma: " << finalBalance << '\n';
    std::cout << "Bendras programos laikas: " << totalTime << " s\n";

    return 0;
}
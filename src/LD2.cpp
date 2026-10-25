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

void runLoop() {
    auto startAll = std::chrono::steady_clock::now();

    std::vector<User> users = generateUsers();
    std::vector<Transaction> pool = generateTransactions(users);

    std::map<std::string, long long> balances;
    for (auto& u : users) balances[u.public_key] = u.balance;

    auto totalSum = [](const std::map<std::string, long long>& bal) {
        long long sum = 0;
        for (auto& p : bal) sum += p.second;
        return sum;
    };

    auto blockHash = [](const Block& b) {
        std::string s = b.prev_hash + "|" + std::to_string(b.timestamp) + "|" + std::to_string(b.version) + "|" + b.tx_root + "|" + std::to_string(b.nonce) + "|" + std::to_string(b.difficulty);
        return hash(convertTextTB(s));
    };

    auto calcTxRoot = [](const std::vector<Transaction>& txs) {
        std::string allIDs = "";
        for (size_t i = 0; i < txs.size(); i++) {
            if (i > 0) allIDs += "|";
            allIDs += txs[i].transaction_id;
        }
        return hash(convertTextTB(allIDs));
    };

    auto goodPow = [](const std::string& h, int difficulty) {
        if ((int)h.size() < difficulty) return false;
        for (int i = 0; i < difficulty; i++) {
            if (h[i] != '0') return false;
        }
        return true;
    };

    auto structureError = [](const Transaction& t, const std::map<std::string, long long>& bal) -> std::string {
        if (bal.find(t.sender) == bal.end()) return "nezinomas siuntejas";
        if (bal.find(t.receiver) == bal.end()) return "nezinomas gavejas";
        if (t.sender == t.receiver) return "siuntejas ir gavejas tas pats";
        if (t.amount <= 0) return "neigiama arba nuline suma";
        std::string s = t.sender + "|" + t.receiver + "|" + std::to_string(t.amount) + "|" + std::to_string(t.sequence);
        if (hash(convertTextTB(s)) != t.transaction_id) return "neteisingas transakcijos ID";
        return "";
    };

    long long startSum = totalSum(balances);
    std::vector<Block> chain;
    Block genesis = genesisBlock();
    while (!goodPow(blockHash(genesis), genesis.difficulty)) genesis.nonce++;
    chain.push_back(genesis);

    std::vector<std::pair<Transaction, std::string>> rejected;
    int included = 0;
    std::mt19937 generator(222);

    while (!pool.empty()) {
        std::vector<int> order;
        while (order.size() < 100 && order.size() < pool.size()) {
            int idx = generator() % pool.size();

            if (std::find(order.begin(), order.end(), idx) == order.end()) {
                order.push_back(idx);
            }
        }

        std::map<std::string, long long> temp = balances;
        std::vector<Transaction> chosen;
        std::vector<std::string> removeIds;

        for (int idx : order) {
            if (chosen.size() >= 100) break;
            Transaction& t = pool[idx];

            std::string err = structureError(t, temp);
            if (err != "") {
                rejected.push_back({t, err});
                removeIds.push_back(t.transaction_id);
                continue;
            }
            if (temp[t.sender] < t.amount) {
                continue;
            }
            temp[t.sender] -= t.amount;
            temp[t.receiver] += t.amount;
            chosen.push_back(t);
        }

        for (auto& id : removeIds) {
            for (size_t i = 0; i < pool.size(); i++) {
                if (pool[i].transaction_id == id) { pool.erase(pool.begin() + i); break; }
            }
        }

        if (chosen.empty()) {
            if (!removeIds.empty()) continue;
            break;
        }

        Block block;
        block.prev_hash = blockHash(chain.back());
        block.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        block.version = 1;
        block.transactions = chosen;
        block.tx_root = calcTxRoot(chosen);
        block.difficulty = 3;
        block.nonce = 0;

        long long attempts = 1;
        auto startMine = std::chrono::steady_clock::now();
        while (!goodPow(blockHash(block), block.difficulty)) {
            block.nonce++;
            attempts++;
        }
        auto endMine = std::chrono::steady_clock::now();
        double mineTime = std::chrono::duration<double>(endMine - startMine).count();

        bool ok = true;
        if (block.prev_hash != blockHash(chain.back())) ok = false;
        if (calcTxRoot(block.transactions) != block.tx_root) ok = false;
        if (!goodPow(blockHash(block), block.difficulty)) ok = false;
        std::map<std::string, long long> check = balances;
        for (auto& t : block.transactions) {
            if (structureError(t, check) != "" || check[t.sender] < t.amount) { ok = false; break; }
            check[t.sender] -= t.amount;
            check[t.receiver] += t.amount;
        }
        if (!ok) {
            std::cout << "Blokas nepraejo patikros\n";
            break;
        }

        chain.push_back(block);
        balances = check;
        for (auto& t : chosen) {
            for (size_t i = 0; i < pool.size(); i++) {
                if (pool[i].transaction_id == t.transaction_id) { pool.erase(pool.begin() + i); break; }
            }
        }
        included += chosen.size();

        std::cout << "Bloko aukstis" << chain.size() - 1 << "\n";
        std::cout << "transakciju skaicius: " << chosen.size() << "\n";
        std::cout << "prev_hash: " << block.prev_hash << "\n";
        std::cout << "tx_root: " << block.tx_root << "\n";
        std::cout << "hash: " << blockHash(block) << "\n";
        std::cout << "difficulty: " << block.difficulty << "\n";
        std::cout << "nonce: " << block.nonce << "\n";
        std::cout << "bandymu: " << attempts << "\n";
        std::cout << "kasimo laikas: " << mineTime << " s\n";
    }

    auto endAll = std::chrono::steady_clock::now();
    double allTime = std::chrono::duration<double>(endAll - startAll).count();

    std::cout << "\n-------------------------------\n";
    std::cout << "Vartotoju skaicius: " << users.size() << "\n";
    std::cout << "Transakciju skaicius: " << pool.size() << "\n";
    std::cout << "Priimta bloku (be genesis): " << chain.size() - 1 << "\n";
    std::cout << "Itraukta: " << included << "\n";
    std::cout << "Atmesta: " << rejected.size() << "\n";
    std::cout << "Liko fonde: " << pool.size() << "\n";
    std::cout << "Pradine lesu suma: " << startSum << "\n";
    std::cout << "Galutine lesu suma: " << totalSum(balances) << "\n";
    std::cout << "Bendras laikas: " << allTime << " s\n";

    std::cout << "\nAtmestos:\n";
    for (auto& r : rejected)
        std::cout << "  seq " << r.first.sequence << " suma " << r.first.amount << " - " << r.second << "\n";

    std::cout << "\nLikusios:\n";
    for (auto& t : pool)
        std::cout << "  seq " << t.sequence << " - siuntejas turi " << balances[t.sender] << ", reikia " << t.amount << "\n";
}

int main() {
    runLoop();
    return 0;
}

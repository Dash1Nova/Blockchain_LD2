#include <iostream>
#include <random>
#include <chrono>
#include <map>
#include <set>

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

const int requiredDifficulty = 3;
const int supportedVersion = 1;
const size_t maxTransactions = 100;

Block genesisBlock() {
    Block genesis;
    genesis.prev_hash = "0000000000000000000000000000000000000000000000000000000000000000";
    genesis.timestamp = 0;
    genesis.version = 1;
    genesis.tx_root = hash(convertTextTB(""));
    genesis.nonce = 0;        
    genesis.difficulty = 3;   
    genesis.transactions = std::vector<Transaction>();
    return genesis;
}

std::string calculateMerkleRoot(const std::vector<Transaction>& txs);

long long totalSum(const std::map<std::string, long long>& bal) {
    long long sum = 0;
    for (auto& p : bal) sum += p.second;
    return sum;
}

std::string blockHash(const Block& b) {
    std::string s = b.prev_hash + "|" + std::to_string(b.timestamp) + "|" + std::to_string(b.version) + "|" + b.tx_root + "|" + std::to_string(b.nonce) + "|" + std::to_string(b.difficulty);
    return hash(convertTextTB(s));
}

bool goodPow(const std::string& h, int difficulty) {
    if (difficulty <= 0) return false;
    if (h.size() < (size_t)difficulty) return false;
    for (int i = 0; i < difficulty; i++) if (h[i] != '0') return false;
    return true;
}

std::string structureError(const Transaction& t, const std::map<std::string, long long>& balances) {
    if (balances.find(t.sender) == balances.end()) return std::string("nezinomas siuntejas");
    if (balances.find(t.receiver) == balances.end()) return std::string("nezinomas gavejas");
    if (t.sender == t.receiver) return std::string("siuntejas ir gavejas tas pats");
    if (t.amount <= 0) return std::string("suma turi buti teigiama");
    std::string s = t.sender + "|" + t.receiver + "|" + std::to_string(t.amount) + "|" + std::to_string(t.sequence);
    if (hash(convertTextTB(s)) != t.transaction_id) return std::string("neteisingas transakcijos ID");
    return std::string("");
}

bool checkBlock(const Block& b, const Block& prev, const std::map<std::string, long long>& balances, const std::set<std::string>& includedIds, std::map<std::string, long long>& check) {
    if (b.prev_hash != blockHash(prev)) return false;
    if (b.version != supportedVersion) return false;
    if (b.difficulty != requiredDifficulty) return false;
    if (b.transactions.empty()) return false;
    if (b.transactions.size() > maxTransactions) return false;

    check = balances;
    std::set<std::string> blockIds;

    for (auto& t : b.transactions) {
        if (structureError(t, check) != "") return false;
        if (includedIds.count(t.transaction_id) != 0) return false;
        if (blockIds.count(t.transaction_id) != 0) return false;
        if (check.at(t.sender) < t.amount) return false;
        if (check.at(t.receiver) > std::numeric_limits<long long>::max() - t.amount) return false;
        check.at(t.sender) -= t.amount;
        check.at(t.receiver) += t.amount;
        blockIds.insert(t.transaction_id);
    }

    if (calculateMerkleRoot(b.transactions) != b.tx_root) return false;
    if (!goodPow(blockHash(b), requiredDifficulty)) return false;
    return true;
}

void runLoop() {
    auto startAll = std::chrono::steady_clock::now();
    std::vector<User> users = generateUsers();
    std::vector<Transaction> allTransactions = generateTransactions(users);
    std::vector<Transaction> pool;
    size_t transactionCount = allTransactions.size();
    std::map<std::string, long long> balances;
    for (auto& u : users) balances[u.public_key] = u.balance;
    std::set<std::string> poolIds;
    std::vector<std::pair<Transaction, std::string>> rejected;

    for (auto& t : allTransactions) {
        std::string s = t.sender + "|" + t.receiver + "|" + std::to_string(t.amount) + "|" + std::to_string(t.sequence);
        std::string error = "";

        if (balances.find(t.sender) == balances.end()) error = "nezinomas siuntejas";
        else if (balances.find(t.receiver) == balances.end()) error = "nezinomas gavejas";
        else if (t.sender == t.receiver) error = "siuntejas ir gavejas tas pats";
        else if (t.amount <= 0) error = "suma turi buti teigiama";
        else if (hash(convertTextTB(s)) != t.transaction_id) error = "neteisingas transakcijos ID";
        else if (poolIds.count(t.transaction_id) != 0) error = "pasikartojantis ID fonde";

        if (error != "") rejected.push_back({t, error});
        else {
            pool.push_back(t);
            poolIds.insert(t.transaction_id);
        }
    }


    std::set<std::string> includedIds;
    long long startSum = totalSum(balances);
    std::vector<Block> chain;
    Block genesis = genesisBlock();
    chain.push_back(genesis);
    const int requiredDifficulty = genesis.difficulty;
    const int supportedVersion = 1;
    const size_t maxTransactions = 100;

    std::mt19937 generator(222);
    std::string stopReason;

    while (!pool.empty()) {
        for (size_t i = 0; i < pool.size();) {
            std::string err = structureError(pool[i], balances);
            if (err == "" && includedIds.count(pool[i].transaction_id) != 0) err = "ID jau itrauktas i grandine";
            if (err != "") {
                rejected.push_back({pool[i], err});
                pool.erase(pool.begin() + i);
            } else {
                i++;
            }
        }

        if (pool.empty()) break;
        std::vector<Block> candidates;
        std::string prevHash = blockHash(chain.back());
        long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        for (int k = 0; k < 5; k++) {
            std::vector<size_t> order;
            for (size_t i = 0; i < pool.size(); i++) order.push_back(i);
            std::map<std::string, long long> temp = balances;
            std::set<std::string> candidateIds;

            Block b;
            b.prev_hash = prevHash;
            b.timestamp = timestamp;
            b.version = supportedVersion;
            b.difficulty = requiredDifficulty;
            b.nonce = 0;

            for (size_t idx : order) {
                if (b.transactions.size() >= maxTransactions) break;
                const Transaction& t = pool[idx];
                if (candidateIds.count(t.transaction_id) != 0) continue;
                if (temp[t.sender] < t.amount) continue;
                if (temp[t.receiver] > std::numeric_limits<long long>::max() - t.amount) continue;
                temp[t.sender] -= t.amount;
                temp[t.receiver] += t.amount;
                candidateIds.insert(t.transaction_id);
                b.transactions.push_back(t);
            }

            b.tx_root = calculateMerkleRoot(b.transactions);
            candidates.push_back(b);
        }

        bool hasTransactions = false;
        for (auto& b : candidates) if (!b.transactions.empty()) hasTransactions = true;
        if (!hasTransactions) {
            stopReason = "nepavyko sudaryti kandidato su tinkamu mokejimu";
            break;
        }

        std::cout << "\nAukstis: " << chain.size() << "\n";
        std::cout << "Kandidatu tvarka: 1, 2, 3, 4, 5\n";
        std::cout << "Laimi pirmas sia tvarka rastas tinkamas blokas\n";
        std::cout << "Biudzetai: 10000, 20000, 40000, 80000 bandymu\n";
        long long budget = 10000;
        std::vector<long long> attempts(5, 0);
        std::vector<double> mineTime(5, 0.0);
        int winner = -1;

        for (int round = 0; round < 4; round++) {
            std::cout << "Ratas " << round + 1 << ", papildomas biudzetas: " << budget << "\n";
            for (int k = 0; k < 5; k++) {
                Block& b = candidates[k];
                if (b.transactions.empty()) continue;
                auto startMine = std::chrono::steady_clock::now();
                for (long long n = 0; n < budget; n++) {
                    attempts[k]++;
                    if (goodPow(blockHash(b), requiredDifficulty)) break;
                    b.nonce++;
                }

                auto endMine = std::chrono::steady_clock::now();
                mineTime[k] += std::chrono::duration<double>(endMine - startMine).count();
                std::cout << "Kandidatas " << k + 1 << ": bandymu " << attempts[k] << ", kasimo laikas " << mineTime[k] << " s\n";
            }

            for (int k = 0; k < 5; k++) {
                Block& b = candidates[k];
                if (b.transactions.empty()) continue;
                if (goodPow(blockHash(b), requiredDifficulty)) {
                    std::map<std::string, long long> check;
                    if (checkBlock(b, chain.back(), balances, includedIds, check)) {
                        winner = k;
                        break;
                    }
                }
            }
            if (winner != -1) break;
            budget *= 2;
        }

        if (winner == -1) {
            stopReason = "pasiekta kasimo bandymu riba";
            break;
        }

        Block& block = candidates[winner];
        std::map<std::string, long long> check;
        if (!checkBlock(block, chain.back(), balances, includedIds, check)) {
            stopReason = "laimetojas nepraejo pakartotines patikros";
            break;
        }
        chain.push_back(block);
        balances = check;

        for (auto& t : block.transactions) {
            includedIds.insert(t.transaction_id);
            for (size_t i = 0; i < pool.size(); i++) {
                if (pool[i].transaction_id == t.transaction_id) {
                    pool.erase(pool.begin() + i);
                    break;
                }
            }
        }

        std::cout << "Laimetojas: " << winner + 1 << "\n";
        std::cout << "Bloko aukstis: " << chain.size() - 1 << "\n";
        std::cout << "Transakciju skaicius: " << block.transactions.size() << "\n";
        std::cout << "prev_hash: " << block.prev_hash << "\n";
        std::cout << "tx_root: " << block.tx_root << "\n";
        std::cout << "hash: " << blockHash(block) << "\n";
        std::cout << "difficulty: " << block.difficulty << "\n";
        std::cout << "nonce: " << block.nonce << "\n";
        std::cout << "Bandymu: " << attempts[winner] << "\n";
        std::cout << "Kasimo laikas: " << mineTime[winner] << " s\n";
    }

    auto endAll = std::chrono::steady_clock::now();
    double allTime = std::chrono::duration<double>(endAll - startAll).count();

    std::cout << "\n-------------------------------\n";
    std::cout << "Vartotoju skaicius: " << users.size() << "\n";
    std::cout << "Pradinis transakciju skaicius: " << transactionCount << "\n";
    std::cout << "Priimta bloku (be genesis): " << chain.size() - 1 << "\n";
    std::cout << "Itraukta: " << includedIds.size() << "\n";
    std::cout << "Atmesta: " << rejected.size() << "\n";
    std::cout << "Liko fonde: " << pool.size() << "\n";
    std::cout << "Pradine lesu suma: " << startSum << "\n";
    std::cout << "Galutine lesu suma: " << totalSum(balances) << "\n";
    std::cout << "Bendras laikas: " << allTime << " s\n";

    if (stopReason != "") std::cout << "Sustabdymo priezastis: " << stopReason << "\n";

    std::cout << "\nAtmestos:\n";
    for (auto& r : rejected) std::cout << "seq " << r.first.sequence << " - " << r.second << "\n";

    std::cout << "\nLikusios:\n";
    for (auto& t : pool) {
        std::string reason = structureError(t, balances);
        if (reason == "" && includedIds.count(t.transaction_id) != 0) reason = "ID jau itrauktas i grandine";
        if (reason == "" && balances[t.sender] < t.amount) reason = "siuntejui truksta lesu dabartineje busenoje";
        if (reason == "" && balances[t.receiver] > std::numeric_limits<long long>::max() - t.amount) reason = "gavejo balansas perpildytu long long";
        if (reason == "") reason = "neitraukta, nes darbas sustabdytas: " + stopReason;
        std::cout << "seq " << t.sequence << " - " << reason << "\n";
    }
}

int hexToNumber(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

std::string twoHashJoin(const std::string& left, const std::string& right) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < left.length(); i += 2) {
        int first = hexToNumber(left[i]);
        int second = hexToNumber(left[i + 1]);
        bytes.push_back(first * 16 + second);
    }
    for (size_t i = 0; i < right.length(); i += 2) {
        int first = hexToNumber(right[i]);
        int second = hexToNumber(right[i + 1]);
        bytes.push_back(first * 16 + second);
    }
    return hash(bytes);
}

std::string calculateMerkleRoot(const std::vector<Transaction>& txs) {
    if (txs.empty()) return hash(convertTextTB(""));
    std::vector<std::string> hashes;
    for (const auto& tx : txs) hashes.push_back(tx.transaction_id);

    while (hashes.size() > 1) {
        std::vector<std::string> new_level;
        for (size_t i = 0; i < hashes.size(); i += 2) {
            if (i + 1 < hashes.size()) new_level.push_back(twoHashJoin(hashes[i], hashes[i + 1]));
            else new_level.push_back(twoHashJoin(hashes[i], hashes[i]));
        }
        hashes = new_level;
    }
    return hashes[0];
}

void testMerkleTree() {
    std::vector<Transaction> testTransactions;
    for (int i = 1; i <= 5; i++) {
        Transaction t;
        t.sender = "user" + std::to_string(i);
        t.receiver = "user" + std::to_string(i + 1);
        t.amount = i * 10;
        t.sequence = i;
        std::string data = t.sender + "|" + t.receiver + "|" + std::to_string(t.amount) + "|" + std::to_string(t.sequence);
        t.transaction_id = hash(convertTextTB(data));
        testTransactions.push_back(t);
    }

    std::cout << "1 transakcija: " << calculateMerkleRoot({testTransactions[0]}) << "\n";
    std::cout << "2 transakcijos: " << calculateMerkleRoot({ testTransactions[0], testTransactions[1]}) << "\n";
    std::cout << "3 transakcijos: " << calculateMerkleRoot({testTransactions[0], testTransactions[1], testTransactions[2]}) << "\n";
    std::cout << "5 transakcijos: " << calculateMerkleRoot(testTransactions) << "\n";
}

#ifndef UNIT_TESTS
int main() {
    testMerkleTree();
    std::cout << "\n--------------------------------------\n";
    runLoop();
    return 0;
}
#endif
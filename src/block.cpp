#include "block.h"
#include "hash.h"
#include "merkle.h"
#include "transaction.h"


Block genesisBlock() {
    Block genesis;
    genesis.prev_hash = "0000000000000000000000000000000000000000000000000000000000000000";
    genesis.timestamp = 0;
    genesis.version = supportedVersion;
    genesis.tx_root = hash(convertTextTB(""));
    genesis.nonce = 0;
    genesis.difficulty = requiredDifficulty;  
    genesis.transactions = std::vector<Transaction>();
    return genesis;
}

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

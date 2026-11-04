#include "merkle.h"
#include "hash.h"

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
#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

#include "transaction.h"

inline constexpr int requiredDifficulty = 3;
inline constexpr int supportedVersion = 1;
inline constexpr size_t maxTransactions = 100;

struct Block {
    std::string prev_hash;
    long long timestamp;
    int version;
    std::string tx_root;
    long long nonce;
    int difficulty;
    std::vector<Transaction> transactions;
};

Block genesisBlock();

long long totalSum(const std::map<std::string, long long>& bal);

std::string blockHash(const Block& b);

bool goodPow(const std::string& h, int difficulty);

bool checkBlock(
    const Block& b,
    const Block& prev,
    const std::map<std::string, long long>& balances,
    const std::set<std::string>& includedIds,
    std::map<std::string, long long>& check
);
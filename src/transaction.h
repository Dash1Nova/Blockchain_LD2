#pragma once
#include <string>
#include <map>

struct Transaction {
    std::string sender;
    std::string receiver;
    long long amount;
    int sequence;
    std::string transaction_id;
};

std::string structureError(
    const Transaction& t,
    const std::map<std::string, long long>& balances
);
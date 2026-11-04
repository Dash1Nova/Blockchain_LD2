#ifndef UTXO_H
#define UTXO_H

#include <string>
#include <vector>
#include <map>
#include <utility>

struct User;

struct TxOutput {
    std::string owner;
    long long amount;
};

struct TxInput {
    std::string previousTxId;
    int outputIndex;
};

struct UTXO {
    std::string txId;
    int outputIndex;
    std::string owner;
    long long amount;
};

struct UTXOTransaction {
    std::string txId;
    std::string sender;
    std::vector<TxInput> inputs;
    std::vector<TxOutput> outputs;
};

using UTXOSet = std::map<std::pair<std::string, int>, UTXO>;

UTXOSet createInitialUTXOs(const std::vector<std::pair<std::string, long long>>& initialBalances);

std::string calculateUTXOTxId(const UTXOTransaction& tx);

bool createPayment(
    const std::string& sender,
    const std::string& receiver,
    long long amount,
    const UTXOSet& utxos,
    UTXOTransaction& tx,
    std::string& error
);

bool applyUTXOTransaction(
    const UTXOTransaction& tx,
    UTXOSet& utxos,
    std::string& error
);

long long totalUTXOSum(const UTXOSet& utxos);

#endif
#include "utxo.h"
#include "hash.h"

#include <set>
#include <stdexcept>

UTXOSet createInitialUTXOs(
    const std::vector<std::pair<std::string, long long>>& initialBalances
) {
    UTXOSet utxos;

    for (const auto& user : initialBalances) {
        UTXO u;
        u.txId = hash(convertTextTB("GENESIS|" + user.first));
        u.outputIndex = 0;
        u.owner = user.first;
        u.amount = user.second;

        utxos[{u.txId, u.outputIndex}] = u;
    }

    return utxos;
}

std::string calculateUTXOTxId(const UTXOTransaction& tx) {
    std::string data = tx.sender;

    for (const auto& input : tx.inputs) {
        data += "|" + input.previousTxId;
        data += "|" + std::to_string(input.outputIndex);
    }

    for (const auto& output : tx.outputs) {
        data += "|" + output.owner;
        data += "|" + std::to_string(output.amount);
    }

    return hash(convertTextTB(data));
}

bool createPayment(
    const std::string& sender,
    const std::string& receiver,
    long long amount,
    const UTXOSet& utxos,
    UTXOTransaction& tx,
    std::string& error
) {
    error.clear();

    if (sender == receiver) {
        error = "siuntejas ir gavejas tas pats";
        return false;
    }

    if (amount <= 0) {
        error = "suma turi buti teigiama";
        return false;
    }

    tx = UTXOTransaction{};
    tx.sender = sender;

    long long selectedAmount = 0;

    for (const auto& entry : utxos) {
        const UTXO& u = entry.second;

        if (u.owner != sender) continue;

        if (selectedAmount > std::numeric_limits<long long>::max() - u.amount) {
            error = "UTXO sumos perpildymas";
            return false;
        }

        tx.inputs.push_back({u.txId, u.outputIndex});
        selectedAmount += u.amount;

        if (selectedAmount >= amount) break;
    }

    if (selectedAmount < amount) {
        error = "siuntejui truksta lesu";
        return false;
    }

    tx.outputs.push_back({receiver, amount});

    long long change = selectedAmount - amount;

    if (change > 0) {
        tx.outputs.push_back({sender, change});
    }

    tx.txId = calculateUTXOTxId(tx);

    return true;
}

bool applyUTXOTransaction(
    const UTXOTransaction& tx,
    UTXOSet& utxos,
    std::string& error
) {
    error.clear();

    if (tx.inputs.empty()) {
        error = "transakcija neturi ivesciu";
        return false;
    }

    if (tx.outputs.empty()) {
        error = "transakcija neturi isvesciu";
        return false;
    }

    if (tx.txId != calculateUTXOTxId(tx)) {
        error = "neteisingas transakcijos ID";
        return false;
    }

    std::set<std::pair<std::string, int>> usedInputs;
    long long inputSum = 0;
    long long outputSum = 0;

    for (const auto& input : tx.inputs) {
        auto key = std::make_pair(input.previousTxId, input.outputIndex);

        if (!usedInputs.insert(key).second) {
            error = "pasikartojanti ives tis";
            return false;
        }

        auto it = utxos.find(key);

        if (it == utxos.end()) {
            error = "UTXO neegzistuoja arba jau panaudotas";
            return false;
        }

        if (it->second.owner != tx.sender) {
            error = "siuntejas nera UTXO savininkas";
            return false;
        }

        if (it->second.amount <= 0 ||
            inputSum > std::numeric_limits<long long>::max() - it->second.amount) {
            error = "neteisinga ivesciu suma";
            return false;
        }

        inputSum += it->second.amount;
    }

    for (const auto& output : tx.outputs) {
        if (output.owner.empty() || output.amount <= 0) {
            error = "neteisinga isvestis";
            return false;
        }

        if (outputSum > std::numeric_limits<long long>::max() - output.amount) {
            error = "isvesciu suma perpildyta";
            return false;
        }

        outputSum += output.amount;
    }

    if (inputSum != outputSum) {
        error = "ivesciu ir isvesciu sumos nesutampa";
        return false;
    }

    // Pirmiausia patikrinome visas salygas.
    // Tik dabar sunaikinamos senos isvestys.
    for (const auto& input : tx.inputs) {
        utxos.erase({input.previousTxId, input.outputIndex});
    }

    for (size_t i = 0; i < tx.outputs.size(); i++) {
        UTXO u;
        u.txId = tx.txId;
        u.outputIndex = static_cast<int>(i);
        u.owner = tx.outputs[i].owner;
        u.amount = tx.outputs[i].amount;

        utxos[{u.txId, u.outputIndex}] = u;
    }

    return true;
}

long long totalUTXOSum(const UTXOSet& utxos) {
    long long sum = 0;

    for (const auto& entry : utxos) {
        if (sum > std::numeric_limits<long long>::max() - entry.second.amount) {
            throw std::overflow_error("UTXO bendra suma perpildyta");
        }

        sum += entry.second.amount;
    }

    return sum;
}
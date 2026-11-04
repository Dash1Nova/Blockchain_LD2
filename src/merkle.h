#pragma once
#include <string>
#include <vector>
#include "transaction.h"

std::string calculateMerkleRoot(const std::vector<Transaction>& txs);
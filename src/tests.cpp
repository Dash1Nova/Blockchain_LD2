#include <gtest/gtest.h>
#define UNIT_TESTS
#include "LD2.cpp"

// ---------- pagalbines funkcijos ----------

static std::string keyOf(int i) { return hash(convertTextTB("user" + std::to_string(i))); }

static Transaction makeTx(const std::string& s, const std::string& r, long long amount, int seq) {
    Transaction t;
    t.sender = s;
    t.receiver = r;
    t.amount = amount;
    t.sequence = seq;
    t.transaction_id = hash(convertTextTB(s + "|" + r + "|" + std::to_string(amount) + "|" + std::to_string(seq)));
    return t;
}

static Block makeBlock(const Block& prev, const std::vector<Transaction>& txs) {
    Block b;
    b.prev_hash = blockHash(prev);
    b.timestamp = 1;
    b.version = supportedVersion;
    b.difficulty = requiredDifficulty;
    b.nonce = 0;
    b.transactions = txs;
    b.tx_root = calculateMerkleRoot(txs);
    while (!goodPow(blockHash(b), b.difficulty)) b.nonce++;
    return b;
}

class ChainTest : public ::testing::Test {
protected:
    Block genesis = genesisBlock();
    std::map<std::string, long long> bal = {{keyOf(1), 1000}, {keyOf(2), 1000}, {keyOf(3), 1000}};
    std::set<std::string> included;
    std::map<std::string, long long> out;
};

// ---------- 1. Galiojantis mokejimas ----------

TEST_F(ChainTest, ValidPaymentAccepted) {
    Transaction t = makeTx(keyOf(1), keyOf(2), 300, 1);
    Block b = makeBlock(genesis, {t});
    ASSERT_TRUE(checkBlock(b, genesis, bal, included, out));
    EXPECT_EQ(out[keyOf(1)], 700);
    EXPECT_EQ(out[keyOf(2)], 1300);
    EXPECT_EQ(out[keyOf(3)], 1000);
    EXPECT_EQ(totalSum(out), totalSum(bal));
}

// ---------- 2. Blogos transakcijos (priezastis matoma) ----------

TEST_F(ChainTest, UnknownReceiver) {
    EXPECT_EQ(structureError(makeTx(keyOf(1), keyOf(99), 10, 1), bal), "nezinomas gavejas");
}

TEST_F(ChainTest, UnknownSender) {
    EXPECT_EQ(structureError(makeTx(keyOf(99), keyOf(1), 10, 1), bal), "nezinomas siuntejas");
}

TEST_F(ChainTest, ZeroAmount) {
    EXPECT_EQ(structureError(makeTx(keyOf(1), keyOf(2), 0, 1), bal), "suma turi buti teigiama");
}

TEST_F(ChainTest, NegativeAmount) {
    EXPECT_EQ(structureError(makeTx(keyOf(1), keyOf(2), -5, 1), bal), "suma turi buti teigiama");
}

TEST_F(ChainTest, SameSenderAndReceiver) {
    EXPECT_EQ(structureError(makeTx(keyOf(1), keyOf(1), 5, 1), bal), "siuntejas ir gavejas tas pats");
}

TEST_F(ChainTest, TamperedTransactionId) {
    Transaction t = makeTx(keyOf(1), keyOf(2), 10, 1);
    t.amount = 999;
    EXPECT_EQ(structureError(t, bal), "neteisingas transakcijos ID");
}

TEST_F(ChainTest, ValidTransactionHasNoError) {
    EXPECT_EQ(structureError(makeTx(keyOf(1), keyOf(2), 10, 1), bal), "");
}

// ---------- 3. Balanso virsijimas ----------

TEST_F(ChainTest, TwoPaymentsExceedingBalanceRejected) {
    Transaction a = makeTx(keyOf(1), keyOf(2), 600, 1);
    Transaction b = makeTx(keyOf(1), keyOf(3), 600, 2);
    EXPECT_FALSE(checkBlock(makeBlock(genesis, {a, b}), genesis, bal, included, out));
}

TEST_F(ChainTest, FirstOfTwoPaymentsAloneIsValid) {
    Transaction a = makeTx(keyOf(1), keyOf(2), 600, 1);
    EXPECT_TRUE(checkBlock(makeBlock(genesis, {a}), genesis, bal, included, out));
    EXPECT_EQ(out[keyOf(1)], 400);
}

// ---------- 4. Dublikatai ----------

TEST_F(ChainTest, DuplicateIdInsideBlockRejected) {
    Transaction t = makeTx(keyOf(1), keyOf(2), 100, 1);
    EXPECT_FALSE(checkBlock(makeBlock(genesis, {t, t}), genesis, bal, included, out));
}

TEST_F(ChainTest, ReplayOfIncludedTransactionRejected) {
    Transaction t = makeTx(keyOf(1), keyOf(2), 100, 1);
    Block b = makeBlock(genesis, {t});
    std::set<std::string> already = {t.transaction_id};
    EXPECT_FALSE(checkBlock(b, genesis, bal, already, out));
}

// ---------- 5. Sugadinti blokai ----------

TEST_F(ChainTest, ChangedTransactionContentRejected) {
    Block b = makeBlock(genesis, {makeTx(keyOf(1), keyOf(2), 100, 1)});
    b.transactions[0].amount = 500;
    EXPECT_FALSE(checkBlock(b, genesis, bal, included, out));
}

TEST_F(ChainTest, ChangedTxRootRejected) {
    Block b = makeBlock(genesis, {makeTx(keyOf(1), keyOf(2), 100, 1)});
    b.tx_root = hash(convertTextTB("kita"));
    EXPECT_FALSE(checkBlock(b, genesis, bal, included, out));
}

TEST_F(ChainTest, ChangedPrevHashRejected) {
    Block b = makeBlock(genesis, {makeTx(keyOf(1), keyOf(2), 100, 1)});
    b.prev_hash = hash(convertTextTB("kita"));
    EXPECT_FALSE(checkBlock(b, genesis, bal, included, out));
}

TEST_F(ChainTest, InvalidProofOfWorkRejected) {
    Block b = makeBlock(genesis, {makeTx(keyOf(1), keyOf(2), 100, 1)});
    while (goodPow(blockHash(b), b.difficulty)) b.nonce++;
    EXPECT_FALSE(checkBlock(b, genesis, bal, included, out));
}

TEST_F(ChainTest, WrongDifficultyRejected) {
    Block b = makeBlock(genesis, {makeTx(keyOf(1), keyOf(2), 100, 1)});
    b.difficulty = 1;
    EXPECT_FALSE(checkBlock(b, genesis, bal, included, out));
}

TEST_F(ChainTest, EmptyBlockRejected) {
    EXPECT_FALSE(checkBlock(makeBlock(genesis, {}), genesis, bal, included, out));
}

TEST_F(ChainTest, RejectedBlockDoesNotChangeConfirmedState) {
    auto balBefore = bal;
    auto includedBefore = included;
    Block b = makeBlock(genesis, {makeTx(keyOf(1), keyOf(2), 100, 1)});
    b.transactions[0].amount = 500;
    EXPECT_FALSE(checkBlock(b, genesis, bal, included, out));
    EXPECT_EQ(bal, balBefore);
    EXPECT_EQ(included, includedBefore);
}

// ---------- 6. Merkle ----------

class MerkleTest : public ::testing::Test {
protected:
    std::vector<Transaction> tx;
    void SetUp() override {
        for (int i = 1; i <= 5; i++) tx.push_back(makeTx(keyOf(1), keyOf(2), i * 10, i));
    }
    std::string id(int i) const { return tx[i].transaction_id; }
};

TEST_F(MerkleTest, Empty) {
    EXPECT_EQ(calculateMerkleRoot({}), hash(convertTextTB("")));
}

TEST_F(MerkleTest, OneTransactionRootIsItsId) {
    EXPECT_EQ(calculateMerkleRoot({tx[0]}), id(0));
}

TEST_F(MerkleTest, TwoTransactions) {
    EXPECT_EQ(calculateMerkleRoot({tx[0], tx[1]}), twoHashJoin(id(0), id(1)));
}

TEST_F(MerkleTest, ThreeTransactionsDuplicatesLast) {
    std::string expected = twoHashJoin(twoHashJoin(id(0), id(1)), twoHashJoin(id(2), id(2)));
    EXPECT_EQ(calculateMerkleRoot({tx[0], tx[1], tx[2]}), expected);
}

TEST_F(MerkleTest, FiveTransactions) {
    std::string ee = twoHashJoin(id(4), id(4));
    std::string left = twoHashJoin(twoHashJoin(id(0), id(1)), twoHashJoin(id(2), id(3)));
    EXPECT_EQ(calculateMerkleRoot(tx), twoHashJoin(left, twoHashJoin(ee, ee)));
}

TEST_F(MerkleTest, OrderMatters) {
    EXPECT_NE(calculateMerkleRoot({tx[0], tx[1]}), calculateMerkleRoot({tx[1], tx[0]}));
}

// ---------- 7. Nelaimejes kandidatas ----------

TEST_F(ChainTest, LosingCandidateRejectedAfterWinnerAccepted) {
    Transaction a = makeTx(keyOf(1), keyOf(2), 600, 1);
    Transaction b = makeTx(keyOf(1), keyOf(3), 600, 2);
    Block candA = makeBlock(genesis, {a});
    Block candB = makeBlock(genesis, {b});

    std::map<std::string, long long> afterA;
    ASSERT_TRUE(checkBlock(candA, genesis, bal, included, afterA));
    std::set<std::string> includedA = {a.transaction_id};

    // B dabar remiasi sena virsune, o balansai jau pasikeite
    std::map<std::string, long long> tmp;
    EXPECT_FALSE(checkBlock(candB, candA, afterA, includedA, tmp));
    EXPECT_EQ(afterA[keyOf(3)], 1000);
}
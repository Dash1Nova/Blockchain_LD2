def test_payment_is_confirmed(rpc, payment_txid, block_hash):
    tx = rpc.getrawtransaction(
        payment_txid,
        True,
        block_hash
    )

    assert tx["txid"] == payment_txid
    assert tx["confirmations"] >= 1
    assert tx["blockhash"] == block_hash


def test_payment_has_block_height(rpc, payment_txid, block_hash):
    tx = rpc.getrawtransaction(
        payment_txid,
        True,
        block_hash
    )

    assert tx["confirmations"] >= 1
    assert tx["blocktime"] > 0
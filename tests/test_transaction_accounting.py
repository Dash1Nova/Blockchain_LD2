from decimal import Decimal


SATOSHIS_PER_BTC = Decimal("100000000")


def btc_to_sats(value):
    return int(Decimal(str(value)) * SATOSHIS_PER_BTC)


def test_transaction_outputs(rpc, payment_txid, block_hash):
    tx = rpc.getrawtransaction(payment_txid, True, block_hash)

    output_sum = sum(
        (Decimal(str(output["value"])) for output in tx["vout"]),
        Decimal("0")
    )

    assert output_sum == Decimal("49.99999718")


def test_payment_recipient(rpc, payment_txid, block_hash):
    tx = rpc.getrawtransaction(payment_txid, True, block_hash)

    recipient = "bcrt1qcy7fa33eguuluhdunr3n0pc5ny8c79c6lvnxkv"

    matching_outputs = [
        output for output in tx["vout"]
        if output["scriptPubKey"].get("address") == recipient
    ]

    assert len(matching_outputs) == 1
    assert Decimal(str(matching_outputs[0]["value"])) == Decimal("1.00000000")


def test_transaction_fee(rpc, payment_txid, block_hash):
    tx = rpc.getrawtransaction(payment_txid, True, block_hash)

    previous_output_sum = Decimal("0")

    for vin in tx["vin"]:
        previous_txid = vin["txid"]

        wallet_tx = rpc.gettransaction(previous_txid)
        previous_tx = rpc.decoderawtransaction(wallet_tx["hex"])

        previous_output = previous_tx["vout"][vin["vout"]]
        previous_output_sum += Decimal(str(previous_output["value"]))

    current_output_sum = sum((Decimal(str(o["value"])) for o in tx["vout"]), Decimal("0"))

    fee_sats = btc_to_sats(previous_output_sum - current_output_sum)

    assert fee_sats == 282


def test_transaction_virtual_size(rpc, payment_txid, block_hash):
    tx = rpc.getrawtransaction(payment_txid, True, block_hash)

    assert tx["vsize"] == 141
    assert tx["weight"] == 561
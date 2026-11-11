import pytest


def get_output_address(output):
    script = output.get("scriptPubKey", {})
    return script.get("address")


def test_missing_address_does_not_crash():
    output = {
        "value": 1.0,
        "scriptPubKey": {
            "type": "nulldata"
        }
    }

    address = get_output_address(output)

    assert address is None


def test_missing_script_does_not_crash():
    output = {
        "value": 1.0
    }

    address = get_output_address(output)

    assert address is None


def test_unavailable_transaction_history_is_reported(rpc):
    unknown_txid = "0" * 64

    with pytest.raises(Exception):
        rpc.getrawtransaction(unknown_txid, True)
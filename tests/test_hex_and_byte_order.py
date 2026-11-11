import pytest

from block_header_utils import hex_to_internal_bytes


def test_valid_txid_has_32_bytes():
    txid = "8c14f0db3df150123e6f3dbbf30f8b955a8249b62ac1d1ff16284aefa3d06d87"

    result = hex_to_internal_bytes(txid)

    assert len(result) == 32


def test_byte_order_is_reversed():
    txid = "8c14f0db3df150123e6f3dbbf30f8b955a8249b62ac1d1ff16284aefa3d06d87"

    result = hex_to_internal_bytes(txid)

    assert result == bytes.fromhex(txid)[::-1]


def test_invalid_hex_is_rejected():
    with pytest.raises(ValueError):
        hex_to_internal_bytes("xyz123")


def test_short_hex_is_rejected():
    with pytest.raises(ValueError):
        hex_to_internal_bytes("1234")


def test_non_string_is_rejected():
    with pytest.raises(ValueError):
        hex_to_internal_bytes(123)
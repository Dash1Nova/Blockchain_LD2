from block_header_utils import (
    build_block_header,
    calculate_block_hash,
    calculate_sha256d,
)


def test_real_block_header_is_80_bytes(rpc, block_hash):
    block = rpc.getblockheader(block_hash, True)

    header = build_block_header(
        block["version"],
        block["previousblockhash"],
        block["merkleroot"],
        block["time"],
        int(block["bits"], 16),
        block["nonce"]
    )

    assert len(header) == 80


def test_real_block_hash_matches_rpc(rpc, block_hash):
    block = rpc.getblockheader(block_hash, True)

    header = build_block_header(
        block["version"],
        block["previousblockhash"],
        block["merkleroot"],
        block["time"],
        int(block["bits"], 16),
        block["nonce"]
    )

    assert calculate_block_hash(header) == block_hash


def test_sha256d_returns_32_bytes():
    result = calculate_sha256d(bytes(80))

    assert len(result) == 32


def test_nonce_change_changes_hash(rpc, block_hash):
    block = rpc.getblockheader(block_hash, True)

    header = build_block_header(
        block["version"],
        block["previousblockhash"],
        block["merkleroot"],
        block["time"],
        int(block["bits"], 16),
        block["nonce"]
    )

    changed_header = (
        header[:76]
        + ((block["nonce"] + 1) % (2 ** 32)).to_bytes(4, "little")
    )

    assert calculate_block_hash(changed_header) != block_hash
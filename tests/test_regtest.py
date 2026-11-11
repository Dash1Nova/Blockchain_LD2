import json
import shutil
import subprocess

import pytest


DATADIR = "/home/user11/bitcoin-regtest-mif"


def test_python_rpc_uses_regtest(rpc):
    info = rpc.getblockchaininfo()

    assert info["chain"] == "regtest"
    assert info["blocks"] >= 1


def test_bitcoin_core_version(rpc):
    info = rpc.getnetworkinfo()

    assert info["version"] == 230000


def test_cli_and_python_use_same_node(rpc):
    bitcoin_cli = shutil.which("bitcoin-cli")

    if bitcoin_cli is None:
        pytest.skip("bitcoin-cli nerastas PATH aplinkoje")

    command = [
        bitcoin_cli,
        f"-datadir={DATADIR}",
        "-regtest",
        "-rpcport=19011",
        "getblockchaininfo"
    ]

    result = subprocess.run(
        command,
        capture_output=True,
        text=True,
        timeout=30
    )

    assert result.returncode == 0, result.stderr

    cli_info = json.loads(result.stdout)
    python_info = rpc.getblockchaininfo()

    assert cli_info["chain"] == python_info["chain"]
    assert cli_info["blocks"] == python_info["blocks"]
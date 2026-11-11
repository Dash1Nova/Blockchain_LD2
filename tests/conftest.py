from pathlib import Path

import bitcoin
import pytest
from bitcoin.rpc import RawProxy


bitcoin.SelectParams("regtest")


@pytest.fixture(scope="session")
def rpc():
    conf = Path.home() / "bitcoin-regtest-mif" / "bitcoin.conf"

    if not conf.exists():
        pytest.fail("Nerastas bitcoin.conf failas")

    return RawProxy(
        btc_conf_file=str(conf),
        service_port=19011,
        timeout=30
    )


@pytest.fixture
def payment_txid():
    return (
        "6e81edf620e0f413960991b6e57cc38a"
        "c5195316533fdf4ac8844da573ef02b0"
    )


@pytest.fixture
def block_hash():
    return (
        "5242bf8d90e2b8bd729e095dafcc4297"
        "c1e79c522a5a1464631a60681f9cbfcc"
    )
from pathlib import Path
from decimal import Decimal

import bitcoin
from bitcoin.rpc import RawProxy

bitcoin.SelectParams("regtest")

conf = Path.home() / "bitcoin-regtest-mif" / "bitcoin.conf"

BLOCKHASH = "5242bf8d90e2b8bd729e095dafcc4297c1e79c522a5a1464631a60681f9cbfcc"

try:
    rpc = RawProxy(
        btc_conf_file=str(conf),
        service_port=19011,
        timeout=30
    )

    block = rpc.getblock(BLOCKHASH, 2)

    print("=== BLOKO ANALIZĖ ===")
    print("Bloko hash:", block["hash"])
    print("Bloko aukštis:", block["height"])
    print("Transakcijų skaičius:", len(block["tx"]))

    total_outputs = Decimal("0")

    for tx in block["tx"]:
        for output in tx["vout"]:
            total_outputs += Decimal(str(output["value"]))

    print(
        "Visų bloko transakcijų išvesčių suma:",
        total_outputs,
        "BTC"
    )

except Exception as error:
    print("Bloko analizės klaida:", error)

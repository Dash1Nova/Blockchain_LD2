from pathlib import Path
from decimal import Decimal

import bitcoin
from bitcoin.rpc import RawProxy

bitcoin.SelectParams("regtest")

conf = Path.home() / "bitcoin-regtest-mif" / "bitcoin.conf"

TXID = "6e81edf620e0f413960991b6e57cc38ac5195316533fdf4ac8844da573ef02b0"
BLOCKHASH = "5242bf8d90e2b8bd729e095dafcc4297c1e79c522a5a1464631a60681f9cbfcc"

try:
    rpc = RawProxy(
        btc_conf_file=str(conf),
        service_port=19011,
        timeout=30
    )

    tx = rpc.getrawtransaction(TXID, 1, BLOCKHASH)

    print("=== TRANSAKCIJOS ANALIZĖ ===")
    print("TXID:", tx["txid"])
    print("Wtxid (hash):", tx.get("hash", "Nenurodytas"))
    print("Patvirtinimai:", tx["confirmations"])
    print("Dydis (size):", tx["size"], "bytes")
    print("Virtualus dydis (vsize):", tx["vsize"], "vB")
    print("Svoris (weight):", tx["weight"], "WU")

    total = Decimal("0")

    print("\n=== ĮVESTYS (VIN) ===")
    for vin in tx["vin"]:
        print("Ankstesnis TXID:", vin.get("txid", "coinbase"))
        print("VOUT indeksas:", vin.get("vout", "-"))
        print("scriptSig:", vin.get("scriptSig", {}).get("hex", ""))
        print("txinwitness:", vin.get("txinwitness", "Nėra"))

    print("\n=== IŠVESTYS (VOUT) ===")
    for index, output in enumerate(tx["vout"]):
        amount = Decimal(str(output["value"]))
        total += amount

        script = output["scriptPubKey"]

        print("\nIšvestis:", index)
        print("Suma BTC:", amount)
        print("Skripto tipas:", script.get("type", "nežinomas"))
        print("Adresas:", script.get("address", "nenurodytas"))

    print("\nVisų išvesčių suma:", total, "BTC")

except Exception as error:
    print("Transakcijos analizės klaida:", error)

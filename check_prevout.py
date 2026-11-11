from pathlib import Path
import bitcoin
from bitcoin.rpc import RawProxy

bitcoin.SelectParams("regtest")

conf = Path.home() / "bitcoin-regtest-mif" / "bitcoin.conf"

TXID = "d5fede9a7154dd8bf8eb52aebbd7ce87a6248a223d45efe22500ef3412e64b51"
VOUT = 0

try:
    rpc = RawProxy(
        btc_conf_file=str(conf),
        service_port=19011,
        timeout=30
    )

    tx = rpc.getrawtransaction(TXID, 1)

    output = tx["vout"][VOUT]

    print("Ankstesnės transakcijos TXID:", TXID)
    print("VOUT indeksas:", VOUT)
    print("Išvesties vertė:", output["value"], "BTC")
    print("Skriptas:", output["scriptPubKey"].get("type"))
    print("Adresas:", output["scriptPubKey"].get("address", "nenurodytas"))

except Exception as error:
    print("Nepavyko gauti ankstesnės transakcijos:")
    print(error)
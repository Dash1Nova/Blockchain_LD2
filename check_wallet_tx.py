
from pathlib import Path
from urllib.parse import quote

import bitcoin
from bitcoin.rpc import RawProxy

bitcoin.SelectParams("regtest")

conf = Path.home() / "bitcoin-regtest-mif" / "bitcoin.conf"
PAYMENT_TXID = "6e81edf620e0f413960991b6e57cc38ac5195316533fdf4ac8844da573ef02b0"

try:
    settings = {}

    for line in conf.read_text().splitlines():
        line = line.strip()

        if "=" in line and not line.startswith("#"):
            key, value = line.split("=", 1)
            settings[key.strip()] = value.strip()

    user = quote(settings["rpcuser"], safe="")
    password = quote(settings["rpcpassword"], safe="")
    port = settings.get("rpcport", "19011")

    url = f"http://{user}:{password}@127.0.0.1:{port}/wallet/MIF"

    wallet_rpc = RawProxy(
        service_url=url,
        timeout=30
    )

    payment = wallet_rpc.gettransaction(PAYMENT_TXID)

    print("=== MIF PINIGINĖ ===")
    print("TXID:", payment["txid"])
    print("Mokestis:", payment.get("fee", "Nenurodytas"), "BTC")
    print("Vsize:", payment.get("vsize", "Nenurodytas"))
    PREV_TXID = "d5fede9a7154dd8bf8eb52aebbd7ce87a6248a223d45efe22500ef3412e64b51"

    previous = wallet_rpc.gettransaction(PREV_TXID)
    decoded = wallet_rpc.decoderawtransaction(previous["hex"])

    print("\n=== ANKSTESNĖ TRANSAKCIJA ===")
    print("Ankstesnės transakcijos TXID:", decoded["txid"])
    print("Įvesties VOUT indeksas: 0")
    print("Ankstesnės transakcijos 0 išvesties suma:", decoded["vout"][0]["value"], "BTC")

except Exception as error:
    print("RPC užklausos klaida:")
    print(error)

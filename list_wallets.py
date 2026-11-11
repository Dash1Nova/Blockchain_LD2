from pathlib import Path
import bitcoin
from bitcoin.rpc import RawProxy

bitcoin.SelectParams("regtest")

conf = Path.home() / "bitcoin-regtest-mif" / "bitcoin.conf"

try:
    rpc = RawProxy(
        btc_conf_file=str(conf),
        service_port=19011,
        timeout=30
    )

    wallets = rpc.listwallets()

    print("Įkeltos piniginės:")
    for wallet in wallets:
        print("-", wallet)

except Exception as error:
    print("Klaida:", error)
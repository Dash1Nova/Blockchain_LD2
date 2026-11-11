from pathlib import Path
from datetime import datetime, timezone

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

    info = rpc.getblockchaininfo()
    network = rpc.getnetworkinfo()

    print("Stebėjimo laikas:",
          datetime.now(timezone.utc).isoformat())
    print("Chain:", info["chain"])
    print("Blocks:", info["blocks"])
    print("Headers:", info["headers"])
    print("Initial block download:",
          info["initialblockdownload"])
    print("Bitcoin Core versija:", network["version"])
    print("Aktyvūs ryšiai:", network["connections"])

except Exception as error:
    print("Nepavyko prisijungti prie Bitcoin Core RPC:")
    print(error)

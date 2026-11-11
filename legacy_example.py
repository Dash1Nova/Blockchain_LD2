from pathlib import Path
import bitcoin
from bitcoin.rpc import RawProxy

bitcoin.SelectParams("regtest")

conf = Path.home() / "bitcoin-regtest-mif" / "bitcoin.conf"

RAW_HEX = """
0100000001186f9f998a5aa6f048e51dd8419a14d8a0f1a8a2836dd734d2804fe65fa35779000000008b4830450
22100884d142d86652a3f47ba4746ec719bbfbd040a570b1deccbb6498c75c4ae24cb02204b9f039ff08df09cbe9
f6addac960298cad530a863ea8f53982c09db8f6e381301410484ecc0d46f1918b30928fa0e4ed99f16a0fb4fde07
35e7ade8416ab9fe423cc5412336376789d172787ec3457eee41c04f4938de5cc17b4a10fa336a8d752adfffffffff0
260e31600000000001976a914ab68025513c3dbd2f7b92a94e0581f5d50f654e788acd0ef8000000000001976a
9147f9b1a7fb68d60c536c2fd8aeaa53a8f3cc025a888ac00000000
"""

try:
    raw = "".join(RAW_HEX.split())

    rpc = RawProxy(
        btc_conf_file=str(conf),
        service_port=19011,
        timeout=30
    )

    tx = rpc.decoderawtransaction(raw)

    print("=== LEGACY TRANSAKCIJOS ANALIZĖ ===")
    print("TXID:", tx["txid"])
    print("Wtxid (hash):", tx.get("hash", "Nenurodytas"))
    print("Version:", tx["version"])
    print("Size:", tx["size"], "bytes")
    print("Vsize:", tx["vsize"], "vB")
    print("Weight:", tx["weight"], "WU")
    print("Locktime:", tx["locktime"])

    print("\nĮvesčių skaičius:", len(tx["vin"]))
    print("Išvesčių skaičius:", len(tx["vout"]))

    for vin in tx["vin"]:
        print("\nscriptSig:", vin.get("scriptSig", {}).get("hex", ""))
        print("txinwitness:", vin.get("txinwitness", "Nėra"))

except Exception as error:
    print("Legacy transakcijos analizės klaida:")
    print(error)
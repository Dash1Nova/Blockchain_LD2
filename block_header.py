from pathlib import Path
import hashlib
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

    # Gauname 102 aukščio bloko maišą
    height = 102
    block_hash = rpc.getblockhash(height)

    # Gauname iškoduotus bloko duomenis
    block = rpc.getblockheader(block_hash, True)

    # Gauname originalią 80 baitų antraštę HEX formatu
    original_hex = rpc.getblockheader(block_hash, False)

    # Atkuriame antraštę iš atskirų laukų
    version = block["version"]
    previous_hash = block["previousblockhash"]
    merkle_root = block["merkleroot"]
    timestamp = block["time"]
    bits = block["bits"]
    nonce = block["nonce"]

    header_bytes = (
        version.to_bytes(4, "little", signed=True)
        + bytes.fromhex(previous_hash)[::-1]
        + bytes.fromhex(merkle_root)[::-1]
        + timestamp.to_bytes(4, "little")
        + int(bits, 16).to_bytes(4, "little")
        + nonce.to_bytes(4, "little")
    )

    reconstructed_hex = header_bytes.hex()

    # Apskaičiuojame dvigubą SHA-256
    first_hash = hashlib.sha256(header_bytes).digest()
    second_hash = hashlib.sha256(first_hash).digest()
    calculated_hash = second_hash[::-1].hex()

    print("=== BLOKO ANTRAŠTĖS ANALIZĖ ===")
    print("Bloko aukštis:", height)
    print("Bloko hash:", block_hash)
    print("Version:", version)
    print("Previous block hash:", previous_hash)
    print("Merkle root:", merkle_root)
    print("Time:", timestamp)
    print("Bits:", bits)
    print("Nonce:", nonce)

    print("\n=== ANTRAŠTĖS PATIKRINIMAS ===")
    print("Antraštės dydis:", len(header_bytes), "baitų")
    print("Atkurta HEX sutampa:", reconstructed_hex == original_hex)

    print("\nOriginalus HEX:")
    print(original_hex)

    print("\nAtkurtas HEX:")
    print(reconstructed_hex)

    print("\n=== SHA256d PATIKRINIMAS ===")
    print("Apskaičiuotas hash:", calculated_hash)
    print("Hash sutampa:", calculated_hash == block_hash)

    # Pakeičiame nonce ir perskaičiuojame maišą
    changed_nonce = (nonce + 1) % (2 ** 32)

    changed_header = (
        header_bytes[:76]
        + changed_nonce.to_bytes(4, "little")
    )

    changed_hash = hashlib.sha256(
        hashlib.sha256(changed_header).digest()
    ).digest()[::-1].hex()

    print("\n=== NONCE PAKEITIMO EKSPERIMENTAS ===")
    print("Pradinis nonce:", nonce)
    print("Pakeistas nonce:", changed_nonce)
    print("Pradinis hash:", calculated_hash)
    print("Naujas hash:", changed_hash)
    print("Hash pasikeitė:", changed_hash != calculated_hash)

except Exception as error:
    print("Bloko antraštės analizės klaida:")
    print(error)
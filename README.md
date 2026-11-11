# Blockchain_LD3

# Bitcoin duomenų analizė su Python

## Darbo tikslas

Šio darbo tikslas – naudojant Python ir `python-bitcoinlib` biblioteką prisijungti prie lokalaus Bitcoin Core regtest tinklo per RPC sąsają ir išanalizuoti tinklo informaciją, Bitcoin transakcijas bei blokus.

Darbo metu atliekama:

- Bitcoin Core tinklo ir blokų grandinės informacijos analizė.
- Bitcoin transakcijos įvesčių ir išvesčių analizė.
- Bloko transakcijų ir išvesčių analizė.
- Transakcijos mokesčio apskaičiavimas.
- Legacy ir SegWit transakcijų palyginimas.
- Bitcoin bloko antraštės atkūrimas ir SHA256d maišos patikrinimas.

Naudojamas tinklas – `regtest`. RPC prievadas – `19011`. Python aplinkoje naudojama `python-bitcoinlib` versija `0.12.2`.

## Naudojamos programos

| Programa             | Paskirtis                                                   |
| -------------------- | ----------------------------------------------------------- |
| `rpc_example.py`     | Bitcoin Core tinklo ir programos informacijos gavimas       |
| `rpc_transaction.py` | 1 BTC mokėjimo transakcijos analizė                         |
| `rpc_block.py`       | Bloko transakcijų išvesčių analizė                          |
| `check_prevout.py`   | Ankstesnių transakcijų išvesčių tikrinimas                  |
| `check_wallet_tx.py` | Ankstesnės įvesties vertės ir piniginės mokesčio tikrinimas |
| `list_wallets.py`    | Bitcoin Core piniginių sąrašo gavimas                       |
| `legacy_example.py`  | Istorinės Legacy transakcijos analizė                       |
| `block_header.py`    | Bloko antraštės atkūrimas ir SHA256d skaičiavimas           |

---

# 1. Bitcoin Core tinklo informacijos analizė

## `rpc_example.py` – tinklo informacija

Programa per RPC iškviečia `getblockchaininfo` ir `getnetworkinfo` metodus. Ji pateikia aktyvaus tinklo pavadinimą, blokų ir antraščių skaičių, pradinio sinchronizavimo būseną, Bitcoin Core versiją, aktyvių ryšių skaičių ir stebėjimo laiką.

<img width="1002" height="187" alt="Ekrano kopija 2026-11-09 151919" src="https://github.com/user-attachments/assets/19b7c190-043d-49bd-b10f-fa71545dc516" />

Gauti rezultatai:

| Rodiklis               | Reikšmė               |
| ---------------------- | --------------------- |
| Chain                  | `regtest`             |
| Blocks                 | `103`                 |
| Headers                | `103`                 |
| Initial block download | `False`               |
| Bitcoin Core versija   | `230000`              |
| Aktyvūs ryšiai         | `0`                   |
| Stebėjimo laikas (UTC) | `2026-11-09 13:04:43` |

**Paaiškinimas.**

`chain` reikšmė `regtest` patvirtina, kad naudojamas vietinis testinis Bitcoin tinklas. `blocks` ir `headers` reikšmės sutampa, todėl šiame mazge žinomų blokų antraščių skaičius atitinka atsisiųstų blokų skaičių. `Initial block download = False` reiškia, kad mazgas nėra pradinio sinchronizavimo būsenoje.

Aktyvių ryšių skaičius yra 0. Izoliuotame regtest tinkle tai savaime nėra gedimas, nes toks tinklas gali veikti lokaliai ir neturėti išorinių mazgų. Regtest nėra skirtas sinchronizuoti visą viešojo Bitcoin tinklo istoriją.

---

# 2. Bitcoin transakcijos analizė

## `rpc_transaction.py` – 1 BTC transakcijos analizė

Programa naudoja `getrawtransaction(txid, 1, blockhash)` metodą, kad gautų išsamų patvirtintos transakcijos JSON aprašą.

Analizuotos transakcijos TXID:

`6e81edf620e0f413960991b6e57cc38ac5195316533fdf4ac8844da573ef02b0`

Bloko hash:

`5242bf8d90e2b8bd729e095dafcc4297c1e79c522a5a1464631a60681f9cbfcc`

Transakcija turi 2 patvirtinimus.

### Įvestis (vin)

| Laukas          | Reikšmė                                                            |
| --------------- | ------------------------------------------------------------------ |
| Ankstesnis TXID | `d5fede9a7154dd8bf8eb52aebbd7ce87a6248a223d45efe22500ef3412e64b51` |
| VOUT indeksas   | `0`                                                                |

Įvestis nurodo ankstesnę transakciją ir jos išvesties indeksą, iš kurios panaudotos lėšos.

### Išvestis (vout)

| Indeksas |  Suma (BTC) | Skripto tipas        | Adresas                                        |
| -------- | ----------: | -------------------- | ---------------------------------------------- |
| 0        | 48.99999718 | `witness_v0_keyhash` | `bcrt1qldlnxgu9x2q89q0zhnf3zw3s0swx5mrnz7vmdz` |
| 1        |  1.00000000 | `witness_v0_keyhash` | `bcrt1qcy7fa33eguuluhdunr3n0pc5ny8c79c6lvnxkv` |

Bendra išvesčių suma: **49.99999718 BTC**.

**Paaiškinimas.**

Viena iš išvesčių yra 1 BTC mokėjimas nurodytu adresu. Kita išvestis yra grąža į kitą adresą. Bitcoin transakcijoje visa įvesties vertė nebūtinai perduodama gavėjui – nepanaudota dalis gali būti grąžinama siuntėjui. Skirtumas tarp įvesties ir išvesčių sumos yra transakcijos mokestis.

Programa analizuoja `vin` ir `vout` atskirai. `vin` nurodo ankstesnių transakcijų išvestis, o `vout` aprašo šios transakcijos sukurtas naujas išvestis.

---

# 3. Bitcoin bloko analizė

## `rpc_block.py` – bloko analizė

Programa naudoja `getblock(blockhash, 2)` metodą, kuris grąžina išsamų bloko ir jo transakcijų JSON aprašą. Taip nereikia kiekvienai bloko transakcijai atskirai iškviesti `getrawtransaction`.

<img width="978" height="141" alt="Ekrano kopija 2026-11-09 152111" src="https://github.com/user-attachments/assets/44a6554d-30a6-485a-9c5f-eb418a64c2d6" />

Gauti rezultatai:

| Rodiklis             | Reikšmė                                                            |
| -------------------- | ------------------------------------------------------------------ |
| Bloko aukštis        | `102`                                                              |
| Bloko hash           | `5242bf8d90e2b8bd729e095dafcc4297c1e79c522a5a1464631a60681f9cbfcc` |
| Transakcijų skaičius | `2`                                                                |
| Visų išvesčių suma   | `100.00000000 BTC`                                                 |

**Paaiškinimas.**

Bloke yra dvi transakcijos: coinbase transakcija ir analizuotas 1 BTC mokėjimas. Susumavus abiejų transakcijų išvestis gaunama 100 BTC.

Ši suma nėra naujai sukurtų monetų kiekis ir nėra ekonominių mokėjimų apimtis. Į ją įtraukiamos visos išvestys, įskaitant coinbase transakcijos išvestis ir mokėjimo transakcijos grąžą. Todėl ši reikšmė apibūdina tik bendrą konkretaus bloko transakcijų išvesčių sumą.

---

# 4. Mokestis, SegWit ir bloko maiša

Šioje darbo dalyje atlikta išsamesnė Bitcoin transakcijos ir bloko analizė. Apskaičiuotas transakcijos mokestis, palygintos Legacy ir SegWit transakcijos bei savarankiškai atkurta bloko antraštė.

## 4.1. Transakcijos mokesčio apskaičiavimas

### Tikslas

Apskaičiuoti transakcijos mokestį pagal ankstesnių išleidžiamų išvesčių ir naujų transakcijos išvesčių vertes. Gautą rezultatą patikrinti su MIF piniginės `gettransaction` metodu.

### Ankstesnės įvesties vertė

Analizuojamos transakcijos įvestis nurodo ankstesnę transakciją ir jos išvesties indeksą:

- Ankstesnės transakcijos TXID:

`d5fede9a7154dd8bf8eb52aebbd7ce87a6248a223d45efe22500ef3412e64b51`

- VOUT indeksas: `0`
- Ankstesnės išvesties vertė: `50.00000000 BTC`

Ankstesnės transakcijos išvesties vertė nustatyta naudojant Bitcoin Core RPC metodą `decoderawtransaction`.

Svarbu susieti abu laukus – TXID ir VOUT indeksą, nes viena transakcija gali turėti kelias skirtingas išvestis.

### Naujos transakcijos išvestys

| VOUT        |      Suma (BTC) |
| ----------- | --------------: |
| 0           |     48.99999718 |
| 1           |      1.00000000 |
| **Iš viso** | **49.99999718** |

### Mokesčio skaičiavimas

Transakcijos mokestis apskaičiuojamas pagal formulę:

**Mokestis = ankstesnių išvesčių suma − naujų išvesčių suma**

50.00000000 − 49.99999718 = 0.00000282 BTC.

Kadangi 1 BTC = 100 000 000 satoshi:

0.00000282 × 100 000 000 = **282 satoshi**.

Taigi apskaičiuotas transakcijos mokestis yra 282 satoshi.

### Mokesčio patikrinimas su MIF pinigine

Naudojant `check_wallet_tx.py` programą ir MIF piniginės RPC metodą `gettransaction`, gautas toks rezultatas:

```text
=== MIF PINIGINĖ ===
TXID: 6e81edf620e0f413960991b6e57cc38ac5195316533fdf4ac8844da573ef02b0
Mokestis: -0.00000282 BTC
```

Piniginės `fee` laukas pateikia mokestį su neigiamu ženklu, nes tai yra piniginės išlaidos. Todėl palyginimui naudojama absoliuti vertė.

| Šaltinis                            |       Mokestis |
| ----------------------------------- | -------------: |
| Apskaičiuotas mokestis              | 0.00000282 BTC |
| MIF piniginės `fee` absoliuti vertė | 0.00000282 BTC |

Abu rezultatai sutampa.

### Faktinio mokesčio tarifo skaičiavimas

Transakcijos virtualus dydis (`vsize`) yra 141 vB.

Mokesčio tarifas apskaičiuojamas pagal formulę:

**Mokesčio tarifas = mokestis (sat) / vsize (vB)**

282 / 141 = **2 sat/vB**.

Skaičiavimui naudojamas virtualus dydis (`vsize`), o ne įprastas transakcijos dydis (`size`).

### A dalies išvada

Nustatyta, kad transakcijos įvesties vertė yra 50 BTC, o bendra išvesčių vertė – 49.99999718 BTC. Apskaičiuotas mokestis yra 282 satoshi arba 0.00000282 BTC. Rezultatas sutampa su MIF piniginės pateiktu `gettransaction` mokesčiu, įvertinus jo neigiamą ženklą. Faktinis mokesčio tarifas yra 2 sat/vB.

---

## 4.2. Legacy ir SegWit transakcijų palyginimas

### Tikslas

Palyginti laboratorijoje atliktą SegWit mokėjimo transakciją su istorine Legacy transakcija iš paskaitos medžiagos. Analizuojami transakcijų identifikatoriai, dydžiai, svoriai, `scriptSig` ir `txinwitness` laukai.

### Legacy transakcijos analizė

Istorinės Legacy transakcijos duomenys paimti iš paskaitos medžiagos ir analizuoti naudojant `legacy_example.py` programą.

Legacy transakcijos TXID:

`0627052b6f28912f2703066a912ea577f2ce4da4caa5a5fbd8a57286c345c2f2`

Gauti rezultatai:

```text
TXID: 0627052b6f28912f2703066a912ea577f2ce4da4caa5a5fbd8a57286c345c2f2
Wtxid (hash): 0627052b6f28912f2703066a912ea577f2ce4da4caa5a5fbd8a57286c345c2f2
Version: 1
Size: 258 bytes
Vsize: 258 vB
Weight: 1032 WU
Locktime: 0

Įvesčių skaičius: 1
Išvesčių skaičius: 2
```

Legacy transakcijoje `scriptSig` lauke yra parašas ir viešasis raktas. `txinwitness` duomenų nėra.

### SegWit transakcijos analizė

Laboratorijoje atliktos SegWit transakcijos TXID:

`6e81edf620e0f413960991b6e57cc38ac5195316533fdf4ac8844da573ef02b0`

Gauti rezultatai:

```text
TXID: 6e81edf620e0f413960991b6e57cc38ac5195316533fdf4ac8844da573ef02b0
Wtxid (hash): 0213d4f8827ceb33ff6f47802422cddd7d38fb39d8f817ddc6d10d3016f70c97
Patvirtinimai: 2
Size: 222 bytes
Vsize: 141 vB
Weight: 561 WU
```

SegWit transakcijos įvesties `scriptSig` laukas yra tuščias, o `txinwitness` lauke pateikti du elementai – parašas ir viešasis raktas.

Tai patvirtina, kad įvestyje naudojama SegWit struktūra. Vien tai, kad transakcijos išvestys yra SegWit tipo, neįrodo, kad jos įvestys taip pat turi Witness duomenų. Todėl buvo tikrinama pati įvestis.

### Transakcijų palyginimas

| Rodiklis           |                Legacy |                SegWit |
| ------------------ | --------------------: | --------------------: |
| TXID               | `0627052b...c345c2f2` | `6e81edf6...73ef02b0` |
| RPC `hash` (Wtxid) |       Sutampa su TXID | `0213d4f8...16f70c97` |
| Version            |                     1 |                     2 |
| Size               |             258 bytes |             222 bytes |
| Vsize              |                258 vB |                141 vB |
| Weight             |               1032 WU |                561 WU |
| `scriptSig`        |                   Yra |               Tuščias |
| `txinwitness`      |                  Nėra |           2 elementai |
| Įvesčių skaičius   |                     1 |                     1 |
| Išvesčių skaičius  |                     2 |                     2 |

### TXID ir Wtxid skirtumai

Legacy transakcijoje nėra atskiros Witness dalies, todėl TXID ir Wtxid sutampa.

SegWit transakcijoje naudojami du skirtingi identifikatoriai:

- **TXID** – apskaičiuojamas iš transakcijos serializacijos, neįtraukiant Witness duomenų.
- **Wtxid** – apskaičiuojamas iš visos transakcijos serializacijos, įskaitant Witness duomenis.

Todėl SegWit transakcijoje TXID ir Wtxid reikšmės skiriasi.

Svarbu atskirti transakcijos identifikatorių nuo pasirašomo pranešimo. TXID ir Wtxid yra transakcijos identifikatoriai, o skaitmeninis parašas tikrinamas pagal konkrečios įvesties pasirašymo taisykles sudarytą pranešimą. Jo turinys priklauso nuo skripto tipo ir `SIGHASH` režimo. Pasirašomas pranešimas nėra tiesiog TXID arba Wtxid.

### Weight ir vsize skaičiavimas

Bitcoin transakcijų svoris apskaičiuojamas pagal formulę:

**weight = 3 × base_size + total_size**

Virtualus dydis:

**vsize = ceil(weight / 4)**

#### Legacy transakcija

Legacy transakcijoje nėra Witness duomenų, todėl:

- `base_size = 258 bytes`
- `total_size = 258 bytes`

Svoris:

3 × 258 + 258 = **1032 WU**.

Virtualus dydis:

ceil(1032 / 4) = **258 vB**.

#### SegWit transakcija

SegWit transakcijos duomenys:

- `base_size = 113 bytes`
- `total_size = 222 bytes`

Svoris:

3 × 113 + 222 = **561 WU**.

Virtualus dydis:

ceil(561 / 4) = **141 vB**.

Abiejų transakcijų skaičiavimai sutampa su Bitcoin Core pateiktais dydžio ir svorio rodikliais.

### Bloko talpos apribojimas

Bitcoin bloko svorio riba yra 4 000 000 WU. Tai nėra paprasta 4 MB dydžio riba. SegWit leidžia Witness duomenims turėti mažesnį svorio poveikį, todėl bloko talpa išnaudojama efektyviau.

### B dalies išvada

Palyginus Legacy ir SegWit transakcijas nustatyta, kad Legacy transakcijoje TXID ir Wtxid sutampa, o SegWit transakcijoje jie skiriasi. Legacy transakcijos parašas ir viešasis raktas pateikiami `scriptSig` lauke, o SegWit transakcijoje jie pateikiami `txinwitness` lauke.

Legacy transakcijos virtualus dydis yra 258 vB, o SegWit transakcijos – 141 vB. SegWit transakcijos svoris yra 561 WU, palyginti su Legacy transakcijos 1032 WU. Tai parodo SegWit efektyvumą mažinant transakcijos svorio ir virtualaus dydžio poveikį.

---

## 4.3. Bloko antraštės atkūrimas ir SHA256d maišos patikrinimas

### Tikslas

Savarankiškai atkurti Bitcoin bloko 80 baitų antraštę iš Bitcoin Core pateiktų dekoduotų duomenų, palyginti ją su originalia antrašte ir apskaičiuoti dvigubą SHA-256 maišą (SHA256d).

Eksperimentui pasirinktas laboratorijos regtest bloko aukštis 102.

### Naudotas blokas

Bloko hash:

`5242bf8d90e2b8bd729e095dafcc4297c1e79c522a5a1464631a60681f9cbfcc`

Bitcoin Core pateikti antraštės laukai:

| Laukas              | Reikšmė                                                            |
| ------------------- | ------------------------------------------------------------------ |
| Height              | 102                                                                |
| Version             | 536870912                                                          |
| Previous block hash | `6479e759c6cb9face7a160f758e73cc3401a2bff2c064f190d8f7dbd94cc676a` |
| Merkle root         | `df468b6931aeb1c0cb0ce7991d3cb1e7880fdce138c45255ff0bfe284ae107f4` |
| Time                | 1792661385                                                         |
| Bits                | `207fffff`                                                         |
| Nonce               | 4                                                                  |

### Antraštės serializavimas

Bitcoin bloko antraštę sudaro 80 baitų:

| Laukas              |     Dydis | Serializavimo tvarka                                               |
| ------------------- | --------: | ------------------------------------------------------------------ |
| Version             |  4 baitai | Little-endian                                                      |
| Previous block hash | 32 baitai | Baitų seka apverčiama                                              |
| Merkle root         | 32 baitai | Baitų seka apverčiama                                              |
| Time                |  4 baitai | Little-endian                                                      |
| Bits                |  4 baitai | Hex reikšmė paverčiama 32 bitų skaičiumi ir užrašoma little-endian |
| Nonce               |  4 baitai | Little-endian                                                      |

Atkuriant antraštę kiekvienas laukas buvo serializuotas atskirai. Maišų baitų seka apversta, nes Bitcoin Core pateikia jų tekstinį atvaizdavimą priešinga baitų tvarka nei naudojama bloko antraštėje.

### Atkurtos antraštės patikrinimas

Programoje `block_header.py` atkurtos antraštės HEX buvo palygintas su Bitcoin Core metodu `getblockheader(hash, false)` gautu originaliu HEX.

Gauti rezultatai:

```text
Antraštės dydis: 80 baitų
Atkurta HEX sutampa: True
```

Tai patvirtina, kad antraštės laukai buvo serializuoti teisingai ir atkurtas HEX tiksliai sutampa su Bitcoin Core pateiktais baitais.

### SHA256d skaičiavimas

Bitcoin bloko identifikatorius apskaičiuojamas du kartus pritaikius SHA-256 algoritmą:

**SHA256d = SHA256(SHA256(header))**

Galutinio maišos rezultato baitai apverčiami, kad būtų gautas Bitcoin Core naudojamas tekstinis bloko hash atvaizdavimas.

Apskaičiuotas hash:

`5242bf8d90e2b8bd729e095dafcc4297c1e79c522a5a1464631a60681f9cbfcc`

Bitcoin Core pateiktas hash:

`5242bf8d90e2b8bd729e095dafcc4297c1e79c522a5a1464631a60681f9cbfcc`

Patikrinimo rezultatas:

```text
Hash sutampa: True
```

Apskaičiuota maiša sutampa su Bitcoin Core pateiktu bloko hash.

### Nonce pakeitimo eksperimentas

Papildomai atliktas eksperimentas, kurio metu pradinis nonce buvo pakeistas iš 4 į 5. Tuomet iš naujo apskaičiuota bloko antraštės SHA256d maiša.

| Rodiklis |      Pradinė reikšmė |      Pakeista reikšmė |
| -------- | -------------------: | --------------------: |
| Nonce    |                    4 |                     5 |
| Hash     | `5242bf8d...f9cbfcc` | `c470162f...2fd7c33d` |

Visas naujas hash:

`c470162fee5864cc15d3b3702b0fc4741a9e33dd594fc2c04a6ab59a2fd7c33d`

Gautas patikrinimas:

```text
Hash pasikeitė: True
```

Pakeitus nonce, pasikeitė ir visos bloko antraštės SHA256d rezultatas. Tai parodo, kad net nedidelis antraštės pakeitimas sukuria visiškai kitokią maišos reikšmę.

Šis eksperimentas yra antraštės maišos atkūrimo patikra, o ne viso bloko galiojimo arba Proof-of-Work patvirtinimas.

### C dalies išvada

Sėkmingai atkurta 80 baitų Bitcoin bloko antraštė iš dekoduotų Bitcoin Core laukų. Atkurtas HEX sutapo su originalia antrašte. Apskaičiuota SHA256d maiša taip pat sutapo su Bitcoin Core pateiktu bloko hash. Pakeitus nonce reikšmę, gauta kita maiša.

Tai patvirtina, kad bloko antraštė buvo teisingai serializuota, o SHA256d skaičiavimas atliktas pagal Bitcoin protokolo taisykles.

---

# 5. Skaičiavimų tikslumas ir klaidų apdorojimas

Piniginėms sumoms naudojamas `Decimal` tipas, o ne dvejetainis `float`. Tai leidžia išvengti netikslių dešimtainių skaičių apvalinimo klaidų sumuojant BTC reikšmes.

Programose naudojamas klaidų apdorojimas: nepavykus prisijungti prie Bitcoin Core RPC arba įvykus kitai vykdymo klaidai, pateikiamas klaidos pranešimas.

Transakcijos ir bloko analizėje taip pat naudojamos numatytosios reikšmės tais atvejais, kai tam tikro lauko nėra, pavyzdžiui, kai skripto adreso laukas nepateikiamas.

Ankstesnių transakcijų išvesčių vertės nustatomos pagal TXID ir VOUT indeksą. Jau išleistoms išvestims `gettxout` metodas gali negrąžinti informacijos, todėl istorinių įvesčių vertei nustatyti naudojama ankstesnė transakcija arba išsaugoti prevout duomenys.

Coinbase transakcijoms įprasta mokesčio formulė netaikoma, nes jos neturi įprastų ankstesnių transakcijų įvesčių.

---

# 6. Naudojamos technologijos ir komandos

## Technologijos

- Python 3.10.12
- Bitcoin Core 23.0
- `python-bitcoinlib` 0.12.2
- Bitcoin Core JSON-RPC
- SHA-256 kriptografinė maišos funkcija
- Ubuntu 22.04.5 LTS

## Virtualios aplinkos aktyvavimas

```bash
source .venv/bin/activate
```

## Programų paleidimas

```bash
python rpc_example.py
python rpc_transaction.py
python rpc_block.py

python check_prevout.py
python check_wallet_tx.py
python list_wallets.py

python legacy_example.py
python block_header.py
```

## Priklausomybių įdiegimas

```bash
python -m pip install -r requirements.txt
```

---

# 7. Bendros darbo išvados

1. Išanalizuota Bitcoin Core regtest tinklo informacija, nustatytas blokų skaičius, tinklo būsena ir aktyvių ryšių skaičius.

2. Išanalizuota 1 BTC mokėjimo transakcija, jos įvestis ir išvestys. Nustatyta, kaip ankstesnės transakcijos išvestis naudojama naujai transakcijai sudaryti.

3. Apskaičiuotas transakcijos mokestis yra 282 satoshi arba 0.00000282 BTC. Rezultatas sutapo su MIF piniginės `gettransaction` metodu pateiktu mokesčiu, įvertinus neigiamą ženklą. Faktinis mokesčio tarifas yra 2 sat/vB.

4. Palyginus Legacy ir SegWit transakcijas nustatyta, kad SegWit naudoja atskirą Witness duomenų dalį. Dėl to skiriasi TXID ir Wtxid, o virtualus transakcijos dydis ir svoris gali būti mažesni.

5. Atkurta 80 baitų bloko antraštė tiksliai sutapo su Bitcoin Core pateiktu HEX. Apskaičiuotas SHA256d hash sutapo su bloko hash. Pakeitus nonce, maišos reikšmė pasikeitė.

**Bendra išvada:** laboratorinio darbo metu praktiškai išanalizuota Bitcoin regtest aplinka, transakcijų struktūra, mokesčio skaičiavimas, Legacy ir SegWit skirtumai bei bloko antraštės maišos sudarymas. Gauti rezultatai patikrinti naudojant Bitcoin Core RPC metodus ir savarankiškus Python skaičiavimus.

## Testų failai

-`test_hex_and_byte_order.py` Netaisyklingas HEX ir baitų tvarka -`test_regtest.py` Vietinis regtest, CLI ir Python prieiga prie to paties mazgo

- `test_payment_confirmations.py` Mokėjimo patvirtinimai
- `test_transaction_accounting.py` Įvestys, išvestys, gavėjas, grąža ir mokestis
- `test_prevout_errors.py` Neprieinama istorija ir išvestis be adreso
- `test_block_header.py` 80 baitų antraštė, HEX, SHA256d ir nonce

## Automatizuotas testavimas

Projekto funkcionalumui ir duomenų analizės teisingumui patikrinti naudojama `pytest` biblioteka. Testai suskirstyti į atskirus failus pagal tikrinamą funkcionalumą. Taip galima nepriklausomai patikrinti Bitcoin transakcijų, blokų antraščių, RPC ryšio ir HEX duomenų apdorojimo veikimą.

Testai paleidžiami projekto virtualioje aplinkoje komanda:

```bash
python -m pytest -v
```

### 7.1. Testų grupės

**1. `test_hex_and_byte_order.py` – HEX formato ir baitų tvarkos testai**

Tikrinama:

- Ar teisingai apdorojamas 64 simbolių HEX identifikatorius (TXID).
- Ar 32 baitų seka teisingai apverčiama, kai reikia vidinės Bitcoin baitų tvarkos.
- Ar atmetamos netinkamos HEX reikšmės.
- Ar aptinkamos per trumpos reikšmės ir netinkamo tipo duomenys.

Visi 5 testai praėjo sėkmingai. Tai patvirtina, kad HEX duomenų tikrinimas ir baitų tvarkos keitimas veikia pagal numatytą logiką.

**2. `test_regtest.py` – Regtest aplinkos ir RPC ryšio testai**

Tikrinama:

- Ar prisijungiama prie `regtest` tinklo.
- Ar Bitcoin Core versija atitinka naudojamą 230000 versiją.
- Ar Python RPC ir `bitcoin-cli` pasiekia tą patį mazgą.
- Ar abiem būdais gaunama vienoda blokų grandinės informacija.

Visi 3 testai praėjo. Tai patvirtina, kad naudojama tinkama laboratorinė aplinka ir RPC ryšys veikia.

**3. `test_payment_confirmations.py` – Transakcijos patvirtinimų testai**

Tikrinama:

- Ar laboratorijoje sukurta mokėjimo transakcija randama blokų grandinėje.
- Ar transakcijos TXID sutampa su nurodytu identifikatoriumi.
- Ar transakcija turi bent vieną patvirtinimą.
- Ar transakcija susieta su numatytu bloku ir turi bloko laiko žymą.

Abu testai praėjo sėkmingai. Tai patvirtina, kad analizuojama transakcija yra įtraukta į bloką ir turi patvirtinimų.

**4. `test_transaction_accounting.py` – Transakcijos verčių ir mokesčio testai**

Tikrinama:

- Ar visų transakcijos išvesčių suma yra 49,99999718 BTC.
- Ar gavėjo adresui priskirta 1 BTC išvestis.
- Ar teisingai apskaičiuojamas transakcijos mokestis pagal įvesties ir išvesčių verčių skirtumą.
- Ar virtualus dydis ir svoris atitinka gautus duomenis.

Trys iš keturių testų praėjo. Nepraėjo `test_transaction_fee`, nes testui nepavyko per RPC gauti ankstesnės transakcijos duomenų. Bitcoin Core grąžino klaidą, kad ankstesnės transakcijos paieškai reikalingas `txindex` arba konkretaus bloko hash. Alternatyvus `gettransaction` metodas taip pat nebuvo pasiekiamas per naudojamą bendrą RPC adresą, nes jam reikalingas konkrečios piniginės RPC kelias.

Svarbu, kad ši klaida susijusi su duomenų gavimo apribojimu, o ne su įrodyta neteisinga mokesčio skaičiavimo formule. Mokestis buvo apskaičiuotas ir patikrintas atskiroje analizėje: įvestis – 50 BTC, išvestys – 49,99999718 BTC, mokestis – 0,00000282 BTC (282 satoshi).

Virtualus dydis taip pat patikrintas: 141 vB, transakcijos svoris – 561 WU.

**5. `test_prevout_errors.py` – Ankstesnių išvesčių ir klaidų apdorojimo testai**

Tikrinama:

- Ar programa tinkamai apdoroja išvestį, kurioje nėra adreso.
- Ar programa nestringa, kai trūksta `scriptPubKey` duomenų.
- Ar tinkamai atpažįstama neegzistuojanti arba nepasiekiama transakcija.

Visi 3 testai praėjo. Tai rodo, kad tikrinami pagalbiniai duomenų apdorojimo atvejai ir numatytos klaidos nesukelia netikėto programos veikimo.

**6. `test_block_header.py` – Bloko antraštės ir SHA256d testai**

Tikrinama:

- Ar atkurtos bloko antraštės ilgis yra tiksliai 80 baitų.
- Ar pagal atkurtą antraštę apskaičiuotas SHA256d hash sutampa su Bitcoin Core pateiktu bloko hash.
- Ar dvigubo SHA-256 funkcija grąžina 32 baitų rezultatą.
- Ar pakeitus nonce reikšmę pasikeičia apskaičiuotas bloko hash.

Visi 4 testai praėjo. Tai patvirtina, kad antraštės laukų serializavimas, baitų tvarka ir hash apskaičiavimas veikia teisingai.

### 7.2. Bendri testavimo rezultatai

| <escape>                         | Testų failas | Testų skaičius | Praėjo    |
| -------------------------------- | -----------: | -------------: | --------- |
| `test_hex_and_byte_order.py`     |            5 |              5 |
| `test_regtest.py`                |            3 |              3 |
| `test_payment_confirmations.py`  |            2 |              2 |
| `test_transaction_accounting.py` |            4 |              3 |
| `test_prevout_errors.py`         |            3 |              3 |
| `test_block_header.py`           |            4 |              4 |
| **Iš viso**                      |       **21** |         **20** | </escape> |

<text weight="semibold">Galutinis rezultatas: 20 iš 21 testų praėjo sėkmingai (95,2 %).</text>

### 7.3. Testavimo išvados

Testavimo metu patikrintas Bitcoin Core RPC ryšys, `regtest` tinklo naudojimas, transakcijų duomenų analizė, HEX reikšmių apdorojimas, išvesčių verčių tikrinimas ir bloko antraštės hash atkūrimas.

Dauguma testų praėjo sėkmingai. Vienintelė nesėkmė susijusi su ankstesnės transakcijos duomenų gavimu per RPC sąsają. Šis apribojimas užfiksuotas ir paaiškintas, o transakcijos mokestis papildomai patikrintas atskirai.

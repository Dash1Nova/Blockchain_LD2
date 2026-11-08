# Blockchain_LD3

# Bitcoin Core mazgas ir Regtest laboratorija

## Vykdymo eiga

Kadangi neturiu pakankamai vietos Bitcoin blokų grandinei saugoti savo kompiuteryje, darbui naudojau Vilniaus universiteto suteiktus Linux serverio resursus. Prisijungiau dviem SSH etapais. Prisijungus, terminale buvo naudojama `Ubuntu 22.04.5 LTS aplinka`.

1. Patikrinau įdiegtos Bitcoin Core programos ir jos RPC kliento versijas.
   **Komanda:**

```bash
bitcoin-cli --version
```

**Gautas rezultatas:**

```text
Bitcoin Core RPC client version v23.0.0
```

**Komanda:**

```bash
bitcoind --version
```

**Gautas rezultatas:**

```text
Bitcoin Core version v23.0.0
```

Taigi serveryje naudojama Bitcoin Core 23.0.0 versija.

2. Patikrinau serveryje pasiekiamo Bitcoin mazgo informaciją pagrindiniame tinkle (mainnet).

**Komanda:**

```bash
bitcoin-cli getblockchaininfo
```

<img width="1307" height="471" alt="Ekrano kopija 2026-11-08 223012" src="https://github.com/user-attachments/assets/197c6b68-c2e0-4cf9-8dee-fe2c4ebd9b81" />

**Pagrindiniai gauti duomenys:**

| Parametras           |            Reikšmė |
| -------------------- | -----------------: |
| chain                |               main |
| blocks               |             975088 |
| headers              |             975088 |
| difficulty           |  148364561003867.4 |
| verificationprogress | 0.9999968140378798 |
| initialblockdownload |              false |
| size_on_disk         | 891087205568 baitų |
| pruned               |              false |
| warnings             |            tuščias |

Šis mazgas jau buvo beveik visiškai sinchronizuotas su pagrindiniu Bitcoin tinklu. Jo duomenų katalogas užima apie 891 GB. Pagrindinius monetų ir piniginių bandymus atlikau ne mainnet, o regtest tinkle, kad nereikėtų atsisiųsti visos pagrindinio tinklo blokų grandinės.

3. Darbui naudojau atskirą duomenų katalogą:

```bash
~/bitcoin-regtest-mif
```

Jame saugoma regtest blokų grandinė ir piniginės. Kadangi dirbau bendrame universiteto Linux kompiuteryje, naudojau užduoties A priede nurodytą atskirų prievadų schemą. Man paskirtas numeris N = 11.

Patikrinau konfigūracijos failą:

```bash
grep -E '^(regtest|\[regtest\]|rpcport|port|prune)=' "$HOME/bitcoin-regtest-mif/bitcoin.conf"
```

**Gautas rezultatas:**

```ini
regtest=1
rpcport=19011
port=20011
```

Taip pat patikrinau veikiančius Bitcoin Core procesus:

```bash
ps -eo pid,args | grep '[b]itcoind'
```

Mano mazgo procesas:

```text
42371 bitcoind -regtest -datadir=/home/user11/bitcoin-regtest-mif -port=20011 -rpcport=19011 -daemon
```

Tai patvirtina, kad regtest mazgas veikia atskirame kataloge ir naudoja A priede numatytus prievadus `20011` ir `19011`.

4. Patikrinau regtest mazgo būseną:

**Komanda:**

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" getblockchaininfo
```

<img width="1551" height="437" alt="Ekrano kopija 2026-11-08 223112" src="https://github.com/user-attachments/assets/46c69007-b5a0-431c-aa47-1d4f86faf3c4" />

**Pagrindiniai gauti duomenys:**

| Parametras           | Reikšmė |
| -------------------- | ------- |
| chain                | regtest |
| blocks               | 102     |
| headers              | 102     |
| verificationprogress | 1       |
| initialblockdownload | false   |
| pruned               | false   |
| warnings             | tuščias |

Mazgo grandinės būsena patvirtino, kad naudojamas regtest tinklas.

5. Patikrinau tinklo informaciją:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" getnetworkinfo
```

<img width="976" height="533" alt="Ekrano kopija 2026-11-08 223156" src="https://github.com/user-attachments/assets/c22bca07-a366-4981-ae80-b6dccf1ffb8d" />
<img width="397" height="525" alt="Ekrano kopija 2026-11-08 223239" src="https://github.com/user-attachments/assets/c9d19706-279c-44df-9a26-b4c9012fc594" />

**Pagrindiniai rezultatai:**

| Parametras      | Reikšmė          |
| --------------- | ---------------- |
| version         | 230000           |
| subversion      | /Satoshi:23.0.0/ |
| protocolversion | 70016            |
| networkactive   | true             |
| connections     | 0                |
| connections_in  | 0                |
| connections_out | 0                |
| relayfee        | 0.00001000       |
| warnings        | tuščias          |

IPv4 ir IPv6 tinklai buvo pasiekiami, tačiau išorinių mazgų ryšių nebuvo.

6. Patikrinau ryšių skaičių:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" getconnectioncount
```

Rezultatas: `0`

Tai reiškia, kad mano regtest mazgas tuo metu neturėjo prisijungusių kitų mazgų.

7. Patikrinau esamas pinigines.

**Komanda:**

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" listwallets
```

**Rezultatas:**

```json
["MIF", "student"]
```

Papildomai patikrinau piniginių katalogą:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" listwalletdir
```

**Rezultatas:**

```json
{
  "wallets": [{ "name": "student" }, { "name": "MIF" }]
}
```

<img width="1552" height="456" alt="Ekrano kopija 2026-11-08 223446" src="https://github.com/user-attachments/assets/d585d7a1-fba9-484f-b065-76b9513faf4d" />

Taigi laboratorijoje naudojamos dvi piniginės: `MIF` ir `student`.

8. Patikrinau MIF piniginės balansą:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" -rpcwallet=MIF getbalances
```

<img width="1545" height="208" alt="Ekrano kopija 2026-11-08 223520" src="https://github.com/user-attachments/assets/7e1bf1c7-17aa-43ff-a1b5-f8d473a499cd" />

**Rezultatas:**

```json
{
  "mine": {
    "trusted": 98.99999718,
    "untrusted_pending": 0.0,
    "immature": 5000.00000282
  }
}
```

Patikrinau student piniginės balansą:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" -rpcwallet=student getbalances
```

<img width="1551" height="227" alt="Ekrano kopija 2026-11-08 223550" src="https://github.com/user-attachments/assets/c6b679c2-5697-4a8d-a050-a70c256f18f6" />

**Rezultatas:**

```json
{
  "mine": {
    "trusted": 1.0,
    "untrusted_pending": 0.0,
    "immature": 0.0
  }
}
```

MIF piniginėje buvo sukaupti kasimo atlygiai, o student piniginėje – 1 BTC.

9. Patikrinau student piniginės transakcijų istoriją:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" -rpcwallet=student listtransactions "*" 10
```

<img width="1541" height="528" alt="Ekrano kopija 2026-11-08 223617" src="https://github.com/user-attachments/assets/5e522c5c-fa2f-4f0d-ae30-a7d32264fe50" />

**Pagrindiniai rezultatai:**

| Parametras    | Reikšmė                                                          |
| ------------- | ---------------------------------------------------------------- |
| category      | receive                                                          |
| amount        | 1 BTC                                                            |
| label         | payment                                                          |
| confirmations | 1                                                                |
| blockheight   | 102                                                              |
| txid          | 6e81edf620e0f413960991b6e57cc38ac5195316533fdf4ac8844da573ef02b0 |
| address       | bcrt1qcy7fa33eguuluhdunr3n0pc5ny8c79c6lvnxkv                     |

Student piniginė gavo 1 BTC, o transakcija buvo patvirtinta 102 bloke.

10. Naudojau `getrawtransaction`, kad gaučiau neapdorotą transakcijos duomenų eilutę.

**Komanda:**

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" getrawtransaction 6e81edf620e0f413960991b6e57cc38ac5195316533fdf4ac8844da573ef02b0 false 5242bf8d90e2b8bd729e095dafcc4297c1e79c522a5a1464631a60681f9cbfcc
```

Gauta neapdorota transakcijos HEX eilutė.

Tuomet ją iškodavau:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" decoderawtransaction "02000000000101514be61234ef0025e2ef453d228a24a687ced7bbae52ebf88bdd54719adefed50000000000fdffffff02e60f102401000000160014fb7f33238532807281e2bcd3113a307c1c6a6c7300e1f50500000000160014c13c9ec6394739fe5dbc98e3378714990f8f171a0247304402201480985248fb9e28e8b6ba7a8902ba792b1f66a28f0810909ef81ae864f99ca7022068c87ea1e170d1b27dba95d317b37694c59214c935ba814adc62a22d6279e110012103dbcf0c380db458d3d3d9b16895b2a0c202ed93e193b762c7ee0910c11e38857665000000"
```

<img width="1116" height="535" alt="Ekrano kopija 2026-11-08 223650" src="https://github.com/user-attachments/assets/c89c9146-8a97-4c3b-b712-3f9d1222b5f6" />
<img width="855" height="535" alt="Ekrano kopija 2026-11-08 223931" src="https://github.com/user-attachments/assets/0b773520-6801-494f-938f-f538237237d3" />

**Pagrindiniai iškodavimo rezultatai:**

| Parametras        | Reikšmė                                                          |
| ----------------- | ---------------------------------------------------------------- |
| txid              | 6e81edf620e0f413960991b6e57cc38ac5195316533fdf4ac8844da573ef02b0 |
| version           | 2                                                                |
| size              | 222 baitai                                                       |
| vsize             | 141 vB                                                           |
| weight            | 561                                                              |
| locktime          | 101                                                              |
| įvesčių skaičius  | 1                                                                |
| išvesčių skaičius | 2                                                                |

Transakcijos išvestys:

| Išvestis |            Suma | Adresas                                      |
| -------- | --------------: | -------------------------------------------- |
| 0        | 48.99999718 BTC | bcrt1qldlnxgu9x2q89q0zhnf3zw3s0swx5mrnz7vmdz |
| 1        |           1 BTC | bcrt1qcy7fa33eguuluhdunr3n0pc5ny8c79c6lvnxkv |

Pirmoji išvestis yra grąža, o antroji – 1 BTC gavėjo mokėjimas.

Taip pat patikrinau pradinę 50 BTC kasimo transakciją:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" -rpcwallet=MIF gettransaction d5fede9a7154dd8bf8eb52aebbd7ce87a6248a223d45efe22500ef3412e64b51 true
```

<img width="1156" height="527" alt="Ekrano kopija 2026-11-08 224006" src="https://github.com/user-attachments/assets/346ecab8-809b-4331-83ec-2a0c11b24af3" />

Gautas atlygis buvo 50 BTC, o transakcija turėjo 102 patvirtinimus.

Pagal įvesties ir išvesčių sumas apskaičiuojamas mokestis:

```text
50 - 48.99999718 - 1 = 0.00000282 BTC
```

Tai atitinka 282 satoshi mokestį.

11. Kad galėčiau parodyti transakcijos būseną prieš patvirtinimą ir po jo, atlikau papildomą bandomąjį pervedimą (mempool bandymas).

Pirmiausia patikrinau mempool:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" -rpcport=19011 getmempoolinfo
```

<img width="1547" height="295" alt="Ekrano kopija 2026-11-08 224713" src="https://github.com/user-attachments/assets/ddfc8872-ce12-4af5-8bba-d4f501da5bf2" />

Rezultatas prieš naują mokėjimą:

```text
size: 0
bytes: 0
total_fee: 0.00000000
```

Patikrinau student piniginės balansą:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" -rpcport=19011 -rpcwallet=student getbalances
```

<img width="1558" height="222" alt="Ekrano kopija 2026-11-08 224742" src="https://github.com/user-attachments/assets/91b73cb0-93e3-4926-bb36-b9470ee822ad" />

Turimas patvirtintas balansas buvo 1 BTC.

Sukūriau naują gavėjo adresą:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" -rpcport=19011 -rpcwallet=student getnewaddress "mempool-test"
```

Gautas adresas:

```text
bcrt1q9frt2lnc0gy0hfm2h269lpkdntuwu7nh4nz6na
```

Iš MIF piniginės išsiunčiau 1 BTC:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" -rpcport=19011 -rpcwallet=MIF -named sendtoaddress address="bcrt1q9frt2lnc0gy0hfm2h269lpkdntuwu7nh4nz6na" amount=1 fee_rate=2
```

Gautas TXID:

```text
3716ee85a8f72bceca29dbc9fb7e117e46a96672895318d0238d8e1eb9009994
```

12. Patikrinau mempool:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" -rpcport=19011 getmempoolinfo
```

<img width="1555" height="362" alt="Ekrano kopija 2026-11-08 224815" src="https://github.com/user-attachments/assets/f8a26ded-56ca-4f11-a2ee-bac9a0d6da8b" />

Rezultatas:

```json
{
  "size": 1,
  "bytes": 141,
  "total_fee": 0.00000282,
  "unbroadcastcount": 1
}
```

Patikrinau transakciją student piniginėje:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" -rpcport=19011 -rpcwallet=student gettransaction 3716ee85a8f72bceca29dbc9fb7e117e46a96672895318d0238d8e1eb9009994
```

<img width="1300" height="536" alt="Ekrano kopija 2026-11-08 225034" src="https://github.com/user-attachments/assets/5e5bf31e-196f-4ee4-8b2f-b8c9660f4083" />

Pagrindiniai rezultatai:

```text
amount: 1.00000000
confirmations: 0
trusted: false
```

Tai patvirtino, kad transakcija dar nebuvo įtraukta į bloką ir buvo mempool.

13. Naujo bloko kasimas. Sukūriau MIF piniginės kasimo adresą:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" -rpcport=19011 -rpcwallet=MIF getnewaddress "mempool-mining"
```

Gautas adresas:

```text
bcrt1q9ga8sw69tvgch3hxurc4rdpc9wh0s6qnlknfek
```

Iškasiau vieną naują bloką:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" -rpcport=19011 -rpcwallet=MIF generatetoaddress 1 bcrt1q9ga8sw69tvgch3hxurc4rdpc9wh0s6qnlknfek
```

Gautas bloko hash:

```text
5bc841b916211793d4c8d2d91c909a37c19b72baf7a3915800f8669ca83b9433
```

14. Būsena po patvirtinimo. Pakartotinai patikrinau transakciją:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" -rpcport=19011 -rpcwallet=student gettransaction 3716ee85a8f72bceca29dbc9fb7e117e46a96672895318d0238d8e1eb9009994
```

<img width="1148" height="526" alt="Ekrano kopija 2026-11-08 225600" src="https://github.com/user-attachments/assets/c9e5f887-28e5-4631-b172-fe824db09656" />

Pagrindiniai rezultatai:

```text
amount: 1.00000000
confirmations: 1
blockheight: 103
blockhash: 5bc841b916211793d4c8d2d91c909a37c19b72baf7a3915800f8669ca83b9433
```

Dar kartą patikrinau mempool:

```bash
bitcoin-cli -regtest -datadir="$HOME/bitcoin-regtest-mif" -rpcport=19011 getmempoolinfo
```

<img width="1550" height="297" alt="Ekrano kopija 2026-11-08 225633" src="https://github.com/user-attachments/assets/05f92cd0-b486-492e-90ca-aa1077e30ab5" />

Rezultatas:

```text
size: 0
bytes: 0
total_fee: 0.00000000
```

### Mempool ir patvirtinimo palyginimas

| Rodiklis                   | Prieš bloko iškasimą | Po bloko iškasimo |
| -------------------------- | -------------------: | ----------------: |
| Mempool dydis              |                    1 |                 0 |
| Transakcijos patvirtinimai |                    0 |                 1 |
| Transakcijos suma          |                1 BTC |             1 BTC |
| Bloko aukštis              |                    – |               103 |
| Būsena                     |        Nepatvirtinta |       Patvirtinta |

Šis bandymas parodė, kad nepatvirtinta transakcija saugoma mempool, o iškasus naują bloką ji įtraukiama į blokų grandinę ir gauna patvirtinimą.

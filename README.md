# Blockchain_LD2

# v0.3

Tobulinimui pasirinktos kryptys: kodo struktūra ir testavimas.

## Kodėl būtent tokios kryptys pasirinktos?

- Kodo struktūra.
  Ankstesnėje v0.2 versijoje didžioji dalis programos logikos buvo sutelkta viename LD2.cpp faile. Dėl to buvo sunkiau atskirti skirtingas programos atsakomybes, rasti reikiamas funkcijas ir atlikti pakeitimus nepažeidžiant kitų programos dalių. Todėl nuspręsta išskaidyti programos kodą į atskirus modulius pagal jų paskirtį. Taip siekiama pagerinti kodo skaitomumą ir palengvinti tolimesnį programos plėtojimą.

- Testavimas.
  Blockchain programoje svarbu užtikrinti, kad netinkamos transakcijos ir blokai nebūtų priimami, o patvirtinti vartotojų balansai nebūtų pakeičiami nesėkmingos validacijos atveju. Todėl pasirinkta papildyti ir tobulinti automatinius GoogleTest testus. Daugiausia dėmesio skiriama transakcijų, blokų, Merkle medžio ir kandidatų tikrinimui.

## Kodo struktūra

v0.3 versijoje pradėtas programos kodo skaidymas į atskirus failus pagal jų atsakomybes.

1. Merkle medžio logikos iškėlimas
   Anksčiau Merkle medžio skaičiavimo funkcija buvo pagrindiniame LD2.cpp faile. v0.3 versijoje ji perkelta į atskirus failus:
   `merkle.h` – Merkle medžio funkcijos deklaracija.
   `merkle.cpp` – Merkle medžio skaičiavimo realizacija.

2. Blokų logikos iškėlimas
   Blokų struktūra ir su blokais susijusios funkcijos perkeltos į atskirą modulį:
   `block.h` – Block struktūra, pagrindinės konstantos ir funkcijų deklaracijos.
   `block.cpp` – bloko sukūrimo, maišos skaičiavimo, Proof-of-Work ir bloko validavimo funkcijos.
   Į šį modulį perkeltos funkcijos:
   `genesisBlock()` – sukuria pradinį genesis bloką.
   `totalSum()` – apskaičiuoja bendrą vartotojų balansų sumą.
   `blockHash()` – apskaičiuoja bloko maišą.
   `goodPow()` – patikrina Proof-of-Work sąlygą.
   `checkBlock()` – patikrina, ar blokas atitinka nustatytus reikalavimus.

3. Transakcijų logikos atskyrimas
   Transakcijos struktūra laikoma `transaction.h` faile. Šiame modulyje taip pat aprašoma transakcijų struktūros tikrinimo funkcija `structureError()`.

4. Pagrindinio failo paskirtis
   Po kodo išskaidymo LD2.cpp faile paliekama pagrindinė programos vykdymo logika:

- Vartotojų generavimas.
- Transakcijų generavimas.
- Transakcijų fondo sudarymas.
- Blokų kandidatų kūrimas.
- Kasimo proceso vykdymas.
- Galutinių rezultatų išvedimas.

## Testavimas

v0.3 versijoje naudojami automatiniai GoogleTest testai, kuriais tikrinamas transakcijų, balansų, blokų, Merkle medžio ir kandidatų validavimas.
Iš viso parašyti 27 automatiniai testai, suskirstyti į 7 grupes.

1. Galiojančio mokėjimo testas (1 testas)

- `ValidPaymentAccepted` – tikrinama, ar teisinga transakcija priimama, ar teisingai atnaujinami siuntėjo ir gavėjo balansai ir ar bendra vartotojų lėšų suma išlieka nepakitusi.

2. Netinkamų transakcijų testai (7 testai)
   Tikrinama, ar programa atmeta netinkamas transakcijas ir pateikia konkrečią klaidos priežastį:

- `UnknownReceiver` – atmetama transakcija su nežinomu gavėju.
- `UnknownSender` – atmetama transakcija su nežinomu siuntėju.
- `ZeroAmount` – atmetama transakcija, kurios suma lygi nuliui.
- `NegativeAmount` – atmetama transakcija su neigiama suma.
- `SameSenderAndReceiver` – atmetama transakcija, kai siuntėjas ir gavėjas yra tas pats vartotojas.
- `TamperedTransactionId` – atmetama transakcija, kurios duomenys pakeisti, bet ID neatnaujintas.
- `ValidTransactionHasNoError` – tikrinama, ar teisingai transakcijai klaidos pranešimas nepateikiamas.

3. Balanso viršijimo testai (2 testai)

- `TwoPaymentsExceedingBalanceRejected` – tikrinama, ar atmetamas blokas, kuriame esančių dviejų transakcijų bendra suma viršija siuntėjo turimą balansą.
- `FirstOfTwoPaymentsAloneIsValid` – tikrinama, ar pirmoji transakcija gali būti priimta atskirai, jeigu siuntėjui pakanka lėšų.

4. Dublikatų testai (2 testai)

- `DuplicateIdInsideBlockRejected` – tikrinama, ar blokas atmetamas, kai jame ta pati transakcija pateikta du kartus.
- `ReplayOfIncludedTransactionRejected` – tikrinama, ar jau patvirtinta transakcija negali būti pakartotinai įtraukta į grandinę.

5. Blokų validavimo testai (7 testai)
   Tikrinama, ar programa atmeta blokus, kurių duomenys arba parametrai yra neteisingi:

- `ChangedTransactionContentRejected` – atmetamas blokas, kai pakeičiamas transakcijos turinys.
- `ChangedTxRootRejected` – atmetamas blokas, kai pakeičiama Merkle šaknis.
- `ChangedPrevHashRejected` – atmetamas blokas, kai neteisingas prev_hash.
- `InvalidProofOfWorkRejected` – atmetamas blokas, neatitinkantis Proof-of-Work reikalavimo.
- `WrongDifficultyRejected` – atmetamas blokas su netinkamu sudėtingumo lygiu.
- `EmptyBlockRejected` – atmetamas blokas, kuriame nėra transakcijų.
- `RejectedBlockDoesNotChangeConfirmedState` – tikrinama, ar atmestas blokas nepakeičia patvirtintų vartotojų balansų ir įtrauktų transakcijų ID.

6. Merkle medžio testai (6 testai)
   Tikrinamas Merkle šaknies apskaičiavimas esant skirtingam transakcijų skaičiui:

- `Empty` – tikrinama tuščio Merkle medžio šaknis.
- `OneTransactionRootIsItsId` – tikrinama, ar vienos transakcijos ID tampa Merkle šaknimi.
- `TwoTransactions` – tikrinamas dviejų transakcijų Merkle šaknies apskaičiavimas.
- `ThreeTransactionsDuplicatesLast` – tikrinama, ar esant trims transakcijoms paskutinis mazgas dubliuojamas.
- `FiveTransactions` – tikrinamas penkių transakcijų Merkle medžio sudarymas.
- `OrderMatters` – tikrinama, ar pakeitus transakcijų eiliškumą pasikeičia Merkle šaknis.

7. Pralaimėjusio kandidato testas (1 testas)

- `LosingCandidateRejectedAfterWinnerAccepted` – tikrinama, ar priėmus vieną kandidatą kitas kandidatas, sukurtas pagal ankstesnę grandinės būseną, yra atmetamas.

Visi 26 GoogleTest testai sėkmingai praėjo.

# Bendrai apie visą darbą

## Programos paleidimas

Projektui sukurti ir paleisti naudojamas CMake.

1. Pirmiausia iš pagrindinio projekto katalogo sukuriamas build katalogas: `mkdir build`, `cd build`
2. Tada sukonfigūruojamas projektas: `cmake ..`
3. Programa sukompiliuojama: `cmake --build .`
4. Po kompiliavimo programa paleidžiama: `Debug/LD2.exe`; testų failą galima paleisti įvedus komandą `Debug/tests.exe`

## Modelis

Transakcijos ID apskaičiuojamas iš laukų sender|receiver|amount|sequence. Laukai atskiriami simboliu |. Tekstas koduojamas UTF-8 formatu ir perduodamas maišos funkcijai. Bloko maiša apskaičiuojama iš jo antraštės laukų nustatyta tvarka, o transakcijų turinys su antrašte susiejamas per Merkle šaknį. Pradinė grandinės būsena apibrėžiama fiksuotu genesis bloku. Jo ankstesnio bloko maiša yra [įrašyti reikšmę], laiko žyma – [įrašyti reikšmę], versija – [įrašyti reikšmę], o tuščio turinio Merkle šaknis apskaičiuojama pagal tuščios eilutės maišą. Genesis blokas naudojamas kaip pradinis grandinės taškas.

## Atkuriamumas

Programos atsitiktiniams duomenims generuoti naudojamas `std::mt19937` generatorius. Naudojamos pradinės reikšmės (`seed`):

- Vartotojų generavimas: `100`.
- Transakcijų generavimas: `111`.
  Pagal šias reikšmes sugeneruojama `1000 vartotojų` ir `10 000 transakcijų`. Pradinių balansų suma apskaičiuojama prieš pradedant vykdyti transakcijas ir palyginama su galutine suma. Blokų kasimui naudojamas `difficulty = 3`. Viename bloke gali būti `iki 100 transakcijų`. v0.2 kandidatų kasimui naudojami nustatyti bandymų biudžetai: `10 000`, `20 000`, `40 000` ir `80 000` bandymų.

- Vykdymo aplinka:
  Operacinė sistema: Windows.
  C++ standartas: 17.
  Kompiliavimo optimizavimo parametrai: nėra.

## Įrodymai

- Projekto struktūra:
<img width="385" height="537" alt="Ekrano kopija 2026-11-04 232251" src="https://github.com/user-attachments/assets/e57b9aeb-8936-406e-bc89-917299a32941" />

- Pirmojo bloko išvestis:
<img width="687" height="268" alt="Ekrano kopija 2026-11-04 233041" src="https://github.com/user-attachments/assets/677e608b-31dc-415e-a6b5-51e8c71fb6e5" />

- Galutinė programos suvestinė:
<img width="701" height="728" alt="Ekrano kopija 2026-11-04 233447" src="https://github.com/user-attachments/assets/73a7c0e5-0b96-4926-bc2f-aa21f42f9028" />

- Automatinių testų rezultatai:
<img width="686" height="736" alt="Ekrano kopija 2026-11-04 233532" src="https://github.com/user-attachments/assets/c3155e83-ff8e-4196-9294-c65f8f6d1d3f" />

## 4. v0.2 ir v0.3 palyginimas

| Sritis                     | v0.2                                                     | v0.3                                                                          |
| -------------------------- | -------------------------------------------------------- | ----------------------------------------------------------------------------- |
| **Programos struktūra**    | Daug logikos pagrindiniame faile                         | Logika išskaidyta į atskirus modulius                                         |
| **Transakcijų validacija** | Įgyvendinta                                              | Išsaugota ir patikrinta automatizuotais testais                               |
| **Blokų validacija**       | Įgyvendinta                                              | Atskirtas validavimo kodas ir papildyti bloko validavimo automatizuoti testai |
| **Merkle medis**           | Įgyvendinta                                              | Išsaugota ir patikrintas veiksmas 6 testais                                   |
| **Automatizuoti testai**   | Įgyvendinti pradiniai funkcionalumo ir validavimo testai | 26 testai iš viso įgyvendinti                                                 |
| **UTXO modulis**           | Nėra                                                     | Sukurtas atskiras modulis ir 6 testai                                         |
| **Vykdymo laikas**         | 238.565 s                                                | 238.565 s                                                                     |

## DI darbo žurnalas

- Įrankis: ChatGPT.
  Modelis: GPT-6.
  Data: 2026-10-25.
  Kryptys: pradinės programos struktūros analizė, transakcijų ir blokų logikos tikslinimas, pagrindinių validavimo principų peržiūra.

- Įrankis: ChatGPT.
  Modelis: GPT-6.
  Data: 2026-11-01.
  Kryptys: Merkle medžio realizacijos ir validavimo logikos peržiūra, testų aprėpties plėtimas, blokų kandidatų tikrinimo analizė.

- Įrankis: ChatGPT.
  Modelis: GPT-6.
  Data: 2026-11-04.
  Kryptys: kodo struktūros tobulinimas ir atsakomybių atskyrimas į atskirus modulius, automatinių testų aprėpties bei jų aiškumo gerinimas, papildomo UTXO modelio realizavimas ir testų analizė.

- Įrankis: ChatGPT.
  Modelis: GPT-6.
  Data: 2026-11-01 ir 2026-11-04.
  Kryptys: testų aprėptis ir kodo struktūra.

# Blockchain_LD2

## Hash funkcijos versijos pasirinkimas

Šiam laboratoriniam darbui nusprendžiau naudoti iš ankstesnio darbo hash funkcijos DI versiją, o ne savo sukurtą pradinę versiją šių priežasčių:
• Pirmo darbo eksperimentų eigoje mano hash funkcija parodė labai prastus rezultatus, kai tikrinau kolizijų atsparumą. Skirtingo ilgio įvesčių rinkiniuose buvo užfiksuota labai daug kolizijų (10 baitų ilgio įvestis generavo 81219 kolizijų, o 1000 baitų ilgio įvestis - 80088 kolizijų). Tai gali turėti neigiamos įtakos šio darbo transakcijų unikalumo užtikrinimui.
• Pirmo darbo mano sukurta hash funckija prastai pasirodė ir lavinos efekto eksperimente. Ties 10 baitų įvesties ilgiu bitų skirtumo vidurkis buvo apie 0,78%, ir didėjant įvesties ilgiui lavinos efektas praktiškai išnykdavo. Bendras bitų skirtumo vidurkis buvo 0.23%; hex skirtumo vidurkis - 0.49% (lyginant su teoriniu reikalavimu, kad pakeitus vieną įvesties simbolų, išvestis turi pasikeisti apie 50%). Jei tęsčiau darbą su savo sukurta funkcija, tai turėtų neigiamos įtakos PoW įgyvendinimui.

## Paleidimas

Projektui sukurti ir paleisti naudojamas CMake.

Pirmiausia iš pagrindinio projekto katalogo sukuriamas build katalogas: `mkdir build`, `cd build`
Tada sukonfigūruojamas projektas: `cmake ..`
Programa sukompiliuojama: `cmake --build .`
Po kompiliavimo programa paleidžiama: `Debug/LD2.exe`

## Lambda išraiškų naudojimas

Funkcijoje runLoop() naudojamos lambda išraiškos, kad galima būtų aprašyti pagalbines operacijas neužteršiant kodą dar papildomomis funkcijomis. Su lambdomis susijusi logika laikoma vienoje funkcijoje `runLoop()` ir kodas tampa lengviau skaitomas.

Naudojamos lambda išraiškos:
• `totalSum` – bendrai vartotojų lėšų sumai apskaičiuoti.
• `blockHash` – bloko maišai apskaičiuoti.
• `calcTxRoot` – transakcijų šakninei maišai apskaičiuoti.
• `goodPow` – Proof of Work sąlygai patikrinti.
• `structureError` – transakcijos klaidoms nustatyti.

## Modelio aprašymas

Kiekvienas vartotojas turi vardą, unikalų viešąjį raktą (public_key) ir pinigų likutį (balance). Programa sugeneruoja 1000 vartotojų, kurių pradinės lėšos yra nuo 100 iki 1000000.
Transakcijos struktūra aprašo pinigų pervedimą iš vieno vartotojo kitam. Ji saugo siuntėją, gavėją, sumą, eilės numerį (sequence) ir unikalų transakcijos ID (transaction_id). ID apskaičiuojamas maišant siuntėjo, gavėjo, sumos ir eilės numerio duomenis.
Transakcija priimama tik tada, kai jos duomenys tinkami ir siuntėjas turi pakankamai lėšų. Įvykdžius pervedimą, siuntėjo likutis sumažėja, o gavėjo padidėja ta pačia suma.
Bloko struktūra saugo nuorodą į ankstesnio bloko maišą (prev_hash), laiko žymą (timestamp), versiją (version), transakcijų šakninę maišą (tx_root), skaičių nonce, sudėtingumo lygį (difficulty) ir transakcijų sąrašą.
Kiekvieno bloko maiša apskaičiuojama pagal jo antraštės laukus. Blokai susiejami į grandinę, nes kiekvienas naujas blokas saugo ankstesnio bloko maišą.
Programa sugeneruoja 10 000 transakcijų ir saugo jas laukiančių transakcijų fonde. Iš fondo atrenkama iki 100 tinkamų transakcijų vienam blokui. Prieš įtraukiant bloką patikrinamos transakcijos ir ar vartotojas turi pakankamai lėšų pervesti norimą sumą.
Naudojamas Proof of Work metodas: didinamas nonce, kol bloko maiša prasideda trimis nuliais. Blokas įtraukiamas į grandinę tik tada, kai patikra sėkminga.
Sistema nenaudoja komisinių ar atlygių už blokų kasimą. Todėl, jei visi pervedimai atliekami teisingai, bendra visų vartotojų lėšų suma išlieka nepakitusi.

## Generavimo seed

• Vartotojų generavimo seed: `100`.
• Transakcijų generavimo seed: `111`.
• Transakcijų pasirinkimo seed: `222`.
• Sugeneruojama 1000 vartotojų ir 10 000 transakcijų.
• Transakcijos suma: nuo 1 iki 1000.
• Viename bloke gali būti iki 100 transakcijų.

## Vieno bloko išvestis (pirmas blokas)

Bloko aukstis: `1`
Transakcijų skaičius: `100`
prev_hash: `000c5dd430a95139c42a27e4e7023f6b6a9e8b01cdc3072a45dbc90d92071546`
tx_root: `f23abe9e14ec154b7fcf6f779e6bce1bb50872fa707e6442e4326c7d9685153d`
Bloko hash: `000e21bf65308ec02e8c66fb706e5b1f7d98b42df70a61b3bad1257ed3bea487`
difficulty: `3`
nonce: `1852`
Bandymų skaičius: `1853`
Kasimo laikas: `0.0787519 s`

## Galutinė suvestinė

Vartotoju skaičius: 1000
Priimta blokų (be genesis): 101
Itrauktų transakcijų skaičius: 9993
Atmestų transakcijų skaičius: 0
Liko fonde transakcijų: 7
Pradine lėšų suma: 491084187
Galutine lėšų suma: 491084187
Vykdymo laikas: 28.9073 s

## Mažo testo rezultatai

Transakcijų maišos paleidžiant kelis kartus sutampa. Bloko hash irgi.
1 paleidimas:
Bloko aukstis: 1
transakciju skaicius: 100
prev_hash: 0003620877a89b26fcba1844a5735579dfb657c105101281985466907c339cd7
tx_root: f23abe9e14ec154b7fcf6f779e6bce1bb50872fa707e6442e4326c7d9685153d
hash: 000543bd5984235ce4a6329b8fafd805b31abb70b0fd10671548b3576a242bea
difficulty: 3
nonce: 1710

2 paleidimas:
Bloko aukstis: 1
transakciju skaicius: 100
prev_hash: 0003620877a89b26fcba1844a5735579dfb657c105101281985466907c339cd7
tx_root: f23abe9e14ec154b7fcf6f779e6bce1bb50872fa707e6442e4326c7d9685153d
hash: 000543bd5984235ce4a6329b8fafd805b31abb70b0fd10671548b3576a242bea
difficulty: 3
nonce: 1710

---

Prieš įtraukiant transakciją taip pat tikrinamas siuntėjo laikinas balansas. Tai užtikrina šios kodo eilutės:
`if (temp[t.sender] < t.amount) {
    continue;
}
temp[t.sender] -= t.amount;
temp[t.receiver] += t.amount;`
Priėmus transakciją, laikini balansai atnaujinami iš karto. Jei lėšų nepakanka, transakcija į bloką neįtraukiama. Taip pat palygintos pradinė ir galutinė bendra lėšų suma — jos sutapo (Pradine lesu suma: 491084187; Galutine lesu suma: 491084187).

---

Ši kodo dalis apsaugo nuo transakcijų dubliavimosi - ji neleisti tai pačiai transakcijai likti fonde po įtraukimo į bloką:
`for (auto& t : chosen) {
    for (size_t i = 0; i < pool.size(); i++) {
        if (pool[i].transaction_id == t.transaction_id) {
            pool.erase(pool.begin() + i);
            break;
        }
    }
}`

---

Paskutinis nepilnas blokas veikia. Programos vykdymo metu 100-ajame bloke buvo 95 transakcijos. Tai parodo, kad programa gali sudaryti blokus su mažesniu nei 100 transakcijų skaičiumi.

Paskutinio bloko išvestis:
Bloko aukstis: 100
transakciju skaicius: 95
prev_hash: 000b3faddb2b0481eb2ee08641f4c3d3cb9dabfff61a7fe57bcefe5a96792b61
tx_root: 40e8faa53c367d04dfa7e6e3bf832a4464e38c98c5e070a18cfb0d2075ee9c2d
hash: 000627138d775ef4309a6fed13ae69b79ba4d664226bd73e8adbfdce7b93d705
difficulty: 3
nonce: 215
bandymu: 216
kasimo laikas: 0.0276275 s

# Blockchain_LD2

# v0.2

## Programos vykdymo eiga

1. Pradinių duomenų paruošimas

- Sukuriama 1000 netikrų vartotojų su atsitiktiniais pradiniais balansais (nuo 100 iki 1 000 000). Vartotojo viešasis raktas (`publick_key`) sugeneruojamas suhashinus vartotojo vardą.
- Sukuriama 10 000 atsitiktinių transakcijų tarp šių vartotojų (pervedamos sumos nuo 1 iki 1000). Kiekvienai transakcijai suteikiamas unikalus ID.

2. Transakcijų filtravimas, mempool sudarymas

- Paimamos visos 10 000 transakcijų ir patikrinama jų pirminė struktūra (ar žinomas siuntėjas/gavėjas, ar suma teigiama, ar sutampa ID).
- Teisingos transakcijos įkeliamos į bendrą laukiančiųjų fondą (pool), o klaidingos – atmetamos į rejected sąrašą.

3. Bloko kandidatų generavimas

- Kol fonde yra transakcijų, pradedamas naujo bloko kūrimo ciklas.
- Sukuriami 5 skirtingi bloko kandidatai. Kiekvienas kandidatas iš fondo pasiima iki 100 transakcijų, kurias siuntėjai išgali apmokėti.
- Kiekvienam kandidatui apskaičiuojama jo transakcijų `merkle root`.

4. Kasimo (Proof-of-Work) simuliacija

- Pradedamas kasimo ratas su pradiniu 10 000 bandymų. Jei bloko rasti nepavyksta, kitame rate bandymų skaičius dvigubinamas (iki 20k, 40k, 80k).
- Visi 5 kandidatai iš eilės keičia savo nonce skaičių ir ieško hash kodo, kuris prasidėtų reikiamu nulių skaičiumi (pagal difficulty, kuris šiuo atveju yra 3).

5. Laimėtojo patvirtinimas ir grandinės pratęsimas

- Pirmasis kandidatas, sėkmingai atlikęs Proof-of-Work, keliauja į galutinę patikrą (`checkBlock`). Čia dar kartą centralizuotai peržiūrima, ar nėra dvigubo išleidimo klaidų ir ar teisingi visi balansai.
- Blokas oficialiai prijungiamas prie grandinės, o jame buvusios transakcijos ištrinamos iš laukiančiųjų fondo (pool). Vartotojų balansai atnaujinami.

6. Rezultatų išvedimas

- Ciklas kartojasi, kol fonde nebelieka tinkamų transakcijų arba pasiekiama kasimo bandymų riba.
- Konsolėje išspausdinama statistika apie kiekvieną bloką, kandidatą ir bendra suvestinė.

## Merkle medis

Medžio lapai yra transakcijų ID jų įtraukimo į bloką tvarka. Vidiniai mazgai apskaičiuojami sujungiant kairiojo ir dešiniojo vaikų dekoduotus baitus ir pritaikant maišos funkciją.
Jeigu tam tikrame lygyje lieka neporinis mazgas, jis sujungiamas su savo kopija.
Jeigu bloke yra tik viena transakcija, jos ID tampa Merkle šaknimi. Tuščio genesis bloko Merkle šaknis apskaičiuojama atskirai - tiesiog tuščia maiša.

Testų su 1, 2, 3 ir 5 transakcijomis rezultatas:

```text
1 transakcija: 15206b9f696aacacef4c7d460a11818d1d5a1300b8eee97d47a4afbd0dc5eb64
2 transakcijos: 4415e0901a92e2d6c948ebc996166700dd30ef5cb63af22293f9caaf3e4f71fe
3 transakcijos: 6a9f89e475aa416fdcaba557181a8552cc7fb73f22ad1668e06a6c958ad18490
5 transakcijos: 4f5cdb1870442fe6629bd51545cfda3cfbf9e9c7781851b33f8e0deb0d15d424
```

## Transakcijų validavimas

Prieš įtraukiant transakcijas į fondą tikrinama:

- Ar siuntėjas ir gavėjas egzistuoja.
- Ar siuntėjas ir gavėjas nėra tas pats vartotojas.
- Ar transakcijos suma yra teigiama.
- Ar transakcijos ID atitinka jos duomenis.
- Ar toks ID dar nėra fonde.
  Vėliau, prieš sudarant blokus, papildomai tikrinama, ar transakcija dar nėra įtraukta į grandinę.
  Sudarydama kandidato bloką programa tikrina, ar siuntėjas turi pakankamai lėšų. Jei jų nepakanka, transakcija į tą bloką neįtraukiama.

## Blokų validavimas

Prieš įtraukiant bloką į grandinę tikrinama:

- Ar prev_hash sutampa su paskutinio grandinės bloko maiša.
- Ar bloko versija yra palaikoma.
- Ar sudėtingumo lygis atitinka nustatytą reikšmę.
- Ar bloke yra nuo 1 iki 100 transakcijų.
- Ar transakcijos yra tinkamos ir jų ID nesikartoja.
- Ar siuntėjams pakanka lėšų ir neperpildomi balansai.
- Ar bloko Merkle šaknis sutampa su apskaičiuota šaknimi.
- Ar bloko maiša atitinka Proof of Work reikalavimą.
  Tik visas patikras praėjęs blokas įtraukiamas į grandinę. Prieš įtraukimą balansai tikrinami laikinoje kopijoje, todėl netinkamo bloko atveju patvirtinti balansai nepakeičiami.

## Kandidatų konkurencija

Kiekviename etape sukuriami 5 kandidatai. Visi jie naudoja tą patį transakcijų fondą, pradinius balansus ir ankstesnio bloko maišą.
Kiekvienam kandidatui sudaromas atskiras transakcijų sąrašas ir laikina balansų kopija. Viename bloke gali būti ne daugiau kaip 100 transakcijų.
Kandidatai kasami tokia tvarka:

- 1 etapas – 10 000 bandymų.
- 2 etapas – papildomi 20 000 bandymų.
- 3 etapas – papildomi 40 000 bandymų.
- 4 etapas – papildomi 80 000 bandymų.
  Po kiekvieno etapo tikrinama, ar kandidatas rado tinkamą bloką. Nurodyti 10 000, 20 000, 40 000 ir 80 000 bandymų skaičiai yra papildomi kasimo bandymai. Tai nėra privalomas bandymų skaičius. Jei kandidatas randa tinkamą bloką anksčiau, kasimas baigiamas. Laimėjo pirmasis tinkamas kandidatas. Prieš įtraukiant jo bloką į grandinę, jis dar kartą patikrinamas.

## Kandidatų bandymų ir laiko suvestinė

Žemiau pateikiami pirmojo programos paleidimo rezultatai.
| Kandidatas | Bandymų skaičius | Kasimo laikas |
|-----|-----|-----|
| 1 | 1135 | 0.0504065 s |
| 2 | 1135 | 0.0482412 s |
| 3 | 1135 | 0.0414935 s |
| 4 | 1135 | 0.0422126 s |
| 5 | 1135 | 0.0426729 s |

Kandidatas 1 laimėjo, nes pagal programos logiką kandidatai tikrinami nustatyta tvarka: 1, 2, 3, 4, 5. 1 kandidato blokas atitiko „Proof of Work“ reikalavimą ir praėjo bloko validaciją. 1 kandidatas laimėjo todėl, kad buvo pirmas tikrinimo eilėje ir jo blokas buvo tinkamas. Kasimo greitis čia neturi įtakos.

## Kodėl ši simuliacija nėra P2P konsensusas?

- `Teorijoje`: P2P konsensusas veikia tik tada, kai daug atskirų ir nepriklausomų mazgų bando susitarti tarpusavyje dėl vienintelės teisingos transakcijų istorijos.
  `Kode`: Jokių atskirų mazgų nėra – viską daro viena programa. Visi 5 bloko kandidatai sukasi tame pačiame cikle for (int k = 0; k < 5; k++). Tai ne nepriklausomi tinklo dalyviai, o tiesiog programos eilutės, laukiančios savo eilės.
- `Teorijoje`: Realiame tinkle informacija keliauja lėtai, pranešimai vėluoja, veikia paskalų principu.
  `Kode`: Kadangi viskas vyksta vieno kompiuterio atmintyje, nauja informacija visiems tampa žinoma akimirksniu. Čia nėra vėlavimo, neįmanoma sumodeliuoti situacijos, kai vienas mazgas apie naują bloką sužino anksčiau, o kitas – vėliau.
- `Teorijoje`: P2P tinkle nėra vieno idealaus laikrodžio, todėl mazgai negali aklai pasitikėti transakcijų laiko žymomis.
  `Kode`: Visi 5 kandidatai laiko žymą pasiima tą pačią - iš kompiuterio procesoriaus per system_clock::now(). Realybėje mazgai gyvena skirtingose laiko zonose ir jų vidiniai laikrodžiai šiek tiek skiriasi, o kode visi turi vienodą laiką.
- `Teorijoje`: Jei du mazgai vienu metu randa bloką, tinklas išsiskiria į dvi šakas ir mazgams reikia grandinės pasirinkimo taisyklės (pavyzdžiui, rinktis ilgesnę grandinę), kad vėl susijungtų.
  `Kode`: Nėra šakojimosi. Programa aklai paima patį pirmą sėkmingai kasimo užduotį įveikusį kandidatą ir iškart jį prijungia. Nėra suprogramuota galimybė išspręsti situacijos, jei du mazgai atneštų skirtingus blokus vienu metu.
- `Teorijoje`: Kiekvienas mazgas pats savarankiškai tikrina gautą bloką ir nusprendžia, tikėti juo ar ne.
  `Kode`: Viską valdo viena bendra funkcija `checkBlock`. Ji viena patikrina bloką ir iškart pakeičia pinigines visų vartotojų sąskaitas bendroje programos atmintyje. Nėra jokio individualaus mazgų „apsisprendimo“.

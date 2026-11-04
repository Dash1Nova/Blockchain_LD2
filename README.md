# Blockchain_LD2

# UTXO

Papildomai realizuotas UTXO (Unspent Transaction Output) modelis, kuriame lėšos saugomos ne vartotojų sąskaitų balansais, o nepanaudotomis transakcijų išvestimis.

## UTXO modelio veikimo aprašymas

1. **Pradinės lėšos** – funkcija `createInitialUTXOs` kiekvieno vartotojo pradinį balansą paverčia pradine UTXO išvestimi.
2. **Mokėjimo sudarymas** – funkcija `createPayment` parenka siuntėjui priklausančias nepanaudotas išvestis, kurios padengia mokėjimo sumą.
3. **Naujos išvestys** – sukuriama išvestis gavėjui, o likusi suma grąžinama siuntėjui kaip grąža.
4. **Transakcijos patvirtinimas** – funkcija `applyUTXOTransaction` patikrina, ar visos įvestys egzistuoja, priklauso siuntėjui, dar nėra panaudotos ir ar įvesčių bei išvesčių sumos sutampa.
5. **UTXO rinkinio atnaujinimas** – patvirtinus transakciją, panaudotos išvestys pašalinamos, o naujos įtraukiamos į UTXO rinkinį.

**Validavimas ir sumų išsaugojimas:**

- Atmetamos neegzistuojančios arba jau panaudotos išvestys.
- Tikrinama, ar nėra pasikartojančių įvesčių.
- Tikrinama, ar siuntėjas yra naudojamų UTXO savininkas.
- Tikrinama, ar įvesčių ir išvesčių sumos sutampa, todėl lėšos neprarandamos ir nesukuriamos.
- Funkcija `totalUTXOSum` apskaičiuoja bendrą nepanaudotų išvesčių sumą.

**## UTXO testavimas**

UTXO funkcionalumui patikrinti papildomai sukurti 6 GoogleTest testai. Visi testai sėkmingai įvykdyti (_Passed_).

1. **Pradinių UTXO sukūrimas (`InitialUTXOsCreated`)**:
   Patikrinama, ar teisingai sukuriamos pradinės nepanaudotos išvestys trims vartotojams. Taip pat tikrinamas bendras jų lėšų kiekis – 3000.
2. **Galiojantis mokėjimas ir grąža (`ValidPaymentAcceptedWithChange`)**:
   Tikrinamas 300 lėšų pervedimas iš pirmojo vartotojo antrajam. Patikrinama, ar gavėjas gauna 300, siuntėjui grąžinama likusi 700 suma ir ar bendra UTXO suma išlieka nepakitusi.
3. **Nežinoma išvestis (`UnknownOutputRejected`)**:
   Bandoma panaudoti neegzistuojančią transakcijos išvestį. Patikrinama, ar tokia transakcija atmetama ir UTXO rinkinio bendra suma nepasikeičia.
4. **Pakartotinis išleidimas (`DoubleSpendRejected`)**:
   Pirmiausia sėkmingai panaudojama viena UTXO išvestis. Tuomet bandoma dar kartą pateikti tą pačią transakciją. Patikrinama, ar pakartotinis išleidimas atmetamas ir lėšos nepakinta.
5. **Įvesčių ir išvesčių sumų neatitikimas (`ValueMismatchRejected`)**:
   Pakeičiama sukurtos transakcijos išvesties suma, kad ji nebesutaptų su įvesčių suma. Patikrinama, ar tokia transakcija atmetama ir UTXO rinkinys lieka nepakeistas.
6. **Nepakankamas lėšų kiekis (`InsufficientFundsRejected`)**:
   Bandoma atlikti 2000 lėšų pervedimą, nors siuntėjas turi tik 1000. Patikrinama, ar mokėjimas nesudaromas ir pradinė UTXO suma išlieka nepakitusi.

**Rezultatas:** visi 6 UTXO testai sėkmingai praėjo. Patvirtinta, kad sistema atmeta neegzistuojančias ir pakartotinai naudojamas išvestis, tikrina įvesčių bei išvesčių sumų atitikimą ir išsaugo bendrą lėšų kiekį atliekant galiojančius pervedimus.

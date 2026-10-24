# Blockchain_LD2

## Hash funkcijos versijos pasirinkimas

Šiam laboratoriniam darbui nusprendžiau naudoti iš ankstesnio darbo hash funkcijos DI versiją, o ne savo sukurtą pradinę versiją šių priežasčių:
• Pirmo darbo eksperimentų eigoje mano hash funkcija parodė labai prastus rezultatus, kai tikrinau kolizijų atsparumą. Skirtingo ilgio įvesčių rinkiniuose buvo užfiksuota labai daug kolizijų (10 baitų ilgio įvestis generavo 81219 kolizijų, o 1000 baitų ilgio įvestis - 80088 kolizijų). Tai gali turėti neigiamos įtakos šio darbo transakcijų unikalumo užtikrinimui.
• Pirmo darbo mano sukurta hash funckija prastai pasirodė ir lavinos efekto eksperimente. Ties 10 baitų įvesties ilgiu bitų skirtumo vidurkis buvo apie 0,78%, ir didėjant įvesties ilgiui lavinos efektas praktiškai išnykdavo. Bendras bitų skirtumo vidurkis buvo 0.23%; hex skirtumo vidurkis - 0.49% (lyginant su teoriniu reikalavimu, kad pakeitus vieną įvesties simbolų, išvestis turi pasikeisti apie 50%). Jei tęsčiau darbą su savo sukurta funkcija, tai turėtų neigiamos įtakos PoW įgyvendinimui.

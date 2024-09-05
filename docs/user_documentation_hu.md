# Aknakereső - Felhasználói Dokumentáció

## A játékszabályok

Az aknakereső egy logikai játék, amely egy aknamezőn játszódik, a játékos célja pedig a mezőn található aknák felderítése. Ebben több dolog segíti: megjelölheti az általa aknának vélt mezőket, megtudhatja a már felderített mezők szomszédságában levő aknák számát, továbbá ha üres mezőt talál, akkor az azzal összefüggő további üres mezők is fel lesznek derítve.

A program az aknakereső játék szabályrendszere alapján működik, amely a következő fő pontokból áll:
- A játék addig tart, amíg minden egyes aknát nem tartalmazó mező fel nincs fedve.
- A játéknak vége, ha a játékos aknát tartalmaző mezőre lép, vagy a beállított időkorlát lejár.
- A játékos által elsőként kiválasztott mezőn, valamint annak szomszédságában nem lehet akna.
- A mező kiválasztása az egér gombjának felengedésével válik véglegessé.
- Az egér jobb gombjával a mező megjelölhető zászlóval, vagy kérdőjellel. Utóbbi ritkán használt.

A  játékban előfordulhatnak olyan szituációk, amelyeket pusztán logikai úton nem lehet megoldani. Ezeket az eseteket a játék nem figyeli, ilyenkor sajnos a szerencsére van bízva a játék kimenetele.

## A program követelményei
A program futtatásához a végrehajtható fájlon kívül szükségesek az alábbi fájlok is:
 - `faces.png`
 - `fields.png`
 - `mine_numbers.png`
 - `numbers.png`
 
Ezeket a fájlokat a futtatható programmal megegyező mappába kell helyezni.

## A program indítása
Legcélszerűbb a programot a parancssorból (avagy terminálból) indítani, mivel így egyszerűen megszabhatóak a játék paraméterei. 

Az indítás módja:  
`minesweeper [játékosnév] [pálya_szélessége] [pálya_magassága] [aknák_száma] [időlimit]`

A paraméterek magyarázata:
 - játékosnév: A játékos neve szőköz nélkül.
 - pálya szélessége: Egy szám: az aknamezőn az oszlopok száma.
 - pálya magassága: Egy szám: az aknamezőn a sorok száma.
 - aknák száma: Egy szám: a pályán található aknák száma.
 - időlimit: Szintén egy szám: a játék kezdetétől eltelt másodpercek maximális száma.
 
Minden paraméter opcionális. Alapértelmezésben
 - játékosnév: "Anonymous",
 - szélesség: 9,
 - magasság: 9,
 - aknák száma: 10,
 - időlimit: nincs
 
**Figyelem!**  
A pálya mérete és az aknák száma nincs limitálva, ugyanakkor ha a megadott méret nagyobb, mint ami a képernyőre kiférhet, akkor a program hibával ki fog lépni.
Amennyiben az aknák száma nagyon közel kerül a pályán található mezők számához, abban az esetben az aknák generálása hosszú időt vehet igénybe.

## A játék
A játéktáblát egérrel lehet vezérelni. Mezőre kattintva a fenti szabályzatnak megfelelő esemény fog bekövetkezni.

A játéktábla közepén található fejre kattintva mindig új játék kezdődik, függetlenül attól, hogy a jelenlegi befejeződött-e.

Ha a fej napszemüveget vesz fel, akkor megnyertük a játékot. Ha viszont halott (x.x) arcot mutat, akkor sajnos vesztettünk. (Tulajodnképpen felrobbantunk az aknamezőn.)

Bal oldalt, a bal felső sarokban láthatjuk az aknák számát. Ez minden egyes lehelyezett zászlóval csökken. (0 alá nem!). Ha törlünk egy zászlót, akkor értelemszerűen nagyobb lesz eggyel.

Jobb oldalt, a jobb felső sarokban pedig az eltelt másodperceket láthatjuk. Ez folyamatosan számol a játék kezdetétől. Ha nincs beállított limit, akkor csak az eredmény mentésekor van jelentősége. Egyébként pedig a beállított limit elérésekor megállítja a játékot. Ha van beállított limit, akkor a játék elkezdése előtt a limitet mutatja.

## Eredmények
A bal felső sarokban található még egy gomb, amellyel válthatunk az eredményjelző és a játék között. Folyamatban levő játék esetén a gomb inaktív, nem lehet megnyomni. Amikor megnyílik az eredményjelző, a gomb felirata "Játék"-ra változik. Ha megnyomjuk, visszatérünk a játékhoz, és új játék indul.

A program az eredményeket a vele azonos mappában a `results.csv` fájlban tárolja, CSV formátumban. Ez szinte bármelyik táblázatkezelő szoftverrel megnyitható.

## Befejezés
A program a jobb (vagy bizonyos rendszereken bal) felső sarokban található (általában) piros (általában) x-el zárható be. Egyébként az Alt+F4 is működik.   
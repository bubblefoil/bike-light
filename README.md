# Cargo bike illumination

Cusomized bicycle RGB LED strip illumination controlled by Arduino UNO

# TODO
## Rezimy
- __Nocni:__ svitit vpredu max intenzitou (cca 6 LED z kraje), zbytek do konce nosice prichciple. To same v cervene vzadu.
- __Denni:__ To same co nocni, akorat ty prichciple nesviti vubec (stejne by to nebylo videt)
- __Parkovacky:__ sviti to prichciple cele, pricemz by bylo hezke umet menit intenzitu.
- __Blinkry:__ Max intenzivne oranzove z kraje prvnich cca 12LED prerusovane (oranzova/zhasnuto) a zbytek pri dennim nic a nocnim prichciple (bila ^ cervena)

## Rezimy 2.0

- __OFF:__ Vse 0%. Nejaka indikace?
- __Auto:__
  - Stred: 100%
  - Strany: Podle senzoru. Viz tabulka niz. Moznost menit citlivost stejne jako nastaveni jasu, tj. drzenim prepinace rezimu. Melo by stacit posouvat hranici sero/svetlo, kdy se strany vypinaji.
- __Manual:__
  - Vse podle hodnoty nastavene drzenim prepinace rezimu: 0-100%
  - 0-50%
    - Stred: 10-100%
    - Strany: Vypnute
  - 50-100%
    - Stred: 100%
    - Strany: 0-100%
- __Parking:__
  - Jas podle hodnoty nastavene drzenim prepinace rezimu: 0-100%
  - Vse sviti stejne
- __Usporny rezim:__
  - Jas podle hodnoty nastavene drzenim prepinace rezimu: 0-100%
  - Stred: Blikani (plynule pulzovani? Animace jednotlivych LED?)
  - Strany: Vypnute
  - Zmensit jas/sirku blinkru?

| Senzor    | Jas [%] |
| --------- | ------- |
| Tma       | ~1      |
| Sero      | ~25     |
| Svetlo    | 0       |

> To by bylo v prvni verzi vsechno. Couvacky, signalizace zamykani, kradeni, svetelna houkacka, vykreslovani bassu hudby apod.nechame na pozdeji ;)

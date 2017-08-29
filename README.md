# IR controller

Projekt zawiera program w języku C, który może być użyty do sterowania roletą przeciwsłoneczną. System opiera się na mikrokontrolerze STM32.

## Potrzebne elementy: <br>
STM32F407G-DISC1 <br>
roleta przeciwsłoneczna <br>
silnik dc z przekładnią 180:1 <br>
fotorezystor 10-20kΩ <br> 
odbiornik podczerwieni TSOP2236 <br>
rezystor 110Ω <br>
pilot IR NEC <br>
płytka prototypowa <br>
mostek H L293D <br>

## Podłączenie <br>
![alt text](https://d3higte790sj35.cloudfront.net/images/jv/eq/b31444264dc2138a171f760342ef9217.jpeg) <br>
Przy połączeniu fotorezystora zastosowano dzielnik napięcia. <br>
Ponadto należy podłączyć silnik do rolety. <br>

### Odbiornik podczerwieni: <br>
![alt text](http://hobby.abxyz.bplaced.net/img/art016_01.jpg) <br>
1 - PC13 <br>
2 - 3V <br>
3 - GND <br>

### Mostek H: <br>
![alt text](https://d3higte790sj35.cloudfront.net/images/bv/fk/90c0c5536d69a4162eb6145825c04c70.jpeg) <br>

## Użycie: <br>
Należy skompilować program przy użyciu środowiska CooCox IDE. <br>
![alt text](https://d3higte790sj35.cloudfront.net/images/hi/ln/2d8c418666f0866435cf3f8cc5eb30a5.jpeg) <br>
CH-  - całkowite zwinięcie rolety <br>
CH  - całkowite rozwinięcie rolety <br>
VOL-  - zwinięcie rolety do wybranej wysokości <br>
VOL+  - rozwinięcie rolety do wybranej wysokości <br>
EQ  - autopilot, całkowite zwinięcie lub rozwinięcie w zależności od natężenia światła <br>

## Autorzy: <br>
Małgorzata Sierbin i Robert Greliński

Projekt wykonano podczas zajęć laboratoryjnych Podstaw technik mikroprocesorowych prowadzonych przez Instytut Automatyki i Inżynierii Informatycznej Politechniki Poznańskiej pod nadzorem dra Marka Krafta.

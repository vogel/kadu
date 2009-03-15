INFORMACJE O MODULE

Modul oferuje integracje powiadomien Kadu z aplikacja Growl dla systemu MacOS X 
(http://growl.info/). 
Modul zostal napisany w oparciu o klase GrowlNotifier z PSI (http://psi-im.org/).
Modul zostal napisany i testwany z wykorzystaniem Growla w wersji 1.1.4.


KOMPILACJA

Zrodla modulu nalezy rozpakowac i umiescic w podkatalogu modules w zrodlach
Kadu. Nastepnie w pliku .config znajdujacym sie w glownym katalogu zrodel
Kadu zaznaczamy modul do kompilacji (ustawiajac m - modul dynamiczny).
Przed kompilacja nalezy sciagnac SDK Growla w tej samej wersji co uzywana
w systemie (np. http://growl.info/files/Growl-1.1.4-SDK.dm) z ktorego nalezy 
skopiowac katalog Growl.framework do /Libraries/Frameworks.
Teraz mozna przystapic do skompilowania Kadu a nastepnie jego instalacji.


URUCHOMIENIE

Po zainstalowaniu kadu wraz z modulem growl_notify nalezy, korzystajac z
Zarzadcy modulow zaladowac modul, klikajac dwukrotnie jego nazwe na liscie
modulow. Status modulu powinien sie zmienic na Zaladowany. 
Nastepnie w konfiguracji Kadu zaznaczamy ktore powiadomienia nas interesuja,
mozemy tez zmienic skladnie powiadomien.
Zaladowanie modulu spowoduje automatyczne zarejestrowanie Kadu w Growlu,
totez wybrane powiadomienia stana sie aktywne zaraz po zaakceptowaniu
konfiguracji.

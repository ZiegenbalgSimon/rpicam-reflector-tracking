<!--
- Einleitung (Ziel der Doku, Zielgruppe der Doku, Aufbau der Doku)
- Hardware (Software Versionen) + Verbindungen
- Erklärung Projekt (Hintergrund rpicam-apps, Erklärung der Stufen, LED, config, Programme, Test der Schnittstellen)
- Pi einrichten (von neu, Konfiguration des Betriebssystems, Tracking-Software einrichten)
- Eingerichteten Pi bedienen
- Erweiterungsmöglichkeiten
- Raspberry Pi-Grundlagen (wichtige Linux-Befehle, Bedienungsmöglichkeiten)
-->

# rpicam-reflector-tracking

Das Projekt rpicam-reflector-tracking dient dazu, mit einem **Raspberry Pi** kamerabasiert und in Echtzeit die Position eines **retroreflektiven Markers** zu verfolgen. Es stellt verschiedene **Schnittstellen** zur Übertragung der ermittelten Position zur Verfügung.

<div align="center">
<picture>
<img src="/images/camera_tripod_setup_diagonal.jpeg" alt="Kamera mit Objektiv und blau leuchtendem LED-Ring auf einem Stativ, verbunden mit einem Raspberry Pi 5" width="30%">
</picture>
</div>

Diese **Dokumentation** erklärt die Bestandteile des Projekts, und wie es auf einem Raspberry Pi genutzt werden kann. Zusätzlich werden Möglichkeiten zu Erweiterung des Projekts aufgezeigt.

> [!NOTE]
> Die Dokumentation ist darauf ausgelegt, dass das Projekt auch ohne Vorerfahrung mit Raspberry Pi genutzt werden kann.

### Gliederung der Dokumentation

- [Verwendete Hardware](#verwendete-hardware)
- [Erklärung von rpicam-reflector-tracking](#erklärung-von-rpicam-reflector-tracking)
- [Raspberry Pi einrichten mit rpicam-reflector-tracking](#raspberry-pi-einrichten-mit-rpicam-reflector-tracking)
- [Eingerichteten Raspberry Pi zum Marker Tracking nutzen](#eingerichteten-raspberry-pi-zum-marker-tracking-nutzen)
- [Möglichkeiten zur Erweiterung des Projekts](#möglichkeiten-zur-erweiterung-des-projekts)
- [Grundlagen der Arbeit mit Raspberry Pi](#grundlagen-der-arbeit-mit-raspberry-pi)

Der letzte Abschnitt soll Personen **ohne Vorerfahrung** mit Raspberry Pi den Einstieg erleichtern. Es wird in diesem Fall empfohlen, mit [Grundlagen der Arbeit mit Raspberry Pi](#grundlagen-der-arbeit-mit-raspberry-pi) zu beginnen.

## Verwendete Hardware
Das Projekt wurde für die folgende Hardware entwickelt und mit dieser getestet.

- Raspberry Pi 5, 16 GB RAM
  - Betriebssystem: Raspberry Pi OS (64-bit), veröffentlicht 04.12.2025
- Raspberry Pi Global Shutter Kamera
- 35mm Teleobjektiv, C-Mount (BerryBase CLENS-35TEL)
- Adafruit NeoPixel Ring - 24 x 5050 RGB LED mit integrierten Treibern (Durchmesser: 52,3&nbsp;mm innen / 65,5&nbsp;mm außen)
- Gehäuse mit Lüfter für Raspberry Pi 5

Auf einem anderen Raspberry Pi-Modell kann beispielsweise die Adressierung der verwendeten Schnittstellen abweichen. Bei Nutzung einer anderen Kamera ist die Sensorauflösung zu beachten.

### Verbindung der Hardware
Das **Objektiv** wird an das **Kameramodul** geschraubt, das Kameramodul beispielsweise auf einem **Stativ** befestigt. Gemäß der [offiziellen Dokumentation](https://www.raspberrypi.com/documentation/accessories/camera.html#install-a-raspberry-pi-camera) wird die Kamera über ein Flachbandkabel mit `CAM/DISP 0` auf dem Raspberry Pi verbunden.

Die **Halterung** im Ordner `stl` kann mit einem 3D-Drucker gefertigt und dazu genutzt werden, den LED-Ring am Objektiv zu befestigen.

Abhängig davon, welche Schnittstellen zur verwendet werden sollen, sind entsprechende Verbindungen herzustellen. Erklärungen zu den Schnittstellen finden sich unter [Ausgabe und Schnittstellen](#ausgabe-und-schnittstellen).

Um kabelgebundenes **Ethernet** als Schnittstelle zur Datenübertragung zu nutzen, kann ein Gerät über ein direktes Kabel mit der RJ45-Buchse des Raspberry Pis verbunden werden.

Für den LED-Ring, sowie für UART und PWM als Schnittstellen zur Ausgabe der bestimmten Position des Markers wird die **GPIO-Anschlussleiste** genutzt, wie unten dargestellt.

<div align="center">
<picture>
<img src="/images/raspberry_pi_pinout.svg" alt="GPIO-Anschlussleiste mit Verbindungen. LED-Ring: Pin 2 (5v Power), Pin 19 (GPIO 10, SPI0 MOSI), Pin 20 (Ground); UART: Pin 6 (Ground), Pin 14 (GPIO 14, UART0 TX), Pin 15 (GPIO 15, UART0 RX); PWM: Pin 12 (GPIO 18, PWM0), Pin 14 (Ground), Pin 35 (GPIO 19, PWM1)" width="75%">
</picture>
</div>

Stromversorgung und Steuerung des LED-Rings funktionieren über die GPIO-Anschlussleiste. Dazu werden `5V` und `Ground` des Rings mit den entsprechenden Pins des Raspberry Pis verbunden, sowie `IN` des LED-Rings mit `GPIO 10 (SPI0 MOSI)` der GPIO-Anschlussleiste. Da das Steuersignal mithilfe von SPI erzeugt wird, werden auch `GPIO 9 (SPI0 MISO)`, `GPIO 11 (SPI0 SCLK)` und `GPIO 8 (SPI0 CE0)` verwendet.

> [!WARNING]
> Die Leistung, die der Raspbery Pi über die GPIO-Anschlussleiste bereitstellen kann, ist begrenzt. Den Ring rot, grün oder blau bei maximaler Helligkeit zu betreiben funktioniert zuverlässig. Vor der Nutzung von Einstellungen mit höherer Leistung (z.B. helles Weiß) ist zu prüfen, ob ausreichend Leistung bereitgestellt werden kann.

Um **UART** (*Universal Asynchronous Receiver Transmitter*) als serielle Schnittstelle zur Datenübertragung zu nutzen ist ein gemeinsames Nullniveau herzustellen (z.B. an `Pin 6 (Ground)`). Zum Senden (*Transmit*) dient `GPIO 14 (UART0 TX)`, zum Empfangen (*Receive*) `GPIO 15 (UART0 RX)`.

> [!WARNING]
> Der Raspberry Pi nutzt TTL-Pegel mit 0&nbsp;V als `LOW` und +3,3&nbsp;V als `HIGH`. Den Rsapberry Pi an eine Schnittstelle anzuschließen, die andere Spannungspegel nutzt, kann zu Schäden führen.

Der Raspberry Pi verfügt über zwei Hardware-**PWM**-Kanäle (*Pulsweitenmodulation*), die an `GPIO 18 (PWM0)` und `GPIO 19 (PWM1)` ausgegeben werden. Um zu validieren, dass PWM-Signale abhängig von der Position des Markers erzeugt werden, kann die folgende einfache Schaltung angeschlossen werden. Die Helligkeit der LEDs verändert sich hierbei mit der Pulsweite, die abhängig von der Position des Markers gesetzt wird.

<div align="center">
<picture>
<img src="/images/pwm_validation_circuit.svg" alt="PWM0 und PWM1, jeweils über eine LED und einen 100 Ohm Widerstand mit Ground verbunden" width="45%">
</picture>
</div>

## Erklärung von rpicam-reflector-tracking
Im Zentrum von rpicam-reflector-tracking steht die Gewinnung von Lageinformationen mithilfe der an den Raspberry Pi angeschlossenen Kamera. Außerdem werden verschiedene Schnittstellen zur Verfügung gestellt, um die gewonnenen Informationen zu übertragen.
### Marker-Tracking in der rpicam-apps pipeline
Raspberry Pi stellt mit `rpicam-apps` einige Applikationen zur Verwendung vom Kameras zur Verfügung (siehe [Dokumentation](https://www.raspberrypi.com/documentation/accessories/camera.html#install-a-raspberry-pi-camera)).

In diesem Projekt werden konkret die folgenden Apps verwendet.

- `rpicam-hello`: zeigt ein Vorschaufenster an
- `rpicam-vid`: nimmt ein Video auf

Für die Apps stehen verschiedene Optionen zur Verfügung ([Übersicht](https://www.raspberrypi.com/documentation/computers/camera_software.html#rpicam-apps-options-reference)), die verwendeten sind hier aufgelistet.

| Option | Flag | Beschreibung |
| --- | --- | --- |
| timeout | -t   | Gib die Laufzeit an, nach der sich die App schließt (0 für unbegrenzt) |
| config | -c    | Gib eine Datei mit Konfigurationsdaten an |
| output | -o    | Gib den Dateinamen an (nicht für `rpicam-hello`) |
| post-process-libs | --post-process-libs | Gib den Speicherort der zu nutzenden Postprocessing-Stufen an |
| post-process-file | --post-process-file | Gib eine JSON-Datei an, die das Postprocessing konfiguriert |
| width | --width | Breite (Pixel) des Ausgangs (effektlos für `rpicam-hello`) |
| height | --height | Höhe des (Pixel) Ausgangs (effektlos für `rpicam-hello`) |
| viewfinder-width | --viefinder-width | Breite (Pixel) der Vorschau |
| viewfinder-height | --viewfinder-height | Höhe (Pixel) der Vorschau |
| framerate | --framerate | Bildrate |
| shutter | --shutter | Belichtungszeit in Mikrosekunden |

rpicam-apps (Apps, frame-Objekt mit Metadaten, post-processing Stufen)
Algorithmus (Schwellenwertentfernung und helligkeitsgewichtetes Zentrum)
### Ausgabe und Schnittstellen

### Projektstruktur
Erklärung der Ordner und wie sie sich referenzieren

## Raspberry Pi einrichten mit rpicam-reflector-tracking
### Raspberry Pi vorbereiten
```bash
sudo apt install -y libcamera-dev libepoxy-dev libjpeg-dev libtiff5-dev libpng-dev libopencv-dev
sudo apt install -y qtbase5-dev libqt5core5a libqt5gui5 libqt5widgets5
sudo apt install libavcodec-dev libavdevice-dev libavformat-dev libswresample-dev
```
```bash
sudo apt install -y cmake libboost-program-options-dev libdrm-dev libexif-dev
sudo apt install -y meson ninja-build
```
### rpicam-reflector-tracking einrichten
```bash
cd ~/rpicam-reflector-tracking
```
```bash
git clone https://github.com/raspberrypi/rpicam-apps.git
```
```bash
rm rpicam-apps/meson.build
```
```bash
mv new_post_processing_stages rpicam-apps/
mv meson.build rpicam-apps/
```
```bash
cd ~/rpicam-reflector-tracking/rpicam-apps
```
```bash
meson setup build
```
```bash
meson compile -C build
```
```bash
cd ~/rpicam-reflector-tracking
```
```bash
chmod +x programs/*.sh
chmod +x set_leds/*.py
```
```bash
cd ~/rpicam-reflector-tracking/interfaces
```
```bash
g++ ethernet_await_start.cpp -o ethernet_await_start
g++ ethernet_recv_byte.cpp -o ethernet_recv_byte
g++ ethernet_recv_coor.cpp -o ethernet_recv_coor
g++ ethernet_send_byte.cpp -o ethernet_send_byte
g++ ethernet_send_coor.cpp -o ethernet_send_coor
g++ pwm_clear.cpp -o pwm_clear
g++ pwm_set_coor.cpp -o pwm_set_coor
g++ uart_recv_coor.cpp -o uart_recv_coor
g++ uart_send_coor.cpp -o uart_send_coor
```

## Eingerichteten Raspberry Pi zum Marker Tracking nutzen

## Möglichkeiten zur Erweiterung des Projekts

## Grundlagen der Arbeit mit Raspberry Pi
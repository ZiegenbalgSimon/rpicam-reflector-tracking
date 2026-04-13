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
- 35&nbsp;mm Teleobjektiv, C-Mount (BerryBase CLENS-35TEL)
- Adafruit NeoPixel Ring - 24 x 5050 RGB LED mit integrierten Treibern (Durchmesser: 52,3&nbsp;mm innen / 65,5&nbsp;mm außen)
- Gehäuse mit Lüfter für Raspberry Pi 5

Auf einem anderen Raspberry Pi-Modell kann beispielsweise die Adressierung der verwendeten Schnittstellen abweichen. Bei Nutzung einer anderen Kamera ist die Sensorauflösung zu beachten.

### Verbindung der Hardware
Das **Objektiv** wird an das **Kameramodul** geschraubt, das Kameramodul beispielsweise auf einem **Stativ** befestigt. Gemäß der [Raspberry Pi-Dokumentation](https://www.raspberrypi.com/documentation/accessories/camera.html#install-a-raspberry-pi-camera) wird die Kamera über ein Flachbandkabel mit `CAM/DISP 0` auf dem Raspberry Pi verbunden.

Die **Halterung** im Ordner `stl/` kann mit einem 3D-Drucker gefertigt und dazu genutzt werden, den LED-Ring am Objektiv zu befestigen.

Abhängig davon, welche Schnittstellen verwendet werden sollen, sind entsprechende Verbindungen herzustellen. Erklärungen zu den Schnittstellen finden sich unter [Ausgabe und Schnittstellen](#ausgabe-und-schnittstellen).

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

Um **UART** (*Universal Asynchronous Receiver Transmitter*) als serielle Schnittstelle zur Datenübertragung zu nutzen, ist ein gemeinsames Nullniveau herzustellen (z.B. an `Pin 6 (Ground)`). Zum Senden (*Transmit*) dient `GPIO 14 (UART0 TX)`, zum Empfangen (*Receive*) `GPIO 15 (UART0 RX)`.

> [!WARNING]
> Der Raspberry Pi nutzt TTL-Pegel mit 0&nbsp;V als `LOW` und +3,3&nbsp;V als `HIGH`. Den Raspberry Pi an eine Schnittstelle anzuschließen, die andere Spannungspegel nutzt, kann zu Schäden führen.

Der Raspberry Pi verfügt über zwei Hardware-**PWM**-Kanäle (*Pulsweitenmodulation*), die an `GPIO 18 (PWM0)` und `GPIO 19 (PWM1)` ausgegeben werden. Um zu validieren, dass PWM-Signale abhängig von der Position des Markers erzeugt werden, kann die folgende einfache Schaltung angeschlossen werden. Die Helligkeit der LEDs verändert sich hierbei mit der Pulsweite, die abhängig von der Position des Markers gesetzt wird.

<div align="center">
<picture>
<img src="/images/pwm_validation_circuit.svg" alt="PWM0 und PWM1, jeweils über eine LED und einen 100 Ohm Widerstand mit Ground verbunden" width="45%">
</picture>
</div>

## Erklärung von rpicam-reflector-tracking
Im Zentrum von rpicam-reflector-tracking steht die Gewinnung von Lageinformationen mithilfe der an den Raspberry Pi angeschlossenen Kamera. Außerdem werden verschiedene Schnittstellen zur Verfügung gestellt, um die gewonnenen Informationen zu übertragen.
### Marker Tracking in der rpicam-apps pipeline
Raspberry Pi stellt mit `rpicam-apps` einige Applikationen zur Verwendung vom Kameras zur Verfügung (siehe [Raspberry Pi-Dokumentation](https://www.raspberrypi.com/documentation/accessories/camera.html#install-a-raspberry-pi-camera)).

In diesem Projekt werden konkret die folgenden Apps verwendet.

- `rpicam-hello`: zeigt ein Vorschaufenster an
- `rpicam-vid`: nimmt ein Video auf

Für die Apps stehen verschiedene **Optionen** zur Verfügung, die verwendeten sind hier aufgelistet. Ein vollständige Übersicht bietet die [Raspberry Pi-Dokumentation](https://www.raspberrypi.com/documentation/computers/camera_software.html#rpicam-apps-options-reference).

| Option | Flag | Angabe |
| --- | --- | --- |
| `timeout` | `-t`   | Laufzeit, nach der sich die App schließt (0 für unbegrenzt) |
| `config` | `-c`    | Datei mit Konfigurationsdaten |
| `output` | `-o`    | Name der erzeugten Datei (nicht für `rpicam-hello`) |
| `post-process-libs` | `--post-process-libs` | Speicherort der zu nutzenden Postprocessing-Stufen |
| `post-process-file` | `--post-process-file` | JSON-Datei, die das Postprocessing konfiguriert |
| `width` | `--width` | Breite (Pixel) des Ausgangs (effektlos für `rpicam-hello`) |
| `height` | `--height` | Höhe (Pixel) des Ausgangs (effektlos für `rpicam-hello`) |
| `viewfinder-width` | `--viefinder-width` | Breite (Pixel) der Vorschau |
| `viewfinder-height` | `--viewfinder-height` | Höhe (Pixel) der Vorschau |
| `framerate` | `--framerate` | Bildrate |
| `shutter` | `--shutter` | Belichtungszeit in Mikrosekunden |

Der folgende Befehl startet für 30 Sekunden ein Vorschaufenster mit einer Belichtungszeit von 100 Mikrosekunden.

```bash
rpicam-hello -t 30s --shutter 100
```

Die Bestimmung der Markerposition sowie deren Darstellung und Übertragung über verschiedene Schnittstellen werden in Form von **Postprocessing-Stufen** in die `rpicam-apps`-Pipeline implementiert. Die für dieses Projekt relevante Struktur von `rpicam-apps` und insbesondere die Rolle von Postprocessing-Stufen ist hier dargestellt.

<div align="center">
<picture>
<img src="/images/post_processing_pipeline.svg" alt="Strukturdiagramm. rpicam-apps steuert die Kamera, die Frames an rpicam-hello oder rpicam-vid gibt. Innerhalb dieser können Postprocessing-Stufen (aus rpicam-apps und extern) hinzugefügt werden, die Bildmatrix und Metadaten verändern. Die Ausgabe erfolgt auf ein Vorschaufenster und bei rpicam-vid zusätzlich in eine Datei." width="90%">
</picture>
</div>

**Postprocessing-Stufen** können der `rpicam-apps`-Pipeline hinzugefügt werden, um Stufe nach Stufe die Bildmatrix und die Metadaten jedes Frames analysieren und modifizieren zu können ([Raspberry Pi-Dokumentation](https://www.raspberrypi.com/documentation/computers/camera_software.html#post-processing-with-rpicam-apps)). Die zu verwendenden Postprocessing-Stufen werden in einer JSON-Datei angegeben, welche mithilfe der Option `post-process-file` an die entsprechende Applikation übergeben wird.

> [!IMPORTANT]
> Die Auflösung der an die Postprocessing-Stufen übergebenen Frames hängt von der Ausgabegröße für das Vorschaufenster beziehungsweise die Datei ab, die mit den oben aufgeführten Optionen konfiguriert werden können.

Zusätzlich zu den in `rpicam-apps` enthaltenen Postprocesing-Stufen können gemäß [Raspberry Pi-Dokumentation](https://www.raspberrypi.com/documentation/computers/camera_software.html#write-your-own-post-processing-stages) **weitere Stufen** in *C++* geschrieben und zur Pipeline hinzugefügt werden. Dazu wird der entsprechenden Applikation mithilfe der Option `post-process-libs` der Speicherort der kompilierten zusätzlichen Postprocessing-Stufen übergeben.

Die `.cpp`-Dateien der zusätzlichen Postprocessing-Stufen dieses Projektes befinden sich im Ordner `new_post_processing_stages`.

Von diesen neuen Stufen ist `"marker_tracking_cv"` dafür zuständig, aus jedem Frame die Position des retroreflektiven Markers zu bestimmen. Dazu werden in einem ersten Schritt alle Pixel, deren Helligkeit unter einem **Schwellenwert** liegt, in der Bild-Matrix auf 0 gesetzt. Anschließend wird das **helligkeitsgewichtete Zentrum** der verbleibenden Pixel berechnet. Die so bestimmte Position wird in die **Metadaten** des Frames geschrieben, um von nachfolgenden Postprocessing-Stufen genutzt zu werden. `"marker_tracking_cv"` nutzt die Bibliothek [OpenCV](https://docs.opencv.org/4.x/d1/dfb/intro.html).

### Ausgabe und Schnittstellen

Das Projekt `rpicam-reflector-tracking` enthält Postprocessing-Stufen, mit denen die durch `"marker_tracking_cv"` bestimmte Markerposition visualisiert und über verschiedene Schnittstellen übertragen werden kann.

`"draw_centroid_cv"` liest die von `"marker_tracking_cv"` bestimmte Position aus den Metadaten des Frames aus und zeichnet an der entsprechenden Stelle der Bild-Matrix eine Markierung ein, die im Vorschaufenster beziehungsweise in der Datei zu sehen ist. `"draw_centroid_cv"` nutzt die Bibliothek [OpenCV](https://docs.opencv.org/4.x/d1/dfb/intro.html).

`"position_ethernet"` sendet die aus den Metadaten gelesene Position des Markers per Ethernet an die IP-Adresse `192.168.1.2`. Außerdem prüft `"position_ethernet"`, ob per Ethernet Daten empfangen wurden. Wenn die `STOP`-Flag `0xFF` empfangen wurde, beendet `"position_ethernet"` die laufende Kameraapplikation, zum Beispiel `rpicam-hello`.

> [!IMPORTANT]
> Dem Empfänger in dem lokalen Netzwerk ist die IP-Adresse `192.168.1.2` zuzuordnen, um Daten per Ethernet empfangen zu können.

`"position_cout"` überträgt die Markerposition aus den Metadaten als Text an die Standardausgabe `cout`. Diese wird typischerweise auf das Terminal ausgegeben, von dem aus eine Applikation mit `"position_cout"` als Postprocessing-Stufe gestartet wurde.

`"position_uart"` sendet die Markerposition aus den Metadaten über eine serielle UART-Schnittstelle mit einer Baudrate von 115200. Teil der Übertragung ist ein Synchronisationsbyte `0xAA`, die genaue Struktur der Datenübertragung ist in dem entsprechenden Code dokumentiert.

Die von den Postprocessing-Stufen `"position_ethernet"`, `"position_cout"` und `"position_uart"` ausgegebene Position hat das Format `(FLOOR(x * 10), FLOOR(y * 10))`, wobei `x` und `y` die Position des Markers in Pixeln abbilden. So können Ganzzahlen verwendet und trotzdem, zugunsten der Auflösung, die erste Nachkommastelle der Pixelposition erhalten werden.

`"position_pwm"` setzt den Tastgrad der Hardware-PWM-Kanäle 0 und 1 des Raspberry Pis abhängig von der Pixelposition des Markers, wobei `PWM0` die horizontale Position und `PWM1` die vertikale Position repräsentiert. Der Tastgrad wird konkret auf `x / frame_width` beziehungsweise `y / frame_height` gesetzt. Durch Verwendung der mittleren Spannung innerhalb eines Zeitintervalls oder durch Pulsdauermessung kann die Positionsinformation aus dem PWM-Signal zurückgewonnen werden.

**Liste der Postprocessing-Stufen**

| Bezeichnung | Konfigurierbare Parameter (default) |
| --- | --- |
| `"marker_tracking_cv"` | `"threshold"`&nbsp;(150) |
| `"draw_centroid_cv"` | `"radius"`&nbsp;(16), `"thickness"`&nbsp;(2) |
| `"position_ethernet"` | `"ip"`&nbsp;("192.168.1.2"), `"port"`&nbsp;(8080) |
| `"position_cout"` | / |
| `"position_uart"` | / |
| `"position_pwm"` | `"period"`&nbsp;(100000), `"frame_width"`&nbsp;(1456), `"frame_height"`&nbsp;(1088) |

### Projektstruktur

Nachdem [rpicam-reflector-tracking einrichten](#rpicam-reflector-tracking-einrichten) befolgt wurde, liegt auf dem Raspberry Pi die folgende Ordnerstruktur vor.

```text
rpicam-reflector-tracking/
├── configuration/
│   ├── configuration_post_processing/
│   └── configuration_camera.txt
├── images/
├── interfaces/
├── programs/
├── rpicam-apps/
│   ├── build/
│   ├── new_post_processing_stages/
│   ├── meson.build
│   └── ...
├── set_leds/
├── stl/
└── README.md
```

`programs/` enthält *Shell*-Skripte, die mit verschiedenen Ausgabeformen jeweils die Position des retroreflektiven Markers bestimmen. Zum Teil erfüllen sie weitere Funktionen. Die verschiedenen Programme sind unter [Eingerichteten Raspberry Pi zum Marker Tracking nutzen](#eingerichteten-raspberry-pi-zum-marker-tracking-nutzen) erklärt. Dazu wird `rpicam-hello` oder `rpicam-vid` aufgerufen und durch entsprechende [Optionen](#marker-tracking-in-der-rpicam-apps-pipeline) konfiguriert, wobei andere Dateien des Projektes referenziert werden.

`rpicam-apps/build/` wird durch die Option `post-process-libs` referenziert, da sich hier die kompilierten zusätzlichen Postprocessing-Stufen befinden. Um `rpicam-apps/build/` zu erzeugen, werden die Postprocessing-Stufen in `new_post_processing_stages/` mithilfe der `meson.build`-Datei kompiliert (siehe [rpicam-reflector-tracking einrichten](#rpicam-reflector-tracking-einrichten)). Alternativ könnte `rpicam-apps` gemäß der [Raspberry Pi-Dokumentation](https://www.raspberrypi.com/documentation/computers/camera_software.html#build-libcamera-and-rpicam-apps) mit den zusätzlichen Postprocessing-Stufen neu gebaut und installiert werden, die hier gewählte Methode erlaubt aber eine schnellere Iteration bei Anpassung der Postprocessing-Stufen.

`configuration_camera.txt` wird der Option `config` übergeben, um Videoeigenschaften der Kamera einzustellen, die für alle Programme in `programs/` gleich sind. Die JSON-Dateien im Ordner `configuration_post_processing/` werden durch die Option `post-process-file` referenziert. Sie konfigurieren, welche Postprocessing-Stufen genutzt werden.

`set_leds` enthält *Python*-Dateien, um den LED-Ring zu steuern. Dazu wird die im Ordner enthaltene Bibliothek [Pi5Neo](https://github.com/vanshksingh/Pi5Neo/tree/main) genutzt. Die Steuerung funktioniert erfahrungsgemäß, obwohl der Raspberry Pi 3,3&nbsp;V-Logk nutzt, der LED-Ring aber 5&nbsp;V-Logik. `clear_leds.py` deaktiviert die LEDs. `set_leds` setzt die alle LEDs des Rings auf eine festgelegte Farbe. Die Farbe kann mit den Flags `--r`, `--g` und `--b` (jeweils 0 bis 255) in der folgenden Form gesetzt werden.

```bash
./set_leds.py --r 70 -- g 30
```

Im Ordner `stl/` befindet sich ein Modell der Halterung, die zur mechanischen Verbindung des LED-Rings mit dem Objektiv genutzt wird. `README.md` und `images/` dienen dieser Dokumentation.

`interfaces` enthält *C++*-Programme, mit denen die Schnittstellen Ethernet, UART und PWM getestet werden können. So können Signale gesendet und auch empfangen werden. Insbesondere können diese Programme auch auf einem zweiten Raspberry Pi genutzt werden, um die über Ethernet oder UART gesendeten Positionsdaten zu empfangen zu verwenden, oder um den zum Marker Tracking genutzten Raspberry Pi über Ethernet zu steuern.

**Programme in `interfaces`**

| Datei | Funktion | Vorgesehenes Gerät |
| --- | --- | --- |
| `ethernet_await_start` | Wartet, bis die `START`-Flag `0xAA` per Ethernet empfangen wird | Gerät mit Kamera |
| `ethernet_recv_byte` | Empfängt ein Byte per Ethernet und gibt es aus (`cout`) | Gerät mit Kamera |
| `ethernet_recv_coor` | Empfängt Positionen per Ethernet und gibt sie aus (`cout`) | Empfänger/Steuerungsgerät |
| `ethernet_send_byte` | Sendet ein Byte per Ethernet (auch für `START`- und `STOP`-Flag) | Empfänger/Steuerungsgerät |
| `ethernet_send_coor` | Sendet eine Position per Ethernet (Testzwecke) | Gerät mit Kamera |
| `pwm_clear` | Deaktiviert die beiden Hardware-UART-Kanäle des Raspberry Pis | Gerät mit Kamera |
| `pwm_set_coor` | Setzt den Tastgrad der beiden Hardware-PWM-Kanäle | Gerät mit Kamera |
| `uart_recv_coor` | Empfängt Positionen per UART und gibt sie aus (`cout`) | Empfänger/Steuerungsgerät |
| `uart_send_coor` | Sendet eine Position per UART (Testzwecke) | Gerät mit Kamera |

## Raspberry Pi einrichten mit rpicam-reflector-tracking

Voraussetzung für die Nutzung von rpicam-reflector-tracking ist ein ein Raspberry Pi mit dem Betriebssystem Raspberry Pi OS. Dieses kann gemäß [Raspberry Pi Dokumentation](https://www.raspberrypi.com/documentation/computers/getting-started.html#installing-the-operating-system) mithilfe des *Raspberry Pi Imagers* installiert werden. Bereits im Raspberry Pi Imager können WLAN und, abhängig von der geplanten Nutzung, [SSH](#ssh), [VNC](#vnc) und [Raspberry Pi Connect](#raspberry-pi-connect) konfiguriert werden.

### Raspberry Pi vorbereiten

Raspberry Pi OS kann auf der grafischen Benutzeroberfläche (GUI) in den Einstellungen konfiguriert werden. Falls noch nicht während der Installation geschehen, kann hier eine WLAN-Verbindung eingerichtet werden.

In den [Interface options](https://www.raspberrypi.com/documentation/computers/configuration.html#interfacing-options) können *SSH* und *VNC* aktiviert werden, falls sie genutzt werden sollen. Zur Steuerung des LED-Rings mithilfe con `Pi5Neo` muss hier *SPI* aktiviert werden. Zur Verwendung von UART zur Datenübertragung sollte *Serial Port* aktiviert und *Serial console* deaktiviert werden.

Um die zusätzlichen Postprocessing-Stufen kompilieren zu können müssen die folgenden **Abhängigkeiten und Packages** installiert werden.

```bash
sudo apt install -y libcamera-dev libepoxy-dev libjpeg-dev libtiff5-dev libpng-dev libopencv-dev
sudo apt install -y qtbase5-dev libqt5core5a libqt5gui5 libqt5widgets5
sudo apt install libavcodec-dev libavdevice-dev libavformat-dev libswresample-dev
sudo apt install -y cmake libboost-all-dev libdrm-dev libexif-dev
sudo apt install -y meson ninja-build
```

**Ethernet**

Um Ethernet zur Übertragung der Position zu nutzen, wird für den Raspberry Pi im lokalen Netzwerk `eth0` die statische IP-Adresse `192.168.1.1` gesetzt.

```bash
sudo nmcli con add type ethernet ifname eth0 con-name eth0-static ip4 192.168.1.1/24
sudo nmcli con up eth0-static
```

Ob die Adresse korrekt gesetzt wurde kann mit diesem Befehl geprüft werden.

```bash
ip a
```

Für den Empfänger ist im lokalen Netzwerk die IP-Adresse `192.168.1.2` zu setzen. Ob der Raspberry Pi zum Empfänger eine Verbindung herstellen kann, kann folgendermaßen geprüft werden.

```bash
ping 192.168.1.2
```

Falls die IP-Adresse zu einem späteren Zeitpunkt geändert werden soll, kann der folgende Befehl genutzt werden.

```bash
sudo nmcli con modify eth0-static ipv4.addresses [new_ip]/24
sudo nmcli con up eth0-static
```

**PWM**

Um die Harware-PWM-Kanäle des Raspberry Pis nutzen zu können, sind die folgenden Schritte zu befolgen.

1. Konfigurationsdatei zur Bearbeitung öffnen.

```bash
sudo nano /boot/firmware/config.txt 
```

2. Folgenden Ausdruck unten in die Konfigurationsdatei einfügen.

```text
dtoverlay=pwm-2chan
```

3. Änderung mit `Strg+O` speichern und Editor mit `Strg+X` schließen.

4. Raspberry Pi neustarten

```bash
sudo shutdown -r now
```

5. Nach korrekter Konfiguration gibt der folgende Befehl `pwmchip0` zurück.

```bash
ls /sys/class/pwm
```

### rpicam-reflector-tracking einrichten

Um rpicam-reflector-tracking einzurichten muss zunächst der **Projektordner** aus diesem Repository nach `~` des Raspberry Pis **kopiert** werden. Falls der Projektordner an anderer Stelle gespeichert wird, muss in den aufgeführten Befehlen der Dateipfad angepasst werden. Nachdem die folgenden Schritte durchgeführt wurden, kann der Raspberry Pi zum Marker Tracking eingesetzt werden.

In den Projektordner wechseln.

```bash
cd ~/rpicam-reflector-tracking
```

`rpicam-apps`-Ordner von GitHub klonen.

```bash
git clone https://github.com/raspberrypi/rpicam-apps.git
```

Ursprüngliche `meson.build`-Datei aus dem Ordner `rpicam-apps/` entfernen.

```bash
rm rpicam-apps/meson.build
```

`new_post_processing_stages/` und die neue `meson.build`-Datei in den Ordner `rpicam-apps/` verschieben.

```bash
mv new_post_processing_stages rpicam-apps/
mv meson.build rpicam-apps/
```

`new_post_processing_stages/` wurde in den Ordner `rpicam-apps/` eingebettet, damit alle in den Postprocessing-Stufen referenzierten Dateien beim Kompilieren gefunden werden. Für das Kompilieren ist die `meson.build`-Datei zuständig.

In den `rpicam-apps`-Ordner wechseln.

```bash
cd ~/rpicam-reflector-tracking/rpicam-apps
```

Kompilieren der Postprocessing-Stufen vorbereiten.

```bash
meson setup build
```

Postprocessing-Stufen kompilieren.

```bash
meson compile -C build
```

In den `rpicam-reflector-tracking`-Ordner wechseln.

```bash
cd ~/rpicam-reflector-tracking
```

Dateien in `programs/` und `set_leds/` Ausführungsberechtigungen zuweisen.

```bash
chmod +x programs/*.sh
chmod +x set_leds/*.py
```

In `interfaces`-Ordner wechseln.

```bash
cd ~/rpicam-reflector-tracking/interfaces
```

*C++*-Dateien im `interfaces`-Ordner kompilieren.

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

<a name="modifikation-von-postprocessing-stufen"></a>**Modifikation von Postprocessing-Stufen**

Die folgende Anleitung erklärt das Vorgehen, wenn die zusätzlichen Postprocessing-Stufen verändert wurden und erneut kompiliert werden sollen.

In `rpicam-apps`-Ordner wechseln.

```bash
cd ~/rpicam-reflector-tracking/rpicam-apps
```

`build`-Ordner entfernen.

```bash
rm -rf build
```

Falls zu den Postprocessing-Stufen dieses Projektes eine weitere hinzugefüngt wurde und auch kompiliert werden soll, muss die `meson.build`-Datei bearbeitet werden.

Postprocessing-Stufen kompilieren.

```bash
meson setup build
```

```bash
meson compile -C build
```

## Eingerichteten Raspberry Pi zum Marker Tracking nutzen

Nach der [Einrichtung von rpicam-reflector-tracking](#rpicam-reflector-tracking-einrichten) können retroreflektive Marker getrackt werden, wie nachfolgend beschrieben.

### Konfiguratonsdateien von rpicam-reflector-tracking

Die JSON-Dateien im Ordner `configuration/configuration_post_processing/` konfigurieren, welche Postprocessing-Stufen genutzt werden. Hier ist `configuration_ethernet.json` gezeigt.

```json
{
    "marker_tracking_cv": {
        "threshold": 150
    },
    "position_ethernet": {
        "ip": "192.168.1.2",
        "port": 8080
    },
    "draw_centroid_cv": {
        "radius": 16,
        "thickness": 2
    }
}
```

Bei Verwendung dieser JSON-Datei werden auf jeden Kameraframe die Postprocessing-Stufen `"marker_tracking_cv"`, `"position_ethernet"` und `"draw_centroid_cv"` in dieser Reihenfolge angewendet. Zusätzlich können jeweils Parameter der Stufen angepasst werden. Um eine weitere Stufe hinzuzufügen kann deren Name an der entsprechenden Stelle in der JSON-Datei hinzugefügt werden.

**Liste der Konfigurationsdateien**

| JSON-Datei | Postprocessing-Stufen |
| --- | --- |
| `configuration_cout.json` | `"marker_tracking_cv"`, `"position_cout"`, `"draw_centroid_cv"` |
| `configuration_ethernet.json` | `"marker_tracking_cv"`, `"position_ethernet"`, `"draw_centroid_cv"` |
| `configuration_pwm.json` | `"marker_tracking_cv"`, `"position_pwm"`, `"position_cout"`, `"draw_centroid_cv"` |
| `configuration_uart.json` | `"marker_tracking_cv"`, `"position_uart"`, `"draw_centroid_cv"` |

Die Postprocessing-Stufen sind in der [Erklärung von rpicam-reflector-tracking](#erklärung-von-rpicam-reflector-tracking) erklärt.

### Programme von rpicam-reflector-tracking

Wenn keine Anpassungen vorgenommen werden sollen, genügt zum Starten des Marker Trackings das Ausführen eines der Programme im `programs`-Ordner. Beispielsweise `tracking_ethernet_remote.sh` kann folgendermaßen ausgeführt werden.

In `programs`-Ordner wechseln.

```bash
cd ~/rpicam-reflector-tracking/programs
```

`tracking_ethernet_remote.sh` ausführen.

```bash
./tracking_ethernet_remote.sh
```

In der folgenden Auflistung ist angegeben, welche Postprocessing-Stufen und welche Applikation die Programme jeweils nutzen. Mit der Erklärung der [Konfiguratonsdateien von rpicam-reflector-tracking](#konfiguratonsdateien-von-rpicam-reflector-tracking) erschließen sich daraus die Eigenschaften der Programme.

| Program | Applikation | JSON-Datei |
| --- | --- | --- |
| `tracking_cout.sh` | `rpicam-hello` | `configuration_cout.json` |
| `tracking_cout_vid.sh` | `rpicam-vid` | `configuration_cout.json` |
| `tracking_ethernet.sh` | `rpicam-hello` | `configuration_ethernet.json` |
| `tracking_ethernet_remote.sh` | `rpicam-hello` | `configuration_ethernet.json` |
| `tracking_pwm.sh` | `rpicam-hello` | `configuration_pwm.json` |
| `tracking_uart.sh` | `rpicam-hello` | `configuration_uart.json` |

`tracking_ethernet_remote.sh` dient dazu, das Marker Tracking über Ethernet vom Empfänger beziehungsweise Steuerungsgerät aus steuern zu können. Wenn das Programm gestartet wird, wartet der Raspberry Pi mit Kamera, er die `START`-Flag per Ethernet empfängt. Daraufhin wird der LED-Ring aktiviert und `tracking_ethernet.sh` gestartet. Sobald die `STOP`-Flag per Ethernet empfangen wird wird das Marker Tracking beendet, der LED-Ring deaktiviert und erneut auf die `START`-Flag gewartet. 

`tracking_cout_vid.sh` erstellt im `rpicam-marker-tracking`-Ordner einen Ordner `tracking_videos/`, falls dieser noch nicht existiert. Dort werden die aufgenommenen Videos gespeichert, wobei der Aufnahmezeitpunkt für den Dateinamen genutzt wird.

## Möglichkeiten zur Erweiterung des Projekts

**Robustes Marker Tracking**

Die Postprocessing-Stufe `marker_tracking_cv` nutzt eine Schwellenwertentfernung gefolgt von der Berechnung des helligkeitsgewichteten Zentrums. Dieser Algorithmus basiert auf dem Paper

```text
Shortis, M.R., Clarke, T.A., Short, T. 1994., A comparison of some techniques for the subpixel location of discrete target images, Videometrics III. SPIE Vol. 2350. Boston. pp. 239-250.
```

Um die Markererkennung robuster zu gestalten könnte `marker_tracking_cv` erweitert werden. Nach (Shortis, M.R. et al., 1994) könnten dazu Ausreißerpixel entfernt werden, deren Helligkeit oberhalb des Schwellenwertes liegt, ohne in einer Ansammlung heller Pixel zu liegen (*blob testing*). Außerdem könnte die Geometrie der als Marker identifizierten Pixelansammlung geprüft werden (Shortis, M.R. et al., 1994). Zudem bietet [OpenCV](https://docs.opencv.org/4.x/d1/dfb/intro.html) Funktionen mit denen es mögliche wäre, Verzerrungen des Objektives zu korrigieren.

**Tracking einer Form statt eines Punktes**

Um statt der Position eines Markermitelpunktes die Form eines Objektes zu beobachten müsste eine entsprechende neue Postprocessing-Stufe programmiert und im Ordner `new_post_processing_stages/` abgelegt werden. Diese Stufe könnte beispielsweise Koeffizienten der polynomialen Mittellinie einer Form in die Metadaten des Frames schreiben. Die Übertragung dieser Information könnte äquivalent zur Übertragung in diese Projekt als zusätzliche Postprocessing-Stufe implementiert werden. Die Postprocessing-Stufen müssten [neu kompiliert werden](#modifikation-von-postprocessing-stufen). Außerdem wären eine entsprechende Konfigurationsdatei und eine *Shell*-Datei im `programs`-Ordner vorzusehen.

**Nutzung einer zweiten Kamera**

Um dreidimensionals Tracking zu ermöglichen, könnte eine zweite Kamera hizugefügt werden. Denkbar sind sowohl ein Stereo-Setup mit parallel ausgerichteten Kameras, und rechtwinklig zueinander ausgerichtete Kameras denkbar. Raspberry Pi Global Shutter Kameras können auf zwei verschiedenen Wegen synchronisiert werden:

- [Synchronisation mittels externem Trigger](https://www.raspberrypi.com/documentation/accessories/camera.html#external-trigger)
- [Software-Kamerasynchronisation](https://www.raspberrypi.com/documentation/computers/camera_software.html#software-camera-synchronisation)

Die Informationen der beiden Kameras können entweder auf dem Raspberry Pi zusammengeführt werden, oder getrennt an ein Verarbeitungsgerät übertragen werden. Bei Verarbeitung auf dem Raspberry Pi ist die Nutzung von [OpenCV](https://docs.opencv.org/4.x/d1/dfb/intro.html)-Funktionen möglich.

## Grundlagen der Arbeit mit Raspberry Pi

Dieser Abschnitt soll Personen ohne Vorerfahrung den Einstieg in die Arbeit mit Raspberry Pi erleichtern.

Raspberry Pi OS ist eine **Linux**-Distribution mit grafischer Benutzeroberfläche (GUI). Diese Dokumentation beschreibt eine *textbasierte* Bedienung über das **Terminal**. Zum Lernen von Linux bezehungsweise der Arbeit mit Command Line Interfaces (CLI) sind online viele Ressourcen verfügbar.

### Möglichkeiten zur Steuerung des Raspberry Pis

**Monitor**

Über die (micro) HDMI-Buchse kann ein Bildschirm an den Raspberry Pi angeschlossen werden. Über eine angeschlossene Maus und Tastatur kann dieser direkt gesteuert werden.

Alternativ kann eine der folgenden Methoden zur [Fernsteuerung](https://www.raspberrypi.com/documentation/computers/remote-access.html) genutzt werden. **SSH** und **VNC** erfordern, dass sich der Raspberry Pi und das zur Steuerung genutzte Gerät im gleichen WLAN befinden.

> [!TIP]
> Als gemeinsames WLAN kann auch ein mobiler Hotspot genutzt werden.

<a name="ssh"></a>**SSH**

SSH (*Secure Shell*) liefert ein Command Line Interface (**CLI**) zur Fernsteuerung des Raspberry Pis auf einem anderen Gerät. Zum Öffnen des Zugangs ist auf dem zur Steuerung genutzte Gerät ein Terminal zu öffnen (z.B. *PowerShell* auf Windows-Geräten) und ein Befehl der folgenden Form auszuführen.

```bash
ssh <username>@<hostname>.local
```

Nach Eingabe des Passworts können Befehle auf dem Raspberry Pi ausgeführt werden. Statt `<hostname>.local` kann auch die IP-Adresse genutzt werden. *PowerShell* kann auf Windows-Geräten durch Rechtsklick in einem gewünschten Ordner direkt an dieser Stelle geöffnet werden. Mehr Informationen zu **SSH** liefert die [Raspberry Pi-Dokumentation](https://www.raspberrypi.com/documentation/computers/remote-access.html#ssh).

Mit SCP (*Secure Copy Protocol*) können Dateien vom Raspberry Pi auf das zur Steuerung genutzte Gerät kopiert werden und umgekehrt ([Raspberry Pi-Dokumentation](https://www.raspberrypi.com/documentation/computers/remote-access.html#scp)). Dieser Befehl kopiert eine angegebene Datei vom Raspberry Pi in das aktuelle Arbeitsverzeichnis auf einem Windows-Gerät. `..` wird dabei in relativen Dateipfaden genutzt und meint das nächste übergeordnete Verzeichnis.

```bash
scp <username>@<hostname>.local:<file path> ..\
```

> [!IMPORTANT]
> Auf Linux-Geräten wird `/` für Dateipfade genutzt, auf Windows-Geräten `\`.

Gleichermaßen kann von einem Windos-Gerät auf den Raspberry Pi kopiert werden. Hier wird die Flag `-r` (*rekursiv*) genutzt, um einen ganzen Ordner zu kopieren.

```bash
scp -r ..\<folder> <username>@<hostname>.local:<file path>
```

<a name="vnc"></a>**VNC**

VNC (*Virtual Network Computing*) teilt den gesamten Bildschirm des Raspberry Pis und somit auch die GUI. Es erfordert die Installation eines entsprechenden Programms auf das zur Steuerung genutzte Gerät und ein von beiden Geräten genutztes Netzwerk. Weitere Informationen liefert die [Raspberry Pi-Dokumentation](https://www.raspberrypi.com/documentation/computers/remote-access.html#vnc).

<a name="raspberry-pi-connect"></a>**Raspberry Pi Connect**

Mit Raspberry Pi Connect kann auf einen Raspberry Pi auch aus einem anderen Netzwerk heraus über einen Browser zugegriffen werden. Erforderlich ist hierfür ein entsprechender **Account**. Informationen finden sich in der [Raspberry Pi-Dokumentation](https://www.raspberrypi.com/documentation/services/connect.html).

### Wichtige Befehle

Der Software des Raspberry Pis sollte regelmäßig folgendermaßen **aktualisiert** werden.

```bash
sudo apt update
sudo apt full-upgrade
```

Vor Ziehen des Steckers sollte der Raspberry Pi **heruntergefahren** werden. Über das Terminal geht das folgendermaßen.

```bash
sudo shutdown -h now
```

Dies ist der Befehl für einen **Neustart**.

```bash
sudo shutdown -r now
```

Die folgende Tabelle enthält Befehle, die für den Anwendungsfall dieses Projektes relevant sind.

| Command | Erklärung |
| --- | --- |
| `cd <directory>` | Verzeichnis wechseln (absoluter oder relativer Pfad) |
| `chmod +x <program>` | Berechtigungen ändern (`+x` fügt Ausführungsberechtigung hinzu) |
| `clear` | Terminal leeren |
| `cp` <file> <new name/path> | Datei kopieren |
| `ip a` | Netzwerkverbindungen anzeigen |
| `ls` | Inhalte des aktuellen Arbeitsverzeichnisses auflisten |
| `ls -l` | `-l`-Flag listet Berechtigungen mit auf |
| `mkdir <name>` | Verzeichnis erstellen |
| `mv` <file> <new name/path> | Datei verschieben |
| `nano <program>` | `nano`-Editor zum Bearbeiten einer Datei öffnen (`Strg+O` zum Speichern, `Strg+X` zum verlassen) |
| `ping <ip address>` | Antwortzeit eines Gerätes im Netzwerk ausgeben (wenn erreichbar) |
| `rm <file>` | Datei löschen |
| `rm -rf <folder>` | Ordner löschen |
| `sudo apt install <package>` | Package installieren |

`Strg+C` terminiert Programme, die vom Terminal aus gestartet wurden.

`sudo` (*superuser do*) wird genutzt um Befehle mit Administratorrechten auszuführen.

`*` in Dateinamen stehen stellvertretend für beliebigen Text. `*.mp4` beispielsweise meint alle MP4-Dateien im aktuellen Abeitsverzeichnis.

Viele Befehle verfügen über die `help`-Option (Flag `-h`), die eine Erklärung des Befehls und der verfügbaren Optionen ausgeben.
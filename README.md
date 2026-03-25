## Struktur
- Einleitung (Ziel der Doku, Zielgruppe der Doku, Aufbau der Doku)
- Hardware (Software Versionen) + Verbindungen
- Erklärung Projekt (Hintergrund rpicam-apps, Erklärung der Stufen, LED, config, Programme, Test der Schnittstellen)
- Pi einrichten (von neu, Konfiguration des Betriebssystems, Tracking-Software einrichten)
- Eingerichteten Pi bedienen
- Erweiterungsmöglichkeiten
- Raspberry Pi-Grundlagen (wichtige Linux-Befehle, Bedienungsmöglichkeiten)

# rpicam-reflector-tracking

Das Projekt rpicam-reflector-tracking dient dazu, mit einem **Raspberry Pi** kamerabasiert und in Echtzeit die Position eines **retroreflektiven Markers** zu verfolgen. Es stellt verschiedene **Schnittstellen** zur Übertragung der ermittelten Position zur Verfügung.

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
Das Kameramodul wird beispielsweise auf einem Stativ befestigt, das Objektiv an das Kameramodul geschraubt. Gemäß der [offiziellen Dokumentation](https://www.raspberrypi.com/documentation/accessories/camera.html#install-a-raspberry-pi-camera) wird die Kamera über ein Flachbandkabel mit ... auf dem Raspberry Pi verbunden.

## Erklärung von rpicam-reflector-tracking

## Raspberry Pi einrichten mit rpicam-reflector-tracking

## Eingerichteten Raspberry Pi zum Marker Tracking nutzen

## Möglichkeiten zur Erweiterung des Projekts

## Grundlagen der Arbeit mit Raspberry Pi
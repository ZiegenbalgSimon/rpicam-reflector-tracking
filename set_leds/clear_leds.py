#!/usr/bin/env python3

from pi5neo import Pi5Neo

# initialize the Pi5Neo class with 24 LEDs and an SPI speed of 800kHz
neo = Pi5Neo('/dev/spidev0.0', 24, 800)

# clear the strip
neo.clear_strip()
neo.update_strip()  # Commit changes to the LEDs

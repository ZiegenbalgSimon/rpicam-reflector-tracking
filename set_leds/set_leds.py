#!/usr/bin/env python3

import argparse
from pi5neo import Pi5Neo

# read in color values parsed to the program
# e.g. ./set_leds.py --r 70 -- g 30
# WARNING: GPIO header of Raspberry Pi may not be able to provide enough power for high brightness settings
parser = argparse.ArgumentParser()

parser.add_argument("--r", type=int, default=0)
parser.add_argument("--g", type=int, default=0)
parser.add_argument("--b", type=int, default=0)

args = parser.parse_args()

# initialize the Pi5Neo class with 24 LEDs and an SPI speed of 800kHz
neo = Pi5Neo('/dev/spidev0.0', 24, 800)

# set all LEDs to the requested color
neo.fill_strip(args.r, args.g, args.b)
neo.update_strip()

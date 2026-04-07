while :
do
    # wait until start flag is received via ethernet
    ../interfaces/ethernet_await_start
    # set LEDs to bright blue
    ../set_leds/set_leds.py --b 255
    # track marker and transmit position via ethernet until terminated
    ./tracking_ethernet.sh 
    # turn of LEDs
    ../set_leds/clear_leds.py
done

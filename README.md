# SX1272-Communication

These two Arduino sketches were used to measure electromagnetic attacks impacts against the LoRa (phy) protocol.

# LoRa Modulation parameters used during these measures

These parameters provide a better sensibility.

* Spreading Factor (SF): 12

* Bandwidth (BW): 125 kHz

* Coding Rate (CR): 4/5

* Center Frequency (FC): 868.3 MHz

* Explicit Header: Enabled

* Preamble Length: 12

* Payload Size: 9


## Transmitter 

Broadcasts, every 2155ms a new frame (LoRa packet) over the air.


## Receiver 

Receives and prints the last packet's RSSI and the global RSSI for every received packet.





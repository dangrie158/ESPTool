# ESPTool
## A WiFi Security Swiss Army Knife

The ESPTool is a convenient Tool to test your WiFi Security against attackers.
It can demonstrate how easy it is to crack a WiFi Password or jam your WiFi so you cant access the internet.
This Tool is designed for education and self tests.

**Be aware that it is illegal to use this device on Networks other than your own.**

_**Disclaimer** The authors of this reposity are not accountable for any action users may perform with this tool_

## Hardware

The Hardware is designed to be a relative generic and reusable Module around the ESP8266 WiFi Plattform. The Schematic is loosley based on the [NodeMCU](http://nodemcu.com) Project, however instead of breaking out all the free pins, all pins are used for a bunch of devices.

The Device has the following Hardware

* SSD1306 based OLED Display, connected via I2C
* microSD Card Socket connected over the SPI interface
* 3 general purpose buttons
* ESP8266-12E Module
* TPS63031 Buck-/Boost-Converter with an input range from 1.8V - 5.5V
* MCP73831 Single-Cell LiPo Charger Chip
* CP2012 USB to UART converter Chip

## Software
The software is of course the part where all the security features are implemented.

It still is under heavy development. A List of planned features are listed here:

* Layer 1:
	* *Since the ESPs Radio is not really configurable it is not possible to create a WiFi Jammer that works by emitting broadband noise or any other Layer 1 attacks*
* Layer 2:
	* [Deauthentification attack](https://en.wikipedia.org/wiki/Wi-Fi_deauthentication_attack)
	* collecting authentification frames and save the keys to SD card for later decryption (using a wordlist on a computer, the ESP neither has enough memory nor enough processing power to crack them on the system)
	* arp spoofing and session hijacking when connected (needs investigation)
* Layer 3 (after connecting to an AP):
	* Host discovery (IP Scanner)
	* Ping flooding (ICMP Pings)
* Layer 4 (after connecting to an AP):
	* Port Scan on Host

### Incognito Features (TBD):
* Random MAC-Address on Power Up
* Random Mac Address while performing critical Tasks

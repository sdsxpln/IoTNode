# IoT Node

(Name suggestions are welcome)

A base project IoT node for [Name of IoT project here]. Targeting ESP32 modules using [esp-idf](https://github.com/espressif/esp-idf).

## Goals

 - Implement a lightweight protocol for accessing node data 
   + [CoAP](http://coap.technology/) - Constrained Application Protocol
   + [MQTT](http://mqtt.org/) - A machine-to-machine (M2M)/"Internet of Things" connectivity protocol.
   + ...or any other suitable protocol that provides longer battery life 
 - Secure by default
   + None of that pesky "*default password*" crap
   + Take advantage of cerficates that are either geneated by users or provided by a Server/Hub  
 - Easy to configure 
   + A intuitive app on a mobile device is used to discover, adopt and configure nodes using (hopefully) Out-of-Band methods such as NFC

 ## Requirements

  - [esp-idf](https://github.com/espressif/esp-idf) - for compiling the source files

## Setup

1. Download `esp-idf`, follow it's setup instructions and configure `IDF_PATH` environment variable to have the path of `esp-idf` on your file system.

2. Ensure `PATH` environment variable includes the xtensa compiler

3. Running `make` in the project's root directory should build everyhting without any problems!

    a. There's a chance you may need be asked to set configuration defaults during `make`. That's okay! ESP-IDF is still under active development and new configuration options are expected.

## TODO 

  - Clean up project structure
  - Add unit tests for TDD
  - Create HAL to support TDD
  - Experiment with CoAP or MQTT as protocols
    + CoAP is UDP and provides a familiar API to other RESTful applications (can be standalone)
    + MQTT is a Pub/Sub protocl designed to relay all information through a broker (Server instance)
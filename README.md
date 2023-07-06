
# TODO
* Optimize measurements so all the data is stored ONLY in system variable.
* Add second IMU
* Add filtering to IMU data
* Add depth sensing
* Test control loops
* Add packet for telemetry setup
* Allow multiple telemetry links
* Add tasks stats to system variables
* Reorganize control loop so info about used PID controlers, variables etc can be stored in configuration file
* Implementation of EKF 
# Comunication protocol
Each message sended from and to TCM must follow protocol described in this section.
The bit order is LSB first.

## Packet structure


| Byte name        | value/length |
|-----------------|------|
| Header 0        | 0x69   |   
| Header 1        | 0x68   |   
| Payload length MSB |  byte  |  
| Payload length LSB |   byte |
|Message type      | byte|
| Payload|0 - 500|
|Checksum MSB|byte|
|Checksum LSB|byte|

## Checksum calculation


Cheksum is 16-bit CRC-CCITT. It is calculated with following function:
```

uint16_t calculateChecksum(uint8_t* data_p, uint16_t length)
{
    uint8_t x;
    uint16_t crc = 0xFFFF;

    while (length--){
        x = crc >> 8 ^ *data_p++;
        x ^= x>>4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
    }
    return crc;
}
```

## Message types

They are few types of message that you can send or receive with TCM. If the payload is not described it means that it length is 0.

### Message types to TCM

| Type       | value |
|-----------------|------|
| Request   | 0x0   |   
| Service   | 0x1   |   
| Control    |  0x2  |  

#### Request message
User can request data from TCM. 
| Type       | value | Service mode required|
|-----------------|------|---|
| PID   | 0x0   | No |
| Control matrix   | 0x1   | Yes|   
| Data    |  0x2  | No| 

#### Service message
These messages are used to test, set, load and save new settings.
| Type       | value |Service mode required|
|-----------------|------|--|
| Enter service mode| 0x0|  No|   
| Reboot   | 0x1   |   Yes|
| Enable direct thrusters ctrl   |  0x2  |  Yes|
| Disable direct thrusters ctrl   |  0x3  |   Yes|
| Direct motors ctrl    |  0x4  |  Yes|
| Direct motors ctrl via matrix    |  0x5  |  Yes|
|Save settings|0x6|Yes|
|Load settings from flash|0x7|Yes|
|New PID values|0x8|No|

#### Control message

The only option to control the boat is using sticks. Stick values are float big endian. It is possible but not mandatory to send up to 16 stick commands in one message. 

For now sticks are corresponding to the control inputs as follow:

    - stick 0 - roll
    - stick 1 - pitch
    - stick 2 - yaw
    - stick 3 - vertical
    - stick 4 - forward
    - stick 5 - enable thrusters

Example message for commanding the sticks:

| Byte name        | value/length |
|-----------------|------|
| Header 0        | 0x69   |   
| Header 1        | 0x68   |   
| Payload length (20) MSB |  byte  |  
| Payload length (20) LSB |   byte |
|0x2 - ctrl msg     | byte|
| stick 0 |4 bytes (float)|
| stick 1 |4 bytes (float)|
| stick 2 |4 bytes (float)|
| stick 3 |4 bytes (float)|
| stick 4 |4 bytes (float)|

|Checksum MSB|byte|
|Checksum LSB|byte|



### Message types from TCM

| Type       | value |
|-----------------|------|
| PID              |  0x3  |   
| Control matrix   |  0x4  |  
| Heart beat       |  0x5  |  
| Service confirm  |  0x6  |  
| Telemetry        |  0x7  | 
| Variable         |  0x8  | 


#### PID 



#### Heart beat 
TCM is sending heart beat packet twice a second.

#### Service confirm
After each service message received TCM is responding with this packet.

#### Telemetry


# Compiling
- Copy following lines to install vcpkg:
  - linux: '. <(curl https://aka.ms/vcpkg-init.sh -L)'
  - windows: 'iex (iwr -useb https://aka.ms/vcpkg-init.ps1)'

- Clone TMC code from github
- Open TMC folder with VSCode
- Install Embeded Tools extension
- Install CMake Tools extension
- in terminal type vcpkg activate

Thats it. 
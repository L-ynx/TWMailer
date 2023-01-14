# TWMailer
A multi-threaded client-server application in C++ to send and receive messages like an internal mail-server.

## Created by
  - Raza Ghulam
  - Skyler Mayfield




### Prerequisites
Install LDAP:

`sudo apt-get update -y`

`sudo apt-get install -y libldap2-dev`

### Building
`$ make clean && make`

### Usage
`$ bin/twmailer-server <port> <mail-spool-directoryname>`

`$ bin/twmailer-client <ip> <port>`

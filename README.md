# Matt_daemon

Matt_daemon is a school project, and as the name implies, it is a daemon (or service for windows enthousiasts). It listens on a given network port, accept incoming connections, and logs the input to a file.

## Abstract

The goal of this project was to develop a simple daemon in C++, which listens to a given port and log incoming messages into a logfile. If a signal is raised and it is catchable, it is logged too.

Verifications are made for a correct UserID, aswell as a lockfile to ensure that only one instance runs at a time.

The project contains two binaries: `matt_daemon` and `BEN_AFK`, respectively the server (or daemon) and the client, which enables the user to  connect to the daemon and communicate with it, thanks to a simple prompt.

Only 3 clients can connect simultaneously, and send commands like `system`, with which you can execute system commands, and `quit/exit`, which terminates the remote daemon, every other input is logged to a file, as mentioned above.

There is a authentication system, in which you can define a password to connect to the daemon.

The big thing on this project is the implementation of `OpenSSL`'s `AES` (Symetrical) and `RSA`  (Asymetrical) encryption, thanks to the underlying libcrypto library.
#### AES
The AES encryption implementation uses the `EVP_aes_256_cbc` cypher and a `PBKDF` (Password Based Key Derivation Function) which uses `EVP_sha256` to derive the key from the password. A custom password and salt can be defined with the makefile at compile time.
#### RSA
Encryption and decryption with asymmetric keys is computationally expensive. Typically then messages are not encrypted directly with such keys but are instead encrypted using a symmetric "session" key. This key is itself then encrypted using the public key. In OpenSSL this combination is referred to as an `envelope`. (EVP)
This project bases it's RSA encryption on OpenSSL's Certificate and PrivateKeys, which are also generated with the makefile at compile time.



 ## Dependencies and building
This project was developped and tested on debian 10.

First, make sure that you have OpenSSL's developement header installed, as we use the underlying libcrypto:
`$ sudo apt install libssl-dev`

From then, you can just simply build the project with all the default values defined.

 #### Default AES     -  `Spoiler: this is not recommended.`
This will compile the code with AES encryption, with the default `PBKDF` password `AB1g0lDP4ssW0rd` and salt `42069420`. The default authentication password will also be used: `42born2code`

`$ make`

 #### Custom AES
If you want to define your passwords yourself, you can! Use the following syntax:

`$ make pbkd="MyOwnPBKDFPassword" salt=1337 password="PasswordsOnGithubAreBad"`

This will define the AES password based key derivation password as `MyOwnPBKDFPassword` and the corresponding salt as `1337`. The password for authentication, when connecting to the daemon will be `PasswordsOnGithubAreBad`.

#### RSA
As RSA uses OpenSSL's certificates and private keys, the default behaviour is pretty safe.
You can generate them with the command:

`$ make generate`

This will generate a 2048 bit length Private key and a public certificate for the client and the daemon. Stored in `/tmp/matt_daemon`|
From then, to build the binaries with RSA support:

`$ make use=rsa`

Remember that you still can modify the authentication password!

`$ make use=rsa password="MyLoginPassword`




# Matt_Daemon
Develop a simple daemon which listens to a given port an interpret some commands.

Todo Checklist:
 - [x] Logging (Tintin_reporter)
    -  [x] Add Loglevel (Info, warning, error...)
 - [x] Daemonizing
 - [x] Signal handling
 - [x] Policies (uid, lockfile, etc...)
 - [x] Connection Manager
    - [x] Listen on port
    - [x] Accept clients
    - [x] Handle client communication
    - [x] Exit on 'quit' message
    - [x] Max 3 clients
    - [ ] Crypt communications (Verify error handling)
      - [x] AES 
      - [x] RSA
      - [x] Make PBKDF password & salt modifiable in makefile
 - [x] Client
    - [x] Basic prompt
    - [x] Pop a shell (/bin/sh)
    - [x] Authentication

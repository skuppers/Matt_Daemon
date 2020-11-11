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

 ### Default AES     -  `Spoiler: this is not recommended.`
This will compile the code with AES encryption, with the default `PBKDF` password `aP4ssW0rd` and salt `42069`. The default authentication password will also be used: `42born2code`

`$ make`

 ### Custom AES
If you want to define your passwords yourself, you can! Use the following syntax:

`$ make pbkd="MyOwnPBKDFPassword" salt="1337" password="PasswordsOnGithubAreBad"`

This will define the AES password based key derivation password as `MyOwnPBKDFPassword` and the corresponding salt as `1337`. The password for authentication, when connecting to the daemon will be `PasswordsOnGithubAreBad`.

### RSA
As this implementation of RSA uses OpenSSL's x509 certificates and private keys, the default behaviour is pretty safe.
You can generate them with the command:

`$ make generate`

This will generate a 2048 bit length Private key and a public certificate for the client and the daemon. Stored in `/tmp/matt_daemon`
From then, to build the binaries with RSA support:

`$ make use=rsa`

Remember that you still can modify the authentication password!

`$ make use=rsa password="MyLoginPassword`



# Usage
### Daemon
Matt_daemon's usage is pretty straight-forward, as most of it's configuration is defined a compilation.
However, you can start the server in two different modes: `detached` so as a daemon, or `attached` to the current controlling terminal. Don't forget that you need to execute this program with root permissions.

`$ sudo ./matt_daemon`  - For standart damonized use.

or

`$ sudo ./matt_daemon -d` - To force the program to stay in foreground. This can be pretty usefull if you want to obtain some more information, in case something went wrong.

However, you can always look at the logfile stored in `/var/log/matt_daemon/` for further investigation.

### Client
The client only takes two arguments, no more - no less.

`$ ./BEN_AFK <destination-ip> <port>`

Entering fully qualified domain names (FQDNs) will not work.

### And now ?

Once you connected to the remote daemon, you should see something like this:

![image](https://user-images.githubusercontent.com/29956389/98834960-1db77000-2440-11eb-843b-1d75ecf1ed54.png)

If we look at the logfile, we can see some details about the daemon status:

![image](https://user-images.githubusercontent.com/29956389/98835115-4d667800-2440-11eb-9aa5-3c0c3478a2cf.png)

When entering some text in the client's prompt, it will be logged by the daemon:

![image](https://user-images.githubusercontent.com/29956389/98835444-b6e68680-2440-11eb-92aa-53489edb6c94.png)

Remember the system command ?

![image](https://user-images.githubusercontent.com/29956389/98835626-f3b27d80-2440-11eb-903f-44717a70026a.png)

# About the encryption

The encryption is essentially handled by 3 Classes, located in the `src/common/` directory:
- `Cryptograph.cpp` - That's where the heavy lifting happens: encryption and decryption of data, AES and RSA. This class will only compile the neccesary functions thanks to preprocessor directives.
- `CryptoWrapper.cpp` - A wrapper which contains the routines for exchanging RSA public certificates, alwell as sending and receiving data over the network socket. 
- `KeyLoader.cpp` - This class will read the x509 certificates and private keys, extract the relevant data and return the corresponding `EVP_PKEY` data structures.

#### AES

The AES encryption/decryption process is implemented as follows:
- Allocation of the `_AESEncrypt` and `_AESDecrypt` contextes.
- Initialization of the `EVP_aes_256_cbc` cipher.
- Derivation of the actual `AESKey` using the `PBKD_PASSWORD` and `PBKD_SALT` with the `EVP_sha256` cipher.
- For encryption:
	- Initialise the `_AESEncryptContext` using the `EVP_EncryptInit_ex` function and the `EVP_aes_256_cbc` cipher, aswell as the previously derived `AESKey` and the associated `AESIV`.
	- The actual data encryption is done with the `EVP_EncryptUpdate` function.
	- If padding is enabled (the default) then `EVP_EncryptFinal_ex` encrypts the "final" data, that is any data that remains in a partial encryption block.
	- The encrypted data is then returned, and can be sent over the network socket.
- For decryption, it's prectically the same thing. Note that it is a symetrical encryption, so if the server and the client are given the same password and salt for deriving `AESKey` and `AESIV`, no key exchange is happening:
	- Initialise the `AESDecryptContext` using the `EVP_DecryptInit_ex` function and the `EVP_aes_256_cbc` cipher, aswell as the previously derived `AESKey` and the associated `AESIV`.
	- The actual data decryption is done with the `EVP_DecryptUpdate` function.
	- If padding is enabled (the default) then `EVP_DecryptFinal_ex` decrypts the "final" data, that is any data that remains in a partial encryption block.

#### RSA
If you are unfamiliar with asymetric encryption, you can refresh your memory here: https://en.wikipedia.org/wiki/Public-key_cryptography

Again, this implementation is not encrypting the entire data with the remote public key, but are instead encrypted using a symmetric `session key`. This key is itself then encrypted using the public key. In OpenSSL this combination is referred to as an `envelope`. (EVP)

For RSA, the implementation is as follows:
- The makefile generates a keypair for both the client, and the server. We will call the public key the `certificate`, and the private key, stays the `private key`.
- At startup, each binary loads up their respective `private key` and `certificate`. If their is no key, or it is badly formatted, the binaries will not start, and output an error message.
- The `_rsaEncryptContext` and `_rsaDecryptContext` are allocated and initialized.
- Upon connection, the `RSA key exchange process` will begin, where the client and the server exchange their certificates, to later encrypt their data with those `remote certificates`.
- For encryption:
	- We generate the `sessionkey` with the `EVP_SealInit` function, which will be encrypted using the `remote certificate`,  the data will be encrypted with the `EVP_aes_256_cbc` cipher.
	- The `sessionkey` length, the encrypted `sessionkey` and it's associated `IV`'s form the `message header`.
	- The actual data is then encryped with the `EVP_SealUpdate` function and it's `_rsaEncryptContext`.
	- The final `EVP_SealFinal` function then encrypts the "final" data, that is any data that remains in a partial encryption block.
	- The entire  `mesage header + encrypted data` is then returned, and can be sent over the network socket.
- For decryption:
	- We extract the `sessionkey length`, `sessionkey`, and `IV`'s from the `message header`.
	- The `_rsaDecryptContext` is updated with the extracted data from the `message header` and the local `Private Key`
	- The data following the `message header` is then decrypted with the `EVP_OpenUpdate` function.
	- Any leftover (padded) data is then decrypted with the `EVP_OpenFinal` function.
	- We now have the entire decrypted message and can return it for later use.


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

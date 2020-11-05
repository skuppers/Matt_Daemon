# Matt_Daemon
Develop a simple daemon which listens to a given port an interpret some commands.

Todo Checklist:
 - [x] Logging (Tintin_reporter)
    -  [x] Add Loglevel (Info, warning, error...)
    - [ ] Advanced logs (Rotation, archiving)
 - [x] Daemonizing
 - [x] Signal handling
 - [x] Policies (uid, lockfile, etc...)
 - [x] Connection Manager
    - [x] Listen on port
    - [x] Accept clients
    - [x] Handle client communication
    - [x] Exit on 'quit' message
    - [x] Max 3 clients
    - [ ] Crypt communications (RSA / ECDSA / AES ?)
 - [x] Client
    - [x] Basic prompt
    - [x] Pop a shell (/bin/sh)
    - [ ] Download daemon logs
    - [ ] Authentication

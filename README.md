# Matt_Daemon
Develop a simple daemon which listens to a given port an interpret some commands.

Todo Checklist:
 - Logging (Tintin_reporter)
    - Advanced logs (Rotation, archiving)
 - Daemonizing
 - Signal handling
 - Policies (uid, lockfile, etc...)
 - Connection Manager
    - Listen on port
    - Accept clients
    - Handle client communication
    - Exit on 'quit' message
    - Max 3 clients
    - Authentication
    - Crypt communications (RSA / ECDSA / AES ?)
 - Client
    - Basic prompt
    - Pop a shell (/bin/sh)
    - Download daemon logs
    - Authentication

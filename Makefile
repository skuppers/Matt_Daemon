.SUFFIXES:

SERVER=matt_daemon
CLIENT=BEN_AFK

CC = clang++

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -D _GNU_SOURCE

LIBCRYPTO += -lcrypto

#------------------------------- AUTHENTICATION -------------------------------#

DEFAULT_AUTHENTICATION_PASSWORD="42born2code"

ifneq ($(password),)
	CFLAGS += "-DAUTH_PASSWORD=\"$(password)\""
else
	CFLAGS += "-DAUTH_PASSWORD=\"$(DEFAULT_AUTHENTICATION_PASSWORD)\""
endif

#--------------------------------- AES / RSA ----------------------------------#

ifeq ($(use),rsa)

	CFLAGS += "-DUSE_RSA"

RSA_PRIVKEY      = "_private.pem"
RSA_CERTIFICATE  = "_cert.pem"

RSA_KEYLENGTH    = 2048
RSA_VALID_DAYS   = 365
RSA_SUBJECT	     = "/C=FR/ST=Paris/L=France/O=42born2code/OU=School/CN=www.42.fr"
RSA_FILE_PATH    = "/tmp/matt_daemon/"

CLIENT_KEYFILE  = "$(RSA_FILE_PATH)$(CLIENT)$(RSA_PRIVKEY)"
CLIENT_CERTFILE = "$(RSA_FILE_PATH)$(CLIENT)$(RSA_CERTIFICATE)"

SERVER_KEYFILE  = "$(RSA_FILE_PATH)$(SERVER)$(RSA_PRIVKEY)"
SERVER_CERTFILE = "$(RSA_FILE_PATH)$(SERVER)$(RSA_CERTIFICATE)"

	CREATE_KEY_DIRECTORY := $(shell mkdir $(RSA_FILE_PATH) 2>&-)
	
	CREATE_KEYPAIR_CLIENT = $(shell openssl req -x509 -newkey rsa:$(RSA_KEYLENGTH) -keyout $(CLIENT_KEYFILE) -out $(CLIENT_CERTFILE) -days $(RSA_VALID_DAYS) -nodes -subj $(RSA_SUBJECT))
	CREATE_KEYPAIR_SERVER = $(shell openssl req -x509 -newkey rsa:$(RSA_KEYLENGTH) -keyout $(SERVER_KEYFILE) -out $(SERVER_CERTFILE) -days $(RSA_VALID_DAYS) -nodes -subj $(RSA_SUBJECT))

	CFLAGS += "-DCLIENT_PKEY=\"$(CLIENT_KEYFILE)\""
	CFLAGS += "-DCLIENT_CERT=\"$(CLIENT_CERTFILE)\""
	CFLAGS += "-DSERVER_PKEY=\"$(SERVER_KEYFILE)\""
	CFLAGS += "-DSERVER_CERT=\"$(SERVER_CERTFILE)\""
	CFLAGS += "-DCLIENT_NAME=\"$(CLIENT)\""
	CFLAGS += "-DSERVER_NAME=\"$(SERVER)\""

else
	CFLAGS += "-DUSE_AES"
endif

#---------------------------------- DEBUGGING --------------------------------#

# Compiler Debug Flags
ifeq ($(d), 1)
	CFLAGS += -g3 -ggdb
	CFLAGS += -fsanitize=address,undefined
else ifeq ($(d), 2)
	CFLAGS += -g3
	CFLAGS += -fsanitize=address,undefined
	CFLAGS += -Wpadded
else ifeq ($(d), 3)
	CFLAGS += -g3
	CFLAGS += -fsanitize=address,undefined
	CFLAGS += -Wpadded
	CFLAGS += -pedantic
endif
ifneq ($(err), no)
	CFLAGS += -Werror
endif

# Debug Dir
DSYM += $(NAME).dSYM
DSYM += $(DBNAME).dSYM

#---------------------------------- INCLUDES ----------------------------------#

INCLUDES_MATTDAEMON = includes/

I_INCLUDES += -I $(INCLUDES_MATTDAEMON)

#---------------------------------- HEADERS -----------------------------------#

vpath %.hpp $(INCLUDES_MATTDAEMON)

# Common
HEADER += Cryptograph.hpp
HEADER += CryptoWrapper.hpp
HEADER += KeyLoader.hpp

# client
HEADER += Ben_Afk.hpp

# deaemon
HEADER += general.hpp
HEADER += PolicyManager.hpp
HEADER += ConnectionManager.hpp

#----------------------------------- SOURCES ---------------------------------#

PATH_CLIENT_SRCS += src/client/
PATH_DAEMON_SRCS += src/server/
PATH_COMMON_SRCS += src/common/

### COMMON

COMMON_SRCS += Cryptograph.cpp
COMMON_SRCS += CryptoWrapper.cpp
COMMON_SRCS += KeyLoader.cpp

### CLIENT

CLIENT_SRCS += Ben_Afk.cpp
CLIENT_SRCS += client.cpp

### SERVER

DAEMON_SRCS += Matt_daemon.cpp
DAEMON_SRCS += signals.cpp
DAEMON_SRCS += Tintin_reporter.cpp
DAEMON_SRCS += PolicyManager.cpp
DAEMON_SRCS += ConnectionManager.cpp

vpath %.cpp $(PATH_CLIENT_SRCS)
vpath %.cpp $(PATH_DAEMON_SRCS)
vpath %.cpp $(PATH_COMMON_SRCS)


#----------------------------------- OBJECTS ----------------------------------#

PATH_OBJS = objs/
CLIENT_OBJS = $(patsubst %.cpp, $(PATH_OBJS)%.o, $(CLIENT_SRCS))
DAEMON_OBJS = $(patsubst %.cpp, $(PATH_OBJS)%.o, $(DAEMON_SRCS))
COMMON_OBJS = $(patsubst %.cpp, $(PATH_OBJS)%.o, $(COMMON_SRCS))


#---------------------------------- THA RULES ---------------------------------#

ifeq ($(use),rsa)
all: CREATE_RSA_KEYS $(CLIENT) $(SERVER)
else
all: $(CLIENT) $(SERVER)
endif

$(CLIENT): $(PATH_OBJS) $(CLIENT_OBJS) $(COMMON_OBJS)
	$(CC) $(CFLAGS) $(I_INCLUDES) $(CLIENT_OBJS) $(COMMON_OBJS) $(LIBCRYPTO) -o $@
	printf "$@ is ready.\n"

$(CLIENT_OBJS): $(PATH_OBJS)%.o: %.cpp $(HEADER) Makefile
	$(CC) $(CFLAGS) $(I_INCLUDES) -c $< -o $@


$(SERVER): $(PATH_OBJS) $(DAEMON_OBJS) $(COMMON_OBJS)
	$(CC) $(CFLAGS) $(I_INCLUDES) $(DAEMON_OBJS) $(COMMON_OBJS) $(LIBCRYPTO) -o $@
	printf "$@ is ready.\n"

$(DAEMON_OBJS): $(PATH_OBJS)%.o: %.cpp $(HEADER) Makefile
	$(CC) $(CFLAGS) $(I_INCLUDES) -c $< -o $@


$(COMMON_OBJS): $(PATH_OBJS)%.o: %.cpp $(HEADER) Makefile
	$(CC) $(CFLAGS) $(I_INCLUDES) -c $< -o $@

$(PATH_OBJS):
	mkdir $@

CREATE_RSA_KEYS:
	@$(CREATE_KEYPAIR_CLIENT)
	@$(CREATE_KEYPAIR_SERVER)

#---------------------------------- CLEANING ----------------------------------#

clean:
	$(RM) $(OBJS)
	$(RM) -R $(PATH_OBJS)
	$(RM) -R $(DSYM)
	printf "Objs from $(SERVER) removed\n"
	printf "Objs from $(CLIENT) removed\n"
	$(RM) $(CLIENT_KEYFILE)
	$(RM) $(CLIENT_CERTFILE)
	printf "Client RSA keys deleted\n"
	$(RM) $(SERVER_KEYFILE)
	$(RM) $(SERVER_CERTFILE)
	printf "Server RSA keys deleted\n"
	$(RM) $(RSA_FILE_PATH)

fclean: clean
	$(RM) $(SERVER)
	$(RM) $(CLIENT)
	printf "$(SERVER) removed\n"
	printf "$(CLIENT) removed\n"

re: fclean all

FORCE:

#------------------------------------- MISC -----------------------------------#

.PHONY: clean fclean re all
.SILENT:

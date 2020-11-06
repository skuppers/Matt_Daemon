.SUFFIXES:

SERVER=Matt_daemon
CLIENT=Ben_AFK

CC = clang++

CFLAGS += -Wall
CFLAGS += -Wextra -fsanitize=address
CFLAGS += -D _GNU_SOURCE

LIBCRYPTO += -lcrypto

# Compiler Debug Flags
ifeq ($(d), 0)
	CFLAGS += -g3
else ifeq ($(d), 1)
	CFLAGS += -g3
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

all: $(CLIENT) $(SERVER)

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

#---------------------------------- CLEANING ----------------------------------#

clean:
	$(RM) $(OBJS)
	$(RM) -R $(PATH_OBJS)
	$(RM) -R $(DSYM)
	printf "Objs from $(SERVER) removed\n"
	printf "Objs from $(CLIENT) removed\n"

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

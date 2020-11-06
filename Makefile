.SUFFIXES:

SERVER=Matt_daemon
CLIENT=Ben_AFK

CC=clang++

CFLAGS += -Wall
CFLAGS += -Wextra

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

INCLUDES_TRACEROUTE = includes/

I_INCLUDES += -I $(INCLUDES_TRACEROUTE)

#---------------------------------- HEADERS -----------------------------------#

vpath %.hpp $(INCLUDES_TRACEROUTE)

SERVER_HEADER += general.hpp
SERVER_HEADER += PolicyManager.hpp
SERVER_HEADER += ConnectionManager.hpp

CLIENT_HEADER += Ben_Afk.hpp

#----------------------------------- SOURCES ---------------------------------#

SERVER_DIR = server/

SERVER_SRCS += Matt_daemon.cpp
SERVER_SRCS += signals.cpp
SERVER_SRCS += Tintin_reporter.cpp
SERVER_SRCS += PolicyManager.cpp
SERVER_SRCS += ConnectionManager.cpp


CLIENT_DIR = client/

CLIENT_SRCS += Ben_Afk.cpp
CLIENT_SRCS += client.cpp


vpath %.cpp $(SERVER_DIR)
vpath %.cpp $(CLIENT_DIR)

#----------------------------------- OBJECTS ----------------------------------#

PATH_OBJS = objs/
SERVER_OBJS = $(patsubst %.cpp, $(PATH_OBJS)%.o, $(SERVER_SRCS))
CLIENT_OBJS = $(patsubst %.cpp, $(PATH_OBJS)%.o, $(CLIENT_SRCS))


#---------------------------------- THA RULES ---------------------------------#

all: $(PATH_OBJS) $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(I_INCLUDES) $(SERVER_OBJS)  -o $@
	printf "$@ is ready.\n"

$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $(I_INCLUDES) $(CLIENT_OBJS)  -o $@
	printf "$@ is ready.\n"

$(SERVER_OBJS): $(PATH_OBJS)%.o: %.cpp $(SERVER_HEADER) Makefile
	$(CC) $(CFLAGS) $(I_INCLUDES) -c $< -o $@

$(CLIENT_OBJS): $(PATH_OBJS)%.o: %.cpp $(CLIENT_HEADER) Makefile
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
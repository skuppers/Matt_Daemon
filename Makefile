.SUFFIXES:

NAME=Matt_daemon

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
else ifeq ($(d), 4)
	CFLAGS += -g3
	CFLAGS += -fsanitize=address,undefined
	CFLAGS += -Wpadded
	CFLAGS += -pedantic
	CFLAGS += -Weverything
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

HEADER += general.hpp
HEADER += Matt_daemon.hpp
HEADER += PolicyManager.hpp
HEADER += ConnectionManager.hpp

#----------------------------------- SOURCS -----------------------------------#

PATH_SRCS = src/

SRCS += main.cpp
SRCS += signals.cpp
SRCS += Tintin_reporter.cpp
SRCS += PolicyManager.cpp
SRCS += ConnectionManager.cpp


vpath %.cpp $(PATH_SRCS)

#----------------------------------- OBJECTS ----------------------------------#

PATH_OBJS = objs/
OBJS = $(patsubst %.cpp, $(PATH_OBJS)%.o, $(SRCS))


#---------------------------------- THA RULES ---------------------------------#

all: $(PATH_OBJS) $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(I_INCLUDES) $(OBJS) $(LIBFT) -o $@
	printf "$@ is ready.\n"

$(OBJS): $(PATH_OBJS)%.o: %.cpp $(HEADER) Makefile
	$(CC) $(CFLAGS) $(I_INCLUDES) -c $< -o $@

$(PATH_OBJS):
	mkdir $@

#---------------------------------- CLEANING ----------------------------------#

clean:
	$(RM) $(OBJS)
	$(RM) -R $(PATH_OBJS)
	$(RM) -R $(DSYM)
	printf "Objs from $(NAME) removed\n"

fclean: clean
	$(RM) $(NAME)
	printf "$(NAME) removed\n"

re: fclean all

FORCE:

#------------------------------------- MISC -----------------------------------#

.PHONY: clean fclean re all
.SILENT:
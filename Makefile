CC := cc
CJSON_CFLAGS := $(shell pkg-config --cflags libcjson 2>/dev/null)
CJSON_LIBS := $(shell pkg-config --libs libcjson 2>/dev/null)
CFLAGS := -Wall -Wextra -Werror -Iincludes $(CJSON_CFLAGS)
LDFLAGS := $(CJSON_LIBS) -lm -Wl,-rpath,/usr/local/lib
NAME := mozart
SRC := main.c src/get_models.c src/get_env.c
OBJ := $(SRC:.c=.o)

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)

%.o: %.c includes/mozart.h includes/struct.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

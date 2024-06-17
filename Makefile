NAME = ircserv
OBJS_DIR = objs
DIR_SRC = src/
SRCS = $(wildcard $(DIR_SRC)*.cpp)
OBJS = $(patsubst $(DIR_SRC)%.cpp, $(OBJS_DIR)/%.o, $(SRCS))
CFLAGS = -Wall -Wextra -Werror
CPPFLAGS = -std=c++98

all: $(OBJS_DIR) $(NAME)

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

$(NAME): $(OBJS)
	c++ $(CFLAGS) $(CPPFLAGS) -o $(NAME) $(OBJS)

$(OBJS_DIR)/%.o: $(DIR_SRC)%.cpp
	@mkdir -p $(dir $@)
	c++ $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

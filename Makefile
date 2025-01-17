NAME = ircserv
CC = c++
FLAGS = -Wall -Wextra -Werror -MMD -MP -std=c++98 -g3
BUILD_DIR = .build
SRCS = main Server Client
CPPFILES = $(addsuffix .cpp, $(SRCS))
OBJS = $(addprefix $(BUILD_DIR)/,$(CPPFILES:.cpp=.o))
DEBUG = -D DEBUG_MODE=1

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $@
	@echo "\n$(NAME) is ready for use!\n"

$(BUILD_DIR)/%.o: %.cpp Makefile
	@mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) -c $< -o $@

-include $(OBJS:.o=.d)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean
	@make --no-print-directory

run: all
	./$(NAME) 2222 2

debug: FLAGS += $(DEBUG)
debug: clean all
	valgrind --leak-check=full ./$(NAME) 2222 2

.PHONY: all clean fclean re run

.gitignore: Makefile
	@echo '.build' > .gitignore
	@echo '.vscode' >> .gitignore
	@echo $(NAME) >> .gitignore
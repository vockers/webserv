NAME		= webserv
SRC_DIR		= ./src
INCLUDE_DIR	= ./include
OBJ_DIR		= ./.obj

CXX			= clang++
CXXFLAGS	= -I$(INCLUDE_DIR) -std=c++20
DEBUG_FLAGS	= -MMD -MP -g -fsanitize=address

SRCS		= $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)
OBJS		= $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

all: debug

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(NAME)

release: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

-include $(OBJS:.o=.d)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	-rm -rf $(OBJ_DIR)

fclean: clean
	-rm -f $(NAME)

re: fclean all

test:
	@docker build -t webserv-tests -f ./tests/Dockerfile .
	docker run -it webserv-tests

format:
	@clang-format -i $(SRCS) $(wildcard $(INCLUDE_DIR)/*/*.hpp) $(wildcard $(INCLUDE_DIR)/*/*.hpp)

.PHONY: all debug release clean fclean re test format

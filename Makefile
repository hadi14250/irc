NAME = ircserv

SRCS = main.cpp Server.cpp Client.cpp CommandsV2.cpp Utils.cpp

CXXFLAGS = -Wall -Wextra -Werror 
# -std=c++98

CXX = c++

OBJS = $(SRCS:.cpp=.o)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) -o $(NAME)

all: $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re

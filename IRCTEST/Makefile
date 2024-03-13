NAME = ircserv

SRCS = main.cpp Server.cpp Client.cpp Commands.cpp Utils.cpp Channel.cpp Bot.cpp

CXXFLAGS = -Wall -Wextra -Werror -fsanitize=address -std=c++98

CXX = c++

OBJS = $(SRCS:.cpp=.o)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) -o $(NAME)

all: $(NAME)

bonus: $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
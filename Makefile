# NAME = irc <- commented this out becuz subject pdf wants prog name to be ircserv!
NAME = ircserv

TMP = tmp_dev_cmds.cpp 

SRCS = main.cpp Server.cpp Client.cpp Commands.cpp Utils.cpp Channel.cpp $(TMP)

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
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

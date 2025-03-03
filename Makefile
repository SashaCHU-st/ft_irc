NAME = ircserv
CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++17

SRCS = src/main.cpp \
		src/Serv.cpp \
		src/Socket.cpp \
		src/Client.cpp \
		src/Quit.cpp \
		src/Nick.cpp \
		src/Password.cpp \
		src/Launch.cpp \
		src/ServParse.cpp \
		src/User.cpp \
		src/Utils.cpp \
		src/Channel.cpp \
		src/WelcomeMsg.cpp \
		src/Privmsg.cpp \
		src/commands/JOIN.cpp \
		src/commands/PART.cpp \
		src/commands/MODE.cpp \
		src/commands/KICK.cpp \
		src/commands/INVITE.cpp \
		src/commands/TOPIC.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)
	
%.o: %.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
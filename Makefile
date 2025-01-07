NAME = ircserv
CC = c++

CFLAGS = -Wall -Wextra -Werror

SRCS = src/main.cpp \
		src/Serv.cpp \
		src/Socket.cpp \
		src/Signal.cpp \


		
	
		 



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
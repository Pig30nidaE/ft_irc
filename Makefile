CC			= c++

CFLAGS		= -Wall -Wextra -Werror

SOURCE	= ./source/

IRC		= ircserver

NAME		= $(IRC)

SRCS        =	$(SOURCE)Server.cpp\

OBJS		= $(SRCS:.c=.o)

all			: $(NAME)

$(NAME)		: $(OBJS)
			$(CC) ./main.cpp $(CFLAGS) $^ -o $(NAME)

clean		:

fclean		: clean
			rm -f $(IRC)

re			:  
			make fclean
			make all

.PHONY		: clean, re, fclean, all, mini

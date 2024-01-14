NAME = ircserv
SOURCE = src/
SRCS = $(SOURCE)main.cpp \
	$(SOURCE)Channel.cpp \
	$(SOURCE)ChannelContainer.cpp \
	$(SOURCE)Command.cpp \
	$(SOURCE)Server.cpp \
	$(SOURCE)User.cpp
OBJS = ${SRCS:.cpp=.o}
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror
STDFLAGS = -std=c++98
RM = rm -rf

%.o: %.cpp
	@$(CXX) $(STDFLAGS) $(CXXFLAGS) -c $^ -o $@

all: $(NAME)

$(NAME): $(OBJS)
	@echo "making..."
	@$(CXX) $(CXXFLAGS) $(STDFLAGS) $(SRCS) -o $(NAME)

clean:
	@${RM} ${OBJS}
	@echo "clean finished"

fclean:
	@make clean
	@${RM} ${NAME}
	@echo "fclean finished."

re:	
	@echo "\033[0;33mremake...\033[0m"
	@make fclean
	@make all

.PHONY: clean fclean re all
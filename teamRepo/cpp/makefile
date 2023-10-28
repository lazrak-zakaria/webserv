NAME = Webserv

CXX = c++

CXXFLAGS = #-Wall -Wextra -Werror -std=c++98

SRC = Client.cpp newMain.cpp Request.cpp WebServer.cpp Cgi.cpp\
		ConfigFile.cpp MimeAndError.cpp Response.cpp Server.cpp

OBJ = $(SRC:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean :
	rm -f $(OBJ)

fclean : clean
	rm -f $(NAME)

re : fclean all

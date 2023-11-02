NAME = Webserv

CXX = c++

CXXFLAGS = -fsanitize=address -g3 #-Wall -Wextra -Werror -std=c++98

OBJ = ./cpp/Client.o ./cpp/newMain.o ./cpp/Request.o ./cpp/WebServer.o ./cpp/Cgi.o\
		./cpp/ConfigFile.o ./cpp/MimeAndError.o ./cpp/Response.o ./cpp/Server.o ./cpp/ServerConfig.o

all : $(NAME)

$(NAME) : $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean :
	rm -f $(OBJ)

fclean : clean
	rm -f $(NAME)

re : fclean all

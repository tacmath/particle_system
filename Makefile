#https://stackoverflow.com/questions/7542808/how-to-compile-opencl-on-ubuntu

# nom de l'executable
NAME = particle_system

#sources path
SRC_PATH= srcs

#objects path
OBJ_PATH= .objs

#includes
INC_PATH= srcs/

HEADER= $(INC_PATH)/*.h $(INC_PATH)/*.hpp

#framework
FRAMEWORK= -ldl `pkg-config --static --libs gl glfw3` -L ~/.dep/usr/lib/x86_64-linux-gnu/
FRAMEWORK_INC = -I libs/include

NAME_SRC=	camera.cpp\
			main.cpp\
			particle_system.cpp\
			shader.cpp\
			utils.cpp\
			VAO.cpp\
			VBO.cpp

NAME_SRC_C= glad.c \

OBJ_NAME		= $(NAME_SRC:.cpp=.o)

OBJ_NAME_C		= $(NAME_SRC_C:.c=.o)

OBJS = $(addprefix $(OBJ_PATH)/,$(OBJ_NAME)) $(addprefix $(OBJ_PATH)/,$(OBJ_NAME_C))

DEBUG_FLAG = -Wall -Wextra

CC			= gcc 
GPP			= g++ -std=c++11 $(DEBUG_FLAG)



all: $(NAME)

$(NAME) : $(OBJS)
	$(GPP) $^ -o $@ $(FRAMEWORK)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp $(HEADER) Makefile
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	$(GPP) -I $(INC_PATH) $(FRAMEWORK_INC) -c $< -o $@

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c $(HEADER) Makefile
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	$(CC) $(FRAMEWORK_INC) -c $< -o $@


clean:
	rm -rf $(OBJ_PATH)


fclean: clean
	rm -f $(NAME)

re: fclean all

install:
	./install.sh

.PHONY: all re clean fclean 

#pkg ocl-icd-opencl-dev and ocl-icd-libopencl1

# nom de l'executable
NAME = particle_system

# nom de l'executable
OPENGL_NAME = opengl_particle_system

#sources path
SRC_PATH= srcs

#objects path
OBJ_PATH= .objs

GL_OBJ_PATH= .objs/GL

#includes
INC_PATH= srcs/

IMGUI_DIR = imgui/

IMGUI_SOURCES = $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
IMGUI_SOURCES += $(IMGUI_DIR)/imgui_impl_glfw.cpp $(IMGUI_DIR)/imgui_impl_opengl3.cpp
IMGUI_OBJS = $(addsuffix .o, $(basename $(notdir $(IMGUI_SOURCES))))

CXXFLAGS = -std=c++11 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
CXXFLAGS += -g -Wall -Wformat
CXX = g++

HEADER= $(INC_PATH)/*.h $(INC_PATH)/*.hpp

#framework
FRAMEWORK= -ldl `pkg-config --static --libs gl glfw3 OpenCL` -L ~/.dep/usr/lib/x86_64-linux-gnu/
FRAMEWORK_INC = -I libs/include -I $(IMGUI_DIR)

NAME_SRC=	camera.cpp\
			main.cpp\
			particle_system.cpp\
			opencl_particles_controleur.cpp\
			opengl_particles_controleur.cpp\
			shader.cpp\
			utils.cpp\
			VAO.cpp\
			VBO.cpp

NAME_SRC_C= glad.c \

OBJ_NAME		= $(NAME_SRC:.cpp=.o)

GL_OBJ_NAME		= $(NAME_SRC:.cpp=GL.o)

OBJ_NAME_C		= $(NAME_SRC_C:.c=.o)

OBJS = $(addprefix $(OBJ_PATH)/,$(OBJ_NAME)) $(addprefix $(OBJ_PATH)/,$(OBJ_NAME_C)) $(addprefix $(OBJ_PATH)/,$(IMGUI_OBJS))

GL_OBJS = $(addprefix $(GL_OBJ_PATH)/,$(GL_OBJ_NAME)) $(addprefix $(OBJ_PATH)/,$(OBJ_NAME_C)) $(addprefix $(OBJ_PATH)/,$(IMGUI_OBJS))

DEBUG_FLAG = -Wall -Wextra

CC			= gcc 
GPP			= g++ -std=c++11 $(DEBUG_FLAG)



all: $(NAME) $(OPENGL_NAME)

$(NAME) : $(OBJS)
	$(GPP) $^ -o $@ $(FRAMEWORK)

$(OPENGL_NAME) : $(GL_OBJS)
	$(GPP) $^ -o $@ $(FRAMEWORK)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp $(HEADER) Makefile
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	$(GPP) -I $(INC_PATH) $(FRAMEWORK_INC) -c $< -o $@

$(GL_OBJ_PATH)/%GL.o: $(SRC_PATH)/%.cpp $(HEADER) Makefile
	@mkdir $(GL_OBJ_PATH) 2> /dev/null || true
	$(GPP) -D USE_COMPUTE_SHADER -I $(INC_PATH) $(FRAMEWORK_INC) -c $< -o $@

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c $(HEADER) Makefile
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	$(CC) $(FRAMEWORK_INC) -c $< -o $@

$(OBJ_PATH)/%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<


clean:
	rm -rf $(OBJ_PATH)


fclean: clean
	rm -f $(NAME)
	rm -f $(OPENGL_NAME)

re: fclean all

install:
	./install.sh

.PHONY: all re clean fclean 

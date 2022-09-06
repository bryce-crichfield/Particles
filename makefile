build:
	clang main.cc -o exe -lglfw -lGLU -lGL -lstdc++ -lGLEW -lm -fopenmp -lpthread -limgui
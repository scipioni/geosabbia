EXECUTABLE=geosabbia
ROOT = .
SDK = /lab/gameengine/flex
CUDA = /usr/local/cuda
CUDA_INC = $(CUDA)/include
CUDA_LIB = $(CUDA)/lib64
CORE = $(SDK)/core
SRC = $(ROOT)/src
LIB = $(SDK)/lib/linux64
INCLUDE = $(SDK)/include
CC=g++

CFLAGS=-c -w -I$(CUDA_INC) -I$(SDK) -I$(INCLUDE) -O3 -fPIC -ffast-math -fpermissive -fno-strict-aliasing
#CFLAGS=-c -w -I$(CUDA_INC) -I$(SDK) -I$(INCLUDE) -O0 -fPIC -fpermissive -fno-strict-aliasing
LDFLAGS=-g -L/usr/lib -L$(CUDA_LIB) $(LIB)/flexRelease_x64.a -lGL -lglut -lGLU -lGLEW -lcudart -lpng

SOURCES = \
	$(wildcard $(CORE)/*.cpp) \
	$(wildcard $(SRC)/*.cpp)

OBJECTS=$(SOURCES:.cpp=.o)
HEADERS = $(wildcard $(SRC)/*.h)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) Makefile
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@

%.o: %.cpp $(HEADERS)
	@$(CC) $(CFLAGS) $< -o $@

clean:
	-@rm -f $(OBJECTS) $(EXECUTABLE)

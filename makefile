CC := g++
CFLAGS := -Wall

SRCDIR := src
OBJDIR := obj
BINDIR := bin
INCDIR := include

INCLUDES := -I${INCDIR}
LFLAGS := -Llib
LIBS :=


SRCS := $(shell find $(SRCDIR) -type f -name *.cpp)
OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRCS:.cpp=.o))

MAIN = harvester

.PHONY: clean

all: ${MAIN}

run: ${MAIN}
	${BINDIR}/${MAIN}

${MAIN} : ${OBJS}
	mkdir -p ${BINDIR}
	${CC} ${CFLAGS} ${INCLUDES} -o ${BINDIR}/${MAIN} ${OBJS} ${LFLAGS} ${LIBS}

${OBJDIR}/%.o: ${SRCDIR}/%.cpp
	-clang-format -i -style=LLVM $<
	-clang-format -i -style=LLVM ${INCDIR}/$*.h
	mkdir -p ${OBJDIR}
	${CC} ${CFLAGS} ${INCLUDES} -c $< -o $@

clean:
	rm -rf ${BINDIR}
	rm -rf ${OBJDIR}
CC := g++
CFLAGS := -Wall

SRCDIR := src
OBJDIR := obj
BINDIR := bin
INCDIR := include

INCLUDES := -I${INCDIR}
LFLAGS := -Llib
LIBS := -lcurl


SRCS := $(shell find $(SRCDIR) -type f -name "*.cpp")
OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRCS:.cpp=.o))

MAIN = harvester

.PHONY: clean

all: ${MAIN}

run: ${MAIN}
	${BINDIR}/${MAIN}

${MAIN} : ${OBJS}
	@mkdir -p ${BINDIR}
	${CC} ${CFLAGS} ${INCLUDES} -o ${BINDIR}/${MAIN} ${OBJS} ${LFLAGS} ${LIBS}

${OBJDIR}/%.o: ${SRCDIR}/%.cpp
	@echo "Formatting $< and ${INCDIR}/$*.h, errors are normal."
	@-clang-format -i -style=LLVM $<
	@-clang-format -i -style=LLVM ${INCDIR}/$*.h
	@echo "Formatting done."
	@mkdir -p ${OBJDIR}
	${CC} ${CFLAGS} ${INCLUDES} -c $< -o $@

clean:
	rm -rf ${BINDIR}
	rm -rf ${OBJDIR}
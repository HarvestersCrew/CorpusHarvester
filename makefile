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

OBJDIRS := $(dir $(OBJS))

MAIN = harvester

.PHONY: clean

all: ${BINDIR}/${MAIN}

run: ${BINDIR}/${MAIN}
	@echo "Running $<...\n"
	@$<

${BINDIR}/${MAIN} : ${OBJS}
	@mkdir -p ${BINDIR}
	${CC} ${CFLAGS} ${INCLUDES} -o $@ ${OBJS} ${LFLAGS} ${LIBS}

.SECONDEXPANSION:
${OBJDIR}/%.o: ${SRCDIR}/%.cpp $$(wildcard ${INCDIR}/$$*.h)
	@mkdir -p ${OBJDIRS}
	@# @echo "Formatting $< and ${INCDIR}/$*.h, errors are normal."
	@# @-clang-format -i -style=LLVM $<
	@# @-clang-format -i -style=LLVM ${INCDIR}/$*.h
	@# @echo "Formatting done."
	@mkdir -p ${OBJDIR}
	${CC} ${CFLAGS} ${INCLUDES} -c $< -o $@

docker:
	@# sudo docker-compose run --rm --name gcc gcc
	sudo docker build -t ${MAIN} .
	mkdir -p ${BINDIR}
	echo "sudo docker run --rm -it ${MAIN} \$$1" > ${BINDIR}/${MAIN}
	chmod +x ${BINDIR}/${MAIN}

clean:
	rm -rf ${BINDIR}/*
	rm -rf ${OBJDIR}/*

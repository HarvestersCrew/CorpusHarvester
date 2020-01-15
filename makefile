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
INCS := $(shell find $(INCDIR) -type f -name "*.h")
OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRCS:.cpp=.o))

OBJDIRS := $(dir $(OBJS))

MAIN = harvester

.PHONY: clean all run docker lint format

all: ${BINDIR}/${MAIN}
	@echo "You can now run '${BINDIR}/${MAIN}'".

${BINDIR}/${MAIN} : ${OBJS}
	@mkdir -p ${BINDIR}
	${CC} ${CFLAGS} ${INCLUDES} -o $@ ${OBJS} ${LFLAGS} ${LIBS}

.SECONDEXPANSION:
${OBJDIR}/%.o: ${SRCDIR}/%.cpp $$(wildcard ${INCDIR}/$$*.h)
	@echo "Checking $? formatting..."
	@! clang-format $? -style=LLVM -output-replacements-xml | grep -c "<replacement " > /dev/null
	@mkdir -p ${OBJDIRS}
	${CC} ${CFLAGS} ${INCLUDES} -c $< -o $@

docker:
	docker build -t ${MAIN} .
	@mkdir -p ${BINDIR}
	@echo "docker run --name ${MAIN} --rm -it ${MAIN} \$$1" > ${BINDIR}/${MAIN}
	@chmod +x ${BINDIR}/${MAIN}
	@echo "You can now run '${BINDIR}/${MAIN}'".

format:
	@clang-format ${SRCS} ${INCS} -i -style=LLVM

clean:
	rm -rf ${BINDIR}/*
	rm -rf ${OBJDIR}/*

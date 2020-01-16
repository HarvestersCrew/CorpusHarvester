CC := g++
CFLAGS := -Wall

SRCDIR := src
OBJDIR := obj
BINDIR := bin
INCDIR := include
PWD := $(shell pwd)

INCLUDES := -I${INCDIR}
LFLAGS := -Llib
LIBS := -lcurl


SRCS := $(shell find $(SRCDIR) -type f -name "*.cpp")
INCS := $(shell find $(INCDIR) -type f -name "*.h")
OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRCS:.cpp=.o))

OBJDIRS := $(dir $(OBJS))

MAIN = harvester

.PHONY: clean all run docker docker_down format

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
	@echo "Upping docker-compose if not done..."
	@docker-compose up -d
	@echo "Building exec through Docker..."
	@docker-compose exec harvester make
	@mkdir -p ${BINDIR}
	@echo "docker-compose exec harvester ${BINDIR}/${MAIN} \$$1" > ${BINDIR}/${MAIN}
	@chmod +x ${BINDIR}/${MAIN}

docker_down:
	docker-compose down

format:
	@clang-format ${SRCS} ${INCS} -i -style=LLVM

clean:
	rm -rf ${BINDIR}/*
	rm -rf ${OBJDIR}/*

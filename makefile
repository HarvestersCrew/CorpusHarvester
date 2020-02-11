CC := g++
CFLAGS := -Wall -std=c++17

SRCDIR := src
OBJDIR := obj
BINDIR := bin
INCDIR := include
ENGINEDIR := engine

EXES := cli tests
EXESPATH := $(patsubst %,$(BINDIR)/%, $(EXES))
DOCKERS := $(patsubst %, docker/%, $(EXES))

INCLUDES := $(patsubst %,-I%,$(shell find ../ -type d -name $(INCDIR)))
LFLAGS := -Llib
LIBS := -lcurl -lmysqlcppconn

SRCS := $(shell find -type f -name "*.cpp")
INCS := $(shell find -type f -name "*.h")
OBJS := $(patsubst ./$(SRCDIR)/%,$(OBJDIR)/%,$(SRCS:.cpp=.o))

OBJDIRS := $(dir $(OBJS))

PWD := $(shell pwd)

.PHONY: docker, docker/%

all: $(EXESPATH)

$(BINDIR)/%:
	make -C $(ENGINEDIR) -f $(PWD)/makefile build
	make -C $* -f $(PWD)/makefile build
	make link/$*

link/%:
	@mkdir -p $(BINDIR)
	$(eval ENGINEOBJS := $(shell find $(ENGINEDIR)/$(OBJDIR) -type f -name *.o))
	$(eval MAINOBJS := $(shell find $*/$(OBJDIR) -type f -name *.o))
	$(CC) $(CFLAGS) -o $(BINDIR)/$* $(ENGINEOBJS) $(MAINOBJS) $(LFLAGS) $(LIBS)

build: $(OBJS)

.SECONDEXPANSION:
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $$(wildcard $(INCDIR)/$$*.h)
	@echo "Checking $? formatting, 'clang-format' must be installed..."
	@ clang-format --help 2>&1 > /dev/null
	@! clang-format $? -style=LLVM -output-replacements-xml | grep -c "<replacement " > /dev/null
	@mkdir -p $(OBJDIRS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

format:
	@clang-format $(SRCS) $(INCS) -i -style=LLVM

clean:
	rm -rf $(BINDIR)/*
	rm -rf $(shell find -type d -name $(OBJDIR))

docker: $(DOCKERS)

docker/up:
	@mkdir -p $(BINDIR)
	@chmod -R a+w $(BINDIR)
	docker-compose up -d

docker/down:
	docker-compose down

docker/clean: docker/up clean
	docker-compose exec harvester make clean

docker/%: docker/up
	docker-compose exec harvester make ${BINDIR}/$*
	@echo "#!/bin/bash" > ${BINDIR}/$*
	@echo "docker-compose exec harvester ${BINDIR}/$* \$${@:1}" >> ${BINDIR}/$*
	@chmod a+wx ${BINDIR}/$*

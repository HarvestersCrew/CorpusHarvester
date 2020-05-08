CC := g++
CFLAGS := -Wall -Wextra -std=c++17 -g

SRCDIR := src
OBJDIR := obj
BINDIR := bin
INCDIR := include
ENGINEDIR := engine

EXES := cli tests server
EXESPATH := $(patsubst %,$(BINDIR)/%, $(EXES))
DOCKERS := $(patsubst %, docker/%, $(EXES))

INCLUDES := $(patsubst %,-I%,$(shell find ../ -maxdepth 2 -type d -name $(INCDIR)))
LFLAGS := -Llib
LIBS := -lcurl -lmysqlcppconn -lpthread -lzip -lseasocks

SRCS := $(shell find -type f -name "*.cpp")
INCS := $(shell find -type f -name "*.h")
OBJS := $(patsubst ./$(SRCDIR)/%,$(OBJDIR)/%,$(SRCS:.cpp=.o))

OBJDIRS := $(dir $(OBJS))

PWD := $(shell pwd)

THREADS := $(shell nproc --all)

APPDIR := /tmp/stored/

.PHONY: docker, docker/%

all: $(EXESPATH)

$(BINDIR)/%: $(SRCS) $(INCS)
	$(MAKE) -j$(THREADS) -C $(ENGINEDIR) -f $(PWD)/makefile build
	$(MAKE) -j$(THREADS) -C $* -f $(PWD)/makefile build
	$(MAKE) -j$(THREADS) link/$*

link/%:
	@mkdir -p $(BINDIR)
	$(eval ENGINEOBJS := $(shell find $(ENGINEDIR)/$(OBJDIR) -type f -name *.o))
	$(eval MAINOBJS := $(shell find $*/$(OBJDIR) -type f -name *.o))
	$(CC) $(CFLAGS) -o $(BINDIR)/$* $(ENGINEOBJS) $(MAINOBJS) $(LFLAGS) $(LIBS)

build: $(OBJS)

.SECONDEXPANSION:
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $$(wildcard $(INCDIR)/$$*.h)
	@echo "Checking $? formatting, 'clang-format' must be installed, call 'make format' if it fails here..."
	@ clang-format --help 2>&1 > /dev/null
	@! clang-format $? -style=LLVM -output-replacements-xml | grep -c "<replacement " > /dev/null
	mkdir -p $(OBJDIRS)
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
	docker-compose -f docker-compose.devel.yml up -d --remove-orphans

docker/down:
	docker-compose -f docker-compose.devel.yml down

docker/clean: docker/up clean
	docker-compose -f docker-compose.devel.yml exec harvester $(MAKE) clean

docker/%: docker/up docker/movejson
	docker-compose -f docker-compose.devel.yml exec harvester $(MAKE) ${BINDIR}/$*
	@echo "#!/bin/bash" > ${BINDIR}/$*
	@echo "docker-compose -f docker-compose.devel.yml exec harvester ${BINDIR}/$* \"\$${@:1}\"" >> ${BINDIR}/$*
	@chmod a+wx ${BINDIR}/$*

docker/bash: docker/up
	docker exec -it harvester bash

docker/movejson: docker/up
	@docker-compose -f docker-compose.devel.yml exec harvester mkdir -p $(APPDIR)
	@docker-compose -f docker-compose.devel.yml exec harvester cp -r /project/data/apis $(APPDIR)/
	@docker-compose -f docker-compose.devel.yml exec harvester cp /project/data/api_schema.json $(APPDIR)/

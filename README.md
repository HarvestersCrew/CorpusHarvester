# CorpusHarvester
CS Engineering School Project

## Makefile

### Build and run methods
- `make`: builds the native executable, dependencies need to be resolved on the machine if something goes wrong.
- `make docker`: builds the executable in Docker.

Both those methods resolve in making an executable in `bin/harvester`. The first one is native to the machine, the second one is running through Docker. You may have to apply `sudo` to both Docker methods, depending on how you usually run it. Finally, the Docker executable passes through all parameters and should behave the same way as the native one.

### Phony methods
- `make clean`: cleans `bin` and `obj` folders.
- `make format`: formats all code to the LLVM standard. Note that any compiling file is also checked and won't pass if not applying to LLVM.

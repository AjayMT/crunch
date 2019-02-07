
# crunch

Memory profiling tool for macOS, inspired by valgrind's memcheck.

## Installation
You will need:
- Python 3
- [docopt](http://docopt.org) (`pip3 install docopt`)
- [jinja2](http://jinja.pocoo.org/) (`pip3 install jinja2`)
- GNU make
- gcc

```sh
git clone https://github.com/AjayMT/crunch.git
cd crunch
pip3 install docopt jinja2
python3 install.py
```

The install script (`install.py`) will guide you through the installation process.

## Usage and mechanism
```
crunch [options] <program> [<arguments>...]
```

(Running `crunch --help` will print a help message with a description of all the options.)

crunch will execute `program` with the specified arguments. `program` runs as normal, except:
- `malloc` and `free` are 'overridden' during runtime (see [DYLD_INTERPOSE](https://opensource.apple.com/source/dyld/dyld-97.1/include/mach-o/dyld-interposing.h.auto.html)) and wrapped with extra instrumentation
  - allocations are tracked as pointer+size pairs in a hash table. Making malloc and free slower and/or more expensive is pretty much unavoidable, but a better hash table implementation will likely improve performance significantly (see `src/crunch_set.c`)
  - as of now crunch also synchronizes calls to malloc and free, which severely impacts concurrent programs. There is probably a better way to approach multi-threading than wrapping malloc and free in a mutex
- signal handlers are attached to the following signals:
  - SIGSEGV and SIGABRT: crunch will report an abnormal (fatal) exit and record the offending memory address (i.e the pointer that caused the segfault/abort)

Upon exit, crunch will create a `crunch_out` directory and store its results there.

```sh
crunch_out
├── fatal # this file only exists if the program exits abnormally
├── heap # this folder stores the contents of the heap upon exit, each file is an address
│   └── 0x7fa45ec02af0
├── report.html
└── stats
```

Each file in the `heap` directory stores the contents of a single allocation remaining on the heap. `report.html` reports all of the information recorded by crunch.

**Example usage:**

```c
// /tmp/program.c
#include <stdlib.h>

int main(int argc, char *argv[])
{
  char *buf = malloc(10);
  buf[0xffffff] = 'a';
  free(buf);
  return 0;
}
```

```
$ cc program.c
$ crunch a.out

crunch: SIGSEGV illegal access: address 0x7f828b402aef

writing crash info...
crash info written to /tmp/crunch_out/fatal

=== crunch ===
1 remaining heap allocations
  0x7f828a402af0 : 10 bytes

dumping heap...
heap contents written to /tmp/crunch_out/heap
crunch report written to /tmp/crunch_out/report.html
```

## Roadmap
### planned features
- virtualize resources for better instrumentation?
- track memory usage over time
- view heap snapshots at any point of time during execution

### misc
- create a Homebrew formula
- measure performance

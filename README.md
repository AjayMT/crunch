
# crunch

Memory profiling tool for macOS, inspired by valgrind's memcheck.

**Note:** This is pre-alpha software and is not ready for use yet!

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

Eventually, I will package this into a Homebrew-installable tool.

## Roadmap
### mechanism
- replace malloc/free and other memory management functions
- virtualize resources?

### planned features
- **high priority**: detect memory leaks
- track memory usage
- view heap snapshots

#!/usr/bin/env python3

"""
Crunch: the memory profiling tool.

Usage: crunch [options] <program>

Options:
  --help, -h    Print this help message
  --version     Print version number
"""

import sys
import os
import subprocess
from docopt import docopt


version = '0.0.1'
dylib_name = 'crunch.dylib'
dyld_var = 'DYLD_INSERT_LIBRARIES'


def main(args):
    dylib_path = os.path.join(sys.path[0], dylib_name)
    prog_env = os.environ.copy()
    prog_env[dyld_var] = dylib_path
    subprocess.run(args['<program>'], env=prog_env)


if __name__ == '__main__':
    args = docopt(__doc__, argv=sys.argv[1:], version=version)
    main(args)

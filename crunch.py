#!/usr/bin/env python3

"""
Crunch: the memory profiling tool.

Usage: crunch [options] <program> [<arguments>...]

Options:
  --help, -h       Print this help message
  --version        Print version number
  --no-dump, -n    Don't dump heap contents on exit
  -i <file>        Pipe <file> to program's stdin
  -o <file>        Pipe program's stdout to <file>
"""

import sys
import os
import shutil
import subprocess
from docopt import docopt


version = '0.0.1'
dyld_var = 'DYLD_INSERT_LIBRARIES'
dylib_name = 'crunch.dylib'
out_var = 'CRUNCH_OUT'
out_name = 'crunch_out'
out_flag_name = 'CRUNCH_DUMP_HEAP'


def main(args):
    dylib_path = os.path.join(sys.path[0], dylib_name)
    out_path = os.path.join(os.getcwd(), out_name)
    stdin = None
    stdout = None

    if args['-i']:
        stdin = open(args['-i'])

    if args['-o']:
        stdout = open(args['-o'], 'w')

    prog_env = os.environ.copy()
    prog_env[dyld_var] = dylib_path
    prog_env[out_var] = out_path
    prog_env[out_flag_name] = 'y'

    if args['--no-dump']:
        prog_env[out_flag_name] = 'n'
    else:
        if os.path.exists(out_path):
            shutil.rmtree(out_path)

        os.makedirs(out_path)

    subprocess.run(
        [args['<program>']] + args['<arguments>'],
        env=prog_env,
        stdin=stdin,
        stdout=stdout
    )


if __name__ == '__main__':
    args = docopt(__doc__, argv=sys.argv[1:], version=version)
    main(args)

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
import json
import jinja2
from docopt import docopt


version = '0.0.1'
dyld_var = 'DYLD_INSERT_LIBRARIES'
dylib_name = 'crunch.dylib'
out_var = 'CRUNCH_OUT'
out_name = 'crunch_out'
out_flag_name = 'CRUNCH_DUMP_HEAP'
template_filename = 'template.html'
report_filename = 'report.html'


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

    if not args['--no-dump']:
        create_report(
            ' '.join([args['<program>']] + args['<arguments>']),
            out_path
        )


def create_report(exec_name, out_path):
    template_path = os.path.join(sys.path[0], template_filename)
    report_path = os.path.join(out_path, report_filename)

    ptrs = os.listdir(out_path)
    heap = {}
    for ptr in ptrs:
        with open(os.path.join(out_path, ptr), 'rb') as heap_data:
            heap[ptr] = decode_bytes(heap_data.read())

    with open(template_path) as template_file:
        template = jinja2.Template(template_file.read())
        with open(report_path, 'w') as report_file:
            output = template.render(
                exec_name=exec_name,
                ptrs=ptrs,
                heap_json=json.dumps(heap)
            )
            report_file.write(output)


def decode_bytes(data):
    # TODO find better way to decode bytes
    out = ''
    for b in data:
        try:
            out += bytes([b]).decode('utf-8')
        except:
            out += '?'

    return out


if __name__ == '__main__':
    args = docopt(__doc__, argv=sys.argv[1:], version=version)
    main(args)

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
        report_path = create_report(
            ' '.join([args['<program>']] + args['<arguments>']),
            out_path
        )
        sys.stderr.write(f'crunch report written to {report_path}\n')


def create_report(exec_name, out_path):
    template_path = os.path.join(sys.path[0], template_filename)
    report_path = os.path.join(out_path, report_filename)
    ptrs = [p for p in os.listdir(out_path) if p not in ['stats', 'fatal']]
    heap = {}
    stats = []
    fatal = False
    fatal_msg = ''
    fatal_path = os.path.join(out_path, 'fatal')

    if os.path.exists(fatal_path):
        with open(fatal_path) as fatal_file:
            fatal = True
            fatal_msg = f'SIGSEGV: illegal access: address <strong>{fatal_file.read()}</strong>'

    with open(os.path.join(out_path, 'stats')) as stats_file:
        stats = [int(stat) for stat in stats_file.read().splitlines()]

    for ptr in ptrs:
        with open(os.path.join(out_path, ptr), 'rb') as heap_data:
            data = heap_data.read()
            heap[ptr] = {'data': decode_bytes(data), 'size': len(data)}

    with open(template_path) as template_file:
        template = jinja2.Template(template_file.read())
        with open(report_path, 'w') as report_file:
            output = template.render(
                exec_name=exec_name,
                ptrs=ptrs,
                heap_json=json.dumps(heap),
                malloc_count=stats[0],
                free_count=stats[1],
                max_usage=stats[2],
                current_usage=stats[3],
                fatal=fatal,
                fatal_msg=fatal_msg
            )
            report_file.write(output)
            return report_path


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

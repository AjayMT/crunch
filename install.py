
import crunch
import subprocess
import sys
import os
import shutil
import pip
import importlib


def main():
    print(f'Crunch {crunch.version} install script\n')
    make = check_req('make', 'GNU Make', ['make', '--version'])
    gcc = check_req('gcc', '', ['gcc', '--version'])

    print('Building crunch...')
    subprocess.run(make, env={'GCC': gcc})

    print('\nInstalling crunch...')
    install('~/.crunch/', '/usr/local/bin/')


def check_req(name, prefix, cmd):
    print(f'Checking for {name}... ', end='', flush=True)
    out = subprocess.run(cmd, capture_output=True, text=True).stdout
    if not out.startswith(prefix):
        print(f'{name} not found.')
        alt = input(f'If {name} is a differently named executable, please specify: ')
        if len(alt) == 0:
            sys.exit(1)

        return check_req(name, prefix, [alt] + cmd[1:])

    print(f'{name} found.')
    return name


def install(install_prefix, symlink_prefix):
    print(f'  crunch will be installed to {install_prefix}')
    print(f"  `crunch' executable will be symlinked to {symlink_prefix}")
    proceed = input("Proceed? (Y/n) ")
    if proceed == 'n':
        install_prefix = input('Enter installation directory: ')
        symlink_prefix = input('Enter symlink directory: ')
        install(install_prefix, symlink_prefix)
        return

    # perform installation
    install_prefix = os.path.abspath(os.path.expanduser(install_prefix))
    symlink_prefix = os.path.abspath(os.path.expanduser(symlink_prefix))

    if not os.path.exists(install_prefix):
        print(f'Creating {install_prefix}... ', end='', flush=True)
        os.makedirs(install_prefix)
        print('done.')

    print('Copying files... ', end='', flush=True)
    shutil.copy2(os.path.join(sys.path[0], crunch.dylib_name), install_prefix)
    exec_path = shutil.copy2(os.path.join(sys.path[0], 'crunch.py'), install_prefix)
    print('done.')

    print("Symlinking `crunch'... ", end='', flush=True)
    os.symlink(exec_path, os.path.join(symlink_prefix, 'crunch'))
    print('done.')


if __name__ == '__main__':
    main()

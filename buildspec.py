import os
import stat
import shutil
import subprocess

basedir = os.path.dirname(os.path.abspath(__file__))
target = os.path.join(basedir, 'busybox', '_install')
for dir in os.listdir(os.path.join(basedir, 'test')):
    path = os.path.join(basedir, 'test', dir)
    if os.path.isdir(path):    
        subprocess.run(['make'], cwd=path, shell=True)
        shutil.copy(os.path.join(path, f'{dir}.ko'), target)
        os.chmod(os.path.join(target, f'{dir}.ko'), stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO)

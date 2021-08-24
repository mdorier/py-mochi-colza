from distutils.core import setup
from distutils.extension import Extension
from distutils.sysconfig import get_config_vars
from distutils.command.build_clib import build_clib
from distutils.command.build_ext import build_ext
import numpy
import pybind11
from mpi4py import get_include as mpi4py_get_include
import pkgconfig
import os
import os.path
import sys

(opt,) = get_config_vars('OPT')
os.environ['OPT'] = " ".join(flag for flag in opt.split() if flag != '-Wstrict-prototypes')

colza_client = pkgconfig.parse('colza-client')
colza_admin  = pkgconfig.parse('colza-admin')

extra_compile_args=['-std=c++14']
if sys.platform == 'darwin':
    extra_compile_args.append('-mmacosx-version-min=10.9')

colza_admin_ext = Extension('_colza_admin',
        ['pycolza/src/admin_module.cpp'],
        libraries=colza_admin['libraries'],
        library_dirs=colza_admin['library_dirs'],
        include_dirs=colza_admin['include_dirs'] + [pybind11.get_include()],
        language='c++14',
        extra_compile_args=extra_compile_args,
        depends=[])

colza_client_ext = Extension('_colza_client',
        ['pycolza/src/client_module.cpp'],
        libraries=colza_client['libraries'],
        library_dirs=colza_client['library_dirs'],
        include_dirs=colza_client['include_dirs'] + [pybind11.get_include(), numpy.get_include(), mpi4py_get_include()],
        language='c++14',
        extra_compile_args=extra_compile_args,
        depends=[])

setup(name='py-colza',
      version='0.1',
      author='Matthieu Dorier',
      description='''Python interface for the Sonata Mochi service''',
      ext_modules=[ colza_client_ext,
                    colza_admin_ext,
                  ],
      packages=['pycolza']
    )

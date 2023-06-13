from setuptools import Extension, setup

module = Extension("mykmeanssp", sources=['kmeansmodule_1.c'])
setup(name='mykmeanssp',
     version='1.0',
     description='Python wrapper for custom C extension',
     ext_modules=[module])
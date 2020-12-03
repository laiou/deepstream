from distutils.core import setup
setup (name = 'pyds',
       version = '1.0.1',
       description = """Install precompiled DeepStream Python bindings extension""",
       packages=[''],
       package_data={'': ['pyds.so']},
       )

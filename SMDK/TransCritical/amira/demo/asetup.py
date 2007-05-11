# setup.py
#  Run from dos shell with python setup.py py2exe -k

from distutils.core import setup
import py2exe

setup(name="TTBayer",
      scripts=["amira.py"],
      data_files=[(".",
                   ["./amira.ico", "./amira.dll"])],

      windows=["amira.py"]
)

#      icon_resources=[(1, "e3000.ico")]

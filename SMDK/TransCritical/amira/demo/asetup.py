# setup.py
#  Run from dos shell with python setup.py py2exe -k

from distutils.core import setup
import py2exe

setup(name="TTBayer",
      scripts=["MBayer.py"],
      data_files=[(".",
                   ["./mb.ico", "./MurdochBayer.dll", "./kw.ico"])],

      windows=["MBayer.py"]
)

#      icon_resources=[(1, "e3000.ico")]

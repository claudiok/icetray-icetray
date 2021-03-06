#!/usr/bin/env python

from I3Tray import I3Tray
from icecube import dataio
import sys

tray = I3Tray()

tray.AddModule("BottomlessSource", "bs")
tray.AddModule("AddNulls", "N",
               Where = ['foo'],
               IcePickServiceKey = "key",
               If = lambda f: True)
tray.AddModule("Dump", "d")
tray.AddModule("TrashCan", "tc")

try:
    tray.Execute(1)
except:
    print("ok, threw as expected")
    sys.exit(0)
else:
    sys.exit(1)



#!/usr/bin/env python
# -*- coding: utf-8 -*-
__author__ = 'LiDong'
import os
import sys
import hrpyc

if os.environ.get('DEV'):
    import hrpyc
    connection, hou = hrpyc.import_remote_module()
    ui = connection.modules.hou.ui
    hou.ui = ui
    sys.modules['hou'] = hou
else:
    import hou
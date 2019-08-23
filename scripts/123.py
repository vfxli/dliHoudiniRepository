# coding=utf8

# import os
# import sys
# import hou

#添加root_path
# try:
#     root_path = os.environ['ROOT_PATH']
# except KeyError, e:
#     root_path = r'W:\app_config\release\houdini'
#     os.environ['ROOT_PATH'] = root_path
#添加共用和私有的python库
#sys.path.append(os.path.join(root_path, 'cgtkLibs'))
#sys.path.append(os.path.join(root_path, 'pyLibs'))

# 启动hrpyc
#if hou.isUIAvailable():
#    import hrpyc
#    hrpyc.start_server()

# 关闭更改otl权限
# if hou.hda.safeguardHDAs() is False:
#    hou.hda.setSafeguardHDAs(True)

# 启动刷新gallery的服务
# from gal_exchange import Watcher
# from threading import Event
# event  = Event()
# hou.session.gal_exchange_watcher = Watcher(event)
# hou.session.gal_exchange_event = event
# hou.session.gal_exchange_watcher.start()

# 启动autosavebackup的服务
# from autosavebackup import Watcher
# from threading import Event
# event  = Event()
# hou.session.autosavebackup_watcher = Watcher(event)
# hou.session.autosavebackup_event = event
# hou.session.autosavebackup_watcher.start()

#输出运行记录
#import cgtk_log
#log = cgtk_log.cgtk_log('run_houdini')
#log.debug("debug")

# coding:utf8

import os
root = r'X:\TempProj_TP_171121\_INPUT\20171209\1208\project\3D\thumbnail\E_zhanzhengxiugai'

imgs = os.listdir(root)
cwd = os.getcwd()
print cwd
with open(r"%s\img.txt" % cwd,"w") as file:
	for each in imgs:
		fp = root + "\\" + each
		code = '<table><img src="%s" width="120" height="76"><table>' % fp
		print
		file.write(code)
		file.write("\n")

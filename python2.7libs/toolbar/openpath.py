# -*- coding: utf-8 -*-
# Author: Roffive
# date: 2016/11/29
# e-mail: roffive.vfx@foxmail.com

import hou
import os

def main():
        node = hou.selectedNodes()[0]
        lsparms = node.parms()
        ls = [parm.name() for parm in lsparms]
        parmNameList = ['fileName','filename','file','sopoutput','vm_picture','RS_outputFileNamePrefix',
                        'vm_uvoutputpicture1','vm_uvoutputobject1','vm_background',
                        'bgimage','picture','ri_display','wr_picture','output',
                        'arch_geofile1','arch_matfile1','arch_archive1',
                        'hq_hfs','hq_outputifd','alf_spool','alf_diskfile',
                        'dopoutput','dsm_output','copoutput',
                        'dif_map','opacmap','basecolor_texture','ior_texture',
                        'rough_texture','aniso_texture','anisodir_texture',
                        'metallic_texture','reflecttint_texture','coat_texture',
                        'coatrough_texture','transparency_texture','transcolor_texture',
                        'transdist_texture','dispersion_texture','sss_texture','sssdist_texture',
                        'ssscolor_texture','sheen_texture','sheentint_texture','emitcolor_texture']
        parmName = [val for val in ls if val in parmNameList]
        for parm in parmName:
                val = hou.parm(node.path()+"/"+parm)
                pathStr = val.evalAsString()
                if pathStr != '':
                        if "\\" in pathStr:
                                temp1 = pathStr.split("\\")
                                pathStr = "/".join(temp1)
                        if "/" in pathStr:
                                pathsp = pathStr.split('/')[:-1]
                                path = "\\".join(pathsp)
                                if os.path.isdir(path):
                                        os.startfile(path)
                                else:
                                        choice = hou.ui.displayMessage(text="路径不存在",
                                                                                buttons=["创建该目录并打开","什么都不做"])
                                        if choice==0:
                                                os.makedirs(path)
                                                os.startfile(path)
                                        else:
                                                pass
                        if "/" not in pathStr:
                                if "\\" not in pathStr:
                                        hou.ui.displayMessage(text="这不是一个有效路径",buttons=["关闭"])


if __name__ =='__main__':
    main()
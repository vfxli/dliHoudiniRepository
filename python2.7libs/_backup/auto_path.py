# -*- coding: utf-8 -*-

import re,os,hou
def main():
    rootpath_render = "W:/"
    project_path = r"\\chengpin-server\x"
    rootpath_file = "\\\\\\\\192.168.31.110\\\\z\\\\"
    hfn = hou.hipFile.name()
    hfp = os.path.dirname(hfn)
    hbfn = os.path.splitext(hou.hipFile.basename())[0]
    reg = re.compile("[A-Za-z0-9]{1,10}_[0-9]{3}_[0-9]{4}_(vfx|lgt)_[A-Za-z0-9]{2}_v[0-9]{3}_[A-Za-z]{3}")
    obj = re.match(reg,hbfn)
    renderpath = None
    filepath = None
    if obj is not None:
        pathlst = obj.group().split("_")
        proj_name = pathlst[0].upper()
        proj_name_short = proj_name
        pn = None
        dirs = [d for d in os.listdir(project_path) if os.path.isdir(project_path+"/"+d)]
        for each in dirs:
            if "_"+proj_name+"_" in each:
                pn = each
                break
        if pn is not None:
            proj_name = pn
        else:
            hou.ui.displayMessage(text="不存在%s这个项目，请检查文件命名是否正确!" % proj_name, buttons=["ok"])
            return
        sc_name = pathlst[1]
        sh_name = pathlst[2]
        task_name = pathlst[3].lower()
        des_name = pathlst[4].lower()
        ver_name = pathlst[5].lower()
        usr_name = pathlst[6].lower()
                
        renderpath = rootpath_render + proj_name + "/" + sc_name + "/" + sh_name + "/3d/" + task_name + "/rendering/" + usr_name + "/v001/$OS/" + proj_name_short + "_" + sc_name + "_" + sh_name + "_$OS.$F4.exr"
        filepath = rootpath_file + proj_name + "/" + sc_name + "/" + sh_name + "/" + usr_name + "/" + des_name
    nodes = hou.selectedNodes()
    if nodes is not None:
        for nd in nodes:
            ndpath = nd.path()
            if renderpath is not None:
                if nd.type().name() in ["ifd"]:
                    parm = hou.parm("%s/vm_picture" % ndpath)
                    parm.set("%s" % renderpath)
                elif nd.type().name() in ["Redshift_ROP"]:
                    parm = hou.parm("%s/RS_outputFileNamePrefix" % ndpath)
                    parm.set("%s" % renderpath)
                elif nd.type().name() in ["filecache","file"]:
                    parm = hou.parm("%s/file" % ndpath)
                    parm.set("%s/v001/$OS/$OS.$F.bgeo.sc" % filepath)
                elif nd.type().name() in ["rop_geometry","geometry"]:
                    parm = hou.parm("%s/sopoutput" % ndpath)
                    parm.set("%s/v001/$OS/$OS.$F.bgeo.sc" % filepath)
                elif nd.type().name() in ["rop_alembic"]:
                    parm = hou.parm("%s/filename" % ndpath)
                    parm.set("%s/v001/$OS/$OS.abc" % filepath)
                else:
                    hou.ui.displayMessage(text="不支持此类型的节点，只能是:Mantra,file,filecache,rop_geometry,geometry,rop_alembic!",buttons=["ok"])
                    return
            else:
                hou.ui.displayMessage(text="你的hip文件命名不规范，请规范命名之后再试!",buttons=["ok"])
                return

    else:
        hou.ui.displayMessage(text="请至少选择一个输出节点，比如：Mantra,filecache... !",buttons=["ok"])
        return

if __name__ =='__main__':
    main()

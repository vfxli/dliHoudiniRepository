# -*- coding: utf-8 -*-

import re,os,hou
def main():
    reg = re.compile("v[0-9]{1,3}")
    parmlist = ["vm_picture","file","sopoutput","filename1","RS_outputFileNamePrefix"]
    nodes = hou.selectedNodes()
    if nodes is not None:
        for nd in nodes:
            ndpath = nd.path()
            for parm in nd.parms():
                if parm.name() in parmlist:
                    code = parm.asCode()
                    val = code.split('hou_parm.set("')[1].split('")')[0]
                    vlst = re.findall("v[0-9]{1,3}",val)
                    if vlst is not None:
                        new_vlst = []
                        for v in vlst:
                            num = int(v[1:])
                            pad = "%s0%dd" % ("v%",len(v[1:]))
                            
                            if num > 1:
                                nv = pad % (num-1)
                            else:
                                nv = pad % num
                            new_vlst.append(nv)
                        tmp1 = re.split("v[0-9]{1,3}",val)
                        i = 1
                        for v in new_vlst:
                            tmp1.insert(i,v)
                            i = i+2
                        val = "".join(tmp1)
                        parm.set(val)

if __name__ =='__main__':
    main()
# -*- coding: utf-8 -*-

import hou

def main():
    try:
        cam_nd = hou.selectedNodes()[0]
        aperture = cam_nd.parm("aperture")
    except:
        hou.ui.displayMessage(text="请选择一个相机")
    else:
        shutter = hou.ui.readInput('设置相机视角,maya相机视角', buttons=('确定', '取消'), title='maya相机视角',
                                          initial_contents='54.43', close_choice=1, default_choice=0)
        if shutter[0] == 1:
            return
        angle = 0
        try:
            angle = (float)(shutter[1])
        except:
            hou.ui.displayMessage(text="相机视角是一个浮点数据，请输入一个数值")
            main()
        else:
            aperture.setExpression('tan(%f*0.5)*ch("focal")*2' % angle)
            
if __name__ =='__main__':
    main()
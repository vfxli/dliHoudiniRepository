# -*- coding: utf-8 -*-

import hou,re

def new_cam(i, j, cam, crop):
    root = hou.node("/obj")
    name = "Jigsaw_Camera_%d_%d" % (i,j)
    new_cam = root.createNode('cam',name)
    new_cam.move((i*4,j))
    for parm in cam.parms():
        parm_name = parm.name()
        if parm_name == "cropl":
            new_cam.parm(parm_name).set(crop[0])
        elif parm_name == "cropr":
            new_cam.parm(parm_name).set(crop[1])
        elif parm_name == "cropb":
            new_cam.parm(parm_name).set(crop[2])
        elif parm_name == "cropt":
            new_cam.parm(parm_name).set(crop[3])
        elif parm_name in ["tx","ty","tz","rx","ry","rz","sx","sy","sz","px","py","pz","iconscale","resx","resy","aspect","projection",
                           "focal","aperture","orthowidth","near","far","winx","winy","winsizex","winsizey",
                           "shutter","focus","fstop"]:
            exp = 'ch("%s/%s")' % (cam.path(), parm_name)
            new_cam.parm(parm_name).setExpression(exp)
    return new_cam

def new_mantra(i, j, mantra, cam):    
    root = hou.node("/out")
    name = "Jigsaw_Mantra_%d_%d" % (i,j)
    new_mantra = root.createNode('ifd', name)
    new_mantra.move((i*4,j))
    new_mantra.parm("camera").set(cam)
    
    mantra_name = mantra.name()
    code = mantra.parm("vm_picture").asCode()
    val = code.split('hou_parm.set("')[1].split('")')[0]
    #正则匹配（$F4.exr）
    reg = re.compile("\$F[1-9]?\.[A-Za-z]{3}")
    piclst = re.findall(reg,val)
    if piclst is not None:
        tmp1 = re.split(reg,val)
        tmp2 = tmp1[0]
        if '$OS' in tmp2:
            tmp2 = tmp2.replace('$OS',mantra_name)
        vm_pic_path = tmp2 + ("Jigsaw_%d_%d." % (i,j)) + piclst[0]
        new_mantra.parm("vm_picture").set(vm_pic_path)
    return new_mantra
    
def main():
    try:
        nd = hou.selectedNodes()[0]
        cam = None
        try:
            cam_path = nd.parm("camera")
            cam_path = cam_path.eval()
            cam = hou.node(cam_path)
            if cam == None:
                hou.ui.displayMessage(text="Mantra相机参数无效，请指定一个有效相机")
                return
        except:
            hou.ui.displayMessage(text="请选择一个Mantra节点")
            return
    except:
        hou.ui.displayMessage(text="请选择一个Mantra节点")
        return
    else:
        shutter = hou.ui.readInput('设置相机拼图方式\n横向分段-竖向分段', buttons=('确定', '取消'), title='多相机渲染',
                                      initial_contents='2-2', close_choice=1, default_choice=0)
        x = (int)(shutter[1].split('-')[0])
        y = (int)(shutter[1].split('-')[1])
        x_mv = 1.0/x
        y_mv = 1.0/y
        camfllow = []
        fllow = []
        for i in range(0,x):
            for j in range(0,y):
                lc = x_mv*i
                rc = x_mv*(i+1)
                bc = y_mv*j
                tc = y_mv*(j+1)
                lst = (lc,rc,bc,tc)
                cam_pth = new_cam(i, j, cam, lst)
                camfllow.append(cam_pth)
                mantra = new_mantra(i, j, nd, cam_pth.path())
                fllow.append(mantra)
        #设置最后一个manta节点颜色为红色
        last_mantra_color = hou.Color()
        last_mantra_color.setRGB( (1, 0, 0) )
        fllow[-1].setColor(last_mantra_color)
        
        #串联mantra节点
        for m in fllow:
            if m==fllow[0]:
                continue
            i = fllow.index(m)
            m.setInput(0,fllow[i-1])
        #创建out下的stickyNote
        pos1 = fllow[0].position()
        pos2 = fllow[-1].position()
        size = abs(pos2[0] - pos1[0]),abs(pos2[1] - pos1[1])
        out = hou.node("/out")
        note = out.createStickyNote()
        color = hou.Color()
        color.setRGB( (0.03,0.04,0.1) )
        note.setColor(color)
        note.setPosition( (pos1[0]-1, pos1[1]-1) )
        note.setSize( (size[0]+5, size[1]+3) )
        color1 = hou.Color()
        color1.setRGB( (1, 0.8, 0.3) )
        note.setText("Render Jigsaw puzzle")
        note.setTextColor(color1)
        #创建obj下的stickynote
        pos1 = camfllow[0].position()
        pos2 = camfllow[-1].position()
        size = abs(pos2[0] - pos1[0]),abs(pos2[1] - pos1[1])
        obj = hou.node("/obj")
        note1 = obj.createStickyNote()
        note1.setColor(color)
        note1.setPosition( (pos1[0]-1, pos1[1]-1) )
        note1.setSize( (size[0]+5, size[1]+3) )
        note1.setText("Jigsaw Cameras")
        note1.setTextColor(color1)

if __name__ =='__main__':
    main()

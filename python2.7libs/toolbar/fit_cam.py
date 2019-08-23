# -*- coding: utf-8 -*-

import os
import sys
import hou


#import init_hrpyc
import toolutils

class CamFit(object):
    def __init__(self, selnode, cam):
        self.node = cam
        self.parent = cam.parent()
        self.selnode = selnode
        self.obj = selnode.parent()
        self.cam_scale = None
    
    def get_scale(self):
        if self.selnode.inputs() != ():
            inputnode = self.selnode.inputs()[0]
            if "scale" in [p.name() for p in inputnode.parms()]:
                parm = inputnode.parm("scale")
                if parm.evalAsFloat() != 0:
                    self.cam_scale = parm.path()

    def setfit(self, resx, resy, move_iter):
        self.node.setDisplayFlag(False)

        self.node.parm(self.node.path() + "/resx").set(resx)
        self.node.parm(self.node.path() + "/resy").set(resy)

        camups = self.node.inputAncestors()
        if self.parent.name() == 'obj':
            if camups == ():
                camup = self.node
            else:
                camup = camups = self.node.inputAncestors()[-1]
        else:
            camup = self.selnode
        self.get_scale()
        null = None
        if camup.inputs() == ():
            null = self.obj.createNode('null', 'ReWorld')
            null.move(camup.position() + hou.Vector2(0, 1))
            camup.setNextInput(null)
            null.setDisplayFlag(False)

        blend =  self.obj.createNode('blend', 'Blend_position')
        fetch =  self.obj.createNode('fetch', 'Fetch_NewCam')
        newCam =  self.obj.createNode('cam', 'RenderCamera')

        if self.parent.name() == 'obj':
            move_centroid = self.node.position()
        else:
            move_centroid = self.selnode.position()
        fetch.move(move_centroid + hou.Vector2(move_iter, -1))
        blend.move(move_centroid + hou.Vector2(move_iter, -2))
        newCam.move(move_centroid + hou.Vector2(move_iter, -3))

        blend.setNextInput(fetch)
        newCam.setNextInput(blend)

        blend.setDisplayFlag(False)
        fetch.setDisplayFlag(False)
        if self.cam_scale is not None:
            blend.parm("scale").setExpression('1/ch("%s")' % self.cam_scale)
        else:
            blend.parm("scale").setExpression('1/ch("/obj/%s/scale")' % null.name())
        blend.parm(blend.path() + "/blendm1").set(63)
        fetch.parm(fetch.path() + "/useinputoffetched").set(1)
        fetch.parm(fetch.path() + "/fetchobjpath").set(self.node.path())

        oldCamPath = self.node.path()
        relativePath = newCam.relativePathTo(self.node)
        resx = " ch(\"" + relativePath + "/resx\")"
        resy = " ch(\"" + relativePath + "/resy\")"
        focal = " ch(\"" + relativePath + "/focal\")"
        aperture = " ch(\"" + relativePath + "/aperture\")"
        vm_background = "`ch(\"" + relativePath + "/vm_background\")`"
        newCam.parm('vm_background').set(vm_background)
        newCam.setParmExpressions(dict(resx=resx, resy=resy, focal=focal,
                                       aperture=aperture))

        newCam.parm("vm_bgenable").set(0)
        newCam.parm("vm_bgenable").set(0)
        newCam.parm("vm_bgenable").lock(True)

def getcamlist(selnode):
    resultlist = []
    selnodetype = selnode.type().name()
    if selnodetype == 'cam':
        resultlist.append(selnode)
        return resultlist
    elif selnodetype == 'alembicarchive':
        allsubnode = selnode.allSubChildren()
        for node in allsubnode:
            if node.type().name() == 'cam':
                resultlist.append(node)
        return resultlist
def main():
    view = toolutils.sceneViewer()
    selnodes = view.selectObjects('请选择一个cam或包含cam的AlembicArchive', allowed_types=['cam'])
    if len(selnodes) > 0:
        resolution = hou.ui.readInput('set Resolution', buttons=('Set', 'close'), title='set Resolution',
                                      initial_contents='1920-1080', close_choice=1, default_choice=0)
        resx = resolution[1].split('-')[0]
        resy = resolution[1].split('-')[1]
        for selnode in selnodes:
            print selnode.name()
            camlist = getcamlist(selnode)
            move_iter = 0
            for cam in camlist:
                print cam.name()
                camset = CamFit(selnode, cam)
                if resolution[0] == 0:
                    camset.setfit(resx, resy, move_iter)
                    move_iter += 2
            del camlist[:]


if __name__ =='__main__':
    main()
# -*- coding: utf-8 -*-

import hou

def main():
    nodes = hou.selectedNodes()
    for node in nodes:
        geoname = node.name()
        geo = hou.node('/obj').createNode('geo', geoname)
        geo.setDisplayFlag(False)
        mantra = hou.node('/out').createNode('ifd', geoname)
        mantra.parm('forceobject').set(geoname)
        mantra.parm('vobject').set('')
        mantra.parm('alights').set('')
        objectnode = geo.createNode('object_merge', geoname)
        objectnode.parm('objpath1').set(node.path())
        childrens = geo.children()
        for children in childrens:
            if children.name() == 'file1':
                children.destroy()


if __name__ =='__main__':
    main()
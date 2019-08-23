# -*- coding: utf-8 -*-

import colorsys,random
import hou

def main():
    nodes = hou.selectedNodes()
    nodesNum = len(nodes)

    for i in range(0,nodesNum):
        node = nodes[i]
        
        type = node.name().split("_")[0]
        color = colorsys.hsv_to_rgb(random.random(), random.random(), random.random())
        clr = hou.Color(color)
        
        if(type != "colorize"):
            newNode = node.createOutputNode("color","colorize_0")
            newNode.setColor(clr)
            
            newNode.parm("colorr").set(color[0])
            newNode.parm("colorg").set(color[1])
            newNode.parm("colorb").set(color[2])
        
        else:
            node.setColor(clr)
            node.parm("colorr").set(color[0])
            node.parm("colorg").set(color[1])
            node.parm("colorb").set(color[2])
            
if __name__ =='__main__':
    main()
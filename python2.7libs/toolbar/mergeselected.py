# -*- coding: utf-8 -*-

import hou

def main():
	node_selected = hou.selectedNodes()
	parent = node_selected[0].node('..')
	out_merge = parent.createNode('merge','merge')
	overpos = hou.Vector2(0,0)
	for node in node_selected:
	        out_merge.setNextInput(node)
	        node.setSelected(False)
	        overpos += node.position()
	temp = len(node_selected)
	newpos = overpos * (1.0/temp)
	out_merge.setPosition(newpos)
	out_merge.move([0,-2])
	out_merge.setSelected(True)
	out_merge.setDisplayFlag(True)
	out_merge.setRenderFlag(True)


if __name__ =='__main__':
    main()
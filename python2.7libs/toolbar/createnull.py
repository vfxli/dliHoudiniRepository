# -*- coding: utf-8 -*-

import hou
def main():
	node_selected = hou.selectedNodes()
	for node in node_selected:
	    parent = node.node('..')
	    out_null = parent.createNode('null', 'OUT_0')
	    out_null.setNextInput(node)
	    out_null.setPosition(node.position())
	    out_null.move([0, -1])
	    out_null.setSelected(True)
	    node.setSelected(False)
	    new_color = hou.Color([0, 0, 0])
	    out_null.setColor(new_color)

if __name__ =='__main__':
    main()
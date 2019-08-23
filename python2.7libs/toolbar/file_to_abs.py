# -*- coding: utf-8 -*-

import hou

def main():
	nodes = hou.selectedNodes()
	for node in nodes:
	    file_path = node.parm('file').eval();
	    temp = file_path.split('.');
	    temp[-2] = '$F';
	    file_path = '.'.join(temp);
	    abs_file = node.parent().createNode('file');
	    abs_file.parm('file').set(file_path);
	    abs_file.move(node.position() + hou.Vector2(-3,0));

if __name__ =='__main__':
    main()
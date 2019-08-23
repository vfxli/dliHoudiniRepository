# -*- coding: utf-8 -*-
#    Athor：  roffive
#     date：  2016/11/15
#       QQ:  670729017
# function:  将选择的节点用objmerge引用到另一个geo里面，第1步
#     note:  在‘smile—滴水’教程的基础上加入了‘null’节点引入到geo
#            可以选择merge类型,one by one或者together

import hou,tempfile

def main():
    sel_nodes = hou.selectedNodes()
    path = []
    #下面是一个递归函数
    def search_children(node):
        if node.children():
            for ch in node.children():
                if ch.type().name() == 'file' or ch.type().name() == 'alembic':
                    nodePath = ch.path()
                    path.append(nodePath)
                else:
                    search_children(ch)
        if not node.children():
            #print "no children !"
            if node.type().name() == 'file' or node.type().name() == 'alembic':
                nodePath = node.path()
                path.append(nodePath)
            elif node.type().name() == 'null':
                nodePath = node.path()
                path.append(nodePath)
            else:
                pass

    for node in sel_nodes:
        search_children(node)

    #升序排列并删除重复元素    
    path = sorted(list(set(path)))

    #这个geo_list.log文件会生成在houdini_temp临时文件夹里面
    tempdir = tempfile.gettempdir()
    fl = open("%s\\geo_list" % tempdir,"w")
    for node_path in path:
            fl.write( node_path )
            fl.write( "\n" )
    fl.close()

if __name__ =='__main__':
    main()
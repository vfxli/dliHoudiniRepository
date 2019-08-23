# -*- coding: utf-8 -*-
#    Athor：  roffive
#     date：  2016/11/15
#       QQ:  670729017
# function:  将选择的节点用objmerge引用到另一个geo里面，第2步
#     note:  在‘smile—滴水’教程的基础上加入了‘null’节点引入到geo
#            可以选择merge类型,one by one或者together

import hou,tempfile

def main():
    plane = hou.ui.paneTabOfType(hou.paneTabType.NetworkEditor)

    pos = plane.selectPosition()

    currentNode = plane.currentNode()
    tempdir = tempfile.gettempdir()
    fl = open("%s\\geo_list" % tempdir,"r")
    lineList = fl.readlines()
    numline = len(lineList)
    fl.close()

    choice = hou.ui.displayMessage(text="Make choice what you want !",buttons=["merge one by one","merge together"])
    i=0
    # objmerge one by one
    if choice==0:
        #print "one by one"
        for eachLine in range(numline):
            if eachLine == lineList[0]:
                i = 0
            else:
                i = i+1
            pos[0]+=1
            try:
                objMergeNode = currentNode.createNode("object_merge")
            except:
                parent = currentNode.parent()
                objMergeNode = parent.createNode("object_merge")
                
            objMergeNode.setPosition(pos)
            objMergeNode.parm("xformtype").set( 1 )
            objMergeNode.parm("objpath1").set( lineList[eachLine][0:-1] )
        #print "finally : ",i," objects"
    # objmerge together
    if choice==1:
        #print "together"
        try:
            objMergeNode = currentNode.createNode("object_merge")
        except:
            parent = currentNode.parent()
            objMergeNode = parent.createNode("object_merge")
        objMergeNode.setPosition(pos)
        objMergeNode.parm("xformtype").set( 1 )
        objMergeNode.parm("numobj").set( numline )
        
        
        i=0
        for eachLine in range(numline):
            if eachLine == lineList[0]:
                i = 0
            else:
                i = i+1
            param_objpath = "objpath%d" % i
            objpath = objMergeNode.parm(param_objpath).set( lineList[eachLine][0:-1] )
        #print "finally : ",i," objects"

if __name__ =='__main__':
    main()
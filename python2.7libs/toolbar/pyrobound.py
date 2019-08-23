# -*- coding: utf-8 -*-
#  script name: 添加pyro流体框表达式
#         date: 2016-10-9 
#       E-mail: junvfx@foxmail.com
#           PS: 请确保发射器同层级下有一个OUT_bound节点

import hou
def main():
	selnode = hou.selectedNodes()
	if(selnode[0].type().name()=="smokeobject"):
	        pyroSolver = selnode[0].outputConnections()
	        pyroSolverNode = pyroSolver[0].outputNode()
	        solverInput = pyroSolverNode.inputAncestors()
	        for sourceVolume in solverInput:
	                if(sourceVolume.type().name()=="sourcevolume"):
	                        sourceNode = sourceVolume
	                        break
	        #print sourceNode
	        sourcePath = sourceNode.evalParm("source_path")
	        bbox = sourcePath.split("/")
	        bbox[len(bbox)-1] = "OUT_bound"
	        slash = "/"
	        bboxPath = slash.join(bbox)
	        if(hou.node(bboxPath)!="None"):
	                bboxX = '''bbox("%s",D_XSIZE)'''%bboxPath
	                bboxY = '''bbox("%s",D_YSIZE)'''%bboxPath
	                bboxZ = '''bbox("%s",D_ZSIZE)'''%bboxPath
	                centroidX = '''centroid("%s",D_X)'''%bboxPath
	                centroidY = '''centroid("%s",D_Y)'''%bboxPath
	                centroidZ = '''centroid("%s",D_Z)'''%bboxPath    
	                selnode[0].setParmExpressions({ "sizex" : bboxX, "sizey" : bboxY, "sizez" : bboxZ,
	                                                                          "tx" : centroidX, "ty" : centroidY, "tz" : centroidZ})
	        else:
	                hou.ui.displayMessage(text = "%s not exist"%bbox)
	else:
	        hou.ui.displayMessage(text = "Please select smokeObject node", severity = hou.severityType.Error)
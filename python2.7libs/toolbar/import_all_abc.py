# -*- coding: utf-8 -*-

import os
import hou
def main():
        filePath = hou.houdiniPath()
        abcFolder = "/import"
        abcPath = filePath[0] + abcFolder   
        geo = hou.selectedNodes()
        if(os.path.exists(abcPath)):
                abcList = os.listdir(abcPath)
                geo[0].createNode( "null" ).setDisplayFlag(True)
                for abcFile in abcList:
                        if(abcFile.endswith( ".abc" )):
                                abcSop = geo[0].createNode( "alembic" )
                                abcSop.setDisplayFlag(False)
                                houAbc = "$HIP"+ abcFolder +"/" + abcFile  
                                abcSop.parm( "fileName" ).set(houAbc)
                geo[0].layoutChildren()
                hou.ui.displayMessage(text = "succes")
        else:
                hou.ui.displayMessage(text = "abc folder not exist!!!", severity = hou.severityType.Error)

if __name__ =='__main__':
    main()
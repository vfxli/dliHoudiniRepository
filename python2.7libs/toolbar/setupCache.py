# -*- coding: utf-8 -*-
"""
=================================
set up caching for selected node.
=================================


tested:     Houdini version 14.0
python:     H14 default

ctrl click uses local cache.
"""


__version__     = '0.4'
__author__      = 'forgacs.daniel@gmail.com'



try:
    import hou
except:
    pass


def setup_cache(localcache):
    nodes           = { 'geo'   : hou.selectedNodes()[0],
                        'root'  : hou.node('/obj')}
    # 如果按住ctrl点击的工具那么root就是选中的节点的父节点
    if localcache:
        nodes['root']   = nodes['geo'].parent()
    # 这个createOutputNode就等于创建节点后把他的输入端连接到选中的节点上
    nodes['null']   = nodes['geo'].createOutputNode('null', 'TO_CACHE_')
    nodes['read']   = nodes['null'].createOutputNode('file')

    nodes['read'].setDisplayFlag(True)
    nodes['read'].setRenderFlag(True)
    # 参数（name, label, 此参数有多少个输入框, 类型）
    parmtemplate    = hou.StringParmTemplate('rop', 'rop', 1,
                            string_type = hou.stringParmType.NodeReference)
    # 给file节点添加一个选择节点路径的参数，引用rop下的输出节点
    nodes['read'].addSpareParmTuple(parmtemplate)
    # 锁定模式为读取
    nodes['read'].parm("filemode").lock(True)
    # 根据输出节点获得输出的路径把文件读回来
    nodes['read'].parm('file').set('`chs(chs("rop") + "/sopoutput")`')
    # 选中此节点
    nodes['read'].setCurrent(True, clear_all_selected = True)
    # 如果当前节点内已经有一个ropnet了那么就在这个ropnet下操作，避免每次点工具都会创建一个ropnet
    if nodes['root'].node('cache'):
        nodes['ropnet']  = nodes['root'].node('cache')
    else:
        nodes['ropnet']  = nodes['root'].createNode('ropnet', 'cache')

    nodes['ropnet'].moveToGoodPosition()
    # 创建geometry节点用于输出。节点名就是选中的节点名
    nodes['rop'] = nodes['ropnet'].createNode('geometry', nodes['geo'].name())
    # 设置read节点的rop参数为创建的rop节点的路径
    nodes['read'].parm('rop').set(nodes['rop'].path())
    # 声明模板，rop节点的一些参数
    rop_parms   = {'soppath'    : nodes['null'].path(),
                    'sopoutput' : '{0}/$OS/$OS.$F4.bgeo.sc'.format('$CACHE'),
                    'trange'    : 2,
                    'mkpath'    : True,
                    'saveretry' : 2,
                    '@f1'       : '$FSTART',
                    '@f2'       : '$FEND + 1'
                    }
    # 如果是按住ctrl点击
    if localcache:
    	# 设置soppath就是选中的那个null，覆盖上面的模板，这个是做相对路径
        rop_parms['soppath'] = nodes['rop'].relativePathTo(nodes['null'])
        # fule节点也是一样的，如果是当前节点内创建的roppnet就使用相对路径
        nodes['read'].parm('rop').set(nodes['read'].relativePathTo(nodes['rop']))
        # nodes['read'].parm('rop').set('111')
    # 应用参数
    for key in rop_parms:
    	# 普通只有一行的参数直接设置
        if '@' not in key:
            nodes['rop'].parm(key).set(rop_parms[key])
        # 前面加@区分开一个参数有多个值的情况，比如帧范围
        # key[1：]就是去掉@f1前面的@
        # 就是把f1设置表达式为这个字典对应的值
        # 就是循环到@f1的时候把parm(f1)的值设置为这个key对应的value就是$FSTART
        else:
            nodes['rop'].parm(key[1:]).setExpression(rop_parms[key])



def main(kwargs):
	# 获得一个布尔值，是否按住ctrl后点击的工具
    localcache      = kwargs['ctrlclick']

    setup_cache(localcache)

if __name__ =='__main__':
    main()
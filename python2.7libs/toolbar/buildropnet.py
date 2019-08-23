# -*- coding: utf-8 -*-

import hou

FC_COLOR = (0, 0.533, 0)


class Chain(object):
    def __init__(self, node):
        self.node = node
        self.inputs = []

    # if self.isFC():
    #    node.setColor(hou.Color(FC_COLOR))

    def ropName(self):
        return self.node.name() + '__CHAIN'

    def isFC(self):
        a = self.node.type().name() == 'filecache'
        b = self.node.type().name() == 'zgtxy_filecache'
        return a or b


def directUpstreamFC(node):
    FCs = []
    for i in node.inputs():
        if i.type().name() == 'filecache' or i.type().name() == 'zgtxy_filecache':
            FCs.append(i)
        else:
            FCs.extend(directUpstreamFC(i))
    return FCs


def toChain(node):
    chain = Chain(node)
    chain.inputs = [toChain(i) for i in directUpstreamFC(node)]
    return chain


def toRop(chain, rop_parent):
    r = rop_parent.node(chain.ropName())
    if not r:
        # take the case that first selected node is not filecache into consideration
        r = rop_parent.createNode('fetch' if chain.isFC() else 'merge')
        r.setName(chain.ropName())
    else:
        for i in range(len(r.inputs())):
            r.setInput(i, None)
    r.setColor(chain.node.color())
    if chain.isFC():
        r.parm('source').set(chain.node.path() + '/render')

    for i, input_rop in enumerate([toRop(i, rop_parent) for i in chain.inputs]):
        r.setInput(i, input_rop)
    r.moveToGoodPosition()
    return r
def main():
    sel = hou.selectedNodes()
    if not sel:
        pass
    else:
        for node in sel:
            toRop(toChain(node), hou.node('/out'))

if __name__ =='__main__':
    main()
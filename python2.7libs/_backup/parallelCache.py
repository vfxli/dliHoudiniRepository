# -*- coding: utf-8 -*-

# forgacs.daniel@gmail.com

import hou
import subprocess as sub
import datetime as dt
import sys
import time

def get_parameters( rop, threads, closeDelay):
        hip             = hou.hipFile.path()
        
        if rop.type().name() == 'merge':
                startFrame, endFrame, threads           = 1, 1, 1
        else:
                startFrame, endFrame, step      = rop.parmTuple('f').eval()
                startFrame, endFrame            = int(startFrame), int(endFrame)
        
        if threads == -1 and rop.type().name() != 'dop':
                threads         = min(int( hou.ui.readInput('number of threads')[1]), endFrame)
        elif rop.type().name() == 'dop':
                threads         = 1
                
        threadRange                     = ((endFrame - startFrame + 1) / threads) - 1
        threadEnd                       = startFrame - 1
        remainder                       = (endFrame - startFrame + 1) % threads

        scriptStart     = '\nimport time\nimport datetime'
        scriptStart     = scriptStart + '\nhou.hipFile.load("{0}")'.format(hip)
        scriptStart     = scriptStart + '\nprint ">> hip: {0}"'.format(hip)
        scriptStart     = scriptStart + '\nrop = hou.node("{0}")'.format(rop.path())
        scriptStart     = scriptStart + '\nprint ">> rop: {0}".format(rop.path())'
        scriptStart     = scriptStart + '\nstarttime = datetime.datetime.now()'
        
        scriptEnd       = '\nhou.hipFile.clear()'
        scriptEnd       = scriptEnd + '\nendtime = datetime.datetime.now()'
        scriptEnd       = scriptEnd + '\nprint "\\n>> Done..."'
        scriptEnd       = scriptEnd + '\nprint ">> rendertime: ", endtime - starttime'
        scriptEnd       = scriptEnd + '\ntime.sleep({0})'.format(closeDelay)
        
        print '>> hip: {0}'.format( hip)
        print '>> rop: {0}'.format( rop.path())
        print '>> frame range: {0} - {1}'.format( startFrame, endFrame)
        print '>> threads: {0}'.format( threads)
        print '>> start time: {0} \n'.format( str(dt.datetime.now()))

        for k in range(threads):
                threadEnd, remainder    = start_cache(rop, k, threads, remainder, threadEnd, threadRange, scriptStart, scriptEnd)
        
def start_cache(rop, k, threads, remainder, threadEnd, threadRange, scriptStart, scriptEnd):
        extraFrames             = 1
        
        if remainder < 1:
                extraFrames             = 0

        remainder               = remainder - 1
        threadStart             = threadEnd + 1
        threadEnd               = threadStart + threadRange + extraFrames
        scriptStart             = scriptStart + '\nprint ">> thread: {0}/{1}, range: {2}-{3}, frames: {4}"; print'.format(k+1, threads, threadStart, threadEnd, threadEnd - threadStart + 1)
        
        if rop.type().name() == 'merge':
                scriptStart = scriptStart + '\nrop.render(verbose = True)'
        else:
                scriptStart = scriptStart + '\nrop.render(({0},{1}), verbose = True)'.format(threadStart, threadEnd)

        scriptStart = scriptStart + scriptEnd
        print ">> thread: {0}, range: {1}-{2}, frames: {3}".format(k+1, threadStart, threadEnd, threadEnd - threadStart + 1)

        # if k == 0: print scriptStart # DEBUG
        
        sub.Popen(['hython', '-c', scriptStart])
        # sub.Popen(['hython', '-c', scriptStart], stdout=sub.PIPE) # DEBUG
        
        return (threadEnd, remainder)
                
def get_node():
        goodRopTypes    = ['geometry', 'dop', 'merge']
        rop             = hou.selectedNodes()
        
        if len(rop) == 1 and rop[0].type().name() in goodRopTypes:
                return rop[0]
        else:
                print '!!! WRONG SELECTION !!!'
                return ''

def main( rop = '', threads = -1, closeDelay = 60):
        print '=' * 50
                
        if hou.isUIAvailable() and hou.hipFile.hasUnsavedChanges():
                print '!!! UNSAVED CHANGES !!!'
                closeDelay      = -1
        
        if rop == '':
                rop             = get_node()
        
        if rop != '' and closeDelay != -1:
                get_parameters( rop, threads, closeDelay)
                
def main_v2(kwargs):
        print '='*50
        
        if hou.hipFile.hasUnsavedChanges():
                hou.ui.setStatusMessage('..:: SAVE THE HIP ::..', hou.severityType.Error)
        elif kwargs['ctrlclick']:
                print 'control clicked'
        elif kwargs['altclick']:
                print 'alt clicked'
                
if __name__ =='__main__':
    main()
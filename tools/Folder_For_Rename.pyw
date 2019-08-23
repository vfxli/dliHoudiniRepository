#!/usr/bin/env python
# -*- coding: utf-8 -*-
__author__ = 'liDong'
from Tkinter import *
import tkMessageBox
import os
import shutil
pwd = os.path.abspath('.')
filelist = [x for x in os.listdir('.') if os.path.isfile(x) and os.path.splitext(x)[1]=='.exr']
dir_spl = pwd.split('\\')
class Application(Frame):
    def __init__(self, master=None,):
        Frame.__init__(self, master)
        # super(Application, self).__init__()
        self.pack()
        self.createWidgets()
    def createWidgets(self):
        self.listb = Listbox(self, width=100, height=6, selectmode=MULTIPLE)
        re_dir_spl = list(dir_spl)
        re_dir_spl.reverse()
        for key, item in enumerate(re_dir_spl):
            if key == len(re_dir_spl)-1:continue
            self.listb.insert(0,item)
        self.listb.pack()
        self.text = Text(self, width=85,  height=1)
        self.text.pack()
        self.acceptButton = Button(self, width=100, bg = 'green', text='Accept', command=self.Accept)
        self.acceptButton.pack()
        self.acceptButton = Button(self, width=100, bg = 'green', text='Apply', command=self.Apply)
        self.acceptButton.pack()
    def Accept(self):
        self.text.delete(0.0, END)
        FileNameIndex = self.listb.curselection()
        FileName = []
        for key, i in enumerate(FileNameIndex):
            print key
            temp = dir_spl[i+1]
            FileName.append(temp)
        self.text.insert(1.0, '_'.join(FileName)+'.####.exr')
    def Apply(self):
        cons = 0
        for fl in filelist:
            cons += 1
            newname = self.text.get(0.0,END).split('.')[-3] + '.'+fl.split('.')[-2]+'.exr'
            os.rename(fl, newname)
        tkMessageBox.showinfo('Message', 'Correct '+str(cons))
        exit()
root = Tk()
app = Application(root)
app.master.title('Folder For Rename')
root.geometry("600x190+500+500")
app.mainloop()

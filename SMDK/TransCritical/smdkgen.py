##   Time-stamp: <2007-05-31 05:38:19 Rod Stephenson Transcritical Pty Ltd>
##   smdkgen.py - generate all the files necessary for generic SMDK project
##   Basically just specify the dll group name and go from there...

from string import Template
import os, os.path
from smdkfiles import makeFile, stdafxhFile, stdafxcppFile, \
     dllMainhFile, dllMaincppFile, umcppFile, umhFile


def doFile(fname, templateStr, dic):
    f = open(fname, "w")
    f.write(Template(templateStr).safe_substitute(dic))
    f.close()
            


if __name__=="__main__":
    devName = ""
    clientName = ""
    baseName = "Thingy"
    dllGroupName = devName+clientName+baseName
    
    udllGroupName = dllGroupName.upper()
    srcName = dllGroupName+"UM"
    usrcName = srcName.upper()

    unitList = [baseName]
    classList = ["C"+x for x in unitList]


    ndic = { 'dllGroupName': dllGroupName,
             'udllGroupName': udllGroupName,
             'srcName': srcName,
             'usrcName': usrcName,
             'unitName': unitList[0],
             'className': classList[0]}

    
    if not os.path.exists(dllGroupName):
        print "Creating directory ./%s" % dllGroupName
        os.mkdir(dllGroupName)
    else:
        print "Directory exists, all files will be overwritten..."


    cwd = os.getcwd()
    os.chdir(dllGroupName)
    print os.getcwd()

##  Create makefile
    doFile("makefile", makeFile, ndic)

##  Create stdafx.h file    
    doFile("stdafx.h", stdafxhFile, ndic)
    
##  Create dll main header file
    doFile(dllGroupName+".h", dllMainhFile, ndic)

##  Create dll main source file
    doFile(dllGroupName+".cpp", dllMaincppFile, ndic)

##  Create unit source and header files:
    doFile(srcName+".cpp",umcppFile, ndic) 
    doFile(srcName+".h", umhFile, ndic)

    os.chdir(cwd)
    

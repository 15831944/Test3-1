## Generate call graph for FORTRAN code contained in single file.

import re

classRE = re.compile("^\W*class\W*(\w*)\W*")
restr = "\W+(%s)\W*\("

sfre = re.compile("(?:void|double|int)\W*(\w*)\(") 
s1fre = re.compile("(?:void|double|int)\W*(\w*)::(\w*)\W*\(")

try:
    f = open("TTechQAL/QALPrecip.h")
except:
    print "Unable to open input file"
    

classList = []
afuns = []

for line in f:
    m = classRE.search(line)
    if m:
        localClass = m.group(1)
        classList.append(localClass)
        continue
    
    m = sfre.search(line)
    if m:
        afuns.append(m.group(1))
                     

print afuns
u = [re.compile( (restr % x) ) for x in afuns]

        
subn = None
callList = set()

f.close()

try:
    f = open("TTechQAL/QALPrecip.cpp")
except:
    print "Unable to open input file"



#def op():
#    if subn and callList:
#        print subn, " -> {", ", ".join(callList), "};" 

for line in f:
    if line.startswith("//"):
        continue
    m = s1fre.search(line)
    if m:
##        op()
        print m.group(1), m.group(2)
        subn = m.group(1)
        callList.clear()
        continue

            
##    for x in u:
##        m = x.search(line)
##        if m:
##            callList.add(m.group(1))

#op()

f.close()



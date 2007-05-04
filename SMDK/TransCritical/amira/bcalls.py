import re

subre = re.compile("SUBROUTINE\W*(\w*)")
funre = re.compile("FUNCTION\W*(\w*)")
restr = "\W(%s)\("
sfre = re.compile("(?:SUBROUTINE|FUNCTION)\W*(\w*)") 

f = open("bayer.for")
afuns = [m.group(1) for m in [funre.search(line) for line in f] if m]
f.seek(0)
asubs = [m.group(1) for m in [subre.search(line) for line in f] if m]
f.seek(0)
u = [re.compile( (restr % x) ) for x in afns]
        
subn = None
callList = set()

def op():
    if subn and callList:
        print subn, " -> {", ", ".join(callList), "};" 


for line in f:
    if line.startswith("C"):
        continue
    m = sfre.search(line)
    if m:
        op()
        subn = m.group(1)
        callList.clear()
        continue

    if "CALL" in line:
        for x in asubs:
            if x in line:
                callList.add(x)
            
    for x in u:
        m = x.search(line)
        if m:
            callList.add(m.group(1))

op()

f.close()



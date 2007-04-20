f = file('bla4.for')

lis = f.readlines()



def processFn(x):
    loc = 9+x.find("FUNCTION")
    fcall = x[loc:]
    loc = fcall.find('(')
    fname = fcall[:loc]
    fargs = fcall[loc:].strip()
    arglis = fargs[1:-1].split(",")
    print fname.lower()+"_", "("+",".join(["double *" for x in arglis])+")"
    
for x in lis:
    if x[0] == "C":
        continue
    if 'FUNCTION' in x:
        processFn(x)

        

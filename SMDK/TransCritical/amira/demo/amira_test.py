##
##             Test for amira dll
##

from ctypes import *
alib = CDLL('amira')

##  The Amira dll uses standard c calling convention. 
## The exported functions are lower case and have appended '_'

g0_ = alib.g0h2o_     #  Ex: Free Energy for Water: IAPWS IF97

## Specify return type and argument types
## Arguments are *pointers* to doubles

g0_.argtypes = [POINTER(c_double), POINTER(c_double)] 

## Return Type is double
g0_.restype = c_double  

def g0(p, t):
    return g0_(c_double(p), c_double(t))


## Test function to check on integer type

test1_ = alib.test1_     #  Ex: Free Energy for Water: IAPWS IF97

## Specify return type and argument types
## Arguments are *pointers* to doubles and longs

test1_.argtypes = [POINTER(c_double), POINTER(c_long), POINTER(c_double)] 

## Return Type is long
test1_.restype = c_double  

def test1(p, i, t):
    return test1_(c_double(p), c_long(i), c_double(t))
















if __name__ == "__main__":
    print test1(30., -1, 20.)
    
    for t in range(300,500, 10):
        print t, g0(100, t)




        

## Result:
## 300 -122.932424444
## 310 -119.244650198
## 320 -115.796437906
## 330 -112.565737987
## 340 -109.533113966
## 350 -106.681367643
## 360 -103.995226898
## 370 -101.461084244
## 380 -99.0667767868
## 390 -96.8014001831
## 400 -94.6551506337
## 410 -92.6191901366
## 420 -90.6855311103
## 430 -88.8469372284
## 440 -87.0968378807
## 450 -85.4292541393
## 460 -83.8387344798
## 470 -82.3202988113
## 480 -80.8693896171
## 490 -79.4818292122

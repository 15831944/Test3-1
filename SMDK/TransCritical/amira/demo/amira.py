import sys
sys.path.append("c:/devel/python")


if sys.platform=="win32":
    font3=("Courier", "8")
else:
    font3=("lucidasanstypewriter-12")

      

from Tkinter import *
from math import sqrt
#import TkFileDialog
from generic import GenericMain, TestMenu, GenericMenus, OutputFrame, font4
import entry



from ctypes import *
try:
    alib = CDLL('amira')
except:
    WindowsError, err


s1Entries = [
    ["Temperature", "C", "100."],
    ["Pressure   ", "bar", "10."],
    ["Al2O3      ", "%", "0"],
    ["Na2O       ", "%", "0"],
    ["Al(OH)3    ", "%", "10"],
    ["NaOH       ", "%", "20"],  
    ["NaCl       ", "%", "0"],
    ["Na2CO3     ", "%", "0"],
    ["Na2SO4     ", "%", "0"],       
    ["NaAcetate  ", "%", "0"],
    ["NaOxalate  ", "%", "0"],
    ["NaFormate  ", "%", "0"],
    ["NaF        ", "%", "0"]
    ]



ostr1 =   "t =  %8.2f C       I_m =  %8.4f mol/kg      I_c(t, p) =  %8.4f mol/L"
ostr2 =   "p =  %8.2f bar     p_sat =    %8.3f bar       I_c(25 C) =  %8.4f mol/L"
ostr3 =   "Conc. units   NaOH  Al(OH)3   NaCl   Na2CO3  Na2SO4    NaOx    NaAc   NaForm  NaF"
ostr4 =   "mol/kg       %8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f"
ostr5 =   "mol/L (t,p)  %8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f"
ostr6 =   "mol/L (25C)  %8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f"
ostr7 =   "mass-%%      %8.3f%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f"
ostr8 =   "g/L (25C)  %8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"




g0_ = alib.g0h2o_     #  Ex: Free Energy for Water: IAPWS IF97

## Specify return type and argument types
## Arguments are *pointers* to doubles

g0_.argtypes = [POINTER(c_double), POINTER(c_double)] 

## Return Type is double
g0_.restype = c_double  

def g0(p, t):
    return g0_(c_double(p), c_double(t))


def doG0Test(t0):
    res = "Temperature   Gibbs Free Energy\n\n"
    for t in range(0,200, 10):
        res += "%6.1f   %8.2f\n" % ( t+t0, g0(100, t+t0))
    return res




bayer_ = alib.bayer_    # Main routine in Amira Library

bayer_.restype = None

bayer_.argtypes = [
                   POINTER(c_double),    #  TempC,
                   POINTER(c_long),      #  InUnits,      INTEGER!
                   POINTER(c_double),    #  Pressure,
                   POINTER(c_long),      #  NInComp,      INTEGER!
                   POINTER(c_double),    #  InComp,       ARRAY*11
                   POINTER(c_double),    #  I_m,
                   POINTER(c_double),    #  I_c,
                   POINTER(c_double),    #  I_c25,
                   POINTER(c_double),    #  P_sat,
                   POINTER(c_long),      #  NOutComp,     INTEGER!
#                  c_char_p,             #  OutCompName,
                   POINTER(c_double),    #  Comp_molkg,
                   POINTER(c_double),    #  Comp_molL,
                   POINTER(c_double),    #  Comp_molL25,
                   POINTER(c_double),    #  Comp_mpercent,
                   POINTER(c_double),    #  Comp_gL,
                   POINTER(c_double),    #  Al2O3,
                   POINTER(c_double),    #  TC,
                   POINTER(c_double),    #  TA,
                   POINTER(c_long),      #  NOC,          INTEGER!
#                   c_char_p,             #  OCName,
                   POINTER(c_double),    #  OC,
                   POINTER(c_double),    #  TempSat,
                   POINTER(c_double),    #  BPE,
                   POINTER(c_double),    #  Cp_Liq,
                   POINTER(c_double),    #  Cp_H2O,
                   POINTER(c_double),    #  Rho_Liq,
                   POINTER(c_double),    #  Rho_H2O,
                   POINTER(c_double),    #  Cp_phi,
                   POINTER(c_double),    #  V_phi,
                   POINTER(c_double),    #  Cp_LiqH2O,
                   POINTER(c_double),    #  Phi,
                   POINTER(c_double),    #  Aw,
                   POINTER(c_long),      #  NGamma,      INTEGER!
#                   c_char_p,             #  GammaName,
                   POINTER(c_double),    #  Gamma,
                   POINTER(c_long),      #  NSI,         INTEGER!
#                   c_char_p,             #  SIName,
                   POINTER(c_double),    #  SI,
                   POINTER(c_long),      #  NSol,        INTEGER!
#                   c_char_p,             #  SolName,
                   POINTER(c_double),    #  SolML,
                   POINTER(c_double)     #  Solmkg
                   ]



class AmiraBayer:
    def __init__(self, Temp_C, Pressure_kPa, InUnits=3):

        # Parameters
        self.Temp_C = Temp_C
        self.Pressure_kPa = Pressure_kPa
        self.InComp = (c_double*11)()
        self.InUnits = InUnits


        

        # Exported Character Arrays
##         self.OutCompName = create_string_buffer(20*12)
##         self.OCName = create_string_buffer(20*3)
##         self.GammaName = create_string_buffer(20*13)
##         self.SIName = create_string_buffer(20*11)
##         self.SolName = create_string_buffer(20*7)

        # Exported double Arrays
        self.Comp_molkg    = (c_double*12)()    
        self.Comp_molL     = (c_double*12)()

        self.Comp_molL25   = (c_double*12)()
        self.Comp_mpercent = (c_double*12)()
        self.Comp_gL       = (c_double*12)()
        self.Gamma         = (c_double*12)()
        self.OC            = (c_double*2)()
        self.SI            = (c_double*10)()
        self.SolML         = (c_double*6)()

        self.Solmkg        = (c_double*6)()
        
        # Exported doubles
        self.I_m = c_double()
        self.I_c = c_double()
        self.I_c25 = c_double()
        self.P_Sat = c_double()
        self.Al2O3 = c_double()
        self.TC = c_double()
        self.TA = c_double()
        self.TempSat = c_double()
        self.BPE = c_double()
        self.Cp_Liq   = c_double()    
        self.Cp_H2O   = c_double()    
        self.Rho_Liq  = c_double()   
        self.Rho_H2O  = c_double()   
        self.Cp_phi   = c_double()           
        self.V_phi    = c_double()     
        self.Cp_LiqH2O= c_double() 
        self.Phi      = c_double()       
        self.Aw       = c_double()        

        # Exported longs
        self.NOutComp = c_long(9)
        self.NOC = c_long(2)
        self.NGamma = c_long(12)
        self.NSI = c_long(10)
        self.NSol = c_long(6)
        
    def bayer(self):
        bayer_(
            c_double(self.Temp_C),    #  TempC,
            c_long(3),      #  InUnits,      INTEGER!
            c_double(self.Pressure_kPa/100.),    #  Pressure,
            c_long(9),    #  NInComp,      INTEGER!
            self.InComp,    #  InComp,       ARRAY*11
            self.I_m,    #  I_m,
            self.I_c,    #  I_c,
            self.I_c25,    #  I_c25,
            self.P_Sat,    #  P_sat, % (p, ab.P_Sat.value, ab.I_c25.value)))
            self.NOutComp,      #  NOutComp,     INTEGER!
#            self.OutCompName,             #  OutCompName,
            self.Comp_molkg   ,    #  Comp_molkg,
            self.Comp_molL    ,    #  Comp_molL,
            self.Comp_molL25  ,    #  Comp_molL25,
            self.Comp_mpercent,    #  Comp_mpercent,
            self.Comp_gL      ,    #  Comp_gL,             RETURN

            self.Al2O3,    #  Al2O3,
            self.TC,       #  TC,
            self.TA,       #  TA,
            self.NOC,      #  NOC,          INTEGER!
#            self.OCName,   #  OCName,
            self.OC,       #  OC,
            self.TempSat,  #  TempSat,
            self.BPE,      #  BPE,
            self.Cp_Liq    ,    #  Cp_Liq,
            self.Cp_H2O    ,    #  Cp_H2O,
            self.Rho_Liq   ,    #  Rho_Liq,
            self.Rho_H2O   ,    #  Rho_H2O,
            self.Cp_phi    ,    #  Cp_phi,
            self.V_phi     ,    #  V_phi,
            self.Cp_LiqH2O ,    #  Cp_LiqH2O,
            self.Phi       ,    #  Phi,
            self.Aw        ,    #  Aw,        
            self.NGamma,      #  NGamma,      INTEGER!
#            self.GammaName ,             #  GammaName,
            self.Gamma,    #  Gamma,
            self.NSI,      #  NSI,         INTEGER!
#            self.SIName,             #  SIName,
            self.SI,    #  SI,
            self.NSol,      #  NSol,        INTEGER!
#            self.SolName,             #  SolName,
            self.SolML,    #  SolML,
            self.Solmkg     #  Solmkg
            )


ab = AmiraBayer(100, 200)

class MyMain(GenericMain):

    
    def activateApp(self):
        self.ptype = IntVar()
        self.ptype.set(0)
        f = Frame(self.baseFrame)
        Label(f, text="Data").pack(anchor="w")

        self.s0=entry.EntryFrame(f, s1Entries, lWidth=16, eWidth=8)
        f1 = Frame(f)
        f1.pack(side = LEFT, fill=X, expand=1)
        Button(f1, text="Plot", command=self.test).pack(side=LEFT, fill=X, expand=1)
        Button(f1, text="Print",command=printCanvii).pack(side=LEFT, fill=X, expand=1)
        
        f.pack(side=LEFT, anchor=NW)

        canvasFrame=Frame(self.baseFrame)
        canvasFrame.pack(side=LEFT, fill=BOTH, expand=YES)


        self.of=OutputFrame(canvasFrame, font=font3)




       

    def test(self):
        s = self.s0
        t  = s["Temp"]
        p = s["Press"]

        res = s.getValues()
        print res
        for i, x in enumerate(res[2:]):
            ab.InComp[i] = 100.*x

        print t
        ab.Temp_C = t-273.15
        ab.Pressure_kPa = p

        for x in ab.InComp:
            print x
        ab.bayer()

        print ab.P_Sat.value, ab.TempSat.value, ab.NSol.value, ab.NSI.value
        print ab.Cp_H2O.value, ab.Rho_H2O.value, ab.Phi.value

        # print repr(ab.OutCompName.raw)
        for x in ab.Comp_mpercent:
            pass


        self.of.appendText((ostr1 % (t, ab.I_m.value, ab.I_c.value)))
        self.of.appendText((ostr2 % (p, ab.P_Sat.value, ab.I_c25.value)))
        self.of.appendText(ostr3)
        self.of.appendText((ostr4 % tuple([x for x in ab.Comp_molkg[:9]])))
        self.of.appendText((ostr5 % tuple([x for x in ab.Comp_molL[:9]])))
        self.of.appendText((ostr6 % tuple([x for x in ab.Comp_molL25[:9]])))
        self.of.appendText((ostr7 % tuple([x for x in ab.Comp_mpercent[:9]])))
        self.of.appendText((ostr8 % tuple([x for x in ab.Comp_gL[:9]])))
        self.of.appendText("")





def printCanvii():
    global app
    app.c.postscript(file="ft.ps", pageheight="240m", height=cHeight)



def main():
    global app
    def done():
        root.destroy()
    root=Tk()
    root.protocol("WM_DELETE_WINDOW", done)
    app=MyMain(root, TestMenu())


    root.title("Amira Bayer Calculations")
    root.iconify()
    root.update()
    root.deiconify()
    root.geometry("1000x800-5+5")

    root.mainloop()


main()



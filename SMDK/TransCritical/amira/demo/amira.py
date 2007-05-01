import sys
sys.path.append("c:/devel/python")

from Tkinter import *
from generic import GenericMain, TestMenu, OutputFrame, font3
import entry
from ctypes import *

try:
    alib = CDLL('amira')
except WindowsError, err:
    print "cant find dll"
    
dpnames = ["I_m", "I_c", "I_c25", "P_Sat", "Al2O3", "TC", "TA", "TempSat",
           "BPE", "Cp_Liq", "Cp_H2O", "Rho_Liq", "Rho_H2O", "Cp_phi",
           "V_phi",  "Cp_LiqH2O",  "Phi", "Aw"]

s1Entries = [
    ["Temperature", "C", "100."],
    ["Pressure   ", "bar", "10."],
    ["Al2O3      ", "%", "0"],
    ["Na2O       ", "%", "0"],
    ["Al[OH]3    ", "%", "10"],
    ["NaOH       ", "%", "20"],  
    ["NaCl       ", "%", "0"],
    ["Na2CO3     ", "%", "0"],
    ["Na2SO4     ", "%", "0"],       
    ["NaAcetate  ", "%", "0"],
    ["Na2C2O4    ", "%", "0"],
    ["NaFormate  ", "%", "0"],
    ["NaF        ", "%", "0"]
    ]




syscadNames = ["H2O", "NaOH", "Na2CO3", "NaAl[OH]4", "Na2C5.2O7.2", "Na2C2O4", "NaCl", "Na2SiO3", "Na2SO4"]



# Format Strings for display
ostr1 =   "t =  %8.2f C       I_m =  %8.4f mol/kg      I_c(t, p) =  %8.4f mol/L"
ostr2 =   "p =  %8.2f bar     p_sat =    %8.3f bar       I_c(25 C) =  %8.4f mol/L"
ostr3 =   "Conc. units  Al(OH)3 NaOH  NaCl   Na2CO3  Na2SO4    NaOx    NaAc   NaForm  NaF"
ostr4 =   "mol/kg       %8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f"
ostr5 =   "mol/L (t,p)  %8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f"
ostr6 =   "mol/L (25C)  %8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f"
ostr7 =   "mass-%%      %8.3f%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f"
ostr8 =   "g/L (25C)  %8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"

bayer_ = alib.bayer_    # Main routine in Amira Library
bayer_.restype = None
bayer_.argtypes = [
                   POINTER(c_double),    #  TempC,
                   POINTER(c_long),      #  InUnits,      INTEGER!
                   POINTER(c_double),    #  Pressure,
                   POINTER(c_long),      #  NInComp,      INTEGER!
                   POINTER(c_double),    #  InComp,       ARRAY*11
                   
                   POINTER(c_double),    #  DPDATA        <-- All random return stuff

                   POINTER(c_long),      #  NOutComp,     INTEGER!
                   POINTER(c_double),    #  Comp_molkg,
                   POINTER(c_double),    #  Comp_molL,
                   POINTER(c_double),    #  Comp_molL25,
                   POINTER(c_double),    #  Comp_mpercent,
                   POINTER(c_double),    #  Comp_gL,

                   POINTER(c_long),      #  NOC,          INTEGER!
                   POINTER(c_double),    #  OC,

                   POINTER(c_long),      #  NGamma,      INTEGER!
                   POINTER(c_double),    #  Gamma,

                   POINTER(c_long),      #  NSI,         INTEGER!
                   POINTER(c_double),    #  SI,

                   POINTER(c_long),      #  NSol,        INTEGER!
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
        self.DPDATA = (c_double*18)() # Catchall for doubles...

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

            self.DPDATA, 

            self.NOutComp,      #  NOutComp,     INTEGER!
            self.Comp_molkg   ,    #  Comp_molkg,
            self.Comp_molL    ,    #  Comp_molL,mol/kg     
            self.Comp_molL25  ,    #  Comp_molL25,
            self.Comp_mpercent,    #  Comp_mpercent,
            self.Comp_gL      ,    #  Comp_gL,             RETURN

            self.NOC,      #  NOC,          INTEGER!
            self.OC,       #  OC,

            self.NGamma,      #  NGamma,      INTEGER!
            self.Gamma,    #  Gamma,

            self.NSI,      #  NSI,         INTEGER!
            self.SI,    #  SI,

            self.NSol,      #  NSol,        INTEGER!
            self.SolML,    #  SolML,
            self.Solmkg     #  Solmkg
            )


ab = AmiraBayer(100, 200)

class MyMain(GenericMain):

    def activateApp(self):
        f = Frame(self.baseFrame)
        Label(f, text="Data").pack(anchor="w")
        self.s0=entry.EntryFrame(f, s1Entries, lWidth=16, eWidth=8)
        self.s0.disable("Na2O")
        self.s0.disable("Al2O3")
        f1 = Frame(f)
        f1.pack(side = LEFT, fill=X, expand=1)
        Button(f1, text="Plot", command=self.test).pack(side=LEFT, fill=X, expand=1)
        Button(f1, text="SysCAD", command=self.getSysCAD).pack(side=LEFT, fill=X, expand=1)
        f.pack(side=LEFT, anchor=NW)
        canvasFrame=Frame(self.baseFrame)
        canvasFrame.pack(side=LEFT, fill=BOTH, expand=YES)
        self.of=OutputFrame(canvasFrame, font=font3)

    def extractDPData(self):
        dat = [x for x in ab.DPDATA]
        for x, y in zip(dpnames, dat):
            self.__dict__[x] = y


    def getSysCAD(self):
        s = [l.split() for l in self.of.getSelection().splitlines()]
        cdic = {}
        for x in syscadNames:
            cdic[x] = 0.0
        tot = 0.0
        for lis in s:
            qty = float(lis[2])
            tot += qty
            scspec = lis[0]
            for x in syscadNames:
                if x in scspec:
                    cdic[x] = qty
                    break

        print tot
        for x in cdic.keys():
            cdic[x] /= tot
        for s,v in cdic.iteritems():
            print s, v
        self.s0["NaOH"] = cdic["NaOH"]+40./118.*cdic["NaAl[OH]4"]
        self.s0["Al[OH]3"] = 78./118.*cdic["NaAl[OH]4"]
        for x in ["Na2CO3", "Na2C2O4", "NaCl", "Na2SO4"]:
            self.s0[x] = cdic[x]
        self.s0["NaFormate"] = cdic["Na2C5.2O7.2"]/2.
        self.s0["NaAcetate"] = cdic["Na2C5.2O7.2"]/2.
        
            


                               
    def test(self):
        s = self.s0
        t  = s["Temp"]
        p = s["Press"]

        res = s.getValues()
        for i, x in enumerate(res[2:]):
            ab.InComp[i] = 100.*x
        ab.Temp_C = t-273.15
        ab.Pressure_kPa = p
        ab.bayer()
        self.extractDPData()
        atxt = self.of.appendText
        
        atxt((ostr1 % (t,    self.I_m, self.I_c)))
        atxt((ostr2 % (p,    self.P_Sat, self.I_c25)))
        atxt(ostr3)
        atxt((ostr4 % tuple([x for x in ab.Comp_molkg[:9]])))
        atxt((ostr5 % tuple([x for x in ab.Comp_molL[:9]])))
        atxt((ostr6 % tuple([x for x in ab.Comp_molL25[:9]])))
        atxt((ostr7 % tuple([x for x in ab.Comp_mpercent[:9]])))
        atxt((ostr8 % tuple([x for x in ab.Comp_gL[:9]])))
        atxt("")
        for x in dpnames:
            atxt("%10s     %f" % (x, self.__dict__[x]))

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
    root.geometry("900x600-5+5")
    root.mainloop()

main()



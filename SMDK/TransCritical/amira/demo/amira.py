from Tkinter import *
import tkMessageBox
from generic import GenericMain, OutputFrame, font0, font3
import entry
from ctypes import *

    
dpnames = ["I_m", "I_c", "I_c25", "P_Sat", "Al2O3", "TC", "TA", "TempSat",
           "BPE", "Cp_Liq", "Cp_H2O", "Rho_Liq", "Rho_H2O", "Cp_phi",
           "V_phi",  "Cp_LiqH2O",  "Phi", "Aw"]

s0Entries = [
    ["Temperature", "C", "100."],
    ["Pressure   ", "bar", "10."],
##    ["Al2O3      ", "%", "0"],
##    ["Na2O       ", "%", "0"],
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

s2Entries = [
    ["Temperature", "C", "100."],
    ["Pressure   ", "bar", "10."],
    ["Al2O3      ", "gpl", "220"],
    ["TC         ", "gpl", "250"],
    ["NaCl       ", "gpl", "0"],
    ["TA         ", "gpl", "0"],
    ["Na2SO4     ", "gpl", "0"],
    ["Na Oxalate ", "gpl", "0"], 
    ["NaAcetate  ", "gpl", "0"],
    ["NaFormate  ", "gpl", "0"],
    ["NaF        ", "gpl", "0"]
    ]




s1Entries = [[("%-10s" % x), ".", "0"] for x in dpnames]
syscadNames = ["H2O", "NaAl[OH]4", "NaCl", "Na2C2O4", "Na2C5.2O7.2", "Na2CO3" , "Na2SO4", "NaOH"]

syscadEx = '''
Example: select the following text and try again...

P_HT_out.Qo.H2O(l) (kg/s)	225.58
P_HT_out.Qo.NaAl[OH]4(l) (kg/s)	114.58
P_HT_out.Qo.NaCl(l) (kg/s)	0.98
P_HT_out.Qo.NaOH(l) (kg/s)	10.69
P_HT_out.Qo.Na2C2O4(l) (kg/s)	0.86
P_HT_out.Qo.Na2C5.2O7.2(l) (kg/	15.90
P_HT_out.Qo.Na2CO3(l) (kg/s)	5.26
P_HT_out.Qo.Na2SO4(l) (kg/s)	0.14
'''

solnames = '                Al(OH)3    AlOOH   Na2SO4  Na2C2O4      NaF  Na3FSO4'

sinames = [
'NaCl         ',
'Na2SO4       ',
'Na2SO4.10H2O ',
'Na2CO3.10H2O ',
'Na2CO3.7H2O  ',
'Na2CO3.H2O   ',
'Al(OH)3      ',
'AlOOH        ',
'NaF          ',
'Na3FSO4'
]


# Format Strings for display
ostr1 =   "t =  %8.2f C         I_m =  %8.4f mol/kg      I_c(t, p) =  %8.4f mol/L"
ostr2 =   "p =  %8.2f bar     p_sat =    %8.3f bar       I_c(25 C) =  %8.4f mol/L"
ostr3 =   "Conc. units   Al(OH)3    NaOH    NaCl  Na2CO3  Na2SO4    NaOx    NaAc  NaForm     NaF"
ostr4 =   "mol/kg       %8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f"
ostr5 =   "mol/L (t,p)  %8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f"
ostr6 =   "mol/L (25C)  %8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f%8.4f"
ostr7 =   "mass-%%      %8.3f%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f"
ostr8 =   "g/L (25C)  %8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"

ostrs = [
"                  A            C            S            OC           OC",
"                              NaOH     NaOH+Na2CO3   Na acetate   Na formate",
"g/L (25 C)    %8.2f     %8.2f     %8.2f      %8.2f   %8.2f",
"A/C = %6.3f   (Al2O3 eq)  (Na2CO3 eq)  (Na2CO3 eq)     (C eq)       (C eq)",
"Thermodynamic properties of the aqueous phase       B.p. =%8.2fC (%+7.2fK)",
"Cp_sln = %8.2f kJ/kgK       rho_sln = %8.2f kg/m^3",
"Cp_H2O = %8.2f kJ/kgK       rho_H2O = %8.2f kg/m^3",
"Cp_phi = %8.2f J/K/mol       V_phi =  %8.3f cm3 mol-1",
"Cp_sln = %8.2f kJ/kgK/ (g H2O)-1"
]




def dllSetup():
    global bayer_
    try:
        alib = CDLL('amira')
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



    except WindowsError, err:
        tkMessageBox.showerror("Missing dll","Need to install amira.dll")



class TestMenu:
    def __init__(self):
        self.menuList = []

    def mainMenuBar(self, w):
        mm=Menu(w)  # main menu
        self.mm=mm
        self.menuList.append(Menu(mm, tearoff=0))
        mm.add_cascade(label="File", menu=self.menuList[0],
                       underline=0)
        
        m=self.menuList[0]  #0
        m.add_command(label="Open...", state=DISABLED, 
                      accelerator="Ctrl+O")
        m.add_command(label="Save", state=DISABLED)
        m.add_command(label="Save As...",
                      accelerator="Ctrl+S", state=DISABLED)
        m.add_separator()
        m.add_command(label="Print Setup...", state=DISABLED)
        m.add_command(label="Print", state=DISABLED)
        m.add_separator()
        m.add_command(label="Exit", command=done) #-command done
        return mm



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
        Label(f, text="Data", font=font0).pack(anchor="w")

        self.rbp = IntVar()
        self.rbp.set(0)
        frb = Frame(f)  # Frame for radiobuttons
        Label(frb, text="Mass %").grid(row=0, column=0, sticky="w")
        Radiobutton(frb, variable=self.rbp, value=0, takefocus=False).grid(row=0,column=1, sticky="e")
        Label(frb, text="gpl@25").grid(row=0, column=2, sticky = "w")
        Radiobutton(frb, variable=self.rbp, value=1, takefocus=False).grid(row=0,column=3, sticky="e")
        frb.columnconfigure(0, weight=2)
        frb.columnconfigure(1, weight=1)
        frb.columnconfigure(2, weight=2)
        frb.columnconfigure(3, weight=1)
        frb.pack(side=TOP, anchor="nw", fill='x')

        f0 = Frame(f)
        self.s0=entry.EntryFrame(f0, s0Entries, lWidth=16, eWidth=8, relief=GROOVE, bd=2)
        self.s0.pack()
##        self.s0.disable("Na2O")
##        self.s0.disable("Al2O3")
        self.s2=entry.EntryFrame(f0, s2Entries, lWidth=16, eWidth=8, relief=GROOVE, bd=2)

        f0.pack(side=TOP)
        Label(f, text="Results", font=font0).pack(anchor="w")
        self.s1=entry.EntryFrame(f, s1Entries, lWidth=16, eWidth=8, relief=GROOVE, bd=2)
        self.s1.pack(side=TOP)
        self.s1.disable()
        f1 = Frame(f)
        f1.pack(side = LEFT, fill=X, expand=1)
        Button(f1, text="Calculate", command=self.test).pack(side=LEFT, fill=X, expand=1)
        Button(f1, text="SysCAD", command=self.getSysCAD).pack(side=LEFT, fill=X, expand=1)
        f.pack(side=LEFT, anchor=NW)
        canvasFrame=Frame(self.baseFrame)
        canvasFrame.pack(side=LEFT, fill=BOTH, expand=YES)
        self.of=OutputFrame(canvasFrame, font=font3)
        self.rbp.trace("w", self.doEntryType)


    def doEntryType(self, foo, bar, baz):
        et = self.rbp.get()
        if et==1:
            self.s0.pack_forget()
            self.s2.pack()
        else:
            self.s2.pack_forget()
            self.s0.pack()


    def extractDPData(self):
        for x, y in zip(dpnames, ab.DPDATA):
            self.__dict__[x] = y


    def getSysCAD(self):
        try:
            sel = self.of.getSelection()
        except:
            tkMessageBox.showerror("Selection Invalid","Select SysCAD data\nfrom text window")
            self.of.appendText(syscadEx)
            return
         
        s = [l.split() for l in sel.splitlines()]
        cdic = {}.fromkeys(syscadNames, 0.0)
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
        self.s0["NaFormate"] = 0.12*cdic["Na2C5.2O7.2"]
        self.s0["NaAcetate"] = 0.88*cdic["Na2C5.2O7.2"]
            


                               
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
        for x in dpnames:
            self.s1[x] = self.__dict__[x]
            
        self.of.clearText()

        atxt = self.of.appendText
        
        atxt((ostr1 % (t-273.15,    self.I_m, self.I_c)))
        atxt((ostr2 % (p/100.,    self.P_Sat, self.I_c25)))
        atxt(""); atxt(ostr3)
        atxt("---------------------------------------------")
        atxt((ostr4 % tuple(ab.Comp_molkg[:9])))
        atxt((ostr5 % tuple(ab.Comp_molL[:9])))
        atxt((ostr6 % tuple(ab.Comp_molL25[:9])))
        atxt((ostr7 % tuple(ab.Comp_mpercent[:9])))
        atxt((ostr8 % tuple(ab.Comp_gL[:9])))
        atxt("")
        atxt(ostrs[0]); atxt(ostrs[1])
        atxt(ostrs[2] % (self.Al2O3, self.TC, self.TA, ab.OC[0], ab.OC[1]))
        try:
            ac = self.Al2O3/self.TC
        except:
            ac = 0.0
        atxt(ostrs[3] % ac)
        atxt(ostrs[4] % (self.TempSat, self.BPE))
        atxt(ostrs[5] % (self.Cp_Liq, 1000.*self.Rho_Liq))
        atxt(ostrs[6] % (self.Cp_H2O, 1000.*self.Rho_H2O))
        atxt(ostrs[7] % (self.Cp_phi, self.V_phi))
        atxt(ostrs[8] % self.Cp_LiqH2O)
        atxt("\nSaturation Indices")
        atxt("".join(sinames[:5]))
        atxt(("%-13.5f"*5) % tuple(ab.SI[:5]))
        atxt("".join(sinames[5:]))
        atxt(("%-13.5f"*5) % tuple(ab.SI[5:]))
        atxt("\nSolubilities")
        atxt(solnames)
        atxt(("Solubility M  " + "%9.4f"*6) % tuple(ab.SolML))
        atxt(("Solubility mgk" + "%9.4f"*6) % tuple(ab.Solmkg))
        atxt("")



def main():
    global app, bayer_, root, done
    def done():
        root.destroy()
    root=Tk()
    root.protocol("WM_DELETE_WINDOW", done)
    app=MyMain(root, TestMenu())
    root.title("Amira Bayer Calculations")
    root.iconify()
    root.update()
    root.deiconify()
    root.geometry("900x700-5+5")
    dllSetup()
    root.mainloop()

main()

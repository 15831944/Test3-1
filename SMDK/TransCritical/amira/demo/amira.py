from Tkinter import *
import tkMessageBox
from generic import GenericMain, OutputFrame
import re
import entry
from ctypes import *

font0 = "Helvetica 10 bold"
font3 = "Courier 8"
font4 = "Courier 8 bold"
    
dpnames = ["I_m", "I_c", "I_c25", "P_Sat", "Al2O3", "TC", "TA", "TempSat",
           "BPE", "Cp_Liq", "Cp_H2O", "Rho_Liq", "Rho_H2O", "Cp_phi",
           "V_phi",  "Cp_LiqH2O",  "Phi", "Aw", "V25", "WT", "H", "S"]

dpdnames = dpnames[:-4]+dpnames[-2:]

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
    ["NaOxalate    ", "%", "0"],
    ["NaAcetate  ", "%", "0"],
    ["NaFormate  ", "%", "0"],
    ["NaF        ", "%", "0"]
    ]

s2Entries = [
    ["Temperature", "C", "100."],
    ["Pressure   ", "bar", "10."],
    ["Al2O3      ", "gpl", "220"],
    ["TC         ", "gpl", "250"],
    ["NaCl       ", "gpl", "0"],
    ["TA         ", "gpl", "250"],
    ["Na2SO4     ", "gpl", "0"],
    ["NaOxalate ", "gpl", "0"], 
    ["NaAcetate  ", "gpl", "0"],
    ["NaFormate  ", "gpl", "0"],
    ["NaF        ", "gpl", "0"]
    ]


s0Vars = [x[0].strip() for x in s0Entries]
inSpecies = s0Vars[2:]
             


s1Entries = [[("%-10s" % x), ".", "0"] for x in dpdnames]
syscadNames = ["H2O", "NaAl[OH]4", "NaCl", "Na2C2O4", "Na2C5.2O7.2", "Na2CO3" , "Na2SO4", "NaOH"]

syscadEx ='''Example: select the following text and try again...

XPG_1.Content.H2O(l) (%)	60.32
XPG_1.Content.NaAl[OH]4(l) (%)	30.64
XPG_1.Content.NaCl(l) (%)	0.26
XPG_1.Content.NaOH(l) (%)	2.86
XPG_1.Content.Na2C2O4(l) (%)	0.23
XPG_1.Content.Na2C5.2O7.2(l) (%)	4.25
XPG_1.Content.Na2CO3(l) (%)	1.41
XPG_1.Content.Na2SO4(l) (%)	0.04
'''

helpTxt = '''Edit the individual fields and press the calculate button.

Data may also be edited as text directly in this text window.


Paste SysCAD data into the text area and press SysCAD button
This will parse the data and calculate mass fractions of each
species present.

Each line is parsed to search for standard species, and if found, will look for a
number at the end of the line. Thus

H2O  4
NaCl 1

will give a 20% saline solution.

Properties may also be calculated over a range. Thus to see how the specific heat and
density vary between 200C and 300C, highlight the following line (you can highlight a
line by triple clicking on it):

Cp_Liq, Rho_Liq for Temperature in 200, 300, 10

and press "Data"

Any of the result variables (Im, Temp_Sat, BPE etc) can be chosen to plot against any of
the data variables (Temperature, Concentrations), for example:

BPE for NaCl in 0, 1, .2

Options Menu: AutoClear  - toggle clearing of text screen after each calculation
              Solubility - toggle solubilities and solubility indices display
              Activities - toggle activities display
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
ostr1 = "t =  %8.2f C         I_m =  %8.4f mol/kg      I_c(t, p) =  %8.4f mol/L"
ostr2 = "p =  %8.2f bar     p_sat =    %8.3f bar       I_c(25 C) =  %8.4f mol/L"
ostr3 = "\nConc. units   Al(OH)3    NaOH    NaCl  Na2CO3  Na2SO4    NaOx    NaAc  NaForm     NaF\n"+\
        "-"*85
ostr4 = "mol/kg       " +"%8.4f"*9
ostr5 = "mol/L (t,p)  " +"%8.4f"*9
ostr6 = "mol/L (25C)  " +"%8.4f"*9
ostr7 = "mass-%%       " +"%8.3f"*9
ostr8 = "g/L (25C)    " +"%8.2f"*9

ostrs = [
"                  A            C            S            OC           OC",
"                              NaOH     NaOH+Na2CO3   Na acetate   Na formate",
"g/L (25 C)    %8.2f     %8.2f     %8.2f      %8.2f   %8.2f",
"A/C = %6.3f   (Al2O3 eq)  (Na2CO3 eq)  (Na2CO3 eq)     (C eq)       (C eq)",
"       B.p. =%8.2fC (%+7.2fK)",
"Cp_sln = %8.2f kJ/kgK       rho_sln = %8.2f kg/m^3",
"Cp_H2O = %8.2f kJ/kgK       rho_H2O = %8.2f kg/m^3",
"Cp_phi = %8.2f J/K/mol       V_phi =  %8.3f cm3 mol-1",
"Cp_sln = %8.2f kJ/kgK/ (g H2O)-1"
]




def frange(start, stop, step):
    return [float(start+i*step) for i in range(int((stop-start)/step) + 1)]


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
        self.menuDic = {}

    def mainMenuBar(self, w):
        mm=Menu(w)  # main menu
        self.acv = IntVar()
        self.acv.set(0)
        self.dsi = BooleanVar()
        self.dsi.set(False)
        self.mm=mm
        for x  in ["File", "Options", "Help"]:
            self.menuDic[x] = Menu(mm, tearoff=0)
            mm.add_cascade(label=x, menu=self.menuDic[x])
        
        m=self.menuDic["File"]  #0
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
        self.menuDic["Options"].add_checkbutton(label="AutoClear",
                          variable=self.acv, onvalue=1, offvalue=0) 
        self.menuDic["Options"].add_checkbutton(label="Solubilities",
                          variable=self.dsi) 

        self.menuDic["Help"].add_command(label="Help")
        return mm



class AmiraBayer:

    def __init__(self, Temp_C, Pressure_bar, InUnits=3):

        # Parameters
        self.Temp_C = Temp_C
        self.Pressure_bar = Pressure_bar
        self.InComp = (c_double*11)()
        self.InUnits = InUnits

        # Exported double Arrays
        self.P = c_double()
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
        self.DPDATA = (c_double*22)() # Catchall for doubles...

        # Exported longs
        self.NOutComp = c_long(9)
        self.NOC = c_long(2)
        self.NGamma = c_long(12)
        self.NSI = c_long(10)
        self.NSol = c_long(6)
        
    def fromDic(self, dic):
        self.Temp_C = dic["Temperature"]
        self.Pressure_bar = dic["Pressure"]
        self.InComp[0]=self.InComp[1]=0.0
        for i, x in enumerate(inSpecies):
            self.InComp[i+2] = dic[x]
        self.bayer()

    def bayer(self):
        self.P.value = self.Pressure_bar
        
        bayer_(
            c_double(self.Temp_C),    #  TempC,
            c_long(self.InUnits),      #  InUnits,      INTEGER!
            self.P,    #  Pressure,
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
        self.rbp.set(3)
        frb = Frame(f)  # Frame for radiobuttons
        Label(frb, text="Mass %").grid(row=0, column=0, sticky="w")
        Radiobutton(frb, variable=self.rbp, value=3, takefocus=False).grid(row=0,column=1,sticky="e")
        Label(frb, text="gpl@25").grid(row=0, column=2, sticky = "w")
        Radiobutton(frb, variable=self.rbp, value=11, takefocus=False).grid(row=0,column=3,sticky="e")
        Label(frb, text="Sat Mass %").grid(row=1, column=0, sticky="w")
        Radiobutton(frb, variable=self.rbp, value=2, takefocus=False).grid(row=1,column=1,sticky="e")
        Label(frb, text="Sat gpl@25").grid(row=1, column=2, sticky = "w")
        Radiobutton(frb, variable=self.rbp, value=10, takefocus=False).grid(row=1,column=3,sticky="e")


        for x in range(4):
            frb.columnconfigure(x, weight=2-(x%2))
        frb.pack(side=TOP, anchor="nw", fill='x')

        f0 = Frame(f)
        self.s0=entry.EntryFrame(f0, s0Entries, lWidth=14, eWidth=8, relief=GROOVE, bd=2)
        self.s0.pack()
        self.s2=entry.EntryFrame(f0, s2Entries, lWidth=14, eWidth=8, relief=GROOVE, bd=2)

        f0.pack(side=TOP)
        Label(f, text="Results", font=font0).pack(anchor="w")
        self.s1=entry.EntryFrame(f, s1Entries, lWidth=14, eWidth=8, relief=GROOVE, bd=2)
        self.s1.pack(side=TOP)
        self.s1.disable()
        f1 = Frame(f)
        f1.pack(side = LEFT, fill=X, expand=1)
        Button(f1, text="Calculate", command=self.test, underline=0).grid(sticky="ew")
        Button(f1, text="SysCAD", command=self.getSysCAD).grid(row=0, column=1,sticky="ew")
        Button(f1, text="Data", command=self.getPlotLine,
               underline=0).grid(row=0, column=2,sticky="ew")
        for x in range(3):
            f1.columnconfigure(x, weight=1)
        f.pack(side=LEFT, anchor=NW)
        canvasFrame=Frame(self.baseFrame)
        canvasFrame.pack(side=LEFT, fill=BOTH, expand=YES)
        self.of=OutputFrame(canvasFrame, font=font3, width=90)
        self.of.txt.tag_config("b", font=font4)
        self.rbp.trace("w", self.doEntryType)
        self.menus.menuDic["Help"].entryconfig(0, command=self.doHelp)

    def doEntryType(self, foo, bar, baz):
        et = self.rbp.get()
        if et==11 or et==10:
            self.s0.pack_forget()
            self.s2.pack()
        else:
            self.s2.pack_forget()
            self.s0.pack()
        if et==2:
            self.s0.disable("Pressure")
        elif et==3:
            self.s0.enable("Pressure")
        elif et==10:
            self.s2.disable("Pressure")
        elif et==11:
            self.s2.enable("Pressure")
        


    def doHelp(self):
        self.of.clearText()
        self.of.appendText(helpTxt)


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

        self.rbp.set(3)
        s = sel.splitlines()
        cdic = {}.fromkeys(syscadNames, 0.0)
        tot = 0.0
        for line in s:
            for x in syscadNames:
                if x in line:
                    lis = line.split()
                    try:
                        qty = float(lis[-1])
                        tot+=qty
                        cdic[x] = qty
                    except:
                        pass
                    break
                
        if tot>0.0:
            for x in cdic.keys():
                cdic[x] /= tot
        self.s0["NaOH"] = cdic["NaOH"]+40./118.*cdic["NaAl[OH]4"]
        self.s0["Al[OH]3"] = 78./118.*cdic["NaAl[OH]4"]
        for x in ["Na2CO3", "NaCl", "Na2SO4"]:
            self.s0[x] = cdic[x]
        self.s0["NaOxalate"] = cdic["Na2C2O4"]
        self.s0["NaFormate"] = 0.12*cdic["Na2C5.2O7.2"]
        self.s0["NaAcetate"] = 0.88*cdic["Na2C5.2O7.2"]
            

    def getPlotLine(self):
        try:
            sel = self.of.getSelection()
        except:
            tkMessageBox.showerror("Selection Invalid","Select line\nfrom text window")
            

        ldic = {}
        lre = re.compile("(.*)\Wfor\W(.*)\Win(.*)")
        res = lre.search(sel)
        if res:
            print res.group(0)
            print res.group(1)
            print res.group(2)
            print res.group(3)
        iv = res.group(2).strip()
        if iv not in s0Vars:
            print "%s not in s0Vars" % iv
            return
        dv = [x.strip() for x in res.group(1).split(',')]
        for x in dv:
            if x not in dpnames:
                print "%s not in dpnames" % x
                return
        try:
            rnge = [float(x.strip()) for x in res.group(3).split(',')]
            print rnge
            vals = frange(*rnge)
        except:
            print "frange failure", res.group(3)
            return

        for x in inSpecies:
            ldic[x] = self.s0[x]*100.
        ldic["NaF"]=self.s0["NaF "]
        ldic["Temperature"] = self.s0["Temperature"]-273.15
        ldic["Pressure"] = self.s0["Pressure"]/100.
        ab.InUnits = self.rbp.get()

        os = "\n%12s" % iv
        for x in dv:
            os+="%12s" % x
            
        self.of.appendText(os)

        for x in vals:
            ldic[iv] = x
            ab.fromDic(ldic)
            self.extractDPData()
            os = "%12.3f" % x
            for x in dv:
                os += "%12.3f" % self.__dict__[x]
            self.of.appendText(os)
       
                               
    def test(self):
        calcType = self.rbp.get()
        if calcType in (2,3):
            s = self.s0
        else:
            s = self.s2
        
        t  = s["Temp"]
        p = s["Press"]

        res = s.getValues()
        ab.InUnits = calcType
        if calcType in (2,3):
            ab.InComp[0]=ab.InComp[1]=0.0
            for i, x in enumerate(res[2:]):
                ab.InComp[i+2] = 100.*x
        else:
            for i, x in enumerate(res[2:]):
                ab.InComp[i] = x
        
        ab.Temp_C = t-273.15
        ab.Pressure_bar = p/100.
        ab.bayer()
        self.extractDPData()
        for x in dpdnames:
            self.s1[x] = self.__dict__[x]
        if self.menus.acv.get():
            self.of.clearText()


        if calcType in (2,10):
            p = s["Pressure"]=ab.P.value*100.
        atxt = self.of.appendText
        
        atxt((ostr1 % (t-273.15,    self.I_m, self.I_c)))
        atxt((ostr2 % (p/100.,    self.P_Sat, self.I_c25)))
        atxt(ostr3, None, "b")
        atxt((ostr4 % tuple(ab.Comp_molkg[:9])))
        atxt((ostr5 % tuple(ab.Comp_molL[:9])))
        atxt((ostr6 % tuple(ab.Comp_molL25[:9])))
        atxt((ostr7 % tuple(ab.Comp_mpercent[:9])))
        atxt((ostr8 % tuple(ab.Comp_gL[:9])))
        atxt("")
        atxt(ostrs[0], None, "b"); atxt(ostrs[1], None, "b")
        atxt(ostrs[2] % (self.Al2O3, self.TC, self.TA, ab.OC[0], ab.OC[1]))
        try:
            ac = self.Al2O3/self.TC
        except:
            ac = 0.0
        atxt(ostrs[3] % ac)
        atxt("Thermodynamic Properties of the aqueous phase", 1, "b")
        atxt(ostrs[4] % (self.TempSat, self.BPE))
        atxt(ostrs[5] % (self.Cp_Liq, 1000.*self.Rho_Liq))
        atxt(ostrs[6] % (self.Cp_H2O, 1000.*self.Rho_H2O))
        atxt(ostrs[7] % (self.Cp_phi, self.V_phi))
        atxt(ostrs[8] % self.Cp_LiqH2O)
        if self.menus.dsi.get():
            atxt("\nSaturation Indices", None, "b")
            atxt("".join(sinames[:5]))
            atxt(("%-13.5f"*5) % tuple(ab.SI[:5]))
            atxt("".join(sinames[5:]))
            atxt(("%-13.5f"*5) % tuple(ab.SI[5:]))
            atxt("\nSolubilities", None, "b")
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
    def atst(foo, bar=None):
        print foo, bar
        app.test()
    def agpl(foo, bar=None):
        app.getPlotLine()

    root.bind("<Alt-c>", atst)
    root.bind("<Alt-d>", agpl)
    root.title("Amira Bayer Calculations")
    root.iconify()
    root.update()
    root.deiconify()
    root.geometry("-5+5")
    dllSetup()
    root.mainloop()

main()

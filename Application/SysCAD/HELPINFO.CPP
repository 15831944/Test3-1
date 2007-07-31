//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "..\scdlib\resource.h"
#include "..\scexec\resource.h"
#include "resource.h"
//#include "gpwfuncs.h"
#include "helpinfo.h"
                   
//===========================================================================

CHelpItemArray::CHelpItemArray(UINT GroupID)
  {
  iGroupID = GroupID;
  pNxt = NULL;
  }

//---------------------------------------------------------------------------

int CHelpItemArray::FindItem(UINT ItemID)
  {
  for (int i=0; i<GetSize(); i++)
    {
    if (m_pData[i].iItemID==ItemID)
      return i;
    }
  return -1;
  }

//---------------------------------------------------------------------------

//===========================================================================

CHelpInfoMngr::CHelpInfoMngr()
  {
  pFirst = NULL;
  }

//---------------------------------------------------------------------------

void CHelpInfoMngr::UpdateOptions()
  {
  bShowTooltips = (ScdPFUser.RdInt("General", "ShowDialogTooltips", 1)!=0);
  }

//---------------------------------------------------------------------------

CHelpItemArray* CHelpInfoMngr::FindGroup(UINT GroupID)
  {
  CHelpItemArray* p = pFirst;
  while (p)
    {
    if (p->iGroupID==GroupID)
      return p;
    p = p->Next();
    }
  return NULL;
  }

//---------------------------------------------------------------------------

void CHelpInfoMngr::AddHelpInfo(UINT GroupID, UINT ItemID, char* ShortDesc, char* LongDesc)
  {
  CHelpItemArray* pGrp = FindGroup(GroupID);
  if (pGrp==NULL)
    {
    pGrp = new CHelpItemArray(GroupID);
    pGrp->pNxt = pFirst;
    pFirst = pGrp;
    }
  pGrp->Add(CHelpItem(ItemID, ShortDesc, LongDesc));
  }

//---------------------------------------------------------------------------

CHelpItem* CHelpInfoMngr::GetHelpInfo(UINT GroupID, UINT ItemID)
  {
  CHelpItemArray* pGrp = FindGroup(GroupID);
  if (pGrp)
    {
    const int index = pGrp->FindItem(ItemID);
    if (index>=0)
      return pGrp->HelpItem(index);
    }
  return NULL;
  }

//---------------------------------------------------------------------------

void CHelpInfoMngr::Init()
  {
  //NOTE: This should all go into a seperate data base/spreadsheet file !!!

  //AddHelpInfo(UINT GroupID, UINT ItemID, char* ShortDesc, char* LongDesc)
  /*OLD Balance page of PROBAL solver box...??? still used ???
  AddHelpInfo(IDD_BALANCE         , IDC_PB_METH             , "The method used to solve the flow sheet"   , "");
  AddHelpInfo(IDD_BALANCE         , IDC_PB_REL              , "If increased the solution will converge faster, but will be less accurate"   , "");
  AddHelpInfo(IDD_BALANCE         , IDC_PB_ABS              , "If increased the solution will converge faster, but will be less accurate"   , "");
  AddHelpInfo(IDD_BALANCE         , IDC_PB_STATE            , ""   , "");
  AddHelpInfo(IDD_BALANCE         , IDC_PB_MAXITERS         , "The maximum number of iterations after which SysCAD automatically terminates"   , "");
  AddHelpInfo(IDD_BALANCE         , IDC_PB_DAMPING          , ""   , "");
  AddHelpInfo(IDD_BALANCE         , IDC_PB_ITERS            , "The current iteration"   , "");
  AddHelpInfo(IDD_BALANCE         , IDC_WORSTERRS           , "Lists the units with the worst errors preventing convergence"   , "");
  AddHelpInfo(IDD_BALANCE         , ID_PB_Start             , "Starts the balancing of the flow sheet"   , "");
  AddHelpInfo(IDD_BALANCE         , ID_PB_Stop              , "Stops the balancing of the flow sheet"   , "");
  */
  //Change tag dialog...
  AddHelpInfo(IDD_CHANGETAG       , IDC_OLDTAG              , "The existing tag name"   , "");
  AddHelpInfo(IDD_CHANGETAG       , IDC_NEWTAG              , "The desired tag name which will replace the former name"   , "");
  //Compare tag values dialog...
  AddHelpInfo(IDD_COMPAREDLG      , IDC_FILELIST            , "Lists the scenario comparisions currently configured"   , "");
  AddHelpInfo(IDD_COMPAREDLG      , IDC_ADD                 , "Allows a scenario comparision to be configured"   , "");
  AddHelpInfo(IDD_COMPAREDLG      , IDC_REMOVE              , "Removes the selected scenario comparision"   , "");
  AddHelpInfo(IDD_COMPAREDLG      , IDC_EDIT                , "Edits the selected scenario comparision"   , "");
  AddHelpInfo(IDD_COMPAREDLG      , IDC_COMPARE             , "Compares the selected scenario comparision"   , "");
  AddHelpInfo(IDD_COMPAREDLG      , IDC_OPTIONS             , "Displays the tag value comparison options dialog"   , "");
  AddHelpInfo(IDD_COMPAREDLG      , IDC_RESETOUTFILE        , "If the comparison is output to a file, this file is deleted before the comparison is performed"   , "");
  //Compare tag value comparison options dialog...
  AddHelpInfo(IDD_COMPAREOPTIONSDLG , IDC_SHOWSTRINGS       , ""   , "");//Self explanatory
  AddHelpInfo(IDD_COMPAREOPTIONSDLG , IDC_SHOWALLMISSING    , ""   , "");//Self explanatory
  AddHelpInfo(IDD_COMPAREOPTIONSDLG , IDC_SHOWALLTAGS       , "Displays all tags in both scenarios whether they differ or not"   , "");
  AddHelpInfo(IDD_COMPAREOPTIONSDLG , IDC_SHOWSORTEDFILE    , "The file is sorted from the worst to the least differences, the number of which is specified by 'The maximum number of differences to show'"  , "");
  AddHelpInfo(IDD_COMPAREOPTIONSDLG , IDC_RESETOUTFILE      , "If the comparison is output to a file, this file is deleted before the comparison is performed"   , "");
  AddHelpInfo(IDD_COMPAREOPTIONSDLG , IDC_ABSTOLERANCE      , "Used to determine the required accuracy of the comparison when determining errors"   , "");
  AddHelpInfo(IDD_COMPAREOPTIONSDLG , IDC_RELTOLERANCE      , "Used to determine the required accuracy of the comparison when determining errors"   , "");
  AddHelpInfo(IDD_COMPAREOPTIONSDLG , IDC_MAXCOUNT          , "Specifies the maximum number of differences output in the sorted file"   , "");
  //Compare tag values Edit dialog...
  AddHelpInfo(IDD_COMPAREUPDATEDLG , IDC_FILENAME           , "Name and location of the first scenario or snapshot file"   , "");
  AddHelpInfo(IDD_COMPAREUPDATEDLG , IDC_FILENAME2          , "Name and location of the second scenario or snapshot file"   , "");
  AddHelpInfo(IDD_COMPAREUPDATEDLG , IDC_OUTFILENAME        , "Name and location of the file containing the comparison results"   , "");
  AddHelpInfo(IDD_COMPAREUPDATEDLG , IDC_FILE2RADIO1        , ""   , "");//Self explanatory
  AddHelpInfo(IDD_COMPAREUPDATEDLG , IDC_FILE2RADIO2        , ""   , "");//Self explanatory
  AddHelpInfo(IDD_COMPAREUPDATEDLG , IDC_BROWSE             , "Locate the first scenario or snapshot file"   , "");
  AddHelpInfo(IDD_COMPAREUPDATEDLG , IDC_BROWSE2            , "Locate the second scenario or snapshot file"   , "");
  AddHelpInfo(IDD_COMPAREUPDATEDLG , IDC_BROWSE3            , "Locate the file to contain the results of the comparison"   , "");
  AddHelpInfo(IDD_COMPAREUPDATEDLG , IDC_OUTRADIO1          , "Send the results of the comparison to the 'Tag Value Comparison Results' window"   , "");//Self explanatory
  AddHelpInfo(IDD_COMPAREUPDATEDLG , IDC_OUTRADIO2          , "Send the results of the comparison to the designated file"   , "");
//Driver dialog...   PROPERTY PAGE
  AddHelpInfo(IDD_DRIVER_PAGE     , IDC_NODRIVERS           , "Indicates how many drivers are active"   , "");
  AddHelpInfo(IDD_DRIVER_PAGE     , IDC_NOSLOTS             , "Indicates how many slots are active"   , "");
  AddHelpInfo(IDD_DRIVER_PAGE     , IDC_DRVIGNORESON        , "Ignoring of individual slots has no affect unless it is switched on here"   , "");
  AddHelpInfo(IDD_DRIVER_PAGE     , IDC_DRVFORCESON         , "Forcing of individual slots has no affect unless it is switched on here"   , "");
  AddHelpInfo(IDD_DRIVER_PAGE     , IDC_FORCEALL            , "If Driver Forces is on, this applies the forced value for each slot"   , "");
  AddHelpInfo(IDD_DRIVER_PAGE     , IDC_FORCENONE           , "Switches forcing off for all slots"   , "");
  AddHelpInfo(IDD_DRIVER_PAGE     , IDC_APPLYBTN            , "Applies the changes that have been made"   , "");
//Driver Slots dialog...   PROPERTY PAGE
  AddHelpInfo(IDD_DRIVERSLOTS_PAGE , IDC_SLOTCOMBO          , "Allows the browsing through of the slots"   , "");
  AddHelpInfo(IDD_DRIVERSLOTS_PAGE , IDC_APPLYBTN           , "Applies the changes that have been made"   , "");
  AddHelpInfo(IDD_DRIVERSLOTS_PAGE , IDC_IGNOREREAD         , ""   , "");//Self explanatory
  AddHelpInfo(IDD_DRIVERSLOTS_PAGE , IDC_ENABLEFORCES       , ""   , "");//Self explanatory
  AddHelpInfo(IDD_DRIVERSLOTS_PAGE , IDC_IGNOREWRITE        , ""   , "");//Self explanatory
  AddHelpInfo(IDD_DRIVERSLOTS_PAGE , IDC_FORCEVALUE         , "A force value can be entered in the engineering units shown"   , "");
  AddHelpInfo(IDD_DRIVERSLOTS_PAGE , IDC_DRVDESC            , "Description of the driver address of the slot"   , "");
  AddHelpInfo(IDD_DRIVERSLOTS_PAGE , IDC_SLOTDESC           , "Describes how the value of the slot is processed"   , "");
  AddHelpInfo(IDD_DRIVERSLOTS_PAGE , IDC_DESC               , "The slot description, as given in the slot configuration file"   , "");
  AddHelpInfo(IDD_DRIVERSLOTS_PAGE , IDC_RAWVALUE           , "The raw value for the slot is continuously updated when SysCAD is running"   , "");
  AddHelpInfo(IDD_DRIVERSLOTS_PAGE , IDC_IO                 , "Displays the IO configuration for the slot"   , "");
  AddHelpInfo(IDD_DRIVERSLOTS_PAGE , IDC_VALUE              , "The current value for the slot is continuously updated when SysCAD is running"   , "");
//Find tag dialog...
  AddHelpInfo(IDD_FINDTAGDLG      , IDC_GRFZOOMPROMPT       , ""   , "");//Self explanatory
  AddHelpInfo(IDD_FINDTAGDLG      , IDC_GRFAUTOACCESS       , ""   , "");//Self explanatory
  AddHelpInfo(IDD_FINDTAGDLG      , IDC_FINDTAGCOMBO        , "Enter the name of the unit which you wish to locate, the tag name is not case sensitive"   , "");
  AddHelpInfo(IDD_FINDTAGDLG      , IDC_FIND                , "Searches the graphic pages for the specified tag"   , "");
  AddHelpInfo(IDD_FINDTAGDLG      , IDC_FINDTAGCOMBO2       , ""   , "");//To be re-designed shortly thus pointless to doc
  AddHelpInfo(IDD_FINDTAGDLG      , IDC_FIND2               , ""   , "");//To be re-designed shortly thus pointless to doc
  //General options - Toolbar dialog...
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TB0                 , "Displays or hides the toolbar"                                                   , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TB1                 , "Displays or hides the toolbar"                                                   , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TB2                 , "Displays or hides the toolbar"                                                   , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TB3                 , "Displays or hides the toolbar"                                                   , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TB4                 , "Displays or hides the toolbar"                                                   , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TB5                 , "Displays or hides the toolbar"                                                   , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TB6                 , "Displays or hides the toolbar"                                                   , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TB7                 , "Displays or hides the toolbar"                                                   , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBTITLE0            , "Allows the name of the toolbar to be changed"                                    , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBTITLE1            , "Allows the name of the toolbar to be changed"                                    , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBTITLE2            , "Allows the name of the toolbar to be changed"                                    , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBTITLE3            , "Allows the name of the toolbar to be changed"                                    , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBTITLE4            , "Allows the name of the toolbar to be changed"                                    , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBTITLE5            , "Allows the name of the toolbar to be changed"                                    , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBTITLE6            , "Allows the name of the toolbar to be changed"                                    , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBTITLE7            , "Allows the name of the toolbar to be changed"                                    , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBEDIT0             , "Customises the toolbar"                                                          , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBEDIT1             , "Customises the toolbar"                                                          , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBEDIT2             , "Customises the toolbar"                                                          , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBEDIT3             , "Customises the toolbar"                                                          , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBEDIT4             , "Customises the toolbar"                                                          , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBEDIT5             , "Customises the toolbar"                                                          , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBEDIT6             , "Customises the toolbar"                                                          , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBEDIT7             , "Customises the toolbar"                                                          , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBDEF0              , "Restores the default buttons and title to the toolbar"                           , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBDEF1              , "Restores the default buttons and title to the toolbar"                           , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBDEF2              , "Restores the default buttons and title to the toolbar"                           , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBDEF3              , "Restores the default buttons and title to the toolbar"                           , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBDEF4              , "Restores the default buttons and title to the toolbar"                           , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBDEF5              , "Restores the default buttons and title to the toolbar"                           , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBDEF6              , "Restores the default buttons and title to the toolbar"                           , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBDEF7              , "Restores the default buttons and title to the toolbar"                           , "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBTOOLTIPS          , "Displays or hides the hints in yellow boxes when the mouse is moved over a toolbar button", "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TBFLYBYS            , "Displays or hides the descriptions in the status bar when the mouse is moved over a toolbar button", "");
  AddHelpInfo(IDD_GENTOOLBARS     , IDC_TB_STATUSBAR        , "Displays or hides the status bar at the bottom of the main SysCAD window"        , "");
  //General options - Trend dialog...
  AddHelpInfo(IDD_GENTRENDS       , IDC_PRJDEFRECORDIT      , "Any newly trended tag will also be recorded in the historian ( provided the historian is on)", "");
  AddHelpInfo(IDD_GENTRENDS       , IDC_PRJSHOWNEWDLG       , "Any newly trended tag displays a dialog box containing historian-specific information ( provided the historian is on)", "");
  AddHelpInfo(IDD_GENTRENDS       , IDC_GLOBAL              , "Synchronises the scrolling and zooming of all the trend windows"                 , "");
  AddHelpInfo(IDD_GENTRENDS       , IDC_BROWSE              , "Brings up the trend options dialog box"                                          , "");
  AddHelpInfo(IDD_GENTRENDS       , IDC_REBUILD             , "Refreshes all the tags in all the trend windows"                                 , "");
  //General options - Other dialog...
  AddHelpInfo(IDD_GENOTHER        , IDC_PRJGRFDEFERLOAD     , "Do not load graphics symbols when loading project"                               , "");
  AddHelpInfo(IDD_GENOTHER        , IDC_PRJLOADGRF          , "Loads all the graphics symbols on all the graphics pages"                        , "");
  AddHelpInfo(IDD_GENOTHER        , IDC_PRJGRFAUTOLOAD      , "Automatically load graphics symbols when graphics page is activated"             , "");
  AddHelpInfo(IDD_GENOTHER        , IDC_UPDATEANNOT         , "Update all graphics annotation blocks when solver is stopped"                    , "");
  AddHelpInfo(IDD_GENOTHER        , IDC_ANIMATIONON         , "Update annimation dynamics such as fill areas and bar graphs in graphics while solving", "");
  AddHelpInfo(IDD_GENOTHER        , IDC_DLGTOOLTIPS         , "Show tooltips (descriptions in yellow boxes) for items in dialog boxes"          , "");
  AddHelpInfo(IDD_GENOTHER        , IDC_LOADPREVPRJ         , "Prompt user to load previous project when SysCAD is started"                     , "");
  AddHelpInfo(IDD_GENOTHER        , IDC_EDITOR              , "Full filename (and path if required) of program to be used as a text editor"     , "");
  //General options - DB dialog...
  AddHelpInfo(IDD_GENDB           , IDC_DBCOPYFROMOLD       , "Copy Original Database files to new project when doing Save Version or Save As to retain previous revisions of data", "");
  AddHelpInfo(IDD_GENDB           , IDC_DBREMOVEPREVREVS    , "Removes the previous revisions of data from database when saving", "");
  //General options - Stats dialog...
  AddHelpInfo(IDD_GENSTATS        , IDC_HSTFILENAME         , "Full filename (and path if required) of file to contain the historian statistics", "");
  AddHelpInfo(IDD_GENSTATS        , IDC_HSTITERCNT          , "The time period over which the statistics are generated"                         , "");
  AddHelpInfo(IDD_GENSTATS        , IDC_HSTRESET            , "Resets the number of iterations"                                                 , "");
  AddHelpInfo(IDD_GENSTATS        , IDC_HSTFETCH            , "Generates the statistic file"                                                    , "");
  //General options - Snapshot dialog...
  AddHelpInfo(IDD_GENSNAPSHOTS    , IDC_PROMPT              , "Displays the file save dialog box where a filename can be entered"               , "");
  AddHelpInfo(IDD_GENSNAPSHOTS    , IDC_AUTOINC             , "A filename is generated, based on a combination of the ‘Filename prefix’ and ‘Next SnapShot number’ fields", "");
  AddHelpInfo(IDD_GENSNAPSHOTS    , IDC_REUSE               , "Uses the filename specified in the ‘Default fixed filename’ field, the old snap-shot is always overwritten.", "");
  AddHelpInfo(IDD_GENSNAPSHOTS    , IDC_FIXEDNAME           , "The filename used by the 'Reuse same filename' option"                           , "");
  AddHelpInfo(IDD_GENSNAPSHOTS    , IDC_PREFIX              , "The filename prefix used by the 'Automatically incrementing filename' option"    , "");
  AddHelpInfo(IDD_GENSNAPSHOTS    , IDC_NUMBER              , "The next number suffix used by the 'Automatically incrementing filename' option" , "");
  //General options - Cycles dialog...
  AddHelpInfo(IDD_GENSCRSAVEDLG   , IDC_SYSTEMSCRSAVE       , "Disables the System screen saver"                                                , "");
  AddHelpInfo(IDD_GENSCRSAVEDLG   , IDC_SYSCADSCRSAVE       , "Enables the SysCAD screen saver / screen cycler"                                 , "");
  AddHelpInfo(IDD_GENSCRSAVEDLG   , IDC_SCRCYCLES           , "Specifies the number of screen cycles to implement"                              , "");
  AddHelpInfo(IDD_GENSCRSAVEDLG   , IDC_CYLELIST            , "Selects the screen cycles, so that they can be modified"                         , "");
  AddHelpInfo(IDD_GENSCRSAVEDLG   , IDC_WAITTIME            , "Specifies the display time of the selected screen cycle in seconds"              , "");
  AddHelpInfo(IDD_GENSCRSAVEDLG   , IDC_DESCRIPTION         , "Edits the name of the selected screen cycle"                                     , "");
  AddHelpInfo(IDD_GENSCRSAVEDLG   , IDC_WNDUP               , "Alters the sequence of the displayed windows, by moving the selected window up in the list", "");
  AddHelpInfo(IDD_GENSCRSAVEDLG   , IDC_WNDDWN             , "Alters the sequence of the displayed windows, by moving the selected window down in the list", "");
  AddHelpInfo(IDD_GENSCRSAVEDLG   , IDC_WINDOWLIST          , "Selects the windows to be displayed in the current screen cycle, by double clicking them", "");
  //General options - Debug dialog...
  AddHelpInfo(IDD_GENDBGDLG       , IDC_DBGENSEXNS          , "Selects which general debug section is to be modified"                           , "");
  AddHelpInfo(IDD_GENDBGDLG       , IDC_DBGVALUE            , "Edits the value of the currently selected debug sub-section"                     , "");
  AddHelpInfo(IDD_GENDBGDLG       , IDC_RESET_VAL           , "Sets all the debug sub sections of all the debug general sections to zero" , "");
  AddHelpInfo(IDD_GENDBGDLG       , IDC_DBGON               , "Turns the debug facility of SysCAD on, equivalent to the command line option '/d'", "");
  AddHelpInfo(IDD_GENDBGDLG       , IDC_DBGDELETE           , "Deletes the debug file, by default '\\syscad71\\syscad.dbg'"                       , "");
  AddHelpInfo(IDD_GENDBGDLG       , IDC_TXTVERSION          , "Displays which compiler version has been used ie release, debud or release debug", "");
  AddHelpInfo(IDD_GENDBGDLG       , IDC_TXTOTHER            , "??? Uses SPARES"                                                                 , "");
  AddHelpInfo(IDD_GENDBGDLG       , IDC_DBGSUBSEXNS         , "Selects and alters the value of the debug sub-sections, by double-clicking the required sub-section" , "");
  //Insert Unit dialog...
  AddHelpInfo(IDD_MDLINSERTUNITDLG , ID_MDLHELP             , "Displays the SysCAD user help"   , "");
  AddHelpInfo(IDD_MDLINSERTUNITDLG , ID_MDLDOC              , "Displays the help for the currently selected model"   , "");
  AddHelpInfo(IDD_MDLINSERTUNITDLG , IDC_MODEL_LIST         , "Select the required model from the list of available models in the project"   , "");
  AddHelpInfo(IDD_MDLINSERTUNITDLG , IDC_MDLTAG             , "Alter the tag name for the unit"   , "");
    //see code as it changes according to use   AddHelpInfo(IDD_MDLINSERTUNITDLG , IDC_MORE               , ""   , "");
  AddHelpInfo(IDD_MDLINSERTUNITDLG , IDC_MDLROT             , "Enter the number of degrees to rotate the position of the unit in an anti-clockwise direction"   , "");
  AddHelpInfo(IDD_MDLINSERTUNITDLG , IDC_HideTag            , "Hides the tag from view when the unit is inserted into the graphics page"   , "");
  AddHelpInfo(IDD_MDLINSERTUNITDLG , IDC_ALLSYMB            , "Allows all the symbols in the icon library to become available instead of only the model-specific icons"   , "");
  AddHelpInfo(IDD_MDLINSERTUNITDLG , IDC_MDLSYMBOL          , "Select the icon that best describes the unit"   , "");
  AddHelpInfo(IDD_MDLINSERTUNITDLG , IDC_MDLSCL_X           , "Adjusts the horizontal scaling of the unit, a value less than one decreases the scaling and vice verse"   , "");
  AddHelpInfo(IDD_MDLINSERTUNITDLG , IDC_MDLSCL_Y           , "Adjusts the vertical scaling of the unit, a value greater than one increases the scaling and vice verse"   , "");
  AddHelpInfo(IDD_MDLINSERTUNITDLG , IDC_MODEL_DESC         , "A brief description of the selected model"   , "");
  //General page for configure project dialog...
  //AddHelpInfo(IDD_PRJGENDLG      , IDC_PRJPROBAL           , "This mode determines the mass and energy balance of the model"                                 , "");
  //AddHelpInfo(IDD_PRJGENDLG      , IDC_PRJDYNAMIC          , "This mode models the behaviour of the unit operations and their connectivity"                                 , "");
  AddHelpInfo(IDD_PRJMODESDLG     , IDC_PRJREALTIME         , "Uses the computer clock as the timebase for the project"                                 , "");
  AddHelpInfo(IDD_PRJMODESDLG     , IDC_PRJCURRENTTIME      , ""  , "");//will soon be changed
  AddHelpInfo(IDD_PRJMODESDLG     , IDC_PRJDTIMEMAX         , ""  , "");//will soon be changed
  AddHelpInfo(IDD_PRJGRFTAGSDLG   , IDC_PRJNUMERICSTARTOK   , "Allow tags to begin with a numeric character (0..9)"                      , "");
  AddHelpInfo(IDD_PRJGRFTAGSDLG   , IDC_PRJNUMERICOK        , "Allow tags that only contain numeric characters (0..9)"                   , "");
  AddHelpInfo(IDD_PRJGRFTAGSDLG   , IDC_PRJNUMERICCHAR      , "Character to be inserted at begining of tag if tag is invalid"                   , "");
  //AddHelpInfo(IDD_PRJGRFTAGSDLG   , IDC_PRJDRVON            , "Switch SysCAD communication driver(s) on or off"                                 , "");
  //AddHelpInfo(IDD_PRJGENDLG       , IDC_PRJDRVREADALL       , "Read all the slots when the driver file is reloaded"           , ""); //????                      
  //AddHelpInfo(IDD_PRJGENDLG       , IDC_PRJRELOADDRV        , "Reloads the driver slot-configuration file"                                 , "");
  //AddHelpInfo(IDD_PRJGENDLG       , IDD_PRJDRVNAME          , "Specifies the name of the driver slot-configuration file"                                 , "");
  //Historian page for configure project dialog...
  AddHelpInfo(IDD_PRJHISTDLG      , IDC_PRJHSTNAME          , "The name of the historian-managing files (maximum 8 characters)"    , "");//is the 8 char restrictn still applic                             
  AddHelpInfo(IDD_PRJHISTDLG      , IDC_PRJHSTSIZE          , "The size of the historian files (a multiple of 64K)"                                 , "");
  AddHelpInfo(IDD_PRJHISTDLG      , IDC_PRJHSTFORCECNT      , "The number of iterations after which all tag values are stored in the historian regardless"                                 , "");
  AddHelpInfo(IDD_PRJHISTDLG      , IDC_PRJHSTSLOTNOEST     , "The maximum number of historian tags that are to be recorded"                                 , "");
  AddHelpInfo(IDD_PRJHISTDLG      , IDC_PRJHSTSCENNAME      , "The name of the binary data files used to store the historian data (maximum 8 characters)"                                 , "");
  AddHelpInfo(IDD_PRJHISTDLG      , IDC_PRJHSTFILESMIN      , "The maximum number of files allowed in the data folder before the oldest file is transferred or deleted"                                 , "");
  AddHelpInfo(IDD_PRJHISTDLG      , IDC_PRJHSTDATADIR       , "The folder where the binary data files are saved"                                 , "");
  AddHelpInfo(IDD_PRJHISTDLG      , IDC_PERMHIST            , "This enables the archiving of the old binary data files, once the data files exceed the maximum in the data folder"                                 , "");
  AddHelpInfo(IDD_PRJHISTDLG      , IDC_PRJHSTCATDIR        , "The folder used to archive the old data files"                                 , "");
  AddHelpInfo(IDD_PRJHISTDLG      , IDC_PRJHSTFILESMAX      , "The number of files to be kept in the archive folder, once this number is exceeded the oldest file is deleted"                                 , "");
  AddHelpInfo(IDD_PRJHISTDLG      , IDC_PRJHSTON            , "Activates the historian and thus enables tag values to be written to disk"                                 , "");
  AddHelpInfo(IDD_PRJHISTDLG      , IDC_RESTART             , "Retains the knowledge of which tags are being recorded and deletes the data files"                                 , "");
  AddHelpInfo(IDD_PRJHISTDLG      , IDC_DELETE              , "Deletes all historian files and all knowledge of recorded tags"                                 , "");
  //Merge Project dialog...
  AddHelpInfo(IDD_MERGEDLG        , IDC_TXTMSTRPRJ          , ""    , "");//self explan
  AddHelpInfo(IDD_MERGEDLG        , IDC_SLAVEPRJ            , "Path and name of the project file of the project that is to be merged into the current project"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_BROWSE              , "Allows the location of the project file of the project that is to be merged"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_MSTRCHANGEALL       , "All tags will be changed regardless"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_MSTRCHANGEDUP       , "Only tags that are the same as the those in the merged project will be changed"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_MSTRCHANGENONE      , "Tags will not be changed, thus the merged project should change its duplicate tags"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_MSTRPREFIX          , "Changed tags are prefixed with the designated characters"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_MSTRSUFFIX          , "Changed tags are suffixed with the designated characters"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_MSTRPRESUF          , "Characters to add to changed tags"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_SLVCHANGEALL        , "All tags will be changed regardless"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_SLVCHANGEDUP        , "Only tags that are the same as the those in the current project will be changed"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_SLVCHANGENONE       , "Tags will not be changed, thus the current project should change its duplicate tags"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_SLVPREFIX           , "Changed tags are prefixed with the designated characters"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_SLVSUFFIX           , "Changed tags are suffixed with the designated characters"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_SLVPRESUF           , "Characters to add to changed tags"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_APPENDCHARS         , ""    , "");//self explan
  AddHelpInfo(IDD_MERGEDLG        , IDC_MRGREPORTFILE       , "Specifies the location of the file containing the results of the merging process"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_MDLCFG              , "The model library used by both the current project and the project to be merged"    , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_TXTERR              , ""    , "");//not visible
  AddHelpInfo(IDD_MERGEDLG        , IDC_MSTRSTNUMTAGSOK     , "Allow tags to begin with a numeric character (0..9)"                      , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_MSTRNUMTAGSOK       , "Allow tags that only contain numeric characters (0..9)"                   , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_SLVSTNUMTAGSOK      , "Allow tags to begin with a numeric character (0..9)"                      , "");
  AddHelpInfo(IDD_MERGEDLG        , IDC_SLVNUMTAGSOK        , "Allow tags that only contain numeric characters (0..9)"                   , "");

  //New project dialog...
  AddHelpInfo(IDD_NEWPRJ_DLG      , IDC_MDLLIST             , "Lists the available model libraries, placing the project in its folder"    , "");
  AddHelpInfo(IDD_NEWPRJ_DLG      , IDC_NEWGRFWNDS          , "Adds a graphics window to the project"    , "");
  AddHelpInfo(IDD_NEWPRJ_DLG      , IDC_NEWTRENDWNDS        , "Adds a trend window to the project"    , "");
  //Tag block data transfer dialog...
  AddHelpInfo(IDD_COPYBLKDLG      , IDC_CPYMODEL            , "Select model type"                                                               , "Select model type for data transfer");
  AddHelpInfo(IDD_COPYBLKDLG      , IDC_CPYUNIT             , "Select source unit"                                                              , "");
//  AddHelpInfo(IDD_COPYBLKDLG      , IDC_CPYLBLKSLIST        , "Select source block, checked items are from the common ($CfgFiles) folder"     , "");
//  AddHelpInfo(IDD_COPYBLKDLG      , IDC_CPYRBLKSLIST        , "Checked items are in the common ($CfgFiles) folder"                            , "");
  AddHelpInfo(IDD_COPYBLKDLG      , IDC_CPYLBLKSLIST        , "Select source block, checked items are from the common ($CfgFiles) folder"    , "");
  AddHelpInfo(IDD_COPYBLKDLG      , IDC_CPYRBLKSLIST        , "Checked items are in the common ($CfgFiles) folder"                           , "");
  AddHelpInfo(IDD_COPYBLKDLG      , IDB_CPYNEW              , "Add new block to list"                                                           , "");
  AddHelpInfo(IDD_COPYBLKDLG      , IDB_CPYDELETE           , "Remove selected block from list"                                                 , "");
  AddHelpInfo(IDD_COPYBLKDLG      , IDB_CPYMAKEDEFAULT      , "Mark selected block as default for new units"                                    , "");
  AddHelpInfo(IDD_COPYBLKDLG      , IDC_CPYOTHERUNITSLIST   , "List of possible destination units"                                              , "");
  AddHelpInfo(IDD_COPYBLKDLG      , IDB_CPYTRANSFER         , "Transfer selected data"                                                          , "");
  AddHelpInfo(IDD_COPYBLKDLG      , IDC_CPYDETAILSLIST      , "Preview of individual tags to be copied"                                         , "");
  //SOLVE MODE Running with options ...
  //Active Pages          PROPERTY PAGE
  AddHelpInfo(IDD_SOLVEACTIVE     , IDC_ACTPAGES            , "Lists the graphic and model-library pages associated with the project"                                                               , "");
  AddHelpInfo(IDD_SOLVEACTIVE     , ID_PGACTIVATE           , "Activates the currently selected page"                                                               , "");
  AddHelpInfo(IDD_SOLVEACTIVE     , ID_PGDEACTIVATE         , "Deactivates the currently selected page, if this is a model-library page then all graphic pages associated with it are also deactivated"                                                               , "");
  //DYNAMIC Running with options ...
  //Time - Page          PROPERTY PAGE
  AddHelpInfo(IDD_RUNCFGMAIN      , IDC_RUNFOREVER          , "No limit is put upon the ending of the simulation, thus it needs to be manually stopped"                                                               , "");
  AddHelpInfo(IDD_RUNCFGMAIN      , IDC_RUNSTEADY           , "Running of the simulation will stop once it has achieved steady state condition"                                                               , "");
  AddHelpInfo(IDD_RUNCFGMAIN      , IDC_RUNFOR              , "Requires the required running time of the simulation to be specified"                                                               , "");
  AddHelpInfo(IDD_RUNCFGMAIN      , IDC_RUNUNTIL            , "Requires the time in the future (with respect to the current SysCAD time) at which the simulation will stop"                                                               , "");
  AddHelpInfo(IDD_RUNCFGMAIN      , IDC_RUNFORITERS         , "Requires the number of iterations that the simulation is to achieve"                                                               , "");
  AddHelpInfo(IDD_RUNCFGMAIN      , IDC_FORTIME             , "Time in hours:minutes:seconds:hundredths of a second that the simulation is to run for"                                                               , "");
  AddHelpInfo(IDD_RUNCFGMAIN      , IDC_UNTILTIME           , "Time beyond the current SysCAD time, at which the simulation will stop"                                                               , "");
  AddHelpInfo(IDD_RUNCFGMAIN      , IDC_FORITERS            , "Number of iterations that the simulation is to run for"                                                               , "");
  AddHelpInfo(IDD_RUNCFGMAIN      , IDC_CURRENTTIME         , "Current SysCAD time in hours:minutes:seconds:hundredths of a second"                                                               , "");
  AddHelpInfo(IDD_RUNCFGMAIN      , IDC_CHGTIME             , "Allows the current SysCAD time to be changed"                                                               , "");//unsure
  AddHelpInfo(IDD_RUNCFGMAIN      , IDC_MAXSTEP             , "Allows the maximum time that a step can take to be set "                                                               , "");
  //Method - Page          PROPERTY PAGE
  AddHelpInfo(IDD_RUN_METHOD      , IDC_ODE_METH            , "The method used to solve the flow sheet"                                                               , "");
  AddHelpInfo(IDD_RUN_METHOD      , IDC_ODE_STEP            , "Determines the method for calculating the step size, a variable step size is slower but gives the most accuracy"                                                               , "");
  AddHelpInfo(IDD_RUN_METHOD      , IDC_ODE_REL_EPS         , "To prevent very small values from preventing convergence this must be greater than or equal to the absolute tolerance."                                                               , "");
  AddHelpInfo(IDD_RUN_METHOD      , IDC_ODE_ABS_EPS         , "If increased the solution will converge faster, but will be less accurate"                                                               , "");
  AddHelpInfo(IDD_RUN_METHOD      , IDC_ODE_STEP_CUR        , "Current step size, in seconds - used to determine the fixed step size, the greater the step size the faster the relative speed but the less accurate the solution"                                                               , "");
  AddHelpInfo(IDD_RUN_METHOD      , IDC_ODE_STEP_MIN        , "Minimum step size, in seconds - used when determining the variable step size"                                                               , "");
  AddHelpInfo(IDD_RUN_METHOD      , IDC_ODE_STEP_MAX        , "Maximum step size, in seconds - used for both the variable and fixed step size"                                                               , "");
  //Network - Page          PROPERTY PAGE
  AddHelpInfo(IDD_RUN_NETWORK     , IDC_RUN_LNKABS          , "If increased the solution will converge faster, but will be less accurate"                                                               , "");
  AddHelpInfo(IDD_RUN_NETWORK     , IDC_RUN_LNKREL          , "To prevent very small values from preventing convergence this must be greater than or equal to the absolute tolerance."                                                               , "");
  AddHelpInfo(IDD_RUN_NETWORK     , IDC_RUNSPILLAREA        , "Determines the default vessel to which material, that overflows from any unit, will report"                                                               , "");
  AddHelpInfo(IDD_RUN_NETWORK     , IDC_RUN_NETDERATE       , "The leeway given to overcome the discrepancy between the converged network and individual link values"                                                               , "");
  //Tears - Page          PROPERTY PAGE
  AddHelpInfo(IDD_RUNTEAR         , IDC_RUNTEAR_MAXITERS    , "Maximum number of iterations to be spent resolving the tears"                                                               , "");
  AddHelpInfo(IDD_RUNTEAR         , IDC_RUNTEAR_REL         , "To prevent very small values from preventing convergence this must be greater than or equal to the absolute tolerance."                                                               , "");
  AddHelpInfo(IDD_RUNTEAR         , IDC_RUNTEAR_ABS         , "If increased the solution will converge faster, but will be less accurate"                                                               , "");
  AddHelpInfo(IDD_RUNTEAR         , IDC_RUNTEAR_METH        , "Method used to resolve the tears"                                                               , "");
  AddHelpInfo(IDD_RUNTEAR         , IDC_RUNTEAR_DAMPING     , "For Direct Substition this damping is applied to the previous input, its complement is applied to the current result and both are added together to become the new input"                                                               , "");// self explan for adaptive direct subs :  For Adaptive Direct Substition this is the lowest damping factor to be used
  //Options - Page          PROPERTY PAGE
  AddHelpInfo(IDD_RUNOPTN         , IDC_RUN_NOCHANGE        , "All the surge vessels remain as they were last saved"                                                               , "");
  AddHelpInfo(IDD_RUNOPTN         , IDC_RUN_EMPTYALL        , "All the surge vessels are emptied of material"                                                               , "");
  AddHelpInfo(IDD_RUNOPTN         , IDC_RUN_INITIALALL      , "All the surge vessels are initialized to the conditions specified in each model's 'Setup' section"                                                               , "");
  AddHelpInfo(IDD_RUNOPTN         , IDC_RUN_FLWMOMENTON     , ""                             , "");//omitted as it doesnt work
  //PROBAL Solving with options ...
  //Convergence - Page          PROPERTY PAGE
  AddHelpInfo(IDD_SOLVECFG         , IDC_SLV_MAXITERS       , "The maximum number of iterations after which SysCAD automatically terminates"                                                               , "");
  AddHelpInfo(IDD_SOLVECFG         , IDC_SLV_REL            , "If increased the solution will converge faster, but will be less accurate"                                                               , "");
  AddHelpInfo(IDD_SOLVECFG         , IDC_SLV_ABS            , "If increased the solution will converge faster, but will be less accurate"                                                               , "");
  AddHelpInfo(IDD_SOLVECFG         , IDC_SLV_METH           , "The method used to solve the flow sheet"                                                               , "");
  AddHelpInfo(IDD_SOLVECFG         , IDC_SLV_DAMPING        , "For Direct Substition this damping is applied to the previous input, its complement is applied to the current result and both are added together to become the new input"                                                               , "");// self explan for adaptive direct subs :  For Adaptive Direct Substition this is the lowest damping factor to be used
  //Options - Page          PROPERTY PAGE
  AddHelpInfo(IDD_SOLVEOPTN        , IDC_SLV_INITIALISE     , "Sets all flows to zero, except for the manual tears, which specify their own requirements for initialization"                                                               , "");
  AddHelpInfo(IDD_SOLVECFG         , IDC_SLV_STATEWND       , "Displays the solution window while the flow sheet is being converged, after which it dissapears"                                                               , "");
  AddHelpInfo(IDD_SOLVECFG         , IDC_SLV_PINRQD         , "Keeps the solution window visible, even after the flow sheet has been solved"                                                               , "");
  //Query dialog.. 
  AddHelpInfo(IDD_QUERYDLG         , IDC_FILENAME           , "Path and name of the file which will contain the query"       , "");
  AddHelpInfo(IDD_QUERYDLG         , IDC_STARTTIME          , "The time at which the query is to commence, by default this is the start of the current trend window"       , "");
  AddHelpInfo(IDD_QUERYDLG         , IDC_ENDTIME            , "The time at which the query is to terminate, by default this is the end of the current trend window"       , "");
  AddHelpInfo(IDD_QUERYDLG         , IDC_NOPTS              , "Determines the number of entries to be used if the query is to consist of equally spaced average points"       , "");
  AddHelpInfo(IDD_QUERYDLG         , IDC_OPTION             , "Whenever the value of a recorded tag changes, an entry is logged to the query file"       , "");
  AddHelpInfo(IDD_QUERYDLG         , IDC_OPTION1            , "The specified number of entries are equally distributed throughout the time interval of the query"       , "");
  AddHelpInfo(IDD_QUERYDLG         , IDC_QTIME               , "Hours:Minutes:Seconds.Hundredths of seconds   Day/Month/Year"       , "");
  AddHelpInfo(IDD_QUERYDLG         , IDC_QTIME2             , "The time in seconds, with the starting time of the query being 0"       , "");
  AddHelpInfo(IDD_QUERYDLG         , IDC_QTIME3             , "Both full time and the time in seconds is recorded in the query"       , "");
  AddHelpInfo(IDD_QUERYDLG         , IDC_QTIME4             , "Time is not recorded in the query"       , "");
  AddHelpInfo(IDD_QUERYDLG         , IDC_TAG                , "Used to modify the tags in the query or to add new tags to the query"       , "");
  AddHelpInfo(IDD_QUERYDLG         , IDC_ADDTAG             , "Adds a default entry 'NewTag' to the list of tags, which must then be replaced with the required tag"       , "");
  AddHelpInfo(IDD_QUERYDLG         , IDC_REMOVETAG          , "Removes the currently selected tag from the list of tags"       , "");
  AddHelpInfo(IDD_QUERYDLG         , IDC_TAGLIST            , "Lists the tags that are to be recorded in the query"       , "");
  AddHelpInfo(IDD_QUERYDLG         , IDC_HEADINGS           , "Gives the starting and ending times for the query and labels the time and tag columns appropriately"       , "");
  //Scripts dialog.. 
  AddHelpInfo(IDD_SCRIPTSDLG       , IDC_FILELIST           , "File name and path of all the command scripts used by the project"       , "");
  AddHelpInfo(IDD_SCRIPTSDLG       , IDC_ADD                , "Locate and add a new command script to the list"       , "");
  AddHelpInfo(IDD_SCRIPTSDLG       , IDC_REMOVE             , "Remove the currently highlighted command script from the list"       , "");
  AddHelpInfo(IDD_SCRIPTSDLG       , IDC_EDIT               , "Alter the path or filename of the currently highlighted command script"       , "");
  AddHelpInfo(IDD_SCRIPTSDLG       , IDC_EXECSCRIPT         , "Execute the currently highlighted command script"       , "");
  AddHelpInfo(IDD_SCRIPTSDLG       , IDC_EDITFILE           , "Opens the currently highlighted command script file in a text editor, so that it can be altered"       , "");
  ///Trends timebase dialog
  AddHelpInfo(IDD_TIMEBASE         , IDC_TRENDTITLE         , "The name of the currently selected trend window"       , "");
  AddHelpInfo(IDD_TIMEBASE         , IDC_EDITSTART          , "Commencing time of the selected trend window"       , "");
  AddHelpInfo(IDD_TIMEBASE         , IDC_EDITDURATION       , "Duration of the selected trend window"       , "");
  AddHelpInfo(IDD_TIMEBASE         , IDC_BROWSE             , "Allows you to view and alter all the tags in the selected trend window"       , "");
  AddHelpInfo(IDD_TIMEBASE         , IDC_OPTIONS            , "Allows you to set display options for the selected trend window"       , "");
  UpdateOptions();
  }

//---------------------------------------------------------------------------

void CHelpInfoMngr::Term()
  {
  CHelpItemArray* p = pFirst;
  while (p)
    {
    CHelpItemArray* pd=p;
    p = p->Next();
    delete pd;
    }
  pFirst=NULL;
  }

//===========================================================================

CHelpInfoMngr HelpMngr;

//===========================================================================

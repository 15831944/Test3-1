//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __REVHIST_H
#define  __REVHIST_H

//===========================================================================
//
//  Compile Options
//
//===========================================================================

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ Used for changing SysCAD revision history and       ++
// ++ update/patch notes.                                 ++
// ++ See scdver.h for version and build numbers          ++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#define SCD_PATCHNOTE    ""   /* Update/patch number/comment*/
#define SCD_PATCHNOTE    "Update 4"   /* Update/patch number/comment*/

//#define SCD_PATCHDATE    ""        /* Update/patch release date*/
#define SCD_PATCHDATE    "5 October 2006"
#define SCD_COMPILEDATE    __DATE__

#endif // __REVHIST_H

//===========================================================================
//===========================================================================
/*

Revision History
================

7.1.0.0  20 September 1996
--------------------------
1) First Non-Beta release of SysCAD with version numbering

7.1.0.1 : 3 October 1996
--------------------------
1) Simple graphics printing
2) Removed Splash window, improved about box and progress window

7.1.0.2 : 10 October 1996
--------------------------
1) Fixed bugs in driver manager related to the connect column, generally
   improved driver tag subscriptions.
2) Fixed change tag dialog box bugs and crashes.
3) Changed new project dialog box, made the historian load for a new project.
4) Completely re-organised toolbars. Including new buttons, changes to Options
   Toolbars page and toolbars can now be customized.
5) Added option to allow new tags to be trended without any dialog boxes popping up.
6) Fixes to Specie and elemental break-downs.
7) For access and trend window, while a field is edited, it behaves as a proper
   windows edit control.
8) Made button for switching trend lines on and off in trend window.


7.1.1.0 : 20 October 1996
--------------------------
1) Rewritten TAGOBJ GetData/Putdata Methods & Classes.
   '#define UsingZTagObj' enables the new Code.
2) Rewrite of the way Reactions are done to handle Equilibrium reactions.

7.1.1.1 : 25 October 1996
--------------------------
1) Implemented Dynamic Fills and Dynamic Bars.
2) General graphics improvements. Including disabling DblClick and buttons while
   a command is busy; improved insert link, insert unit, construct link and
   construct unit.
3) Changes to toolbars.

7.1.1.2 : 1 November 1996
--------------------------
1) Implemented Move Entity in graphics.
2) Historian: Removed seperate thread for creating block files; prevent historian
   name and scenario name from being the same; create required directories.
3) Prevent driver tags "getting" other driver tags using the connect column of
   the slot configuration file.
4) Implemented EO_CanClose for OLE and Historian - prevents crashes on exit.
5) Improved OLE Excel Reports - ignore blank rows/columns; checks for formula
6) Re-organised indexed string lists for tagged objects.


7.1.2.0 : 5 November 1996
--------------------------
1) Active / Inactive Units / Pages. Rationalised Probal & Dynamic modes.
2) Implemented Dynamic & Probal Dialogs
3) Merged QAL models (HX/M_Base/M_Surge) back into MdlLib
4) Fixes to OLE Reports ...
5) Re-organised message window
6) Etc.

7.1.2.1 : 12 November 1996
--------------------------
1) PGM fixes: can use nested calls for GetTag/SetTag; now checks for valid
   conversions; new variable "modeltag" can be accessed from PGM
2) Fixed print for access window with one tabbed page
3) Fixed edit toolbar bug; and implemented right-click to edit toolbar

7.1.2.2 : 5 December 1996
--------------------------
 1) Improved messages for PID2 controller
 2) Fixed bug related to window stack, topmost windows etc
 3) Implemented annotation blocks for graphics
 4) Implemented toggles for graphics
 5) Rearranged/reorganised status bar
 6) Added "Other" page to Options property sheet
 7) Added scaling for dynamic bars, annotation and toggles
 8) Improved Find Tag function (error messages)
 9) Reports: Improved OLE recovery if a report fails
10) Trend Reports: Improved OLE trend reports to also draw chart

7.1.2.3 : 9 December 1996
--------------------------
 1) Fixed bug on Debug page of General Options Dialog.
 2) PGM bug fix: In a user defined class function: can now use a local class
    variable as a parameter when calling a global class function.
 3) Fixed scroll bars in pull down list boxes in access window.

7.1.2.4 : 31 December 1996
--------------------------
1) PGM: added variable StopSimulation which can be used to stop SysCAD solving
   from the PGM.
2) Improved change tag (prevent illegal tag from being entered).

7.1 Rev 2.5 : 6 January 1997
------------------------------
1) Added command script functionality.
2) Improved interpretation of command line options.


7.1 Rev 3.0 : 17 January 1997
------------------------------
1) Improvements and extensions to command scripts.
2) Started implementing scenarios. Can load and save scenarios. Scenarios,
   snapshots and current values can be compared using a variety of options.
3) Improved select window dialog. Hopefully fixed recursive bug that occured
   in save window layout or select window.

7.1 Rev 3.1 : 18 January 1997
------------------------------
1) Saving / Recovery of Arrays rearranged to be compatible with named elements
2) AGModel Tweaked to save/recover correctly


7.1 Rev 4.0 : 22 January 1997
------------------------------
1) Changed project view. Now contains a tree with all models/units - can easily
   access model, find it in graphics, change its tag or access attached models.
2) Project window's position was not saved, and it was not shown in select window
   list. Fixed.

7.1 Rev 4.1 : 23 January 1997
------------------------------
1) If a graphics page is minimised when a project is loaded, a find tag will not
   find the tag even though it does exist. Fixed.
2) Prevent user from changing the main flow solver/model dll tag.
3) Fixed new project bug - after selecting a model library, a prompt to create
   the new project was given from the incorrect directory.


7.1 Rev 5.0 : 11 February 1997
------------------------------
1) Implemented SysCAD Security/Licensing functionality. SysCAD can now be protected.
2) Fixed project bugs: (a) If a project is open and another project is loaded or a
   new project is started, and the Cancel button is pressed (in the save old project
   dialog), it did not work correctly (the old project was not unloaded correctly).
   The Cancel button has been removed.  (b) When changing projects, the options,
   project configuration, reports or script commands dialogs were not closed.
   (c) When a project is loaded and a problem with model DLL is found (old style
   project; DLL not found; or wrong version number) a missleading dialog was shown.
3) OLE Excel report fixes: (a) Excel report feedback now includes project file and
   version.  (b) A note is now shown in message window when a report has been completed.
   (c) When Excel file is closed, an Excel 'Do you want to save' dialog was sometimes
   shown when it should not have been.  (d) DoChart(...) for trend reports did not work
   with the correct Time(...) options.  (e) If a requested tag for a trend report did
   not exist in the historian, an error message was not given, junk numbers were returned.
4) Stopped saving full path for all files in reports and command scripts dialogs
   which are in the project path. ie filenames become relative.
5) Improved 'Project Save As Options' dialog to include all file types. It now
   disables all file types that aren't found in the project being saved.
6) Improved project view. It now also contains a tree with most of the files used by
   a project. Can edit some directly, can also see all models that use the same file.
7) When using the browse button to add a report or script file to a list the current
   directory was changed. This can have disasterous results when (for eg) loading
   reactions or pgms. Fixed (current directory is not changed).
8) Can choose text editor (default=notepad.exe) in general options dialog (Other page).
9) Removed ztagobj.* and all references to 'UsingZTagObj'

7.1 Rev 5.1 : 17 February 1997
------------------------------
1) Fixed Project|New and File|Project|New menu options which did not work correctly.
2) Improved security/licensing (check limits, disables options, etc)
3) Changed project view files tree to only show the full path when a file is not in
   the project directory. Implemented default left double click action in file tree.
4) When a project is opened on a smaller resolution, windows that are not visible
   (ie windows completely beyond the edges of the main window) are automatically
   moved to ensure they are visible in the main window.
5) When user right clicks on main background window, a pop-up menu is shown.
6) Added 'reconnect tags' option to pop-up menu for trend text windows.

7.1 Rev 5.2 : 24 February 1997
------------------------------
1) Started implementing merge project functionality.
2) Enabled SysCAD Trial License option.
3) Removed New Window menu item from the Window menu.
4) Changed pop-up menu for main background window, different if solving or stopped.
5) For a new project, the Cancel button for the select model library dialog did
   not work correctly.
6) The Cancel button, (in the save old project dialog shown when changing or starting
   a new project), that was removed in Rev 5.0 has been returned.
7) Project now saves and restores window Z-Order (order of top-most to bottom window).

7.1 Rev 5.3 : 28 February 1997
------------------------------
1) SysCAD should now work on Windows 95.
2) Some improvements to merge project functionality.
3) Fixed change tag bug in trend windows. (eg when changing P_6, P_66 also changed)
4) General improvements to command scripts. Now waits for script command to be
   completely finished before executing the next one.
5) Changed default SnapShot save action for a new project from automatically
   incrementing filename to prompt for filename.
6) Added SysCAD licensing functionality for Windows 95.
7) On Windows 95, the values that should be shown in the model window, were also
   shown on trend windows (ie fixed general paint problems for trends).
8) When loading driver tags, ensure the tags are unique (ie not same as a model tag).

7.1 Rev 5.4 : 6 March 1997
------------------------------
1) Completed merge project functionality.
2) Improved parsing of command script files, specifically the filename parameters for
   all command script functions.
3) When inserting or constructing a unit or link in the graphics, a tag containing a
   dot (.) was accepted as part of the tag name causing errors. This is now prevented.
4) The toolbar buttons for the current or top window which should be active are no
   longer active after performing certain tasks. (eg after saving a project or after
   loading a project) This has been fixed.
5) Fixed bug introduced in Rev 5.0: When editing a new reaction file the extension 'pgm'
   instead of 'rct' was used.
6) While inserting or constructing a unit or link in a graphics window, the user could
   right click to access a model. This caused problems and is now prevented.


7.1 Rev 6.0 : 7 March 1997
------------------------------
1) Rearranged formats and conversions for use in libraries (Now in DataAttr.*)
2) Moved FixEditV back into FixEdit
3) Re-arranged XY-function view. Formats and conversions now available for XY functions.
4) Can have particle size distribution for individual species.
5) File list in project view now includes pgm include files, merge project now also
   copies required pgm include files.

7.1 Rev 6.1 : 11 March 1997
------------------------------
1) Rearranged message window to contain two seperate lists.
2) Rearranged project view to have tabs for viewing different trees. Added tree to
   show engineering units (conversions).
3) Changed "run with options" dialog.
4) Started implementing pump curve visualisation.
5) When inserting or configuring links, the number of connections to each IO is shown.
   Instead of being the first IO tag, the default is now the first IO tag with the
   least connections.

7.1 Rev 6.2 : 19 March 1997
------------------------------
1) Re-organised conversions. SI Units are now fixed in SysCAD and cannot be changed
   in scd_cnvs.csv. (changes to datacnvs.* include more inline code, removed default
   cnv, cnvs use less memory, etc).
2) PGM variables can be marked as read-only using @. See PGM help file Rev 1.7.
3) Conversion information can now be assigned to a PGM variable. See PGM help file.
4) Added PGM functions CnvInfo(...), CnvVal(...) and Cnv(...). See PGM help file.
5) The split between the error lists in the message window can be adjusted. Use the
   left mouse button and drag. Left double click to equally space the lists.
6) Added mouse right click pop-up menu functionality for back-ground of message window.
7) Added mouse right click pop-up menu functionality for individual messages in
   lists. A mouse left double click on a message that refers to a tag brings up
   the access window for that tag.
8) Fixed bugs with script commands: CloseProject() and GenerateTagReport().
9) Improvements to Specification Sheet dialog box. Especially handling of read-only
   specsheets and enabling/disabling buttons.


7.1 Rev 7.0 : 28 March 1997
------------------------------
1) Invalid Numeric Tags
2) SizeDistributions
3) Solubility
4) Extentions to FxdEditBook ..
5) Momentum in flownets works .. to damp down oscillations in flow nets.
6) DataXchg Ids (xid..) rearranged to improve the management of the numbers.
7) Changes to improve the unloading/reloading of projects.
   Also recover most memory on shutdown.
   (MFC and some windows still do not free some memory).
8) Extentions to FxdEdit to allow the management of DataTags in Object Editors
9) Added TestScenario(..) to Scripts
10) Control of Qualities, can now create or remove these from within a link

7.1 Rev 7.1 : 5 April 1997
------------------------------
1) Improvements to unloading/reloading of projects and DLLs.

7.1 Rev 7.2 : 28 April 1997
------------------------------
1) Script commands has progress dialog with abort button.
2) Tag/scenario comparison report is now modeless dialog, can right click or left
   double click to access tags.
3) Some general graphics improvements.
4) Added dialog box to alter tag annotation properties such as show/hide, size
   and rotation. Added appropriate menu command and toolbar button.
5) Can mark a project as read-only, set ReadOnly value to 1 in the .spj file.
6) Improvements to flashing in dynamic mode.

7.1 Rev 7.3 : 7 May 1997
------------------------------
1) Some general graphics improvements including fix to deleting units, exploding
   units and constructing links.
2) Can enter functions in trend windows in the tag column starting with an '='.
   eg "=(GetTag(P_1.Qm)+GetTag(P_2.Qm))/2"

7.1 Rev 7.4 : 9 May 1997
------------------------------
1) Improvements to project window, shows missing models which can be deleted, etc.
2) Other improvements to Change Tag dialog, trend functions and changing projects.
3) Access kw_mdls.hlp as default model help.

7.1 Rev 7.5 : 20 May 1997
------------------------------
1) Further fixes to deleting/exploding graphics units.
2) Fixed PGM bug introduced in Rev 7.3 where PGM variables were deleted or ignored.
3) Re-organised historian query threads to use the stack efficiently.
4) Allow list of model help files to be set in models.cfg in the section ModelHelpFiles.

7.1 Rev 7.6 : 21 May 1997
------------------------------
1) Improvements to GenCon and PID models (PID model supports functions).
2) Improved pop-up menus for access window. Now a pop-up menu when right-clicking in
   the background of the access window.


7.1 Rev 8.0 : 5 June 1997
------------------------------
1) Improvements to right-click popup menu for trend and access windows.
2) Improvements to saving for a number of models so that a flowsheet is recovered
   properly when loading.
3) Started implementing tear-block functionality. This allows access to tear blocks
   and better saving/recovering of flow-sheets.
4) General improvements to a number of models.
5) Re-arranged cross-page connecter to improve management of connected tags.
6) Implemented select tag functionality from a pop-up dialog box.
7) General improvements to access window, scroll bars, buttons at top, spec-sheets,
   custom pages, access history, check-boxes, cursors, etc
8) New menu option for trends to record all tags in trend page in historian.
9) Improvements to message window
10) Other general improvements/bug fixes etc

7.1 Rev 8.1 : 11 June 1997
------------------------------
1) Improvements to message window
2) Fixes for dynamic solutions related to tears, etc
3) Dynamic mode model improvements, including fix to conveyor bugs and flashing.
4) Added tear blocks for PID controllers and improved PGM profilers.
5) Further improvements to access window and change tag dialog.
6) Improved Excel OLE Automation reports to work on Excel version 5 AND version 8.
7) Other general improvements/bug fixes etc

7.1 Rev 8.2 : 20 June 1997
------------------------------
1) Changes for compiles using MSVC 5.0 and smart heap.
2) Improvements to dynamic model for heat exchangers and flashing.
3) Rearranged location of sp_reactions and sp_queue; implemented Alumina models.
4) Other general improvements/bug fixes etc

7.1 Rev 8.3 : 25 June 1997
------------------------------
1) Implemented SysCAD_SetTags functionality from Excel.
2) Fixed error in Pow function for very large numbers.
3) Fixed bug in token parser.

7.1 Rev 8.4 : 4 July 1997
------------------------------
1) Improved SysCAD_SetTags functionality from Excel, changed reports dialog box.
2) Enlarged select window dialog box.
3) Fixed bugs for generating trend reports on Excel 8.
4) Added ProcessSetTags(filename, TagSetName) command to list of script commands.
5) For release versions, the Debug page does not appear in the general options
   tabbed dialog box unless Ctrl+Shift+D is pressed before viewing the dialog.

7.1 Rev 8.5 : 18 July 1997
------------------------------
1) Pressing the Close/Hide button on a temporary floating toolbar (eg temporary
   Zoom or Delete toolbars) is now the same as pressing the Esc key or Cancel
   button. The current command is cancelled instead of just hiding the toolbar.
2) Improved random/noise generator used by the PGM so that it can be seeded and
   each rondom generator uses its own independent random generating sequence.
3) When starting a second copy of SysCAD with a script in the command line, the
   script files in the command line are forwarded to the instance of SysCAD that
   is allready running.
4) Improved Excel OLE Automation to make use of an open instance of Excel instead
   of creating its own each time a report is generated.
5) Where possible, Excel Automation works with an open document. ie do not need to
   always close a spreadsheet before generating reports.
6) Where SysCAD spawns another process (eg notepad for editing a reaction) if you
   swoped back to SysCAD, it was not easy to go back to notepad from SysCAD. This
   has been fixed. eg each time you press Edit RCT in the Access window it goes
   directly back to the same reaction file every time.

7.1 Rev 8.6 : 14 August 1997
------------------------------
1) Major changes to insert unit dialog, including graphics symbol previews.
2) Major improvements to Update Tag Annotation dialog.
3) Major improvements to Annotation. Conversions, formats and labels can now be
   entered for annotation blocks.
4) Improvements to Tie/General-Split model and ProBal heat exchange model. Other
   general fixes/improvements.

7.1 Rev 8.7 : 12 September 1997
------------------------------
1) Project Save As now copies correct reaction (.rct) files to new project.
2) Improvements to General-Split model including loading/recovering projects.
3) Improvements to "noise" function/random number generation. (General and PGM)
4) Now supports floating network licenses.
5) Fixed problem related to recovering PGM variables after error re-loading PGM.

7.1 Rev 8.8 : 30 September 1997
------------------------------
1) General improvements/fixes including stack overflow and memory leaks.
2) Improvement to historian recovery.
3) Fixed Excel SetSysCADTags bug for empty cells.

7.1 Rev 8.9 : 6 October 1997
------------------------------
1) Prevent user from changing views while a graphics command (using a dialog)
   is partly complete.

7.1 Rev 8.10 : 15 October 1997
------------------------------
1) Change tag now changes tag in annotation block on graphics pages if required.
2) First implementation of copy tag block (allows user to copy a set of tag values
   to defauls or other tags or vica-versa).
3) Fixed PGM reload bug introduced in Rev 8.7
4) Fixed crash in access window for size-distributions.

7.1 Rev 8.11 : 17 October 1997
------------------------------
1) Button showing trend line colour in trend tag view now works on Windows 95.

7.1 Rev 8.12 : 23 October 1997
------------------------------
1) Project load and Excel process tag sets much faster due to improvement of
   implementation of XWriteTagItem() for connect/disconnect. (Excel OLE SetTags
   are 3 to 4 times faster!)
2) Improved method used for pop-up dialogs for the message window.
3) Improved message and action if model DLL is missing when loading a project.
4) Fixed recovery of Access page position.
5) Some improvements to copy tag block dialog.

7.1 Rev 8.13 : 30 October 1997
------------------------------
1) Improvements to copy tag block dialog.
2) Extended control of tag naming rules. Can select options in project
   configuration dialog. Rules relate to numeric tags and tags starting with
   a numeric character.


7.1 Rev 9.0 : 1 November 1997
------------------------------
1) Revision 9 is the last "frozen" version of SysCAD using Visual C++ 4.2.

7.1 Rev 9.1 : 7 November 1997
------------------------------
1) Fixes to select tag dialog, editing tags in access window after a PreStart
   check failure, historian and copy tag block dialog.

7.1 Rev 9.2 : 26 November 1997
------------------------------
1) Improvements to accessing driver information from tags in trend pages.
2) General efficiency improvements related to driver tags.
3) Improvements to PID controller model.
4) Improvements to checking when loading specie database.

7.1 Rev 9.3 : 5 December 1997
------------------------------
1) Fixed bug in copy tag block.
2) Improvements to tag parsing in trend view.
3) Improvements to change tag: (a) give major warning when change tag is used
   and all the graphics pages are not loaded. (b) tags used in PID_Con model
   are also changed. (c) tags in access window history list are also changed.
4) Improvements to delete tag/model which improve the behaviour of the trend
   pages, access window and some models (eg PID_Con).
5) Prevent tags from starting with the characters ' or =.
6) Moved screen model from separ1 to separ2. Model library separ1 contains
   'separation' models that do not use size distribution functionality, while
   those in separ2 do.
7) Other minor general improvements such as project configure dialog box for
   changeing numeric tags character; disable (make gray) relavent check boxes
   in access window while solving; fixed special tag copy from access window
   for check box tags; and case insensitive comparisons for Excel OLE report
   workbook names.

7.1 Rev 9.4 : 23 December 1997
------------------------------
1) Save all floating point model values with more precision (15 instead of 9).
2) Trend lines refresh when min or max range is changed in trend tag view.
3) Added new equation (CRC1_Cp) to specie database.
4) Fixed error in spills for conveyor model. (due to bug in specie_queue)
5) Fixed another bug in copy tag block. (for tags with pull down string lists)
6) Correctly handle two OLE Excel reports that have the same name but are in
   different locations by saving and closing the first 'problem' workbook.


7.2 Rev 0.0 : December 1997
------------------------------
1) Tears - Save/Recover improvments
         - Tolerance Strategy
2) Scenarios - Bug Fixes.
3) FlowSolution -
4) Flashing (VLE) - Multiphase and/or dynamic.
5) OEP Stable Solution.
6) Tweaks to pump/compressor curves
7) Trends will now handle DT's less than 1 sec.
8) General improvemonts/changes/etc
9) Includes all improvement upto 7.1 Rev 9.2.

7.2 Rev 0.1 : 17 December 1997
------------------------------
1) Includes remaining fixes from 7.1 Rev 9.3 and all those from 7.1 Rev 9.4.

7.2 Rev 0.2 : 5 January 1998
------------------------------
1) A graphics fix for a crash caused by explode entity.
2) ...


7.2 Rev 1.0 : 9 January 1998
------------------------------
1) Implement new Excel DAO database functionality.


????

1) A graphics fix for a crash caused by explode entity.
2) Added more tooltips for most of the common dialog boxes.
3) Changed the script command ResetHistorian() to RestartHistorian().
4) Improved messages when license conditions are exceeded.
5) Grayed out description for tags in trend window.

1) Fixed mass balance error for variable speed conveyor in dynamic mode.
2) Improved license options to allow for "remote" licenses.
3) Fixed a licensing error that did not allow SysCAD to run in demo
   mode on a windows 95 PC.
4) Disable report generation while solving in ProBal mode.
5) Fixed error relating to accessing model documentation when a new
   project is created.
6) Fixed error for saving window positions when SysCAD is minimised.
7) Fixed a bug intruduced in Rev 9.3 which gives a 'not allowed' error
   message when using a driver tag in a trend page.

1) Increased tag buffer and EO message buffer lengths.
2) Temporarily removed delete tag functionality in trend pages.
3) Fixed crash (under certain conditions) when changing the specie model.
4) Another bug fix/improvement for remote network licenses.
5) Improved some timers in Allen Bradley and GE Fanuc drivers and fixed
   a bug in these drivers which occurs when coms cannot be establish.
6) Added new model (drum filter) to separ1 and (cyclone) to separ2.
   Fixed bug in filter press.

7.2 Rev 2.0 : 4 March 1998
------------------------------
1) Fixes for QAL
2) Excel V7.0 repoting bug in Release "Fixed" - optimization switched off.
3) "QuickView" implemented.
4) SysCAD Fonts reduced to get better use out of "Screen Real Estate".
5) All the Implicit solution to date.

7.2 Rev 2.3 : 5 May 1998
------------------------------
1) Improved checking for long tag names.
2) Uses new version of CrypKey (4.3).
3) Fixed multiprocessor deadlock in Allen Bradley and GE Fanuc drivers.
4) Fixed loop time measurement in drivers for multiple PLCs.
5) Fixed crash that occurs when a tag name is changed while SysCAD is
   running that is used by a driver (connect column).
6) When a driver is reloaded, the previous PLC state is restored.
7) Historian now records value at correct time when recording is switched
   on or off while SysCAD is solving.
8) PGM's did not always delete tmp files, fixed.


... ??? ...


7.2 Rev 2.6 : 12 June 1998
------------------------------
1) Fixed error in PGM profile class SetAllOptions function.
2) Fixed crash that occurs when loading a GenCon model that contains an
   empty PGM string variable on the watch page.
3) Fixed crash that occurs when trying to use the general split model when
   no outputs have been configured; recalc GM page when links are changed.
4) Improve message when models.cfg file is not found.
5) Fixed crash in belt filter model.
5) Use previous directory when loading or saving DXF files.
6) Fixed crash in data transfer dialog for new projects.
7) Improved license failure messages when using license location option.
8) Extensive fixes and improvements to the flash train macro model,
   especially related to heat exchanger and ties.
9) Changes to find tag zoom/pan and highlighting the graphics.
10) Various fixes/improvements.

11) CNM more comments to add ????


... ??? ...



7.2 Rev 3.4 : 5 October 1998
------------------------------
1) Various minor fixes, enhancements and improvements.
2) Fixed speed problem, especially evident when loading projects, caused
   by invalid "locale".

7.2 Rev 3.5 : 6 November 1998
------------------------------
1) Various minor fixes, enhancements and improvements.
2) Enhancements to steam specie model.
3) Added option to set zero flows in all links/pipes.
4) Enhancements to PID model (display and checks conversions).
5) Stronger checking of engineering unit in access windows.

7.2 Rev 3.6 : 17 November 1998
------------------------------
1) Various minor fixes, enhancements and improvements. Including fixes to
   project save as; specie names with "()[]"; access window printing;
   project file structure options; project merges; various access window
   display and speed enhancements; etc.
2) Enhancements to communition / mill model.
3) Speed enhancement related to the specie model.
4) Fixed calculation for display of heat of reaction.
5) In feeder model when using MassFlow option, fixed entry of QmReqd or
   individual specie flows.
6) Implemented prototype for scaling/rotating entities in graphics.
7) Various minor model enhancements and fixes.

7.2 Rev 3.7 : 1 December 1998
------------------------------
1) Updated all conversion/engineering units to use SI units.
2) Implemented new functionality to handle Densities of Solutions.
3) Implemented prototype of Kaiser Precipitator model.
4) Implemented enhanced crusher model with updated partition curve.
5) Improved direct contact heater model.
6) Various clean-ups and enhancements related to the Specie database.
7) Implemented visibility of specie properties, including allowing
   access to the specie properties as tags.
8) Reorginised/modularized access to global data such as tear blocks,
   ProBal info, Dynamic info, PlantModel/solver parameters and specie
   properties. Results in renaming of some old global tags.
9) Various other bug fixes, enhancements and improvements.

7.2 Rev 3.8 : 7 December 1998
------------------------------
1) Specie database enhancements, including view of which specie database
   properties were loaded from, also includes checking for specific
   phases of species for models that require them.
2) Various model display improvements for the crusher and screen models,
   especially related to the size distribution curves.
3) Various other bug fixes, enhancements and improvements.

7.2 Rev 3.9 : 15 December 1998
------------------------------
1) Fixed some bugs in Size Distribution -
   Also retains qualities when flow goes to zero.
2) Improved tear convergence for qualities(SzDist Etc.)
3) Fixed Bugs in SpecieDB Loading/Ordering etc.
4) Fixed Bugs in TagDB relating to loading Old flowsheets where FieldNames
   with spaces have been changed.


=============================================================================
SysCAD 7.3
=============================================================================

Build 1 : 21 December 1998
------------------------------
1) "Re-badged" SysCAD and implemented new version numbering method,
   eg "SysCAD 7.3  (Build 107)".
2) Changed SysCAD icon, busy/progress dialog and about box.
3) Changed some license options for demo and trial versions.

Build 2 : 22 December 1998
------------------------------
1) Tweaked versioning to include more info in file.

Build 3 : 19 January 1999
------------------------------
1) Fix in crusher model, various other minor bug fixes.

Build 4 : 25 January 1999
------------------------------
1) Fixed graphics printing to ensure print is always clear.
2) Fixed bug in data transfer and trend tag view for string list tags.
3) Fixed initialization bug in cyclone model.

Build 5 : 28 January 1999
------------------------------
1) Fixed bug in PGM when using variables of classes nested in classes.
2) Various DDE improvements, including a fix that caused occasional crashs.
3) Implemented a dialog box with various DDE options and statistics.

Build 6 : 5 February 1999
------------------------------
1) Implemented multiple select cut and paste functionality.
2) Implemented multiple select and change properties functionality.
3) Implemented infrastructure for design modules.
4) Copy full tag now includes engineering units.
5) Separated Project menu into Project and Actions menus.
6) Major reorganisation of message window, including: speed of reporting
   messages, display method, layout changes, message filter options, etc

Build 5-QAL : 12 February 1999  (AND Build 5-QAL)
------------------------------
1) Improved Response in QuickView/ Solutions windows at run time
2) Fixed some bugs in Multiple Field Select
3) Enthalpy Strategy-Conserve Heat/Best_Temp
4) Improved way heat balances are checked
5) Extensions to some QAL Models
6) Improved Mixing selection of SpModels

Build 7 : 19 February 1999
------------------------------
1) Fixed some more multiple field cut and paste bugs.
2) Major improvements to size-distribution displays and options.
3) Fixed major bugs for pressure, temperature and flow problems in dynamic mode
   for pipes in transfer mode; and fixed related bugs in conveyor and tie.
4) Improved method for handling retaining and display of temperature and pressure
   in pipes after they go to zero flow.
5) Improved screen updates for trends and trend values.
6) Improved cyclone model to handle multiple cyclones or clusters.
7) Various other minor fixes and enhancements.

Build 8 : 5 March 1999
------------------------------
1) Improvements to screen model including: display options, shows calculated
   partition curve; improved fines/water split methods; applies to multiple-ore
   types; etc
2) Improvements to crusher and mill models including: display options, individual
   or overall discharge partition methods; etc
3) Improvements to size distribution display in pipes.
4) Bug fix to flash train and shell & tube heat exchanger introduced in build 6.

Build 9 : 7 April 1999
------------------------------
1) General improvements to size distribution displays and models.
2) Added compressed air pressure drop equation for pipes.
3) Added over-writable K Factor for Darcy pressure drop equation for pipes.
4) Settings for global conversion overide in access window is saved and recovered.

Build 10 : 19 April 1999
------------------------------
1) Fixed a convergence problem seen on large ProBal projects.
2) Implemented prototype of multi-phase flow design module.
3) Major changes to Mill1 and Crush1 model Partition Curve methods (data now entered
   as size fractions, not as cumulative fractions old models don't recover.
4) General improvements to size distribution displays and models.
5) Fixed solver multithreading bug encountered on dual-proccesor PC.
6) Various other minor bug fixes and enhancements.

Build 11 : 28 May 1999
------------------------------
1) Added new crusher method: breakage extents based on test size data.
2) Minor bug fixes and enhancements to size distribution, screen and crusher models.
3) Improvements to the cyclone model.
4) Implemented general purpose washer model.
5) Various other minor bug fixes and enhancements.

Build 12 : 3 June 1999
------------------------------
1) Added new Solvent Extraction Mixer-Settler model.
2) Minor enhancements to screen model and bug fixes to Karra Screen-Area method.

Build 13 : 11 June 1999
------------------------------
1) Fixed bug for data transfer and reports for specie fractions in feeders and
   size fractions for size-distribution.
2) Implemented functionality to enter a tag or function as SetPoint for PID
   controller.
3) Some enhancements to various models (Cyclone, crusher-selection/breakage
   and screen)
4) Implemented improved Kaiser precipitator model.
5) Changed/removed Excel report message for incomplete tags.
6) Various other minor bug fixes and enhancements.

=============================================================================
================                SysCAD 8.0                   ================
=============================================================================

Build 21 : 1 September 1999
------------------------------
1) First release of SysCAD 8.0
2) See Release notes.

Build 22 : 7 September 1999
------------------------------
1) Fix to right-click for pop-up menu on a field in the access/trend window
   and other minor fixes

Build 23 : 20 September 1999
------------------------------
1) Fix to graphics where scaling went crazy when an insert link failed.
2) Improved right click pop-up menus for trends.
3) Various other minor bug fixes and enhancements.

Build 24 : 7 October 1999
------------------------------
1) Various minor bug fixes and enhancements including memory leaks, improvements
   to PID model, Access window conversions/formats, size distribution enhancements, etc, etc...

Build 25 : 8 October 1999
------------------------------
1) Solubility convergance and load/save fixes.
2) Various other minor bug fixes and enhancements.

Build 26 : 25 October 1999
------------------------------
1) Fix for recovering flowsheets using controllers.
2) Implemented new Excel report lists.
3) Implemented prototype for database functionality.
4) Fixes for specie data import from HSC or MDB.
5) Convergence improvement for projects using solubility.
6) Various other minor bug fixes and enhancements including improvements to PID,
   create new configuration, data transfer for feeder, etc.

Build 27 : 23 November 1999
------------------------------
1) Moved tag database csv files into sub-directory $db.
2) Improvements to convergence for solubility.
3) General improvements to status reporting, especially in general model.
4) Fixed and improvements to feeder: improved ByPhase options, ConstP mode disabled
   in ProBal, QmRqd cannot be set depending on specie breakdown view, etc
5) Improved graphics colour status display, added status colour for FlashTrains.
6) Implemented quick zoom window button.
7) Fixes to report tags (quick view) also added display of condition in top line.
8) Implemented use of CrypKey 5.0 lib files in SysCAD.
9) Various other minor bug fixes and enhancements including strong testing of tag names,
   improvements to dialog box version of message window, changed graphics done toolbar
   button to a green tick, watches in GenCon "removed", improved multi-threaded ProBal
   solver, etc, etc...

Build 28 : 28 November 1999
------------------------------
1) Improvements to Alumina related models.

Build 29 : 18 February 2000
------------------------------
1) Improvements to database functionality.
2) Improvements to controls and auto-tuning.
3) Windows 95/98 fixes, especially related to project configuration.
4) Implemented controls page as special type of trend page for internal testing.
5) Implemented page that shows order of evaluation of models and referenced
   tags for internal testing.
6) Implemented first Visio graphics interface prototype for internal testing.
7) Implemented first prototype for SysCAD to act as an Automation Server for
   internal testing.
8) Fixed convergance problem for specie qualities, improves speed.
9) Added checks to VLE
10) Improved data validation for some tag ranges.
11) Fixes related to multi-phase flow solution.
12) Various other items including: Fix to DLL list with licensing, fix to solver
    preset image problem, some user-interface improvements (trends, graphics, etc),
    fixed bug in PID for setpoint tags, etc

Build 30 : 10 March 2000
------------------------------
1) First release of database functionality in commercial version.
2) First release of controls enhancements and auto-tuning in commercial version.
3) Implemented controls page as special type of trend page.
4) Implemented page that shows order of evaluation of models and referenced tags.
5) Improvements to Visio graphics functionality.
6) Improved/fixed new data validation introduced in Build29.
7) Fixed problem in FlowBlocks for using equation models, etc.
8) Fixed an old problem where GUI DC resources were created/used and not
   deleted/restored properly.
9) Various other items including: Change tag now works in array eg X_1.[P_1].Split;
   fixed bug in tearing algorithm that caused a freeze under certain conditions;
   fix to multi-phase flow convergence; updated IA driver; fixed bug in PID for
   changing priority of PID; made historian recovery more forgiving; etc

Build 31 : 13 March 2000
------------------------------
1) Fix in PID for setting SetPoint where error occurs under certain conditions
   where min range is non-zero.
2) Fix in QAL heat exchanger.

Build 32 : 16 March 2000
------------------------------
1) Fix to historian bug introduced in Build30.
2) Fix to feeder for Qv calc when density changes.
3) Minor QAL updates, and update for PID.

Build 33 : 28 April 2000
------------------------------
1) Driver manager improvements: Loads driver from database, can reload from
   options, extensions to double type slots.
2) Improved low level management of lists of controlers.
3) Bug fix in trends and restored DC resource properly.
4) Added tags to access reaction and reaction extent descriptions in reaction block.
5) Extended Excel reports functionality with SysCAD_TagListOffset(...) similar to
   SysCAD_SetTagListOffset(...) option; added Excel option in Options dialog to
   allow Excel to update links when loaded.
6) Various other items including: bug fix for heat exchanger if secondary is
   not connected; use of alternative graphics symbols on Win 95/98.

Build 34 : 7 June 2000
------------------------------
1) Fix for flash train when flash tank is switched off-line.
2) Improvement in heat exchanger in dynamic mode.
3) Added option to switch trend-line flashing feature on or off.
4) Improvement for reactions in dynamic mode.
5) Fixes to recovering flowsheet state after a project load, especially for dynamic.
6) Added new script command "SaveAndCloseReport(ExcelFileName)" which saves and
   closes specified Excel file.
7) Enhancement to concentration calcs of Na for low values in Bayer specie model.
   Also improved Cp calcs for Bayer specie model.
8) Extended command line otions to handle long file names in ("), added new options
   /n to start New copy and /l to specify license location.
9) Added more ProBal solver status/statistic variables to the $PB access window.
10) Various changes and improvements to reaction block. (Added breakdown of Duty
    values;)
11) Various other items including: fix for PID controller bug introduced in Build33;
    fixed memory/resources bug for trends on Win98; fixed bug and feedback message
    for incorrect syntax for SysCAD_TagList(...); added error message when reaction
    is used with inefficient mixing in washer;
Patches:
 1) PID fix (7 June)
 2) command line (7 June)
 3) command line (8 June)
 4) GM model (8 June)
 5) PID change (9 June)
 6) Project file view (9 June)
 7) Delete unused graphics blocks (9 June) [REMOVED]

Build 35 : 18 October 2000
------------------------------
 1) All patches above
 2) Checks for changes in PID output range (10 June)
 3) Fix for make-up mode/demand in ties & contact heater (10 June)
Patches:
a) 18 October (syscad.exe & control1.dll)
 - PID fix (13 Nov)
 - Project list reference files (13 Nov)
b) 19 October (all)
 - Excel Macro Script command (17 Nov)

Build 36 : 7 December 2000
------------------------------
 1) All patches above
 2) Delete unused graphics blocks. Drawings use less memory and load faster.
 3) Improved license type display; improved network license release after crash;
    use CrypKey5.5 library.
 4) Release extra processing time to system between iterations to try improve
    responsiveness.
 5) Fix for defer graphics load option dialog.
 6) Some minor display/enabling improvements in menus and dialogs.
 7) Warning message for project merge.
 8) Fixed colour status display for splitter, filter press and Mixer/Settler.
 9) Improved error checking of specie database Vp parameters.
Patches:
a) 11 December 2000
 - For specie database Solution table, do not load equation for empty fields
   for DensityCorrFn or SolubilityFn (11 Dec)
 - Fix for Solvent Extraction Mixer-Settler model when using density. Solute
   uses density of solvent in model logic (11 Dec)

Build 37 : 17 January 2001
------------------------------
 1) All patches above
 2) Workarround to prevent memory override for too many conversion families (8 Jan 2001).
 3) Added access to diameter and length parameters required in EHX "Loss to Ambient"
    method in ProBal (8 Jan 2001).
 4) Set default VapSpModel in flash tank to "Steam/Water". (17 Jan 2001)
 5) Fix in display for edit specie database for synchronising specie data with
    selected specie in list under all conditions. (17 Jan 2001)


=============================================================================
================                SysCAD 8.1                   ================
=============================================================================

Build 40? : ? December 2000
------------------------------
Changes implemented from 8 June 2000:
1) Low level look-up test for enum types (string-lists) to ensure closest match
   when wrong value is entered via Excel, driver or trend.
2) Stronger checking of reaction files especially for heat / energy balance options.
   Checks for valid Hf in specie database if Heats of formation are required in
   reaction. Fixed HeatGain calc for ambient loss in reaction block. Fixed
   Extent : FinalConc method (converge temparatures). Improved convergence tests.
3) Various improvements for reactions in dynamic mode, including reaction extent
   method.
4) Steam/water specie model always overrides H20 properties when used in
   mass-weighted mean and other specie models.
5) Added option to allow Cp correction function in a similar method to density
   overrides.
6) Changed reference temperature in mass-weighted mean model to Zero C so that it
   is the same as other specie models. This changes some display values on access
   window for links.
7) Improvements in use of HeatOfFormation(Hf) and Enthalpy functions.
8) Improvements and fixes to drivers, including driver reload & recover.
9) Significant model enhancements and fixes for dynamic mode. (Especially for
   heater and "splitter/make-up" model in tanks.)
10) Fixed project merge for Access database (MDB) type projects and control trends.
11) Significant improvements and changes in graphics for construct unit and
    construct link. Changes to inserting link (dialog and arrow scale). Other
    improvements for inserting unit, graphics symbol scales, etc.
12) Large improvement in memory useage and management, especially for large projects.
13) Project, specie and DLL dependent conversions are deleted when a project is
    closed. Error message is given when their are too many conversion families.
14) Added vent stream to most separation units.
15) Significantly improved behaviour of GM model when streams are connected/disconnected.
16) Various improvements in dynamic mode. Improved flashing; PolyTropic flow; etc
17) Improvements for using correct pressures, especially for reactions with
    water/steam for ProBal and Dynamic.
18) Do not allow solver to start if error in function in PID; improve error state
    reporting for PID and GenCon.
19) Added access to diameter and length parameters required in EHX "Loss to Ambient"
    method in ProBal.
20) Changes to project filing: Improved project view file lists; option to load
    "local copy" project; cannot load project with files from mixed backup/original
    locations; global option to use symbolic or full paths; improved display of pgm
    and rct name in access window; etc, etc ...
21) Change in graphics to delete unused blocks resulting in imported drawings loading
    significantly faster and using less memory.
22) Improvements for steam injector and make up mode in cross page connector, tie
    barametric condensor and contact heater. Fix for tie switched in to and out of
    a flash train.
23) Fix for completing last command in script without dialog prompts; Extended
    command line options with /c option to execute a single specified script command.
24) Added new script command "ExecuteMacro" to run a macro in Excel from SysCAD.
25) Display aditional information on "stand-alone" or "multi-user" license. Added
    new menu option to reset the license service. Improve network license release
    for crashes. Now linked to Crypkey 5.5 library.
26) For specie database Solution table, do not load equation for empty fields
    for DensityCorrFn, SolubilityFn or HDilutionFn.
27) Fix for Solvent Extraction Mixer-Settler model when using density. Solute
    uses density of solvent in model logic.
28) Improvement for historian recovery for bad data event blocks and/or catalog.
29) Set default VapSpModel in flash tank to "Steam/Water". (17 Jan 2001)
30) Various other items including: test for DAO 3.6 / Access 2000 if present;
    thread fix for a Windows2000 bug; general improvements for dynamic mode;
    improvement for "trend flashing" option; added a range of statistics fields
    related to iterations and speed; added new status colour for inactive units;
    improved Excel trend reporting; added new "regular expression" search feature
    for finding tags in graphics; checks and warns user when attempting to load
    project with read-only attribute set;



Changes from Build 34 to Build 40 include:
-------------------------------------------------------------
A) Changes in Build 35,36 and Build40:
1) PID fixes: Crash under certain initial conditions and ranges; fix to prevent step change in output when changing range while solving.
2) Significantly improved behavior of GM model when streams are connected/disconnected.
3) Change in graphics to delete unused blocks resulting in imported drawings loading significantly faster and using less memory.
4) Added access to diameter and length parameters required in EHX "Loss to Ambient" method in ProBal.
5) Improvements for steam injector and make up mode in tie and contact heater.
6) Added new script command "ExecuteMacro" to run a macro in Excel from SysCAD.
7) Display additional information on "stand-alone" or "multi-user" license. Improve network license release for crashes. Now linked to Crypkey 5.5 library.
8) For specie database Solution table, prevent load of default(incorrect) equation for empty fields for DensityCorrFn, SolubilityFn.
9) Fix for Solvent Extraction Mixer-Settler model when using density. Solute uses density of solvent in model logic.

B) Other fixes and improvements in Build 40 only (affecting ProBal):
1) Low level look-up test for enum types (string-lists) to ensure closest match when wrong value is entered via Excel, driver or trend.
2) Reactions: Stronger checking of reaction files especially for heat / energy balance options.  Checks for valid Hf in specie database if Heats of formation are required in reaction. Fixed HeatGain calc for ambient loss in reaction block. Fixed Extent : FinalConc method (converge temperatures). Improved convergence tests.
3) Steam/water specie model ALWAYS overrides H20 properties when used in mass-weighted mean and other specie models.
4) Changed reference temperature in mass-weighted mean model to Zero C so that it is the same as other specie models. This changes some display values on access window for links, etc.
5) Improvements in use of HeatOfFormation(Hf) and Enthalpy functions.
6) Improvements for using correct pressures, especially for reactions with water/steam for ProBal and Dynamic.
7) Fixed project merge for Access database (MDB) type projects and control trends.
8) Significant improvements and changes in graphics for construct unit and construct link. Changes to inserting link (dialog and arrow scale). Other improvements for inserting unit, graphics symbol scales.
9) Large improvement in memory usage and management, especially for large projects.
10) Project, specie and DLL dependent conversions are deleted when a project is closed. Error message is given when their are too many conversion families.
11) Added optional vent stream to most separation units.
12) Do not allow solver to start if error in function in PID; improve error state reporting for PID and GenCon.
13) Changes to project filing (Improvements and simplifications): Improved project view file lists; option to load "local copy" project; cannot load project with files from mixed local-copy/original locations; global option to use symbolic or full paths (default is full-paths); improved display of pgm and rct name in access window.
14) Changed name of root $cfgfiles to $basecfg and root $syscad.mdb to $default.mdb to prevent confusion between project configuration and common distributed data files.
15) Improvements for steam injector and make up mode in cross page connector, tie barametric condenser and contact heater. Fix for tie switched in to and out of a flash train.
16) Fix for completing last command in script without dialog prompts; Extended command line options with /c option to execute a single specified script command.
17) Added new menu option to reset the license service when project is not loaded.
18) For specie database Solution table, improved reporting of errors and status for loading equations for DensityCorrFn or SolubilityFn.
19) Improvement for historian recovery for bad data event blocks and/or catalog.
20) Various other items including: test for DAO 3.6 / Access 2000 if present; thread fix for a Windows2000 shutdown; improvement for "trend flashing" option; added a range of statistics fields related to iterations
and speed; added new status colour for inactive units; added new "regular expression" search feature for finding tags in graphics; checks and warns user when attempting to load project with read-only file attributes;

C) Dynamic and other:
1) Various improvements for reactions in dynamic mode, including reaction extent method.
2) Added prototype of option to allow Cp correction function in a similar method to density correction overrides.
3) Improvements and fixes to drivers, including driver reload & recover.
4) Significant model enhancements and fixes for dynamic mode. (Especially for heater and "splitter/make-up" model in tanks.)
5) Various improvements in dynamic mode. Improved flashing; PolyTropic flow;
6) Various other items including: general improvements for dynamic mode; improved Excel trend reporting;

Update 1 : 21 December 2000
---------------------------
1) Fix for fixed heat of reaction
2) Fix for maximum number of conversion families
3) Change for saturated feeder

Update 2 : 22 December 2000
---------------------------
1) Removed WinNT only SetAffinity function... (8 Jan)

Update 3 : 18 January 2001
---------------------------
1) Patch/update number and date in about box.
2) Fix for a Build40 makeup mode bug: after a change, it was sometimes using "old" data.
3) Don't start solver for incorrect engineering unit in cotroller.
4) Changed $DB to Database, $.spj to project.spj, $CfgFiles to CfgFiles and $SysCAD.mdb to SysCAD.mdb.
5) Fix for project Save As when overwriting old project, ensures rct, etc files are copied.
6) Edit pgm/rct/etc in notepad now works for file names containing spaces.
7) Display full path for historian in project configure dialog.
8) Use system temporary directory for temp files instead of project folder.
9) Fix in display for edit specie database for synchronising specie data with
   selected specie in list under all conditions.
10) General improvements in project file view, including display of files not found.
11) Starting SysCAD from command line with a project and a snapshot specified,
    will load project and snapshot.
12) Improvement in contact heater operation for steam vent.

Update 4 : 8 February 2001
---------------------------
1) Implemented latest English version of code for steam/water specie model.
   Has some improvements for range checking. See http://chemengineer.miningco.com
   and http://chemengineer.miningco.com/library/weekly/aa081798.htm
2) Fixed a bug for GM model in a tank where order is changed when a new feed
   to the tank is inserted.
3) Improved graphics command line to handle more commands per line.
4) Improved tag annotation dialog: to keep focus on active window; hide unused
   bottom half of dialog when tag positions are being set; allow de-selection
   of units when showing or hiding tags.
5) Change to only allow a link to be selected for redraw link.
6) Fix for contact heater in demand mode or flash train mode to ensure correct
   product temperature is achieved when EHX or RB is used.
7) Added display of feed and product temperatures for EHX as per RB.
8) Added option to allow user to cancel a project load for older projects
   that need to be auto-upgraded to Build40 or later.
9) Fix for HeatExchange:Power method in reaction block.
10) Other minor fixes/improvements.

Update 5 : 23 February 2001
---------------------------
1) If a working folder is not specified, SysCAD now uses \SysCAD8\ instead of
   \SysCAD8\bin\.
2) Fix to calculate the correct liquid densities and related volumetric flow
   when using density correction factors for solutes in solution.
3) Change for contact heater used in a flash train to not put excess steam in
   liquor stream.
4) Changed Build Date to 30 January 2001

Update 6 : 9 March 2001
---------------------------
1) Significant changes for managing pressures in ProBal for most units. Added
   option to effectively set the output pressure in tanks and ties using a
   PBMethod field.
2) Added RefTags page to project view to list all full tags potentially set
   or retrieved by PGM and/or PID.
3) Significantly improved efficiency of DDE poke command. (Used whith SysCAD DDE
   RTDB topic to set tags.) Fixed a crash in DDE for a change tag.
4) Added new command line option /fN (or /fn) to stop SysCAD from being forced
   to being the topmost active window.
5) Added Macro page to Excel Reports dialog to allow Excel Macros to be called
   from SysCAD.
6) Added new reaction warning that is displayed when the "intermediate" reaction
   temperature is low (<50K). Fixed calculation of energy error, its display and
   corresponding error-check for these very low temperature reactions.
7) Changed display of RB HsGain,etc to show values at feed AND product temperatures.
8) Added new field OverrideActive which is set to Yes if any override in a pipe
   or container is active. Fixed the tag comment (Ovr) for fields where override
   is present.
9) When using density corrections with species in solution, the density of the
   solution is shown in access window for pipes (eg CorrRho_H2O(l))
10) Fix to force projects to always use the specie database (syscad.mdb) that is
   in the folder cfgfiles one level above the location of the selected .cfg file.
   This fixes problem where "open project alt cfg" used wrong database.
11) Access window now remembers the cursor and scroll position when the user
    presses the 'OK' or '*' (refresh) buttons.
12) Fix for value displayed in the ExtentDiff field for reactions.
13) Other minor fixes/improvements.
14) Changed Build Date to 31 January 2001


=============================================================================
SysCAD 8.1
=============================================================================

Build 41 : 29 March 2001
------------------------------
1) First release of SysCAD 8.1
2) To distinguish projects/licensing/etc from SysCAD 8.0, use INI file
   syscad81.ini instead of syscad8.ini.
3) Excel calculations switched off while generating a report.
4) Remove disabled menus/toolbar buttons when using a run-time license.
5) Added new command line options "/demo" to force demo mode; /"rt" to force
   run-time license.
6) Fixed display of HsGain and HfGain for source/sink species in a reaction block.
7) Licensing: Improved messages and disable some license menu options when licensing
   service initialise fails forcing SysCAD into demo mode.  Changed method for
   disabling specie models for licensing.
8) Removed old screen-saver/screen-cycle functionality.
9) Now uses DAO Access2000 database driver by default, if this fails use /97
   command line option. (ie no longer need to use /2000 command line option).
10) Other minor fixes/improvements.

Update 1 : 19 April 2001
------------------------------
1) Custom layout of toolbars were not restored, fixed.
2) Fixed some minor memory leaks.
3) Double click on feeder with cross-page connector, moves graphics to that unit.
4) Option to edit global SysCAD INI settings before or after a project is loaded.
   Includes option to select DAO Access driver to be loaded at SysCAD start-up,
   and option to not load Evaluation Order Window.
5) A "resource" fix on Windows 98 for loading projects with hundreds of errors.
6) Other minor fixes/improvements.


Build 42 : 21 June 2001
------------------------------
1) Improved responsiveness for "unresponsive/frozen" projects where mouse or user
   input was not processed while solving.
2) Fixed display of Normalised volumetric flows for gases.
3) Improved values returned by steam & water heat capacity and enthalpy calculations
   outside normal P and T range limits.
4) Added field HSGain@Ref for each reaction in a reaction block; added fields
   for RB.dH@Ref.Nett and RB.dH@Ref.Rct to display values at reaction reference
   temperature.
5) Compiled with Visual Studio Service Pack 5 installed (used SP3 previously).
6) Removed ProBal VLE option from tank as this is for Dynamic, for ProBal use VLE
   in pipe or flash tank instead.
7) Fixed error for contact heater in flash train where the required temperature
   cannot be achieved.
8) Other minor fixes/improvements (including: removed incorrect IO error message
   in mixer settler model; PGM Beep function; fixed ocasional crash when changing
   solver from ProBal to Dynamic; separate display of licensing limits for units
   in ProBal & Dynamic; when loading specie database decimals no longer need
   leading zero (eg ".8" is now valid); fixed error message when removing all
   functions (eg "=gettag("xxx..")+gettag...") from a trend page;)


Build 43 : 31 July 2001
------------------------------
1) Fixed energy balance error for ineffecient mixing in washer model.
2) Implemented Crypkey 5.6, which includes new feature to display users of
   floating network licenses.
3) Other minor fixes/improvements (improved database error display when loading
   default.mdb)

Update 1 : 14 August 2001
------------------------------
1) Fixed logic for "transfering" (inheriting) the selected specie model through a tear.
   stream which did not always work.
2) Removed option for opening projects using "local data".

Update 2 : 11 October 2001
------------------------------
1) Fixed delete historian option.
2) Fixed Precip model that gets "locked" when "A" in the precip outlet does not converge.
3) Added option to select Access 97 or Access 2000 database format during project save as.
4) Only report "requirements not met" error in GM (General_Model) if their is flow into
   the unit.


Build 44 : 13 December 2001
------------------------------
1) Fix for transfer of pressure values in a tear for certain conditions.
2) Added option to selectively set feeder parameters by "fetching" data from a specified
   unit or pipe.
3) Added PreReact option in tank with heat exchange so that user can select if heat exchange
   functionality occurs before or after reactions.
4) Added PBPress.Method option in flash tank similar to tanks and ties. This improves
   options/flexibility when using flash tank in flash train or as "stand alone" unit.
   It also eliminates the need for "work arounds" in some flash train arrangements.
5) Added some general information tags in the Access window in PlantModel Statistics tab page.
6) Added option in washer model to apply bypass (mixing efficiency) to liquids only or to
   combined liquids-solids.
7) In the washer for ineffecient mixing, with solids in wash stream or certain reactions, the
   required U/F or O/F solids fractions were not always achieved. This has been fixed.

=============================================================================
================                SysCAD 8.2                   ================
=============================================================================

Changes in various SysCAD 8.2 Beta versions Build 50 to Build 69 were made
between April 2001 and May 2002. First official release of 8.2 is Build 70.
Beta versions were released selectively. Below is a list of many of the changes/
improvements/fixes/new-features:

General changes and changes in ProBal Mode
------------------------------------------
1.	In ProBal, significant improvements in convergence (eg Tear Blocks) management:
    Improved visibility of errors/damping/tolerances; new access window for viewing/
    setting Tolerances for internal solvers (eg VLE, etc); new count of converged
    iterations; new option to set number of consecutive iterations solver must continue
    "solving" for and remain solved after all tear variables/etc have met convergence
    tolerance requirements, this is done in ProBal Setup - convergence tab page.
2.	All specie data for a project is loaded from syscad.mdb located in the project
    cfgfiles folder. The option to allow loading of specie data from the default.mdb
    has been removed.
3.	The specie models previously referred to as "mass-weighted-mean" and Steam/water
    have been renamed as the "Standard" Specie Model.
4.	Improvements for water and steam properties models. Implemented new method based
    on industrial standards IAPWS-IF97. The H2O model is selected and applied globally.
    The old "Steam/Water Specie model" has been removed. Added an option to use
    "function surfaces" which provides a significant speed improvement for retrieving
    H2O properties from a lookup after they have been calculated once.  The water
    properties are selected from the Plant Model page.
5.	New fields in Plant Model Access window for information. A new option to set the
    state of all units in a project.
6.	Rework solver states to have one set of toolbar buttons regardless of Dynamic/
    ProBal mode. Solver has three states (a) Edit (Stopped); (b) Idle (paused); and
    (c) Run (solving/running). Now need to use project Configure to change from ProBal
    to Dynamic mode. New toolbar button to edit solver options.
7.	Updated Project - Configure dialog box to reflect changes made and new options
    available (solver modes, driver, OPC, historian, etc).
8.	Updated ProBal Setup and Dynamic Setup dialog boxes to reflect recent changes.
9.	Improvements in dialog box for editing Models Configuration. Includes improvements
    for editing size distribution data, and new option for specifying low & high
    temperature ranges for checking specie data.
10.	Numerous improvements in Access Window: Improved (and added new) options and
    buttons at top of all Access window tab pages; improved printing (especially
    check-boxes); new option to "send" the selected tag to the next available space
    in the trend window most recently selected; improved right-click pop-up menus;
    support of mouse wheel to change value of currently selected field up or down;
    new buttons to right of fields in access window which contain tags that refer
    to other units/pipes to allow quick access to these units data and/or graphics;
    restore cursor to correct field after pressing OK/Refresh/etc.
11.	Improved display as seen in Access window for most unit models (consistency,
    defaults, hidden/shown, units, etc). The most significant changes/improvements
    in the separation models (filters, etc).
12.	In general options, updated Graphics Symbols Displaying Colours options, where
    line style as well as colour can now be selected.
13.	In general options, a new tab page for setting Mouse Click on Graphics Symbols.
    The behaviour of left click, right click, double-click, etc can be modified from
    the defaults.
14.	Significant improvements in quick-view to allow permanent quick view windows,
    multiple quick view windows, and multiple columns for different tags in a quick
    view. Also an option to "trend" selected values as bar graphs in a quick view.
    Explained under the Mouse Click on Graphics Symbols section.
15.	For Save Snapshot and scenarios a new zip option is available, where the snp or
    scn files are automatically zipped and unzipped significantly reducing disk usage.
16.	Improvements to Message window to allow copy to clipboard of messages via right
    click pop up menu.
17.	As part of a project save, all "external" referenced files are "backed up" into
    a zip file called RmtFilesBackup.zip. The previous "composite" backup file is no
    longer used.
18.	New option to Copy Bitmap onto clipboard in black & white format suitable for pasting
    and printing in other documents.
19.	New option to set the widths of Trend Window Tag List Columns using mouse left-click
    and drag of splitter between columns. New buttons in trend view to refresh trend
    data, etc. Improvements to scale, fit, range, etc trend lines from right-click pop
    up menus.
20.	Significant improvements in "Change Stats" which can be used to display the tags
    that have the largest errors, most changes and worst oscillations. These largest
    changes and oscillations are also shown in each model and pipe if the "collect
    stats" is switched on.
21.	Improved ProBal Solver progress dialog box, including new right click pup-up menu
    to access problem tags.
22.	New Select Window functionality where a small window, which is always visible,
    has a list of all available windows that can be used to navigate window lists.
    This new style or the existing select window method can be used.
23.	New features in the PGM used by the general controller model. Added new functions
    ConditionNote(index, logic expression, message) and ConditionError(...) which can
    be used to display messages in the condition portion of the message window. Added
    functionality to specify engineering units for variables (class data) declared in
    a class definition. Improved functionality so that dynamic tags do not need to be
    defined. They are automatically added at run time when using SetDynTag(...) and
    GetDynTag(...).
24.	Significant improvements to PID_Cont model, including display of engineering units
    for meas/output/etc tags. Improved error reporting and save-load recovery.
25.	New heat-exchange methods available in reactions.  See Models.hlp - sub models -
    RB reactions.
26.	The historian is been separated into a separate DLL. It now supports strings; has
    a new file naming method where historian data files end in .dat; and has a new
    global "compression" option to reduce data points stored to disk based on a tolerance.
27.	The option to allow "old style" text file (csv) format for saving model data has
    been removed.  All model data is now stored in scd_mdls.mdb.
28.	Discontinued option to load and work with "local copy" projects.
29.	Added new display fields to the Bayer Specie Model, reflected in updated Access window.
30.	Significant improvements to contact heater, including new fields and requirement
    options. Improved solve time and error reporting.
31.	Improvements to flash tank including new option to specify required saturation
    temperature.
32.	Improvements to shell&tube heater related to solve time, display fields, options
    and error reporting.
33.	Various minor improvements and new fields and options in various models such as
    pipes, tanks, feeder, general controller, etc.
34.	Significant improvements, new display fields, new options, etc to separation units
    such as belt filter, counter current washer, centrifugal filter, drum filter,
    electrowinning cell, filter press, solvent extraction mixer settler, splitter,
    thickener, washer and dryer.
35.	Solver improvements and management of mass-based / volume-based flow and pressures.
36.	Solver improvements for VLE / flashing.
37.	Implemented new classifier model.
38.	First (Beta) version of a SysCAD COM interface (scdcom.dll). This allows programmable
    access to SysCAD from other applications such as those that include visual basic
    programming features. A wide variety of data, events and functionality in SysCAD
    has been made available. Included in this are the commands that are currently
    included in the scripts functionality.
39.	Changes to handle high-resolution displays with large fonts.
40.	Improved testing for files used by a project that are marked as a read-only.
41.	The old (unused) network functionality where a second copy of SysCAD running on
    network could connect to the first to view tag and trend information, has been
    removed. Thus the DLL files sc_clsv and sc_rpc are no longer required.
42.	The (old) unused DDE commands to request trend and tag data reports has been removed.
    The DDE tag subscription functionality is unchanged.
43.	The spec-sheet functionality, which was used as a method for configuring Access
    window displays, has been removed.
44.	Improved use of version numbers as part of project names (eg "Test Project-01.spf").
45.	Improved operation and management of "control actions". Reduces the lag of
    application of control actions by one iteration for some applications.
46.	General improvements for various SetState functions such as Empty, ZeroFlow, etc.
47.	Improvements to efficiency of Brent solver used internally in many models, where
    an estimate from the previous solution is used.
48.	Other: minor improvements in multi-thread use and management; for management of
    internal flags for all tags changed from 32 to 64 bits; improvements in loading and
    checking specie data from database; minor improvements for loading & saving in MS
    Access database; display full paths of cfg file and project during project load;
    minor licensing management improvements; some new toolbar buttons; improved project
    version numbering as part of project name; improved parsing of command line options;
    minor improvements for Excel reports; improved management of active/in-active units;
    some mulit-thread/multi-processor improvements and many other fixes/improvements/changes.

Changes in Dynamic Mode:
------------------------
1.	Implemented new OPC Server add-on. All SysCAD tags are available to OPC clients. A
    SysCAD project must be loaded with the OPC Server option enabled.
2.	Implemented new OPC Client functionality in SysCAD. SysCAD has an OPC Client driver
    that allows SysCAD to connect to other OPC Servers. This driver is implemented and
    used in a similar way to previous SysCAD drivers.
3.	New IO Points system object.
4.	Improvements to "driver manager" with improved options for viewing, loading and
    managing driver tag lists. New options for the "connection" field for driver tags,
    and other new features and improvements.
5.	Improvements to operation of the contents of a model unit. This includes additional
    display fields, improved operation for closed contents/tank, "extended base volume
    contents" for an existing tank, and other general improvements required for new
    dynamic functionality.
6.	Implemented new "Archive" functionality, where for specified tags, averages/minimum/
    maximum/etc are stored to an archive file in an Access database. Tag list managed in
    a similar way to driver tags.
7.	Improved project load sequence to properly load flowsheet objects then driver tags,
    and then "other" data, before resolving state of all driver tags and flowsheet values.
8.	Improvements in presetting contents and minimum/empty conditions of various units.
9.	Significant changes and improvements for entry and management of flanges or fittings
    between pipes and units. New configuration options for configuring pipe connections
    available in the unit (eg tank). Includes selection of defaults and management of
    overrides for K values based on diameter of pipes at connection points. A new connection
    aperture angle is available.
10.	Significant changes and improvements for pumps and their related DP and control
    equations and options. Includes new "simple curve" pump option.
11.	Improvements for valves and their related DP and control equations and options.
12.	Improved orifice plate and reducer equations.
13.	Significantly changed and improved the dynamic functionality of the heat exchanger.
14.	Improved behaviour in pipes including the pipe contents, DP equations, detection of
    gas / slurry flows.
15.	Improvements in dynamic modelling functionality for barometric condenser, belt conveyor,
16.	New "statistics" functionality in pipes and tanks, useful for analysis of min/max/ave/etc
    in a dynamic run of a set duration.
17.	Significant changes and improvements to dynamic solver capability and speed in closed
    tank/gas/high pressure flowsheets.
18.	New functionality for reactions in dynamic mode, including Free Energy Minimisation.
19.	Implemented a new ejector model.
20.	Implemented new profile controller model, used to set a tag (eg feed) based on a
    profile trend.
21.	Implemented new noise controller model, used to apply random noise to a specified
    tag (eg feed).
22.	Implemented new schedule task model that can be used to generate various report types.
23.	Implemented a new general statistics model that can be used to store min/max/ave/etc
    values for a specified tag.
24.	Implemented a new stack model.
25.	Other: general dynamic solver improvements; other minor improvements related to dynamic
    options in various models; new thermal options for structure/contents of tanks and other
    units; plug flow; other general dynamic mode fixes/improvements.


Build 70 : 24 April 2002
------------------------------
1) First non-beta release of SysCAD 8.2

Update 1 : 23 May 2002
------------------------------
1) Improved management of water/steam properties lookup surfaces/triangles,
   especially along the saturation line.
2) New options in flash-tank to manage split of "other" vapours.
3) Fix for behaviour of CC washer with "invalid" liquid/solid feeds.
4) Improved options/management of SysCAD OPC Server tags.
5) Improved options/management of SysCAD DDE Server tags, now in separate DLL
   ScDdeSrvr which is only loaded if required. Use "DDE On" option in project
   configuration.
6) In dynamic, fixed calculation and display of velocity in Orifice.
7) In dynamic, fixed a test and solve for static-head/flow conditions in certain
   pipes.
8) In dynamic, fixed QmNett error in tanks with certain conditions.
9) In dynamic, improved numerical accuracy of various functions such as flashing
   which use iteration dT for derivatives, etc.
10) Trend line accuracy improvement with numerical rounding.
11) Changed dynamic tags for dTime, etc to TimeInc, etc
12) Fix for a crash in reactions using free energy minimisation.

Update 2 : 30 May 2002
------------------------------
1) Additional pipe & flange pressure displays in dynamic.
2) Change in historian to remove "steps" in recovered trendlines when using
   recording tolerance to save disk space.
3) Various improvements for OPC Server including options and statistics display.


Build 71 : 20 June 2002
------------------------------
1) Some project load recovery improvements for older projects. Includes
   improved snapshot recoveries.
2) Implemented correct behaviour for empty and zero-flow action for overflow/vent
   connections in tanks.
3) In dynamic, various improvements for network solver related to vapour volume
   fractions in pipe flows. Added options for displaying QmEst and ExcessVelocity
   error messages.
4) Implemented functionality for minimum level in tanks.
5) New trend option to auto-scale.
6) New volumetric flow split method in general split model (ie GM/tie).
7) Improved PB Press method behaviour in tanks/tie/etc, especially for ignoring
   pressure values for input pipes with zero flow.
8) New variable volume container/shape as a function of pressure.
9) Rearranged time options in project configure and dynamic solver dialog boxes.
   Added new options to restart historian and/or time in ProBal/Dyn options.
10) Improvements to QAL precip models.
11) Various other minor improvements. Fix for zero flow outputs from screens/
    cyclones when no size data in feed. Fix for large quickviews and setting maximum
    field sizes. Fixed a crash in HeatExchanger caused by deleting vent connections.

Update 1 : 28 June 2002
------------------------------
1) New options in dynamic for pipes with contents for input/output mixing strategy.
2) In Bayer properties specie model, added display for TOC in addition to TOOC.
3) In Bayer properties specie model, fixed ASaturation calc (max error less than 1%).
   Error was for values of TOC, NaCl, Na2SO4 conc used in calc of Ionic Strength.
4) Enhancements to QAL Bayer properties specie model and precip models.
5) Implemented Cyclical ProBal Solver functionality. Uses CyclicStorage in tanks.
6) Various other minor improvements. New warning message when pressure required is
   out of specified project range.


Build 72 : 29 July 2002
------------------------------
1) Some changes to allow compile in MSVC 6.0 and/or Visual Studio .NET.
2) Added display of Entropy values.
3) Changed some engineering units/conversion names primarily for Enthalpy. Added
   new conversion family types for entropy. Hence, requires updated defaults.mdb
   for new conversion families. { Enth -> H(Ms); EnthMl -> H(Ml); EnthV -> H(V);
   EnthNV -> H(NV); Cp -> Cp(Ms); CpMl -> Cp(Ml); HCnv -> HCond; and new cnvs for
   Entropy: S(Ms); S(Ml). }
4) New trapping of critical errors in solver worker threads.
5) Improved options in pumps and valves.
6) Fix for driver filter option.
7) New option to set historian recording of all tags of a type from access window.
8) Prototype for new import and build project from 3D layout data.
9) Other minor improvements.

Update 1 : 2 August 2002
------------------------------
1) New option in pipes for dynamic to set as forward only flow (ie non-return).
2) Added iteration number as part of logged messages display.
3) Some improved error message displays for dynamic solver.

Update 2 : 12 August 2002
------------------------------
1) Significant solver speed improvement in VLE (flashing).
2) Re-introduced SysCAD "screen saver functionality".
3) Improved import and build model from ELMS 3D model files.
4) Fix for shell&tube when "switched off" but still part of flash train.
5) Strict testing for unique tags of specie "Name" as well as "Compound".

Update 3 : 4 September 2002
------------------------------
1) Added new time options for generating trend reports. Options in Excel are
   Time(s) for seconds, Time(m) for minutes, Time(h) for hours and Time(d)
   for days.  These can be used as well as the option Time(f) or Time(Full)
   to display the full date&time. Options are not case sensative and may
   contain the full words, eg "Time(days)"
2) Added new Excel reports option that can be used to prevent save of Excel
   workbook after a report has been generated.
3) In Bayer properties model, the solids concentration displayed incorrect
   value if vapours were present.
4) Fix for crash when inserting reducer model.
5) Changed outlet liquor pressure on contact heater to be same as liquor inlet
   pressure when there is no steam addition.
6) Improvements to client models: (a) Implemented correct DefineLiquor
   properties in feeder for Alcan Bayer model; (b) First version of draft Alcoa
   Bayer Properties model.

Update 4 : 11 October 2002
------------------------------
1) Improved error message display and SysCAD exit for a critical error in solver.
2) For the Saturation Pressure calculation (which includes BPE effect) in the
   Bayer Properties model, improved calculation accuracy and fixed error where
   if BPE>20 then a BPE of zero was used when calculating SaturationP. Now uses
   a BPE of 20 when calculating SaturationP.
3) Include project pressure range warnings for all unit operations.  The project
   pressure range is specified in the configuration file.
4) Fix a crash caused by incorrectly defined size-distribution sieve series.
5) Improvements to client models: (a) Improved QAL Precip PSD model.


Build 73 : 30 October 2002
------------------------------
1) Implemented a new Steam Turbine model (ProBal in heatxch1.dll).
2) Implemented a new Falling Film Evaporator model (ProBal in heatxch1.dll).
3) Implemented new Simple Heater model for single input, single ouput similar
   to EHX in a pipe. (ProBal in heatxch1.dll)
4) Implemented new ProBal functionality and access window for a valve.
5) Implemented new ProBal functionality and access window for a pump.
6) Implemented new ProBal functionality and access window for a reducer. In
   ProBal it is identical to a valve.
7) Additional display fields in Contact Heater. Fixed to use correct pressure
   which includes BPE when this is present.
8) Changed ASat (Aeq) to use concentration parameters at 25dC instead of at T.
9) When using VLE (Vapour Liquid Equilibrium) in a feeder, for the option
   "SuperHeat_Vap", SysCAD now forces feed to vapour. Similarily, when using
   the option "SubCooled_Liq", SysCAD now forces feed to liquid.
10) Improvements in size-distribution functionality. Improved use of multiple
    species with size data, removed unused display column types for size data.
11) For Bayer properties, included new options when using the define liquor
    calculator to define organics/TOC.
12) Fix in Shell&Tube heater where if in condensing mode and liquid was present
    in feed, the Shell&Tube produced a mass error by discarding all vapours.
13) New option when drawing links, to draw a default link/pipe line based on
    selection method. User may override by continuing to draw links manually
    in usual manner.
14) Other various minor improvements.
15) Improved client models: (a) Alcan ASat calculation; (b) QAL Precip model,
    extra QAL bayer precip values, and Alpart ASat calculation.



Build 74 : 19 December 2002
------------------------------
1) For each unit in flowsheet network, control actions now ordered separately
   to the flow order. (ie separate networks for order of evaluation).  New
   option to chose to apply control actions at start or end of an iteration.
   New window to display order of evaluation of flows or controls.
2) New option to allow PGM (general controller) to apply at startup (before
   first iteration) and to apply at end (after last iteration). Use the
   new special flags "OnInitialise" and "OnTerminate" in the PGM code. The
   OnInitialise is especially useful for setting tags before the solve starts
   (ie change tags that cannot be set while solving). SetDynTag must be used.
3) New option in PID to enable tracking when controller is changed between
   manual and auto. (ie make setpoint=measured when in manual)
4) Implemented a new Desuperheater model (ProBal in heatxch1.dll).
5) Implemented a new Boiler model (ProBal in heatxch1.dll).
6) Improvements to new simple heater model, including heater sizing calculations.
7) Improvements in the pump model (ProBal) to ensure it has correct isentropic
   behaviour. Added display field for work. A pump can be used as a compressor
   for vapour streams. The pump is a constant entropy unit, while the valve is
   a constant enthalpy unit, both have a pressure change.
8) In dynamic, new actuator and transmitter models.
9) Fix in dynamic for overflows (density/composition error).
10) Reaction convergence fix and improvements for FEM reactions.
11) When using define liquor calculator in a feeder using Bayer properties
    model, SysCAD automatically changes the feed view option to Mass Fraction.
12) Added underflow solids recovery display field to classifier and thickener.
13) Fixed a crash loading graphics drawing under certain conditions.
14) In access window new right click pupup menu option to "copy tag" which
    only copies the portion after the unit tag (eg "Qi.Vf"). This is useful
    when copying tags to build Excel reports.
15) In dynamic implemented a new regulator function block. This is applied at
    pumps and valves and prevents low or negative pressures ie flow is "self
    regulating". This provides a significant level of stability for network
    under low flow, low pressure and/or reverse flow conditions.
16) Changes to SysCAD OPC Client to allow operation on separate thread or on
    the main thread.
17) Significant changes to size-distribution functionality: Re-arranged
    configuration dialog to enter sieve series data separately to size
    distribution definitions. SysCAD now supports multiple distributions in
    the same project. Implemented a new size distribution change model to
    allow conversion from one distribution to another using a different sieve
    series. Changed the behaviour for defining a distribution. Once the size
    distribution quality is switched on (create), then one distribution
    definition is automatically used. Only one can be used at a time in any
    model or pipe. Changed the taging for size data to use "Sz" rather than
    the distribution name. Various other size distribution related improvements.
18) Upgraded to use CrypKey 5.7
19) Various other minor improvements.

Update 1 : 16 January 2003
------------------------------
1) Implemented new cooling tower methods.
2) Added new tags in IO section InCnt and OutCnt which is number of input and
   output streams from a unit.

Update 2 : 23 January 2003
------------------------------
1) Implemented new CP equation for use in Specie Database. The format is
   Poly_Cp(a,b,c,d) where Cp = a + bT + cT^2 + dT^3.
2) Fix in feeder where under certain conditions a quality could not be switched off.
   Qualities are now removed from pipes when flowsheet Initialise or Empty is selected.
3) Some driver improvements in OPC client. New option in trend page right click popup
   menu to copy driver tag address to clipboard.
4) Turbine model state did not recover correctly after a project solve, save and reload.


Build 75 : 6 March 2003
------------------------------
1) Enabled new Free Energy Minimisation (FEM) option in reaction blocks. FEM uses
   VCS (Villars-Cruise-Smith) algorithm.
2) First implementation of new BlackBox models that are used in conjunction with
   the new "SysCAD Model Developers Kit" (ScdMDK). Beta version!
3) Driver improvements: (a) Improvements for OPC client startup.  (b) The only valid
   driver IO types are now ro, wo or rw. (removed option 'r' and 'w').  (c) Improved
   testing and validation of tags in get/set connections.  (d) improved message
   display for driver load information and errors.  (e) Fixed an error where for
   a 'wo' tag SysCAD received an "unexpected" value from OPC server, SysCAD now logs
   the error and sends back its 'correct' version of the tag.
4) New option in Simple Heater to specify an external tag from which the required
   duty is retrieved.
5) Added meaningful error message to Disk Centrifuge when vapour is in feed.
6) Fixed a project recover in solved state problem for sinks configured as
   "source constant pressure" for ProBal.
7) Improvements for numerical stability for convergence and solve state start/stop for
   ProBal for certain conditions. (Range in max damping).
8) Fixed a large memory loss error introduced in Build74 in order of evaluation for
   controls where a PID referred to another PID in the same unit.
9) Fixed an error introduced in Build74 where a SetDynTag in a PGM did not get
   set under certain circumstances.
9) Significant improvement in time to displaying Access window in large projects.
10) Extended Tag Cross Reference functionality for display of triangles in Access
    window fields. Blue for sets, green for gets.
11) Added new display tags in Alumina species properties page for precipitation
    information 'A/C_Saturation' and 'SSN_Ratio'.
12) Removed list of individual windows in window list menu which did not work
    correctly for projects with more than nine windows.
13) When using "Copy..." to create a new configuration file based on a selection,
    the specie database is now copied from the original.
14) Improved error messages and functionality when using construct link or construct
    unit in graphics and a "configured" graphic symbol is selected.
15) After project load, etc list number of invalid tags in a trend window rather
    then report a separate message for each illegal tag in a trend.
16) Fixed a crash caused by very long message strings sent to the message window.
17) Fixed a crash in specie database import fuctionality.

Update 1 : 19 March 2003
------------------------------
1) Fix for bug introduced in Build75 where under certain conditions referenced tags
   were not refreshed. (eg certain measured tags in a PID). Fix for General Controler
   using gettag(...) to refer to a tag within itself.
2) Fix for "composition fetch" in feeders where the pressure value was not transferred.
3) Changed ProBal pressure behaviour in a sink, the pipe outlet pressure going to a
   sink that is not connected no longer takes the value from the sink P_Rqd.
4) Made general information fields visible in the PlantModel access window on the
   statistics tab page. These were mistakenly hidden.
5) Fixed error where extra buttons and checkboxes appear in access window and never
   get deleted. These are typically from a general controller.
6) Fixed a crash during a PGM load if there is a misplaced "return". Fixed a PGM crash
   caused by suplying an empty string to the lognote command. eg LogNote("")

Update 2 : 28 March 2003
------------------------------
1) Improvements to SMDK ("SysCAD Model Developers Kit"): User can now use cfg file
   to list custom user SMDK DLL files that must be loaded. Vector of "volumetric
   flow" and "normalised volumetric flow" are now provided as well as the mass
   flow vector. Added function to set heat flow for energy balance. Other minor
   improvements as well as enhancements in examples and code documentation.
2) Fixed display of correct size data in crusher and screen models in project using
   multiple sieve series.

Update 3 : 17 April 2003
------------------------------
1) Fix to ensure QFeed tab page in access window in units such as thickener is
   created after a project load rather than when solve is selected.
2) Fix to recover correct organics, TOC and oxalate data in Bayer feed calculator
   after a project load.
3) Fix to correctly transfer SAM data in bayer model. Note that Al2O3.3H2O(s) (THA)
   must be present in stream for correct transfer of SAM data. To override SAM or D
   in a pipe "SetSAM" must be checked otherwise data entered for SAM or D is ignored.
4) Fixed use of reaction extent in dynamic. Note that this should only be used
   in non-surge units such as pipes.
5) Other minor improvements.

Update 4 : 23 May 2003
------------------------------
1) Various changes and enhancements to SMDK, including implementation of DevLic.
2) Ensure correct sieve definition is used in displays for screen and crusher models.
3) Improved reporting when loading PGM for errors caused by misuse of commands
   'return' and 'endfunct'.
4) Fixed problem where the first time 'SetDynTag' is used in a PGM, it was not set.
5) Fix in Bayer feed calculator for occasional crash caused by zero organics or TOC.
6) Changed default definition of Normal Pressure from 101.31 to 101.325. Added these
   as display fields in PlantModel access window.
7) Improved client models: Significant changes to QAL Precip PB model.
   New KaiserAl Precip model.


Build 76 : 29 May 2003
------------------------------
1) Implemented a change which affects values for Volumetric Flows for vapours.
   This affects the feeder in "Source_ConstQv" and "Source_ConstNQv" modes and
   pipe displays Qi and Qo for view "VolFlow", "VolFrac", "NormVolFlow" and
   "NormVolFrac". The change is for the density used when displaying Volumetric
   flowrates and Normalised Volumetric flowrates for vapours.  The calculation
   Volume = mass/density is used. Previously the density values entered in the
   species database were used, now the Ideal gas law PV = nRT is used instead.
   Both options are still available, primarily for backward compatability of
   projects. A constant or the equations 'LinearGasDensity' or 'IdealGasDensity'
   can be entered in the specie database. This can be ignored and forced to
   'IdealGasDensity' in the edit configuration dialog for the 'Species' tab page,
   where for each gas there is a check box to select 'Ideal Formulation'.
2) Fix for crash for invalid steam feed to desuperheater.
3) Fix for crash in project load after removing species from configuration that
   were used in size distribution.

Update 1 : 24 June 2003
------------------------------
1) Added option under menu "Options|General" on "Mouse" tab page to enable/disable
   the mouse wheel action for changing selected field value in access or trend windows.
2) Change to graphic display of sizedistribution graphs to use minimum and maximum
   display values entered by user.
3) Fix in partion curve display (eg screen models) of size data fields which occured
   under certain conditions in projects mith multiple sieve definitions.
4) Added option to switch individual reactions in a reaction block on and off.
5) Changed the options on the tab page "Models" in the "Project View".

Update 2 : 25 July 2003
------------------------------
1) Fix for bug introduced in Build76 Update1 for Bayer feed calculator for setting
   TOC, Organics, etc.


Build 77 : 4 August 2003
------------------------------
1) Improved display fields for precipitation model.
2) Fix for a crash in the belt filter model for "bad" data points in the washing
   curve.
3) First draft of new Excel report type "General Tag Information". This places
   general flowsheet tag information and lists on an Excel worksheet with the
   name 'Tag Info'.  To generate, in the SysCAD Reports tab page, add a new report
   of type 'General Information' (the report name field is not used), then press
   the 'Generate' button in the normal way.
4) Moved SysCAD OPC Server functionality into the DLL scOPCsrv. Added Browse OPC
   tag functionality to SysCAD.
5) Added new EHX method TempDrop where a pipe temperature drop can be specified.
6) For ProBal force operating pressure of most separation and similar units to
   standard pressure (101.325).
7) Other minor improvements. Fix for crash in cooling tower with zero feed. Fix
   for crash in heatexchanger for 'invalid' Bayer feed.

Update 1 : 19 August 2003
------------------------------
1) For the model configuration dialog, implemented various improvements mostly
   related to selecting the required model DLL files. Implemented option to
   select required SMDK DLL files.
2) Improvements to recovery of a project that under certain conditions was not
   recovered in a converged state. Improved recovery of tear streams and precip
   model.
3) Fixed crash from user entering a 0Kelvin temperature in specie database.

Update 2 : 28 August 2003
------------------------------
1) Fixed error where PGM OnInitialise block was used to switch PID controllers
   on or off and this change was not aplied until second SysCAD solve.
2) Enhancements to the new SysCAD Excel General Information report.

Update 3 : 6 October 2003
------------------------------
1) Fix for correct recovery after project load of byte type PGM variables.
   These were incorrectly always loaded as 0 or 255.
2) Fix for DXF block naming in SysCAD that causes a dxf drawing to not be loaded
   in AutoCAD or other CAD package.
3) Added additional display fields in AluminaProperties page for (1) Na2O
   equivilant fields; (2) actual concentrations of selected species.
4) Removed ScandrettEfficiency display in Drum Filter and replaced it with
   WashEfficiency (ConcFeed - ConcCake)/(ConcFeed - ConcWash)

Update 4 : 29 October 2003
------------------------------
1) Some minor improvements: Fix for crash in OxalateEquilibrium equation. Improvement
   for project recovery where PID or PGM refer to unsaved calculated tags.
2) Fix for remembering the correct default SpecieModel in edit configuration.
3) Fix for recovering converged project after snapshot or scenario load.
4) Improvements for ProBal Darcy pressure drop calculation. Limit pressure drop
   to ensure outlet pressure is not less than 10kPa. Added new calculated fields
   such as velocity and pressure drop. Added warning message when calculated
   velocity exceeds "reasonable" maximum. Added option to set friction factor
   rather than calculate it from viscosity and roughness. The Darcy equation
   now determines if it is a vapour stream and uses a different equation for
   a compressable gas. In the past the slurry incompresible pressure drop
   calculation was always used.


Update 5 : 17 November 2003
------------------------------
1) Fix for temporary freeze when ploting log scale graphs for XY functions with
   'bad' data points.
2) Improvements to "Solvent Extraction Mixer Settler" model using Isotherm method.
3) Corrected the Alumina properties fields added in update 3 from Na20 to Na2O.
4) Fixed crash where "orphan" pipes from a tank/tie/unit are connected to a sink.
5) Fix for annotation blocks on a drawing that were lost for a change tag.
6) Fix where if a controller refered to the tag PlantModel.Stats.WorstErr or
   similar it always returned 0.

Update 6 : 1 December 2003
------------------------------
1) Added OxalateEquilibrium field in Bayer Properties model.
2) Updated WashEfficiency calculation (removed ) in Filter Press, Centrifugal
   Filter and Drum Filter. CompWashEff = (ConcFeed - ConcCake)/(ConcFeed - ConcWash)
3) Extended Cyclone model to include additional correction factors.

Update 7 : 20 December 2003
------------------------------
1) Added new Cp equation "Shomate_Cp(A,B,C,D,E)" of the form Cp=A + Bt + Ct^2 + Dt^3 + E/t^2
   where t = temperature(in Kelvin) / 1000.
2) Fix for a crash for ProBal solve after pipes conected to units are deleted but the unit
   itself is not deleted.

=============================================================================
================                SysCAD 9.0                   ================
=============================================================================
Beta version Build 100 to 109
First official release Build 110

Build 113 : 2 December 2004
------------------------------
1) Implemented new Audit section for all models. This is visible on the last
   tab page of all models, with many values only being calculated after a
   solve. This is useful checking the mass and energy balance of a unit.
   You can select the popup menu option to copy to the clipboard a report
   that can be pasted in Excel for analysis.
2) Restructured graphics symbols to belong to groups, where many models may
   belong to the same group for the purpose of selecting symbols. Graphics
   dxf symbols for these groups restructured into separate folders in the
   basefiles\symbols folder.
3) Implemented a range of significant changes and improvements for the insert
   unit command.
4) Implemented a new command to change the graphics symbol for a unit.
5) Implemented new PID option "Clarke FF" which includes a bias and feed forward
   tag option.
6) For a PID if an Integral value of zero is entered, SysCAD actualy uses a value
   equal to half the period (step size). For ProBal this is 0.5 because the
   iteration "time" is 1.0. This value is now changed during solve, this does
   not apply to the new ClarkeFeedForward option where an integral value of
   zero is allowed.
7) Excel trend reports: Implemented new Excel Trend Query option that returns
   the value at the end of a time slice; added new time column reporting options;
   improved feedback for incorrect query time inputs.
8) Improved ProBal pipe Darcy pressure drop options.
9) Added functionality to load a default file for pgm and reaction files.
10) Various improvements to SetTagList model, including option to allow tags
   to be set before the first iteration.
11) Added option to select which 'bin' in a multistorage unit is visible.
12) Implemented new EHX method for loss to ambient.
13) Changed CCD to allow it to operate in dynamic transfer mode.
14) Fix in Mixer Settler for isotherm method for invalid specie selection.
15) Fixed some crashes for PSD display that occur for large amounts of data.
16) Added a new string input field "Location" as part of the standard header
    of all models.
17) Added detailed display and breakdown of various solver timers for tasks
    and threads mainly for analysis and debug purposes in the plant model window.
18) Re-enabled solver multithread options in solver. Error was in controls section
    of multithreaded solver.
19) Some improvements in functionality and reporting for setting of tags from other
    models (eg PGM, PID, profile, etc), especially for setting a tag that is a "button".
20) Improved load time of models with large lists for profiles, events, taglists, etc.
21) Fixed a crash if attempting to store strings in the historian.
22) Removed the option to hide/display a field in the access window by left double click.
23) Added option to select the fill colour for graphics for dynamic annotation
24) Other minor changes including: various improvements in the COM interface; improvements
    in electrical models; improvements in "neutral database" model & graphics import/export
    functionality; begun implementing beta version of group template creation and insert
    (not enabled in released version).

Build 114 : 11 February 2005
------------------------------
1) (#94,#95) Added display for Kinetic Viscosity, Thermal Viscosity, Thermal Conductivity,
   Prandtl number and SurfaceTension. Defaults for these properties are values for water.
2) (#122,#115) In Bayer properties: increased tolerance for internal density calculation
   convergence (this was essential for correct results in reactions using finalconc method
   for Bayer streams);  added display of BoilPtElav; fixed display of LVolume and SLVolume
   for empty stream.
3) In tear streams, added a new option for a manual tear to set initial conditions.
   Various options for how, when and what is being set.
4) Added option in feeder composition fetch to selectively transfer qualities.
5) Reporting of PGM errors now includes iteration number.
6) Improved layout and included new fields in Barometric Condensor and Flash Tank.
7) (#116) Fix for tank inline level control for certain arrangements of multiple outlet pipes.
8) Removed error messages that were reported by reactions where no reaction occurred.
9) Improved functionality and new fields in electrical cable and incomer.
10) Improved data entry dialog for PSD measurements in project configuration.
11) Fixed save and recover for NAN or '*' values.
12) For opening project, etc changed starting folder from cfgfiles to a level below.
13) Changed default naming of new graphics pages.
14) Added Excel trend report options TimeDate and TimeFull.
15) Fixed problem where after starting SysCAD and loading a project, for the first
    dynamic solve, SysCAD appears to solve for one extra, 1 second, iteration!
16) (#139) Fixed problem where if the required simulation time is set ($Dyn.DynStats.RqdTm)
    SysCAD usually solves for one extra iteration.
17) (#134) Improvements to model/graphics import/export and group definition, save and
    insert. This is still beta version functionality.
18) (#145) Fix for a crash when loading a profile with an invalid NAN time value or for
    an empty row entry.
19) (#142) In dynamic transfer mode where the pipe flow is restricted and that pipe is
    an outlet from a tie, the tie did not output the correct flow to that pipe. It
    now sets the correct flow and the pipe reports the error and manages the spill.

Update 1 : 15 February 2005
------------------------------
1) (#130) Fixed a number of bugs in the setting of the mass flow of a feeder when the view
   is "MassFlow". Importantly this fixes a save and recover bug introduced in Build114.

Update 2 : 18 February 2005
------------------------------
1) (#130) Additional fixes for feeder save & recover from update 1.
2) (#144) Fixed an error in profile load where input file contained whitespace.
3) Fixed error introduced in Build114 where value for Integral in PID was forced to 30.
4) Fixed an error for mixing of streams where one stream uses SMDK Bayer properties
   and was pure water or steam.
5) (#128,#132) Prevent user from entering a negative mass for a specie for contents of a tank.
   Assume 0.0 if entry is negative.
6) Some tidy ups of options in new manual tear feature.

Update 3 : 22 February 2005
------------------------------
1) (#151) Added new Matrix class to the PGM. Similar to the existing Array class.
2) Fixed an error in PGM where a solve start failed because of a previous
   reference to a tag that is no longer valid using the SetDynTag or GetDynTag.
3) Fix for checkbox fields in models developed using SMDK. The correct values were
   not being set or retrieved from trend or PGM code.

Update 4 : 1 March 2005
------------------------------
1) (#149,#153) Implemented new option in reactions in tanks for dynamic mode. A
   reaction rate (fraction per second) must be specified for the Extent:Fraction
   method. eg "Extent:Fraction FeS2(s) Rate = 0.0001"
2) (#160) Fixed crash where after deleting models that had tag references (eg PID)
   to existing models, in some circumstances SysCAD crashed on solve.
3) (#120) The insert symbol for the graphics commands, did not remember the most
   recently used symbols. Fixed.
4) (#114) Fixed problem where for the change symbol graphics command, if selecting
   alternative symbols, the original scale was not retained.
5) (#133) Added a string option in SMDK for units.
6) In SMDK Bayer properties example, removed calculator options for organics, and
   implemented simple TOC & Oxalate concentration option.
7) Added FreeCaustic term in Bayer properties model.
8) (#150) Allow use of attributes in calculations in specie configuration.
9) (#124) Display correct values for Cp in audit page, was always 0.

Update 5 : 2 March 2005
------------------------------
1) (#164) When loading a reaction with a specified heat of reaction, SysCAD did
   not recognise the separator '/' resulting in the specie not being recognised
   and a value of 0 being used. This has been fixed so the following example now
   works: "HeatOfReaction: msFixed = -125/Na2CO3(l)". Note that old projects
   loaded in this update would result in different reaction output conditions
   if reactions included this.
2) Fixed a multithread solver crash for dual CPU PC.
3) (#166) Fixed a crash for a PGM reference to an object eg GetTag("P_1").
4) (#150) Fixed an attribute save and recover problem. Added on optional
   specification of min and max range for attributes. (Note that currently
   calculations and attributes can only be specified manualy in the cfg file)
5) Stricter checking and reporting of invalid conversion units in PGM has been
   implemented.

Update 6 : 9 March 2005
------------------------------
1) Various reaction FinalConc fixes and improvements: (#161) When using FinalConc
   in a reaction, there is now an option to specify at what temperature this is
   at. eg "Extent: FinalConc Al2O3(l) = 85 At 25";  (#180) Fixed an error where
   a forward only reaction '->' actualy occured in the reverse direction;
   (#174) Fixed an error where for simultaneous reactions using mixed extent
   fraction and extent finalconc methods, the incorrect result for the finalconc
   portion was achieved;  (#174) The FinalConc method only aplies to the
   concentration after that reaction step in a sequence of reactions. To make this
   more apparant, the tag Conc_Act has been replaced with Conc_EOStep (concentration
   at end of reaction step) and Conc_Final (concentration at end of ALL reactions).
2) (#173) Fixed an error in reactions where a specie occurs as both a reactant
   and a product in the same reaction.
3) (#172) Fixed an error recently introduced where the composition fetch
   transferred the incorrect mass flow.
4) For dynamic, added an option "TrackMinMaxMeas" and "TrackMinMaxOut" to PID
   that, if unchecked, will not show the warnings for the measured or output
   values being out of range.
5) (#179) Fixed a memory leak related to PGM or PID tag references. This problem
   was particularly relevant to dynamic projects.
6) Fixed an error where PGM or PID tag reference lists were unnecessarily
   continuously being rebuilt. This fix results in a significant speed
   improvement, especially in dynamic models.

Update 7 : 12 March 2005
------------------------------
1) In extended unit conversion lists such as those used in reactions the SI unit
   (eg "kg/s" as well as "kg/s.H2O(l)") was not added to the list of available
   conversions (eg for tag xx.RB.R1.msXfer).
2) (#188) Fixed an error introduced in Update 5 where in a feeder in View "MassFrac"
   with Basis "Phase" any data entry was interpreted incorrectly.
3) (#192) Added an option in SMDK to allow visibility of a MStream object.
4) Some minor improvements in the Audit report.
5) (#172) Further improvements and fixes to composition fetch options.
6) Further changes to dynamic reaction rate option first implemented in Update4.
   By example, the two reaction rate options for reactions in tanks for dynamic mode
   are:"Extent:Fraction FeS2(s) Rate = 0.0001" and "Extent:Fraction FeS2(s) Rate =
   0.0001 Stabilised". The first option uses the Rate directly and the fraction
   reacted is given by: Fraction = Rate*DeltaTime. The second with the keyword
   Stabilised is a 1st order response and the fraction reacted is given by:
   Fraction = 1 – exp(-Rate*DeltaTime).

Update 8 : 29 April 2005
------------------------------
1) (#188) Additional corrections to those in update 7 for data entry in a feeder
   in View "MassFrac" with Basis "Individual Phase" where data entry was interpreted
   incorrectly.
2) (#147), (#196) For dynamic transfer mode, under certain conditions a tank or
   connected feeder incorrectly reported an output flowrate when it should be
   zero based on configuration.
3) (#203) Fixed problem where the thermal loss action in the precip model did not
   occur if SSA in feed was not present.
4) (#213) Fixed error in GM in tie for Phase_MassFrac option with PhaseSpec
   Individual where no solids are present in the feed.
5) (#199) Fixed the graphics "Construct Tie" command which did not work from
   Build 113.
6) In dynamic, relax tolerance for reporting spills. Improved spill messages.
7) Added a new function "Setup" for the PGM class Noise. This is the same as
   Init, but doesn't set the seed value. Setup(mean, stddev, typ, global).
8) Fixed crash for setting historian tag list from COM interface.

Update 9 : 8 May 2005
------------------------------
1) (#242) Added tripper functionality to the conveyor belt model where a single
   drive with position logic can represent the tripper movement and feed separate
   individual bins based on position.
2) (#195) Removed functionality that changed toolbars for a runtime license.
3) Fixed error in PGM and RCT where you could not edit the file if it was in
   the CfgFiles folder.
4) Fixed an error for externally setting specie fractions in phase view in a feeder.

Update 10 : 24 May 2005
------------------------------
1) (#237) For SMDK, the state of a MStream class included in a unit is now saved
   and recovered correctly.
2) For SMDK, added some extra virtual functions to the MBayer class.
3) (#252) Fixed bug where the tag "PlantModel.Stats.WorstErr" always returned 0.
4) (#240) Fixed bug where the SetTagList model, did not set any $Dyn or $PB tags.
5) (#246) Fixed error where from the COM interface, the historian was not deleted.
6) (#248) Disabled multi-threading solver control actions which caused a freeze on
   dual CPU or HT PC. To be fixed in future release.
7) (#197) Fix for tie where incorrect outlet temperature is predicted where GM
   model is off and Qm_MakeUp mode functionality is used.

Update 11 : 7 June 2005
------------------------------
1) SMDK: Added Time and DeltaTime functions to MMethod class.
2) Fix for Excel Trend report crash for large number of tags.
3) (#263) Further fixes for conveyor belt mass balance errors.
4) Fix for crash that occurred if project is saved immediately after adding a new
   reaction file.
5) Improved error message reporting for unbalanced reactions.
6) (#159) Implemented new washer model option where the bypass can be directed
   to the underflow rather than the overflow.
7) (#267) Fix for reaction FinalConc method where a specie source is included.
8) (#259) Fix for error where referenced file for SetTagList, Profile or Events
   model was not copied for a project save-version or save-as.
9) (#271), (#129) Fix for freeze during construct unit or link if an invalid
   tag is used.
10) (#198) Fix for crash during construct link where invalid model is selected
    for source or destination unit.

1) (#54) Found and fixed an error in reactions where the feed is a Bayer stream.
   Where the heat of reaction (HOR) override is used an incorrect temperature
   for reaction output was predicted. Also, where the stream is a Bayer stream
   and HOR was not provided, again the incorrect temperature is calculated.
   Both issues were related to the fact that SysCAD was incorrectly using the
   "standard" specie model instead of the "bayer" specie model for
   calculating the Heat Of Reaction and/or the change in Total Enthalpy.
   IMPORTANT NOTE: Old projects loaded in this Build could result in different
   reaction output conditions in circumstances described above.

Build 115 : 14 July 2005
------------------------------
1) (#54) Various improvements and changes in reactions. For a Bayer stream, the
   reaction, by default, now uses the Bayer properties (rather than the "standard"
   specie model) as the reference model for internal heat of reaction (HOR)
   calculations. The new option "StdRefModel" can be checked to force the use of
   the "standard" model. IMPORTANT NOTE: Depending on reactions and reaction
   configuration, old projects loaded in this Build could result in different 
   reaction output conditions.
2) (#208) Added an option in reactions where a reference temperature and pressure
   for HOR can be specified. The pressure is separated by a , and is optional.
   (eg HeatOfReaction : msFixed = -897.87 / NiS(s) At 25,101.325) If reference 
   temperature is not specified then reaction feed temperature (and pressure) 
   is assumed (this is behaviour in previous versions). Other valid
   options for specifying HOR temperature and pressure are "Feed", "Prod" and 
   "Std" (eg "HeatOfReaction: msFixed = -125/Na2CO3(l) At Feed")
3) In reactions added a new option "ExtraInfo" which can be used to remove a 
   number of tags previously shown in reactions.
4) Further improvements in the Audit report, especially in terms of reaction
   energy balances.
5) (#178) Implemented functionality where blocks of tags that are of no interest
   in your project can be globaly removed. Because these tags don't exist, they 
   are not visible and the access windows are shorter and neater without the 
   unwanted data. The other important benefit if this is a significantly improved
   speed performance for tag access (eg PGM and PID tag references, etc). This
   functionality also allows user to configure what views for specie lists are 
   available.
6) Changed layout of RB, EHX and VLE in terms of the check boxes and sequence of 
   tab pages (each always on their own page) to be consistent in all models. Note 
   that if RB and EHX are both present, that EHX occurs after reactions.
7) (#183) Added ability to allow EHX in a tie model (Dynamic and ProBal). Added 
   ability to allow EHX in a tank model in ProBal mode.
8) In the pipe model, you can select the sequence of action for RB and EHX.
   Historically the sequence was EHX then RB, the default for new projects will
   now be RB then EHX.
9) (#176) Most fields that may accept a '*' (NAN) as a valid data entry now have
   a dropdown list display with a short description of what '*' represents. For 
   data entry, a '*' is still a valid input as well as the string or index value.
10) (#189) All Bayer specie model feed calculators now operate correctly if vapours
    or other liquids are included in the makeup.
11) Improvements in reporting pipe spills in dynamic transfer mode.
12) Added option "PlantModel.SpillReporting" that determines behaviour when a unit
    or pipe spills. A usefull option for "debugging" spills is where SysCAD goes to
    idle mode (pauses) when a spill occurs so that you can review the cause of the
    problem.
13) The option "PlantModel.ClosureTests" has been extended similar to the new 
    PlantModel.SpillReporting option. Generally this would be off, but can be 
    switched on for checking mass and energy balance (closure) tests in models
    each iteration.
14) Added option "PlantModel.RandomSeedAtStart" which can be used to seed the
    random number generator at the start of a simulation run. Use "*" to use a
    random seed.
15) In the dyanmic setup dialog, improved method for setting real time, iteration 
    time and step size.
16) (#251) For dynamic mode, implemented an option where the start time can be set at 
    startup, as well as restarting the historian.
17) Improved status bar displays of progress for ProBal and Dynamic solve.
18) (#272) Implemented a new option in insert link and redraw link to leave a break in the
    line. This can be done by holding down the Ctrl key while selecting a point or
    by first selecting the new 'Break' button. Changed the 'Orthogonal' and 'Free'
    drawing mode buttons.
19) (#273) Implemented a new option in insert link and redraw link to select an 
    'arrow drawing rule'.
20) (#273) Improved redraw link to operate similar to insert link.
21) (#279) Fix for moving "dumb" graphics line segment.
22) (#274) Implemented a new option to insert or update text on the graphics pages.
23) (#262) Improvement and clarity in options for setting values for a manual tear.
24) (#214) Do not allow historian 'stop gap' to be very small as this prevents correct
    storage of start and stop values in historian.
25) (#214) For the historian equally spaced average values Trend report, the first 
    data points returned at time 'zero' are the current values at that time, all 
    subsequent points are average values for preceeding time interval.
26) Added a new optional field "MolecularDiam" to the specie database. The optional
    fields are "MolecularDiam", "MolarVol" and "CpCv". If this data is entered it may
    accessed by specialised custom or SMDK models.
27) (#286) In reactions implemented new option "ChangeVars" (similar to ExtraInfo)
    that includes tags for each reaction for the change on a mass or mole basis of
    individual species.
28) (#293) Added test for temperature cross-over for "loss to ambient" methods in
    EHX used in units (eg tanks, ties).
29) Shell&Tube Heat exchanger changes: Added new field "UALMTD" which is the 
    theoretical duty which is not always equal to the actual duty. Added new
    field "ThearArea" which displays the theoretical area required based on HTC,
    actual LMTD and duty. If not in fully condensing mode, there is now an option
    to specify "ApproachDT".
30) For shell&tube in condensing mode, added a new option to set the fraction of
    non-condensable gases to be vented. Added new field to display how much 
    steam is actually condensed. (#249) Error message is now provided when there is
    steam present in the condensate outlet stream. Fixed problem where too much 
    steam was condensed (this occurred where duty was higher than maximum available
    duty defined by U*A*LMTD). For backward compatibility, added option "IgnoreAreaLimit"
    which allows extra steam to be condensed.

Update 1 : 3 August 2005
------------------------------
1) (#158) In SMDK added option to SetDefaultTag to include/exclude the Usr prefix.
2) Fix for crash during program exit under certain conditions.
3) PGM enhancements: Added variable PauseSimulation which can be used to put SysCAD 
   into Idle mode (ie paused). Added function strcat4 and strcat5 which can be used 
   to combine 5 and 6 string or Str parameters respectively. Fixed a PGM crash that 
   occured under certain conditions when referencing parameters of nested classes.

Update 2 : 15 August 2005
------------------------------
1) (#319) In the PGM for the Array or Matrix classes, you can now watch the variables
   corresponding to the contents of the array or matrix. To do this use the watch 
   command. If the index is out of range for the array dimensions then the value 
   returned is always NAN ('*') and a set value is ignored. Examples for an 
   individual element in an array are "Watch A[2]" or "Watch A@[2]" for a read-only 
   tag. An example for a range is "Watch A[All,5]" which would show the first 5
   elements of the array. Examples for an individual element in a matrix are 
   "Watch M[2,1]" or "Watch M@[0,0]" for a read-only tag. An example for a range 
   is "Watch M[All,4,2]".
2) In a PGM, when declaring a variable that should be visible as a read-only tag,
   the '*' is now optional. So instead of "double d*@", "double d@" can be used.
3) (#323) The blowdown connection to the boiler is now optional. If a pipe is
   not connected to the blowdown connection, the specified blowdown fraction is
   ignored (ie assumed zero).
4) (#328) Fixed bug in turbine, where the losses due to mechanical efficiency were
   included in the calculation for determining turbine exit conditions. This has
   been fixed and only the adiabatic efficiency is used. Added new output fields
   "TurbineWork" and "TurbinePower" which are the results before applying losses
   for any form of mechanical efficiency. The fields "Work" and "Power" are the 
   final useful output power or work.
   IMPORTANT NOTE: Any projects using the turbine with a MechEfficiency less than
   100% will now predict different outlet conditions for the turbine.
5) (#220) The flash tank model has been updated to work in dynamic transfer mode
   with same behaviour as ProBal mode.
6) (#324) The shell&tube model has been updated to work in dynamic transfer
   mode with same behaviour as ProBal mode.
7) The VarSelect options were not being saved and recovered for older projects.
8) (#326) Added new Excel trend report option "SysCAD_TrendReport()" where options
   and tags can be listed vertically rather than in two horizontal rows for
   "TrendReport()" option. The options can be listed vertically any order below 
   the keyword SysCAD_TrendReport(ReportName), then there should be at least
   one (up to 5) blank cells, then all the tags should be listed.

Update 3 : 26 August 2005
------------------------------
1) A fix for a crash caused by a NAN (*) value in graphics annotation bars.
2) A fix for a PGM crash for Str parameter for Array class load file.
3) (#333) A fix for setting estimate data in manual tears.

Update 4 : 1 September 2005
------------------------------
1) PGM improvements: EndFile can be used as an alternative to $. Implemented new 
   functions: FltToStr(double Value), IntToStr(long Value), StrToFlt(Str Value)
   and StrToInt(Str Value).
2) (#320) Implemented the ability to declare arrays of user classes. This can
   only be done in the global scope. Use square braces "[" and "]" to index 
   array elements. First element is at index 0. Example of declaration would 
   be "MyClass C[5]". Examples to access a class member "C[0].a = 5"; 
   "C[i].Exec()"; "x = C[i].a + C[i].GetResult(g)".
3) For dynamic bar annotation, added the ability to set the colour for each
   of the bars.
4) (#287) Added the ability to include reactions in the multistore model.
5) (#302) Added the ability to block or allow solids/liquids/vapours from outlet
   pipes in the multistore model.
6) Added SetStopRequired and SetIdleRequired to SMDK API.

Update 5 : 17 September 2005
------------------------------
1) Added getSizePassingFraction and getFractionPassingSize to MIPSD in SMDK.
2) Fixed an error in manual tear for Bayer stream with Bayer feed calculator.
3) Added option "PlantModel.IncludeSpcCnvs" which by default is on. If this
   is switched off then the "special" specie engineering conversion units are 
   not created. eg reaction extent "%.H2O(l)"
4) Fixed a crash in PGM for large number of function string parameters.
5) Added ExcludeWatch command to PGM, which is used to make the variables of a
   class not visible in the access window. eg "ExcludeWatch C[0]"
6) For PGM array (and matrix) classes added the functions IncAt(Index, value) 
   and DecAt(Index,Value). eg "DataArray.IncAt(3, Qty)" is more effecient than
   "DataArray.SetAt(3, DataArray.GetAt(3) + Qty)"
7) (#307) Can move text as part of grouped graphics.

Update 6 : 29 September 2005
------------------------------
1) (#220,#324) Fixes to flash tank and shell&tube operation in dynamic
   transfer mode.
2) Fix for a tie with vapour flows in ProBal mode where the incorrect outlet 
   pressure was used if PSat was selected.
3) (#348) Fix for reaction HeatExchange Power option in dynamic mode.
4) (#349) Fix for COM interface for ProBal start, stop, etc.

Update 7 : 27 October 2005
------------------------------
1) (#372) Fixed error introduced in Update5 for shell&tube in flash train
   where if a new heater was used it would not condense steam.
2) (#350) IgnoreAreaLimit in the stand-alone shell&tube heat exchanger can
   now be used to fully condense all steam regardless of user specified HTC 
   and area.
3) Added an option for dynamic bars to include or exclude the labels.
4) (#354) Fixed error where the selection of Access97 for database format 
   was ignored for many of the create or save database options.
5) (#357) Fixed error for pressure conversion units for gauge pressures.
   Improved display on Conversions tab page in Project window.

Update 8 : 28 November 2005
------------------------------
1) (#366) Fixed problem for increasing number of stores for multistore with 
   shared reactions.
2) (#371) Fixed crash in PSD calulation of cut size in screen model.
3) (#374) Fixed error where data entered in a table in the access window for
   the screen partition curve or belt filter wash curve was ignored.
4) (#367) Added "Mid-time fraction" in historian, usefull in dynamic with
   large step size for storing and recovering values thant change occasionaly.

Update 9 : 22 February 2005
------------------------------
1) Implemented new Blend Optimiser model.
2) (#392) Fix for PSD for calculating MeanMass for a size range.
3) (#396) Added option when editing cfg file on the measurement page to set
   a Count Factor for calculating PSD particle count.
4) (#407) Implemented test for too many PSD intervals.
5) (#235) Fix for large memory usage in projects using SMDK properties.
6) (#408) Speed improvement in density convergence calculation in Bayer
   properties.
7) (#410) Implemented new tags in $Dyn for current day, month and year.
8) Added new PGM functions IsAlpha(str) and AlphToNum(Str).

Update 10 : 28 February 2005
------------------------------
1) Fix for a crash in screen model when more than 12 species have PSD.
2) Fix for a crash in shell&tube where flowrate in one strem is zero and the
   other stream is very large.

Update 11 : 13 March 2005
------------------------------
1) (#416) Added new method "Individual_PartCrv" to screen model to allow
   user to specify a separate partition curve for each solid with PSD data.
2) (#254) Engineering conversion units (in the popup conversions menu) are now 
   listed alphabetically except for the SI unit which is always shown at the
   top of the list.
3) Removed limit on number of conversion unit groups allowed. This was a problem
   for projects controling extents of many species because of the extra
   conversions created for %H2O(l), etc.
4) (#417) Added option in "$Dyn" access window to specify a filename of a
   text file with a list of all tags (ignoring first line) to be recorded in 
   the data historian.

Update 12 : 24 March 2005
------------------------------
1) (#421) Fixed error "unable to open ..\cfgfiles\cfgfiles\.." which sometimes
   occured for opening or creating a project.
2) Added field NoOfReactions to reaction block reporting count of sources, 
   reactions and sinks.
3) (#430) Added tag "IntervalCnt" to the "Sz" tab page in the Access window
   which returns the number of intervals in the PSD.
4) (#422) There are now 5 tuning factors for the cyclone model using the Plitt
   method they are F1 d50, F2 sharpness m, F3 pressure drop, F4 S, and F5 Liquid 
   overflow.
5) (#431) Fixed error in cyclone plitt method lynch partition calculation. 
   NOTE: This will affect the PSD distribution. 
   Added other corrections and improvements that are used if check box "OldCalcs"
   is not selected. Added display fields for Alpha and reference temperature.

Update 13 : 21 May 2006
------------------------------
1) (#439) Fixed crash where large number of species have PSD data.
2) Improved display of PSD data where more than one specie contains PSD data. All
   the cumulative PSD data is now displayed in a separate table.
3) (#417) Improveved functionality in "Dynamic Info" for managing a tag list of
   tags to be recorded in the historian.
4) If shown as column options in the PSD views, all the xxxG (eg QmCumG) options
   have been removed.
5) (#131) Corrected display for BPE in bayer streams. The equation for BPE is 
   based on the stream composition and temperature. For BPE displayed, the stream 
   temperature was incorrectly used, rather the pure water saturation temperature 
   at stream pressure should have been used. NOTE: The values displayed for BPE
   will change, however the correct BPE was used in flash calculations, etc.
6) (#450) Forced check for sequence number in the first reaction if any of the
   subsequent reactions have a sequence number defined.
7) (#452) Fixed error in reactions where the incorrect amount of source material
   was shown when any of the reactions used the FinalConc method.

Update 14 : 25 August 2006
------------------------------
1) In PGM declaration of variables, user can now specify only the conversion family 
   and the SI units are assumed. eg double flow@("Qm"); or double FracRqd*("Frac").
2) (#462,#466) Fixed error where profile file contained a single entry at time zero.
3) (#483) Added new field "RqdCalcMode" in direct contact heater for user to specify 
   the desired calculation mode. If the contact heater is in "stand alone" mode, then 
   "FinalTRqd" or "TRiseRqd" are used as maximum limits and NOT strict requirements.
4) (#487) In graphics when unable to insert a pipe, after 3 failed attempts to select 
   a source or destination unit, the commend is canceled (in past user was stuck).
5) (#494) Added option to set conveyor belt product discharge position to end without
   having to change position if belt length is changed.
6) (#505) Added "On" option to crusher. When off, output equals input.
7) (#506) SysCAD now uses .ini files in basefiles folder rather than the registry.
8) (#492) Fixed beltfilter calculation error. Washing curve is on a mass not volume 
   basis. Added warning message if solute is present in wash water. 
   NOTE: This fix will affect output streams.
9) (#512) Improved error message to report problem model if database error occurred
   during project save.
10) (#519) Fixed crash when blowdown was not connected to boiler.
11) (#431) Further cyclone Plitt method fixes. Use stream temperature for density
    calculations.
12) (#524) Added PrjName display field to PlantModel access window.


Update 15 : 29 August 2006
------------------------------
1) Allow use of Alumina2.DLL.

=============================================================================
================                SysCAD 9.1                   ================
=============================================================================

Build 120 : 9 September 2006
------------------------------
A) Model changes:
1) (#168) Implemented new "AutoVLE" functionality. This is an equilibrium finder
   that can be enabled for selected areas in a similar way to other qualities
   such as solubility. User is able to define options such as Relative Humidity,
   Saturated, SuperSaturated. Display of current RelHumidty , SpecHumidity, DewPt
   and other qualities is shown. AutoVLE is for water only.
2) Implemented a new Thermal Block in pipes where ...
3) Changed underlying data type for the general splitter model (GM) from float
   to double to improve numerical accuracy.
4) Implemented 4way-valve model.
5) Added functionality to pump and valve so that they can operate in dynamic
   transfer mode. In this mode the required maximum flowrate can be specified
   which corresponds to the 100% valve or pump position. In a line with multiple
   pumps, valves, etc the resultant flowrate will be the minimum of all the
   output "capacity" flowrates of all the equipment and pipes in the line.
   The valve and pump capacity can be specified in mass flow or volumetric flow.
6) Dynamic mode improvements to the compressor model and flow regulator block.
7) Added option for gain and offset options to the profile model. The profile 
   output is adjusted as output=value*gain+offset.
8) For reactions, added new msFinalFraction or mlFinalFraction Extent options.

B) Solver changes
1) (#170) Implemented a new "leaks" functionality for full dynamic mode. This
   allows for "leaks" or flow between the environment and models. The leaks would
   optionaly be from flanges on either end of a pipe. The flow may be in either
   direction. The leak flowrate would be dependent on the configuration of the
   "environment envelope area", the state of the model contents and the
   configuration of the leak parameters of the model.
2) Implemented new system "Area" model. Some model types allow "leak" flows of
   material to occur in any direction between the area and any number of pipes
   or units.
3) Enhancements and improvements to the Audit and Closure information page.
4) For pipes in dynamic transfer, the maximum allowed flow capacity tags have 
   been changed and the capacity can now be specified on a mass or volume flow
   basis. For old projects, set UseNewCapVars to true to use new options.
6) In dynamic transfer mode, there are now two mode options in pipes, these are
    "Transfer" as before and "Pipe". The "Pipe" option allows reverse flow in
    streams.
5) Various improvements to the solver for dynamic transfer mode.
6) Improvements to network analysis for dynamic with joins and tears.
7) Added support in dynamic transfer mode for reverse flow in pipes. The pump
   can be configured for reverse flow.
8) Implemented dynamic run to steady state option.

C) User Functionality changes:
1) Implemented new graphics group block functionality. This allows user to
   select a group of graphics and save these to a database. This group (or
   "template" or "assembly") can then be inserted as a group of models in the
   project. When inserting the group, the user can select from various options
   to set the tags of all the models inserted as part of the group. The group
   that has been saved can be used in other projects. This feature is effectively
   a type of block copy and past, as well as having other aplications.
2) Implemented new explorer window for selecting windows. This also shows all the 
   tags grouped by graphics page, model class or by filter selection. Various 
   options are available through right click popup menu. The Explorer window has
   a filter option that can be used to quickly search for a tag.
3) Implemented a bulk tag change option. This provides a range of filters and
   options to select a group of tags and then change this group of tags. The 
   selected tags can also be copied to the clipboard.
4) Added new options for trend and graphics window display options. For example
   'treat as one' overlays all graphics windows such that it appears like there
   is a single graphics window.
5) Implemented new Excel report SysCAD_AutoTags(ReportName, H/V, MaxColCount, 
   MaxRowCount, Option1, Option2...). A SQL select and orderby type statement 
   can be specified as an option to select the primary tags to be included. 
   The report first returns all the tags that meet the selection statement 
   criteria, and then the data as for a normal SysCAD_Tags(...) report. Example: 
   SysCAD_AutoTags("Pipes", H, 200, 30, Select [Graphic]=="05_Flowsheet" and 
   [Type]=="Pipe", OrderBy Tag Asc)
6) Extended edit configuration dialog Step 1 page to include all groups of
   sub-models in the list of model types.
7) Improvements to the neutral database import/export of graphics and models.
8) Added new options to right click pop-up menu for the message window.

C) Other changes:
1) Upgraded licensing to use crypkey 6.5.
2) Syscad91.exe is now the main aplication file. Syscad.exe is a license utility
   aplication. The license, by default is now located in the folder "License"
   and not the "bin" folder. The correct syscad.exe and crp320001.ngn files must
   be located in this folder.


Update 1 : 15 September 2006
------------------------------
1) Improved stream summary for Material Source model.
2) Improved right click popup menu options on the explorer window.
3) (#548) Increased allowance for maximum number of specie phases from 18 to 32.
4) (#555) Fix for crash during load due to bad connections.
5) (#557,559) Thickener and washer improvements for dynamic transfer mode.

Update 2 : 22 September 2006
------------------------------
1) (#551,556) Improvements for archive reporter.
2) (#528,565) Improved tag checking of specie tags and phase names.
3) (#569,570,571) Makeup block improvements.

Update 3 : 28 September 2006
------------------------------
1) (#211) Implemented new EvapBlock option.
2) (#564, 568) New makeup block methods fixed flow and ratio.
3) (#589) SysCAD Marshal improvements. New status displays, deadband option and 
   synchronous writes.

Update 4 : 5 October 2006
------------------------------
1) () Makeup block improvements.

=============================================================================
================                SysCAD 9.2                   ================
=============================================================================

What next...
1) (#200) Implemented new feature where user feedback is provided in any models
   or pipes where, based on the state of stream composition, temperature and
   pressure the properties model reports if any property correlations are out
   of the expected range. Currently only partly implemented in Demo SMDK Bayer.
..............
*/

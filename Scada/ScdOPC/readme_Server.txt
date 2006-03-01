
 FactorySoft
 Release notes for OPC Server toolkit 2.04
 December 01, 2000  

+++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++
+ What's new?
+++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++

This version of the toolkit includes two new server
examples and an enhanced VB application example.
A number of bug fixes are also included this release.
See the Modifications for version 2.04 section for
details.
 
+++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++
+ Installation:
+++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++

This installation should go in the same directory 
as a Client toolkit, if you purchased one.
The default is C:\OPCToolkit.
+++++++++++++++++++++++++++++++++++++++++++++++++++++
+ Modifications for version 2.04 (December 01, 2000)
+++++++++++++++++++++++++++++++++++++++++++++++++++++
1.OPC Toolkit passes OPC Compliance test and it is tested 
  using OPC Data Access Compliance test tool version 1.0
  build 1061
2. Return errors changed to E_INVALIDARG and S_FALSE for 
   GetItemProperties, LookupItemIds, AddItems, ValidateItems,
   RemoveItems, AddItems, ValidateItems, SetActiveState,
   SetClientHandles, SetDataTypes,Sync and Async IO Read 
   and Write.
3. GetItemId is changed to get the itemID for each tag.
4. Refresh is modified so that we dont get more callbacks 
   than requested.
   

+++++++++++++++++++++++++++++++++++++++++++++++++++++
+ Modifications for version 2.03 (December 16, 1999)
+++++++++++++++++++++++++++++++++++++++++++++++++++++
1.  A new Rapid Service server has been added to the
    toolkit.  This server is a version of the Rapid
    Shell server that has been modified to allow it
    to be used as a Service.

2.  A new rapid Modbus server example has been added
    to the toolkit.

3.  Several new features have been added to the sample
    Visual Basic Application.  The VB client now
    supports browsing and can perform synchronous
    reads and writes.

4.  OPCGroupBase.cpp - IOASyncIO2::Read - all values 
    and qualities are now sent to the client regardless
    of active status.

5.  OPCGroupBase.cpp - IOASyncIO2::Refresh - only
    active items are now sent to the client.  Now
    returns E_FAIL if no items are active in group. 

6.  OPCGroupBase.cpp - OPCGroupBase::Dadvise -
    removed the AddRef() for m_DataAdviseSink,
    m_DatatimeAdviseSink, and m_ASyncAdviseSink.

7.  FSServer.dll - OPCGroup::DoUpdateGroup - added
    deadband for VT_UI1 types.

8.  FSServer.dll - Modified IOPCServer::RemoveGroup so 
    that the server would not attempt to delete the group
    after the last reference has been released.

9.  Inprocess server:  Added window messages to handle
    the firing of events. Visual Basic does not support
    events fired directly from any thread other than the 
    main thread created by the VB project without marshalling.
    Modifications have been made to the OPCGroupBase.cpp/.h and 
    to the OPCGroup.cpp/h and OPCServer.cpp/h files for the 
    Inprocess server.  To incorporate, you must use the new base 
    files and add the changes as shown in the InProcess source files.
    To view the changes in the Inprocess source files you should do
    a search on VB6.  If the server is not to be used with an 
    automation client then the windows messaging can be removed.

10. For all of the rapid servers.  Moved the creation of
    ShellCallBack object to before the start FSServer call.
    This will insure that the callback object is fully 
    initialized before the FSServer is started.

11. For all servers, modified the date/time section of 
    GetItemProperties to remove the filetimetodostime call.    

+++++++++++++++++++++++++++++++++++++++++++++++++++++
+ Modifications for version 2.02 (May 10, 1999):
+++++++++++++++++++++++++++++++++++++++++++++++++++++
1.  Shutdown of OPC server changed to avoid race condition.
    The OPCMain thread shuts down in more steps, and
    ExitInstance waits longer on the handle.

2.  OPCGroupBase::Shutdown cleans up its threads and lists.
    OPCGroup's destructor should call Shutdown or do nothing.

3.  Added support to query LocaleIDs. The Modbus server
    enumerates the resources and adds the LCIDs present.

4.  DoRead also updates the item value to comply with
    the OPC 2.0 spec (OPCGroup.cpp).

+++++++++++++++++++++++++++++++++++++++++++++++++++
+ Modifications for version 2.01 (March 23, 1999):
+++++++++++++++++++++++++++++++++++++++++++++++++++
1.  The OPC Foundation distributed new proxy dlls,
    which are contained in this release.

2.  The OPC Foundation header file contained the wrong
    GUID when registering OPC 2.0 Component Category.

3.  Arrays of BSTRs are now supported in the OPC 1.0
    interface (They are packed into the data stream.)

4.  Threads are created using CreateThread to avoid
    synchronization problems when waiting on the handle.

5.  Async requests are handled by a single thread per
    group. They used to start another thread per call.

6.  Items are removed in the Group scan thread, not
    in the RemoveItem call, where the items may currently
    be in use by the scan thread.

7.  Servers shut themselves down automatically when
    launched by COM.

8.  Temporary tags are removed properly when the
    group is done using them.

9.  The Shell example now supports browsing Flat
    in addition to hierarchial.

10. improved use of critical sections and exception handling.

+++++++++++++++++++++++++++++++++++++++++++++++++++
+ Modifications for version 1.33 (March 27, 1998):
+++++++++++++++++++++++++++++++++++++++++++++++++++
1.  An error in the Item Attributes enumerator has
    been fixed. The error occured when making the
    safearray of high and low limits for analog tags.
    The client enumerates attributes when you click
    on item parameters.

2.  The FSServer DLL has been changed to link statically
    with MFC, so it does not require the application using
    it to use MFC. You can make it link with MFC as a DLL
    if you will be using it with an MFC application.

3.  The FSServer DLL has added a callback function
    to get the vendor string, rather than load it from
    a resource. The DLL also exports the OPC "standard"
    MatchPattern function for filtering the browsed strings.

4.  DAdvise did not return an error if the advise
    format was unknown.

5.  The code in the Modbus sample doesn't call MatchPattern
    if the filter string is empty. (Otherwise no strings
    match the filter.)

+++++++++++++++++++++++++++++++++++++++++++++++++++
+ Modifications for version 1.32 (March 4, 1998):
+++++++++++++++++++++++++++++++++++++++++++++++++++
1.  The Group supports and requires DAdvise for
    each data format (data change and write complete).
    ASync write calls are failed if the client has
    not advised the write complete format.

2.  Some files and build settings were missing from
    the OPCAuto automation project.

3.  Client handles are allowed to be duplicates.
    (some clients don't specify unique handles.)

+++++++++++++++++++++++++++++++++++++++++++++++++++
+ Modifications for version 1.31 (February 17, 1998):
+++++++++++++++++++++++++++++++++++++++++++++++++++
1.  Using the new OPC.H file from the Foundation
    (the quality defines are now included).

2.  Data is converted to the client requested type
    in the Read function as well as the data change.
    VariantChangeType is now used so it doesn't 
    throw exceptions.

3.  Browse now includes the OPC "standard" string
    matching function. 

4.  Servers support GetErrorString with OPC error
    descriptions in the string resources.

+++++++++++++++++++++++++++++++++++++++++++++++++++
+ Modifications for version 1.3 (September 23, 1997):
+++++++++++++++++++++++++++++++++++++++++++++++++++
1.  Base classes for OPCServer and OPCGroup separate
    application code from COM code.

2.  FSServer DLL is a reusable, simple to program
    module for developing stand-alone servers.

3.  Improved error checking.

4.  ValidateItems added.

+++++++++++++++++++++++++++++++++++++++++++++++++++
+ Modifications for version 1.22 (June 30, 1997):
+++++++++++++++++++++++++++++++++++++++++++++++++++
1.  The Automation interface has been improved to support
    removing items and groups, as well as some other functions.

+++++++++++++++++++++++++++++++++++++++++++++++++++
+ Modifications for version 1.21:
+++++++++++++++++++++++++++++++++++++++++++++++++++
1.  Project files are provided for both VC++ 4.2 and 5.0.
    Use the project file xxx4.mdp for VC++ 4.2.
    Use the project file xxx.dsw for VC++ 5.0.

2.  Free all BSTRs allocated with AllocSysString.

+++++++++++++++++++++++++++++++++++++++++++++++++++
+ Modifications for version 1.2:
+++++++++++++++++++++++++++++++++++++++++++++++++++
1.  Many type casts to remove warnings from VC++ 5.0.

2.  The default is free-threading, since both Windows NT 4.0 
    and Windows 95 with DCOM support it. The in-process sample
    only uses free threading.

3.  The string enumerator for browsing was renamed from CEnumString
    to CEnumItemIDs.

4.  AddGroup now checks for duplicate names and also generates a
    unique name if a null string is passed.

5.  AddGroup allows the deadband parameter to be optional.
    (a default value is used if none is specified.)

6.  GetItemID has been implemented in the Modbus server.

7.  CEnumItemIDs::Clone has been implemented.

8.  The IEnumOPCItemAttributes interface is now implemented as
    its own object, like an enumerator should be.

9.  OPCGroup::SetState allows null pointers for some optional
    parameters, and returns values for non-null pointers.

10. OPCGroup::SetName now checks for duplicate names.

11. OPCGroup::CloneGroup is now implemented.

12. OPCGroup::Read was not reading from the device, only the cache.

13. The functions OPCGroup::CreateDataTimeStream and
    OPCGroup::CreateDataStream now marshall strings and arrays
    into the stream.

14. CDoc::OnNewDocument and CDoc::OnOpenDocument turn off monitoring
    before changing the doc. This used to cause a crash.

15. CDoc::OnAddNewdevice handles new devices better.

16. When Free threaded, reference counting is done from the main thread.
    _Module used to call MFC's locking functions. When free-threaded,
    these were called from different threads, which breaks MFC. 
    Now a message is posted to the main window to call lock/unlock.

17. Deadband checking is now done in OPCGroup::DataThread.

18. OPCGroup::AddItems and CEnumItemAttributes::Next set dwAccessRights
    based on a tag's type.

19. OPCServer::AddGroup no longer checks for unique client handle.
    It is up to the client to ensure that its handles are unique.

20. OPCAuto used an STL (Standard Template Library) vector class.
    VC++ 5.0's namespaces, etc. caused problems, so a smaller version
    is included in the automation header file.
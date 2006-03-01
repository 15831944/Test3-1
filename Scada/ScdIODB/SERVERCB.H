//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  ServerCB.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//
//  Description:  Topic and Item classes
//
//**************************************************************************

#ifndef __SERVERCB_H__              // Only Include Once
#define __SERVERCB_H__

//*****************************************************************************

class OItem : public CObject
{
   DECLARE_DYNAMIC( OItem )
public:
      OItem( OTag* p, HICKITITEM id )
            : pTag(p), itemId(id), active(FALSE)
            {pTag->AddRef();}
      ~OItem()
            {pTag->Release();}
   OTag*       pTag;
   HICKITITEM  itemId;     // the handle the kit knows this topic by
   BOOL        active;
};

typedef CTypedPtrList<CObList, OItem*> OItemList;


//*****************************************************************************
class OTopic : public CObject
{
   DECLARE_DYNAMIC( OTopic )
public:
      OTopic( ODevice* p, HICKITTOPIC id )
            : pDevice(p), topicId(id)
            {pDevice->AddRef();}
      ~OTopic()
            {while( !items.IsEmpty() )
                delete items.RemoveTail();
             pDevice->Release(); }
   ODevice*    pDevice;
   HICKITTOPIC topicId;
   OItemList   items;
   OTagList    tags;       // ordered by address
};

typedef CTypedPtrList<CObList, OTopic*> OTopicList;

#endif
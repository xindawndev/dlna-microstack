#ifndef _DMS_CP_H_
#define _DMS_CP_H_

#include "MediaServerCP_ControlPoint.h"
#include "MediaServerControlPoint.h"
#include "CdsDidlSerializer.h"
#include "CdsObject.h"

static void * dmsinfos = NULL;
static int g_dms_flag = 0; 
char * resxml = NULL;

struct CdsObject * getCdsObj( char * objid )
{
    int ResultXmlLength = 0;
    struct ILibXMLNode *  root = NULL;
    int relIndex = 0;

    struct ILibXMLNode * objectNode = NULL;
    struct ILibXMLAttribute * attrs = NULL;
    struct CdsObject * object = NULL;
    struct CdsObject * obj = NULL;

    if ( !resxml ) return NULL;
    ResultXmlLength = ILibTrimString( &resxml, (int)strlen(resxml) );
    resxml[ResultXmlLength] = 0;
    if ( ResultXmlLength == 0 ) return NULL;

    root = ILibParseXML(resxml, 0, ResultXmlLength);
    if(root != NULL)
    {
        if(ILibProcessXMLNodeList(root) == 0)
        {
            relIndex = 0;
            objectNode = root->Next;
            ILibXML_BuildNamespaceLookupTable(root);
            while(objectNode != NULL)
            {
                if(objectNode->NameLength == 9 && strncmp(objectNode->Name, "container", 9) == 0)
                {
                    attrs = ILibGetXMLAttributes(objectNode);
                    if(attrs != NULL)
                    {
                        object = CDS_DeserializeDidlToObject(objectNode, attrs, 1, resxml, resxml + ResultXmlLength);
                        ILibDestructXMLAttributeList(attrs);
                        if ( !strcmp( object->ID, objid ) )
                        {
                            obj = object;
                            break;
                        }
                        object->User = (void*)(0 + relIndex);
                    }
                }

                objectNode = objectNode->Peer;
                relIndex++;
            }
        }
        ILibDestructXMLNodeList(root);
    }

    free( resxml );
    resxml = NULL;
    return obj;
}

struct CdsResource * getCdsRes( char * objid )
{
    int ResultXmlLength = 0;
    struct ILibXMLNode *  root = NULL;
    int relIndex = 0;

    struct ILibXMLNode * objectNode = NULL;
    struct ILibXMLAttribute * attrs = NULL;
    struct CdsObject * object = NULL;
    struct CdsResource * res = NULL;

    if ( !resxml ) return NULL;
    ResultXmlLength = ILibTrimString( &resxml, (int)strlen(resxml) );
    resxml[ResultXmlLength] = 0;
    if ( ResultXmlLength == 0 ) return NULL;

    root = ILibParseXML(resxml, 0, ResultXmlLength);
    if(root != NULL)
    {
        if(ILibProcessXMLNodeList(root) == 0)
        {
            relIndex = 0;
            objectNode = root->Next;
            ILibXML_BuildNamespaceLookupTable(root);
            while(objectNode != NULL)
            {
                if(objectNode->NameLength == 4 && strncmp(objectNode->Name, "item", 4) == 0)
                {
                    attrs = ILibGetXMLAttributes(objectNode);
                    if(attrs != NULL)
                    {
                        object = CDS_DeserializeDidlToObject(objectNode, attrs, 1, resxml, resxml + ResultXmlLength);
                        ILibDestructXMLAttributeList(attrs);
                        if ( !strcmp( object->ID, objid ) )
                        {
                            res = object->Res;
                            break;
                        }
                        object->User = (void*)(0 + relIndex);
                    }
                }

                objectNode = objectNode->Peer;
                relIndex++;
            }
        }
        ILibDestructXMLNodeList(root);
    }

    free( resxml );
    resxml = NULL;
    return res;
}

void processResults( char * ResultXml, int startIndex )
{
    int ResultXmlLength = 0;
    struct ILibXMLNode *  root = NULL;
    int relIndex = 0;

    void * linklistobjects = NULL;
    void * linklistnode = NULL;

    struct ILibXMLNode * objectNode = NULL;
    struct ILibXMLAttribute * attrs = NULL, * attrs1 = NULL;
    struct CdsObject * object = NULL, * object1 = NULL, * object2 = NULL;
    if ( !ResultXml ) return;

    ResultXmlLength = ILibTrimString( &ResultXml, (int)strlen(ResultXml) );
    ResultXml[ResultXmlLength] = 0;
    if ( ResultXmlLength == 0 ) return;

    root = ILibParseXML(ResultXml, 0, ResultXmlLength);
    if(root != NULL)
    {
        if(ILibProcessXMLNodeList(root) == 0)
        {
            relIndex = 0;
            linklistobjects = ILibLinkedList_Create();
            objectNode = root->Next;
            ILibXML_BuildNamespaceLookupTable(root);
            while(objectNode != NULL)
            {
                if(objectNode->NameLength == 4 && strncmp(objectNode->Name, "item", 4) == 0)
                {
                    attrs = ILibGetXMLAttributes(objectNode);
                    if(attrs != NULL)
                    {
                        object = CDS_DeserializeDidlToObject(objectNode, attrs, 1, ResultXml, ResultXml + ResultXmlLength);
                        ILibLinkedList_AddTail(linklistobjects, object);
                        ILibDestructXMLAttributeList(attrs);

                        object->User = (void*)(startIndex + relIndex);
                    }
                }
                else if(objectNode->NameLength == 9 && strncmp(objectNode->Name, "container", 9) == 0)
                {
                    attrs1 = ILibGetXMLAttributes(objectNode);
                    if(attrs1 != NULL)
                    {
                        object1 = CDS_DeserializeDidlToObject(objectNode, attrs1, 0, ResultXml, ResultXml + ResultXmlLength);
                        ILibLinkedList_AddTail(linklistobjects, object1);
                        ILibDestructXMLAttributeList(attrs1);
                    }
                }
                objectNode = objectNode->Peer;
                relIndex++;
            }

            linklistnode = ILibLinkedList_GetNode_Head(linklistobjects);

            while( linklistnode != NULL) 
            {
                object2 = ( struct CdsObject * )ILibLinkedList_GetDataFromNode( linklistnode );
                if( ( object2->MediaClass & CDS_MEDIACLASS_CONTAINER ) == CDS_MEDIACLASS_CONTAINER )
                {
                    printf( "----------------------------->[Container]<-----------------------------\n" );
                    printf( "\tID: %s ParentID: %s Title: %s\n", object2->ID, object2->ParentID, object2->Title );
                    CDS_ObjRef_Release(object2);
                }
                else if ( ( object2->MediaClass & CDS_MEDIACLASS_ITEM ) == CDS_MEDIACLASS_ITEM )
                {
                    printf( "----------------------------->[Item]<-----------------------------\n" );
                    printf( "\tID: %s ParentID: %s Title: %s\n", object2->ID, object2->ParentID, object2->Title );
                    printf( "\tValue: %s \n\tProtocolInfo: %s\n", object2->Res->Value, object2->Res->ProtocolInfo );
                    CDS_ObjRef_Release(object2);
                }
                linklistnode = ILibLinkedList_GetNextNode(linklistnode);
            }

            ILibLinkedList_Destroy(linklistobjects);
        }
        ILibDestructXMLNodeList(root);
    }
}

void On_Browse(struct UPnPService *sender,int ErrorCode,void *user,char* Result,unsigned int NumberReturned,unsigned int TotalMatches,unsigned int UpdateID)
{
    printf( "(%d)%s: ErrorCode: %d\n", __LINE__, __FUNCTION__, ErrorCode );

    if ( !ErrorCode )
    {
        printf( "NumberReturned: %d\n", NumberReturned );
        printf( "TotalMatches: %d\n", TotalMatches );
        printf( "UpdateID: %d\n", UpdateID );
        resxml = ( char * )malloc( strlen( Result ) );
        memcpy( resxml, Result, strlen( Result ) );
        processResults( Result, *((int*)(user)) );
    }
}

void On_Search(struct UPnPService *sender,int ErrorCode,void *user,char* Result,unsigned int NumberReturned,unsigned int TotalMatches,unsigned int UpdateID)
{
    printf( "(%d)%s: ErrorCode: %d\n", __LINE__, __FUNCTION__, ErrorCode );

    if ( !ErrorCode )
    {
        printf( "NumberReturned: %d\n", NumberReturned );
        printf( "TotalMatches: %d\n", TotalMatches );
        printf( "UpdateID: %d\n", UpdateID );
        processResults( Result, *((int*)(user)) );
    }
}

void PrintDmsList()
{
    char Key[128] = { 0 };
    int Len = 128, i = 0;
    void * Val = NULL;
    void * dms_enum = NULL;

    if ( dmsinfos == NULL ) return;
    ILibHashTree_Lock( dmsinfos );

    dms_enum = ILibHashTree_GetEnumerator( dmsinfos );
    if ( dms_enum == NULL ) return;
    while ( !ILibHashTree_MoveNext( dms_enum ) )
    {
        ILibHashTree_GetValue( dms_enum, &Key, &Len, ( ( void ** )( &Val ) ) );
        printf( "\t%d. %s [%s]\n", i++, (( struct UPnPDevice * )(Val))->FriendlyName, (( struct UPnPDevice * )(Val))->UDN );
    }
    ILibHashTree_DestroyEnumerator( dms_enum );

    ILibHashTree_UnLock( dmsinfos );
}

void * _getDmsDevice( int id )
{
    char Key[128] = { 0 };
    int Len = 128, i = 0, found = 0;
    void * Val = NULL;
    void * dms_enum = NULL;

    if ( dmsinfos == NULL ) return NULL;

    ILibHashTree_Lock( dmsinfos );
    dms_enum = ILibHashTree_GetEnumerator( dmsinfos );
    if ( dms_enum == NULL ) return NULL;

    while ( !ILibHashTree_MoveNext( dms_enum ) )
    {
        ILibHashTree_GetValue( dms_enum, &Key, &Len, ( ( void ** )( &Val ) ) );
        if ( id == i++ )
        {
            found = 1;
            break;
        }
    }

    ILibHashTree_DestroyEnumerator( dms_enum );

    ILibHashTree_UnLock( dmsinfos );
    return ( found != 0 ) ? Val : NULL;
}

void MCP_GetDLNADOC( int id )
{
    void * Val = _getDmsDevice( id );
    char * Ret = NULL;
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }

    Ret = MediaServerCP_GetCustomXML_X_DLNADOC( ( struct UPnPDevice * )( Val ) );
    printf( "%s\n", Ret ? Ret : "" );

}

void MCP_GetDLNACAP( int id )
{
    void * Val = _getDmsDevice( id );
    char * Ret = NULL;
    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }

    Ret = MediaServerCP_GetCustomXML_X_DLNACAP( ( struct UPnPDevice * )( Val ) );
    printf( "%s\n", Ret ? Ret : "" );
}

void MCP_Browse( int id, int startid, char * objid )
{
    void * Val = _getDmsDevice( id );
    struct UPnPService * Cds = NULL;

    if ( Val == NULL )
    {
        printf( "You choiced a invalid device!\n" );
        return;
    }

    Cds = MediaServerCP_GetService_ContentDirectory( ( struct UPnPDevice * )( Val ) );
    MediaServerCP_Invoke_ContentDirectory_Browse( Cds,
        On_Browse,
        (void *)(&startid),
        objid,
        "BrowseDirectChildren",
        "*", startid, 15, "" );
}

//void MCP_Search( int id, )
//{
//    void * Val = _getDmsDevice( id );
//    struct UPnPService * Cds = NULL;
//
//    if ( Val == NULL )
//    {
//        printf( "You choiced a invalid device!\n" );
//        return;
//    }
//    Cds = MediaServerCP_GetService_ContentDirectory( ( struct UPnPDevice * )( Val ) );
//
//    //MediaServerCP_Invoke_ContentDirectory_Search( Cds,
//    //    On_Search,
//    //    void* _user,
//    //    char* ContainerID,
//    //    char* SearchCriteria,
//    //    char* Filter,
//    //    unsigned int StartingIndex,
//    //    unsigned int RequestedCount,
//    //    char* SortCriteria);
//
//}

void OnAddMediaServer(struct UPnPDevice* mediaServerDevice)
{
    if ( !g_dms_flag )
    {
        dmsinfos = ILibInitHashTree();
        g_dms_flag++;// 需要加锁，这里暂时不考虑
    }
    if ( dmsinfos != NULL )
        ILibAddEntry( dmsinfos, mediaServerDevice->UDN, strlen( mediaServerDevice->UDN ), ( void * )mediaServerDevice );
}

void OnRemoveMediaServer(struct UPnPDevice* mediaServerDevice)
{
    void * dmrinfo;

    if ( !dmsinfos ) return;
    dmrinfo = ILibGetEntry( dmsinfos, mediaServerDevice->UDN, strlen( mediaServerDevice->UDN ) );
    ILibDeleteEntry( dmsinfos, mediaServerDevice->UDN, strlen( mediaServerDevice->UDN ) );
    if ( !( --g_dms_flag ) )// 需要加锁，这里暂时不考虑
    {
        ILibDestroyHashTree( dmsinfos );
        dmsinfos = NULL;
    }
}

#endif // _DMS_CP_H_

#if defined(WINSOCK2)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#elif defined(WINSOCK1)
    #include <winsock.h>
    #include <wininet.h>
#endif

#include "UploadController.h"
#include "MediaServerCP_ControlPoint.h"
#include "ILibParsers.h"
#include "UTF8Utils.h"
#include "CdsStrings.h"
#include "CdsDidlSerializer.h"
#include "DLNAProtocolInfo.h"
#include "FileIoAbstraction.h"

#ifndef _WIN32_WCE
    #include <assert.h>
    #define ASSERT(x) assert(x)
#endif

ILibWebClient_RequestManager    DMU_HttpClient;
// http client pool size
#define HTTP_CLIENT_POOL_SIZE    5
//#define MAX_FILE_PATH_LEN    1024


static void *ILib_Pool;

#define UNSUPPORTED printf("Action is not supported by this implementation.");

struct DMU_UploadSession
{
    int RefCount;

    ILibWebClient_RequestManager HttpClient;

    DMU_OnCreateObjectUploadResponse OnResponse;

    int PausedFlag;                                        //indicate if this transfer is paused by disconnecting

    int PausedByFlowControlFlag;                        //indicate if this transfer is paused using TCP flow control
            
    int ResumeFlag;                                        //indicate if this is a resumed transfer after aborting

    int BytesSent;                                        //indicate bytes sent on this transfer

    int TotalBytes;                                        //indicate total bytes needed to transfer
    
    DH_TransferStatus TransferStatus;                    //transfer status object

    char* ImportUri;                                    //importUri value of the <res> element

    char* UploadFromPath;                                //file path of the uploading file

    struct CdsObject *NewObj;                            //CDS object returned from CreateObject request

    void* UserObj;                                        //user object specified in application
};

struct DMU_DestroySession
{
    DMU_OnDestroyObjectResponse OnResponse;
    void* UserObj;                                        //user object specified in application
};

void DMU_AddRef(DMU_Session session)
{
    struct DMU_UploadSession* upload_session;
    if(session!=NULL)
    {
        upload_session = (struct DMU_UploadSession*) session;
        upload_session->RefCount++;
    }
}

void DMU_Release(DMU_Session session)
{
    struct DMU_UploadSession* upload_session;
    if(session!=NULL)
    {
        upload_session = (struct DMU_UploadSession*) session;
        upload_session->RefCount--;
        if(upload_session->RefCount<0)
        {
            upload_session->HttpClient = NULL;
            upload_session->OnResponse = NULL;
            upload_session->TransferStatus = NULL;
            CDS_ObjRef_Release(upload_session->NewObj);
            upload_session->NewObj = NULL;
            free(upload_session->ImportUri);
            free(upload_session->UploadFromPath);
            free(upload_session);
            session = NULL;
        }
    }
}

void OnHttpPostResponseDone(DH_TransferStatus transfer_status_handle, enum DHC_Errors dhc_error_code, int http_error_code, const char* error_message, void *user_obj)
{
    struct DMU_UploadSession* session;
    enum DMU_Errors errorCode;
    long actualBytesSent = -1;

    /* transfer aborted or finished */

    if(user_obj!=NULL)
    {
        session = (struct DMU_UploadSession*) user_obj;

        /* update transfer status */

        if(session->TransferStatus!=NULL)
        {
            DH_QueryTransferStatus(session->TransferStatus, &actualBytesSent, NULL, NULL, NULL);    
            if(actualBytesSent > 0)
            {

                if(session->ResumeFlag != 0)
                {
                    session->BytesSent += actualBytesSent;
                    /* clear TransferStatus */
                    session->TransferStatus = NULL;
                }
                else
                {
                    session->BytesSent = actualBytesSent;
                }
            }
        }

        switch(dhc_error_code)
        {
            case DHC_ERRORS_NONE:
                errorCode = DMU_ERROR_None;
                break;
            case DHC_ERRORS_CONNECTION_FAILED_OR_ABORTED:
                errorCode = DMU_ERROR_FailedOrAborted;    
                break;
            case DHC_ERRORS_HTTP:
                errorCode = DMU_ERROR_HttpError;    
                break;
            default:
                errorCode = DMU_ERROR_Unknown;
        }

        if(errorCode == DMU_ERROR_FailedOrAborted &&  session->PausedFlag!=0)
        {
            /* if this session is aborted because using Pause, then it is okay to resume */
            session->ResumeFlag = 0;
            session->TransferStatus = NULL;
            DMU_AddRef(session);
        }
        
        session->OnResponse(session, errorCode, http_error_code, session->NewObj, session->UserObj);
        DMU_Release(session);
    }
}


void DMUResponseSink_ContentDirectory_CreateObject(struct UPnPService* Service,int ErrorCode,void *User,char* ObjectID,char* Result)
{
    struct ILibXMLNode* nodeList;
    struct ILibXMLNode* node;
    struct ILibXMLAttribute *attribs;
    
    int resultLen;
    int parsePeerResult = 0;
    char *lastResultPos;

    struct CdsObject* oldObj=NULL;
    struct DMU_UploadSession* session=NULL;

    int isItemFlag = 0;
    struct CdsObject *newObj;
    struct DLNAProtocolInfo *pi = NULL;
    char* mimeType = NULL;
    char* contentFeatures = NULL;

    printf("MSCP Invoke Response: ContentDirectory/CreateObject(%s,%s)\r\n",ObjectID,Result);

    if(User!=NULL)
    {
        oldObj = (struct CdsObject*) User;
        session = (struct DMU_UploadSession*) oldObj->User;
    }

    if (session->OnResponse!= NULL)
    {

        if((ErrorCode == 0)&& (Result != NULL))
        {
            //parse the didl Result into a MSCP_Media Object, 
            newObj = NULL;
            resultLen = (int) strlen(Result);

            lastResultPos = Result + resultLen;
            nodeList = ILibParseXML(Result, 0, resultLen);
            ILibXML_BuildNamespaceLookupTable(nodeList);
            parsePeerResult = ILibProcessXMLNodeList(nodeList);

            if (parsePeerResult != 0)
            {
                session->OnResponse(session, DMU_ERROR_XmlNotWellFormed, 0, NULL, session->UserObj);
            }
            else if (resultLen == 0)
            {
                session->OnResponse(session, DMU_ERROR_XmlNotWellFormed, 0, NULL, session->UserObj);
            }
            else
            {
                node = nodeList;
                while (node != NULL)
                {
                    if (node->StartTag != 0)
                    {
                        /*[DONOTREPARSE] null terminate string */
                        attribs = ILibGetXMLAttributes(node);
                        node->Name[node->NameLength] = '\0';

                        /* IDF#3b: build CDS objects out of browse response */
                        newObj = NULL;
                        if (strcasecmp(node->Name, CDS_TAG_CONTAINER) == 0)
                        {
                            newObj = CDS_DeserializeDidlToObject(node, attribs, 0, Result, lastResultPos);
                        }
                        else if (strcasecmp(node->Name, CDS_TAG_ITEM) == 0)
                        {
                            newObj = CDS_DeserializeDidlToObject(node, attribs, 1, Result, lastResultPos);
                            isItemFlag = 1;
                        }
                        else if (strcasecmp(node->Name, CDS_TAG_DIDL) == 0)
                        {
                            /* this is didl-lite root node, go to first child */
                            node = node->Next;
                        }
                        else
                        {
                            /* this node is not supported, go to next sibling/peer */
                            if (node->Peer != NULL)
                            {
                                node = node->Peer;
                            }
                            else if(node->Parent!=NULL)
                            {
                                node = node->Parent->Peer;
                            }
                            else
                            {
                                node = NULL;
                        }
                        }

                        /* free attribute mappings */
                        ILibDestructXMLAttributeList(attribs);

                        /* IDF#3f: CDS object built... add it to list (refcount too!) */
                        if (newObj != NULL)
                        {
                            /* set reminder of which CDS provided this object */
                            MediaServerCP_AddRef((struct UPnPDevice*) Service->Parent);
                            newObj->CpInfo.Reserved.ServiceObject = Service;
                            break;
                        }
                    }
                    else
                    {
                        node = node->Next;
                    }
                }
            }

            /* free resources from XML parsing */
            ILibDestructXMLNodeList(nodeList);
            
            session->NewObj = newObj;
            /* newObj->Res could be empty, error checking handling needed. */
            if(isItemFlag !=0 && session->HttpClient != NULL)
            {
                if(isItemFlag != 0 && newObj->Res->ImportUri != NULL)
                {
                    // Created an item, now need to upload
                    if(newObj->Res != NULL && newObj->Res->ProtocolInfo != NULL)
                    {
                        pi = DLNAProtocolInfo_Parse(newObj->Res->ProtocolInfo,(int)strlen(newObj->Res->ProtocolInfo));
                        mimeType = ILibString_Copy(pi->MimeType, -1);
                        contentFeatures = ILibString_Copy(pi->ContentFeatures, -1);
                        DLNAProtocolInfo_Destruct(pi);
                    }

                    session->ImportUri = DecodeFromUTF8(newObj->Res->ImportUri);
                    session->TransferStatus = DHC_IssuePostRequestFromFile(session->HttpClient, ILib_Pool, session->ImportUri, session->UploadFromPath, 0, DH_TransferMode_Bulk, mimeType, contentFeatures, session, OnHttpPostResponseDone);
                    if(session->TransferStatus == NULL)
                    {
                        session->OnResponse(session, DMU_ERROR_Unknown, 0, session->NewObj, session->UserObj);
                        DMU_Release(session);
                    }
                }
                else if(isItemFlag !=0 && newObj->Res->ImportUri == NULL)
                {
                    session->OnResponse(session, DMU_ERROR_ImportUriNotAvailble, 0, session->NewObj, session->UserObj);
                    DMU_Release(session);    
                }
                else
                {
                    session->OnResponse(session, DMU_ERROR_None, 0, session->NewObj, session->UserObj);
                    DMU_Release(session);
                }
            }
            else
            {
                //
                // We wanted to create a container, but we got back an item. (Yes, we've seen this happen)
                //
                /* CreateObj failed, need to do an auto DestroyObject */
                if(ObjectID != NULL)
                {
                    MediaServerCP_Invoke_ContentDirectory_DestroyObject(Service, NULL, NULL, oldObj->ID);
                }
                session->OnResponse(session, DMU_ERROR_CreateObjectFailed, 0, NULL, session->UserObj);
                DMU_Release(session);
            }
        }
        else
        {
            /* CreateObj failed, need to do an auto DestroyObject */
            if(ObjectID != NULL)
            {
                MediaServerCP_Invoke_ContentDirectory_DestroyObject(Service, NULL, NULL, oldObj->ID);
            }

            session->OnResponse(session, DMU_ERROR_CreateObjectFailed, 0, NULL, session->UserObj);
            DMU_Release(session);
        }
        CDS_ObjRef_Release(oldObj);
    }
    else
    {
        UNSUPPORTED;
    }
}

void DMUResponseSink_ContentDirectory_DestroyObject(struct UPnPService* Service,int ErrorCode,void *User)
{
    struct DMU_DestroySession* session = NULL;

    printf("MSCP Invoke Response: ContentDirectory/DestroyObject()\r\n");

    if(User!=NULL)
    {
        session = (struct DMU_DestroySession*) User;
    }
    else //  Possible null pointer dereference. add by leochen
    {
        return;
    }

    if (session->OnResponse!= NULL)
    {
        session->OnResponse(ErrorCode, session->UserObj);
    }
    else
    {
        UNSUPPORTED;
    }

    if(session!=NULL)
    {
        free(session);
    }
}

void DMUResponseSink_ContentDirectory_Browse(struct UPnPService* Service,int ErrorCode,void *User,char* Result,unsigned int NumberReturned,unsigned int TotalMatches,unsigned int UpdateID)
{
    struct ILibXMLNode* nodeList;
    struct ILibXMLNode* node;
    struct ILibXMLAttribute *attribs;

    int error = 0;
    int resultLen;
    int parsePeerResult = 0;
    char *lastResultPos;

    struct DMU_UploadSession* session;
    int isItemFlag = 0;
    struct CdsObject *newObj;
    struct DLNAProtocolInfo *pi = NULL;
    char* mimeType = NULL;
    char* contentFeatures = NULL;

    printf("DMU Invoke Response: ContentDirectory/Browse(%s,%u,%u,%u)\r\n",Result,NumberReturned,TotalMatches,UpdateID);


    if(User==NULL)    return;

    session = (struct DMU_UploadSession*) User;

    if (session->OnResponse!= NULL)
    {

        if((ErrorCode == 0)&& (Result != NULL))
        {
            //parse the didl Result into a MSCP_Media Object, 
            newObj = NULL;
            resultLen = (int) strlen(Result);

            lastResultPos = Result + resultLen;
            nodeList = ILibParseXML(Result, 0, resultLen);
            ILibXML_BuildNamespaceLookupTable(nodeList);
            parsePeerResult = ILibProcessXMLNodeList(nodeList);

            if (parsePeerResult != 0)
            {
                error = (int) DMU_ERROR_XmlNotWellFormed;
            }
            else if (resultLen == 0)
            {
                error = (int) DMU_ERROR_XmlNotWellFormed;
            }
            else
            {
                node = nodeList;
                while (node != NULL)
                {
                    if (node->StartTag != 0)
                    {
                        /*[DONOTREPARSE] null terminate string */
                        attribs = ILibGetXMLAttributes(node);
                        node->Name[node->NameLength] = '\0';

                        /* IDF#3b: build CDS objects out of browse response */
                        newObj = NULL;
                        if (strcasecmp(node->Name, CDS_TAG_CONTAINER) == 0)
                        {
                            newObj = CDS_DeserializeDidlToObject(node, attribs, 0, Result, lastResultPos);
                        }
                        else if (strcasecmp(node->Name, CDS_TAG_ITEM) == 0)
                        {
                            newObj = CDS_DeserializeDidlToObject(node, attribs, 1, Result, lastResultPos);
                            isItemFlag = 1;
                        }
                        else if (strcasecmp(node->Name, CDS_TAG_DIDL) == 0)
                        {
                            /* this is didl-lite root node, go to first child */
                            node = node->Next;
                        }
                        else
                        {
                            /* this node is not supported, go to next sibling/peer */
                            if (node->Peer != NULL)
                            {
                                node = node->Peer;
                            }
                            else if(node->Parent!=NULL)
                            {
                                node = node->Parent->Peer;
                            }
                            else
                            {
                                node = NULL;
                            }
                        }

                        /* free attribute mappings */
                        ILibDestructXMLAttributeList(attribs);

                        /* IDF#3f: CDS object built... add it to list (refcount too!) */
                        if (newObj != NULL)
                        {
                            /* set reminder of which CDS provided this object */
                            MediaServerCP_AddRef((struct UPnPDevice*) Service->Parent);
                            newObj->CpInfo.Reserved.ServiceObject = Service;
                            break;
                        }
                    }
                    else
                    {
                        node = node->Next;
                    }
                }
            }

            /* free resources from XML parsing */
            ILibDestructXMLNodeList(nodeList);

            /* free previous object */
            if(session->NewObj != NULL)
            {
                CDS_ObjRef_Release(session->NewObj);
            }

            if(error == 0)
            {

                session->NewObj = newObj;

                if((isItemFlag != 0) && (newObj->Res->UploadedSize>0))
                {
                    // resumable, resume from the last uploaded byte

                    session->ResumeFlag = 1;
                    session->PausedFlag = 0;
                    session->BytesSent = newObj->Res->UploadedSize;

                    if((newObj->Res->ImportUri != NULL) && (strcmp(newObj->Res->ImportUri, session->ImportUri) ==0))
                    {
                        /* update new importUri if changed */
                        free(session->ImportUri);
                        session->ImportUri = (char*) malloc(strlen(newObj->Res->ImportUri) + 1);
                        strcpy(session->ImportUri, newObj->Res->ImportUri);
                    }

                    if(newObj->Res != NULL && newObj->Res->ProtocolInfo != NULL)
                    {
                        pi = DLNAProtocolInfo_Parse(newObj->Res->ProtocolInfo,(int)strlen(newObj->Res->ProtocolInfo));
                        mimeType = ILibString_Copy(pi->MimeType, -1);
                        contentFeatures = ILibString_Copy(pi->ContentFeatures, -1);
                        DLNAProtocolInfo_Destruct(pi);
                    }

                    if(newObj->Res->UploadedSize>0)
                    {
                        session->TransferStatus = DHC_IssuePostRequestFromFile(session->HttpClient, ILib_Pool, session->ImportUri, session->UploadFromPath, newObj->Res->UploadedSize, DH_TransferMode_Bulk, mimeType, contentFeatures, session, OnHttpPostResponseDone);
                    }
                    else
                    {
                        session->TransferStatus = DHC_IssuePostRequestFromFile(session->HttpClient, ILib_Pool, session->ImportUri, session->UploadFromPath, 0, DH_TransferMode_Bulk, mimeType, contentFeatures, session, OnHttpPostResponseDone);
                    }
                    if(session->TransferStatus == NULL)
                    {
                        session->OnResponse(session, DMU_ERROR_ResumeOrRetryFailed, 0, session->NewObj, session->UserObj);
                        DMU_Release(session);
                    }
                    else
                    {
                        session->OnResponse(session, DMU_ERROR_RESUMED, 0, session->NewObj, session->UserObj);
                    }
                }
                else
                {
                    session->OnResponse(session, DMU_ERROR_ResumeOrRetryFailed, 0, session->NewObj, session->UserObj);
                    DMU_Release(session);
                }
            }
            else
            {
                /* error occured parsing XML */
                session->OnResponse(session, error, 0, NULL, session->UserObj);
                DMU_Release(session);
            }
        }
        else
        {
            session->OnResponse(session, DMU_ERROR_CreateObjectFailed, 0, NULL, session->UserObj);
            DMU_Release(session);
        }
    }
    else
    {
        UNSUPPORTED;
    }
}

void DMU_Invoke_Browse(void *serviceObj, struct CdsObject *obj)
{

    /* issue a BrowseMetadata request on the Cds object to find out the res@dlna:uploadedSize */
    if(serviceObj != NULL && obj != NULL)
    {
        MediaServerCP_Invoke_ContentDirectory_Browse
            (
            serviceObj, 
            DMUResponseSink_ContentDirectory_Browse, 
            obj->User, 
            obj->ID, 
            CDS_STRING_BROWSE_METADATA, 
            CDS_STRING_ALL_FILTER,
            0, 
            0, 
            ""
            );
    }
}

/*
 *    Call this method to perform a createobject request.
 *
 *    serviceObj        : the CDS service object for the MediaServer
 *    args            : the arguments of the browse request.
 */
void DMU_Invoke_CreateObject(void *serviceObj, struct CdsObject *obj)
{

    char *didl;
    int didlLen = 0;
    unsigned int filter = 0;

    struct DMU_UploadSession *session = NULL;

    if(serviceObj == NULL || obj == NULL) return;

    session = (struct DMU_UploadSession*) obj->User;

    /*
        *    Get the XML form and send it.
        */
    /* IDF#7a: convert CDS object to DIDL-Lite */
    // we want to print all Res elements and DLNA-specific attributes
    filter |= CdsFilter_ResAllAttribs;
    filter |= CdsFilter_DlnaContainerType;
    filter |= CdsFilter_DlnaManaged;
    filter |= CdsFilter_CreateClass;
    filter |= CdsFilter_TakeOut;

    didl = CDS_SerializeObjectToDidl(obj, 0, filter, 1, &didlLen);
    if (didlLen > 0)
    {
        MediaServerCP_Invoke_ContentDirectory_CreateObject
            (
            serviceObj,
            DMUResponseSink_ContentDirectory_CreateObject,
            obj,
            obj->ParentID,
            didl
            );
        free(didl);
    }
    else
    {
        /* print an error message if we couldn't get the DIDL. */
        fprintf(stderr, "DMU_Invoke_CreateObject() failed to serialize object %s. Reason=%d.\r\n", obj->ID, didlLen);
    }
}

/*
 *    Call this method to perform a destroyobject request.
 *
 *    serviceObj        : the CDS service object for the MediaServer
 *    args            : the arguments of the browse request.
 */
void DMU_Invoke_DestroyObject(void *serviceObj, struct CdsObject *obj)
{
    if(serviceObj != NULL && obj != NULL)
    {
        MediaServerCP_Invoke_ContentDirectory_DestroyObject
            (
            serviceObj,
            DMUResponseSink_ContentDirectory_DestroyObject,
            obj->User,
            obj->ID
            );
    }
}

void DMU_Initialize(void* chain, void* thread_pool)
{
    ASSERT(chain != NULL && thread_pool!=NULL);

    ILib_Pool = thread_pool;

    //Create a web client to do HTTP posts
    DMU_HttpClient = ILibCreateWebClient(HTTP_CLIENT_POOL_SIZE,chain);

}

void DMU_Shutdown(void* chain)
{
    ASSERT(chain != NULL);

    if(DMU_HttpClient != NULL)
    {
        ILibChain_SafeRemove(chain, DMU_HttpClient);
    }
}

DMU_Session DMU_CreateObjectAndUpload(int any_container_flag, int resume_flag, const struct CdsObject* parent_container_or_dms, struct CdsObject* new_object, char* file_path, DMU_OnCreateObjectUploadResponse callback_response, void* user_obj)
{
    struct DMU_UploadSession *session = NULL;

    if (any_container_flag != 0 || parent_container_or_dms->CpInfo.Reserved.ServiceObject != NULL)
    {
        if(new_object == NULL || new_object->Title == NULL)
        {
            return NULL;
        }

        // ref count on the new object to make sure it doesn't get deleted until the response comes back,
        // because it will be passed as an USER object into the CreateObject() calls
        CDS_ObjRef_Add(new_object);

        session = (struct DMU_UploadSession*) malloc(sizeof(struct DMU_UploadSession));
        memset(session,0,sizeof(struct DMU_UploadSession));

        if(DMU_HttpClient == NULL)
        {
            // Report error, HttpClient is 
            free(session);// maybe memory leak. add by leochen
            return NULL;
        }
        session->OnResponse = callback_response;
        session->UserObj = user_obj;

        if(new_object->ID != NULL && new_object->DeallocateThese & CDS_ALLOC_ID)
        {
            /* obj Id has to be an empty value,
                the server is going to respond with the correct obj Id anyway.
            */
            free(new_object->ID);
        }

        new_object->ID = "";

        if((new_object->MediaClass & CDS_CLASS_MASK_OBJECT_TYPE) ==  CDS_CLASS_MASK_ITEM)
        {
            if(
                (new_object->Res != NULL) &&
                (new_object->Res->ProtocolInfo != NULL) &&
                (ILibFileDir_GetType(file_path) == ILibFileDir_Type_FILE)
                )
            {
                session->TotalBytes = ILibFileDir_GetFileSize(file_path);
                session->HttpClient = DMU_HttpClient;
                session->UploadFromPath = (char*) malloc(strlen(file_path) + 1);
                strcpy(session->UploadFromPath, file_path);

                if(new_object->Res == NULL)
                {
                    new_object->Res = CDS_AllocateResource();
                }
                if(resume_flag)
                {
                    new_object->Res->ResumeUpload = 1;
                }
            }
            else
            {
                /* missing protocolInfo or this is not a file */
                free(session);
                return NULL;
            }
        }

        // if uploading using AnyContainer, then change the parentId of the CdsObject to use "DLNA.ORG_AnyContainer"
        if(any_container_flag == 1)
        {

            if ((new_object->DeallocateThese & CDS_ALLOC_ParentID)    && (new_object->ParentID != NULL))
            {
                // free previous parentId
                free(new_object->ParentID);
            }
            new_object->ParentID = CDS_STRING_DLNA_ANYCONTAINER;
        }
        else
        {
            new_object->ParentID = parent_container_or_dms->ID;
        }    

        new_object->User = session;

        DMU_Invoke_CreateObject(parent_container_or_dms->CpInfo.Reserved.ServiceObject, new_object);

        return session;
    }

    return NULL;
}


void DMU_DestroyObject(struct CdsObject *obj, DMU_OnDestroyObjectResponse callback_response, void* user_obj)
{

    struct DMU_DestroySession *session = NULL;

    if (obj->CpInfo.Reserved.ServiceObject != NULL)
    {

        session = (struct DMU_DestroySession*) malloc(sizeof(struct DMU_DestroySession));
        memset(session,0,sizeof(struct DMU_DestroySession));
        session->OnResponse = callback_response;
        session->UserObj = user_obj;

        obj->User = session;

        DMU_Invoke_DestroyObject(obj->CpInfo.Reserved.ServiceObject, obj);

    }

}

int DMU_IsResumeSupported(DMU_Session session)
{
    struct DMU_UploadSession* upload_session = (struct DMU_UploadSession*) session;
    int retVal = 0;

    if((upload_session != NULL) && (upload_session->NewObj != NULL) && (upload_session->NewObj->Res != NULL))
    {
        retVal = upload_session->NewObj->Res->ResumeUpload;
    }

    return retVal;
}

int    DMU_GetBytesSent(DMU_Session session)
{
    struct DMU_UploadSession* upload_session = (struct DMU_UploadSession*) session;
    long bytesSent = 0;
    long actualBytesSent = -1;

    if(upload_session != NULL)
    {
        if(upload_session->ResumeFlag != 0 || upload_session->PausedFlag !=0)
        {
            /* this is a resumed session */
            bytesSent = upload_session->BytesSent;
        }

        if(upload_session->TransferStatus != NULL)
        {

            DH_QueryTransferStatus(upload_session->TransferStatus, &actualBytesSent, NULL, NULL, NULL);
            if(actualBytesSent > 0)
            {    
                bytesSent += actualBytesSent;
            }
        }
    }
    return bytesSent;
}

int DMU_GetTotalBytes(DMU_Session session)
{
    struct DMU_UploadSession* upload_session = (struct DMU_UploadSession*) session;

    int totalBytes = -1;

    if(upload_session != NULL)
    {
        totalBytes = upload_session->TotalBytes;
    }

    return totalBytes;
}

int DMU_IsPaused(DMU_Session session)
{
    struct DMU_UploadSession* upload_session = (struct DMU_UploadSession*) session;
    int retVal = 0;

    if(upload_session != NULL)
    {
        retVal = upload_session->PausedFlag;;
    }

    return retVal;
}

int DMU_IsPausedByFlowControl(DMU_Session session)
{
    struct DMU_UploadSession* upload_session = (struct DMU_UploadSession*) session;
    int retVal = 0;

    if(upload_session != NULL)
    {
        retVal = upload_session->PausedByFlowControlFlag;
    }

    return retVal;
}

int    DMU_PauseTransfer(DMU_Session session)
{
    struct DMU_UploadSession* upload_session = (struct DMU_UploadSession*) session;
    int retVal = -1;

    if((upload_session != NULL) && (upload_session->PausedFlag == 0) && upload_session->PausedByFlowControlFlag==0)
    {
        retVal = DMU_AbortTransfer(session);
        
        if(retVal == 0)
        {
            /* aborted successfully */

            upload_session->PausedFlag = 1;
        }
    }
    return retVal;
}

int DMU_PauseTransferByFlowControl(DMU_Session session)
{
    struct DMU_UploadSession* upload_session = (struct DMU_UploadSession*) session;
    long actualBytesSent = -1;

    if((upload_session != NULL) && (upload_session->PausedFlag == 0) && (upload_session->PausedByFlowControlFlag==0))
    {
        /* this session was not paused before or aborted */

        if(upload_session->TransferStatus!=NULL)
        {
            DH_QueryTransferStatus(upload_session->TransferStatus, &actualBytesSent, NULL, NULL, NULL);

            if((actualBytesSent > 0) && (actualBytesSent < upload_session->TotalBytes))
            {
                /* pause using TCP flow control */
                DHC_Pause(upload_session->TransferStatus);
                upload_session->PausedByFlowControlFlag = 1;
                return 0;
            }
        }

    }
    return -1;
}

int DMU_ResumeTransfer(DMU_Session session)
{
    struct DMU_UploadSession* upload_session = (struct DMU_UploadSession*) session;

    /* make sure connection is not paused using TCP flow control */
    if((upload_session != NULL) && (upload_session->PausedByFlowControlFlag==0))
    {
        /*
            * resume supported on server
            * use byte range request to resume upload
            */

        /* this session was aborted, add ref to the session because it will be released by the callback */
        if(upload_session->PausedFlag==0)
        {
            DMU_AddRef(session);
        }

        upload_session->NewObj->User = upload_session;
        DMU_Invoke_Browse(upload_session->NewObj->CpInfo.Reserved.ServiceObject, upload_session->NewObj);

        return 0;
    }
    return -1;
}

int DMU_ResumeTransferByFlowControl(DMU_Session session)
{
    struct DMU_UploadSession* upload_session = (struct DMU_UploadSession*) session;
    long actualBytesSent = -1;

    if((upload_session != NULL) && (upload_session->PausedFlag == 0) && (upload_session->PausedByFlowControlFlag==1))
    {
        /* this session was paused using TCP flow control */

        if(upload_session->TransferStatus!=NULL)
        {
            DH_QueryTransferStatus(upload_session->TransferStatus, &actualBytesSent, NULL, NULL, NULL);

            if((actualBytesSent > 0) && (actualBytesSent < upload_session->TotalBytes))
            {
                /* pause using TCP flow control */
                DHC_Resume(upload_session->TransferStatus);
                upload_session->PausedByFlowControlFlag = 0;
                return 0;
            }
        }

    }
    return -1;
}

int DMU_AbortTransfer(DMU_Session session)
{
    struct DMU_UploadSession* upload_session = (struct DMU_UploadSession*) session;
    long actualBytesSent = -1;

    if(upload_session != NULL)
    {
        if(upload_session->PausedFlag!=0)
        {
            upload_session->PausedFlag = 0;
            /* this transfer was paused by disconnecting, therefore the connection is already closed */
            upload_session->OnResponse(upload_session, DMU_ERROR_FailedOrAborted, 0, upload_session->NewObj, upload_session->UserObj);
            DMU_Release(session);
            return 0;
        }
        else
        {
            if(upload_session->TransferStatus==NULL)
            {
                return -1;
            }
            DH_QueryTransferStatus(upload_session->TransferStatus, &actualBytesSent, NULL, NULL, NULL);

            if((actualBytesSent > 0) && (actualBytesSent < upload_session->TotalBytes))
            {
                DH_AbortTransfer(upload_session->TransferStatus);
                return 0;
            }
            else if(actualBytesSent <= 0)
            {
                ILibWebClient_CancelRequest(upload_session->TransferStatus->RequestToken);
                return 0;
            }
        }
    }
    return -1;
}

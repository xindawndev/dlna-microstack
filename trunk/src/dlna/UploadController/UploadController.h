/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2002 - 2006 Intel Corporation.  All rights reserved.
 * 
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors.  Title to the
 * Material remains with Intel Corporation or its suppliers and
 * licensors.  The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and
 * licensors. The Material is protected by worldwide copyright and
 * trade secret laws and treaty provisions.  No part of the Material
 * may be used, copied, reproduced, modified, published, uploaded,
 * posted, transmitted, distributed, or disclosed in any way without
 * Intel's prior express written permission.
 
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly, by
 * implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 * 
 * $Workfile: UploadController.h
 *
 *
 *
 */

#ifndef __UPLOAD_CONTROLLER_H__
#define __UPLOAD_CONTROLLER_H__

#include "CdsObject.h"
#include "DlnaHttpClient.h"
/*! \file UploadController.h 
	\brief This is the DMU component for upload capabilities on CDS servers and
	provides methods for useful for creating and destroying CDS objects on a DMS.
*/

/*! \defgroup UploadController DLNA - Upload Controller
	\brief 
	This module provides the functionality for uploading the 
	\ref CdsObject with valid \ref CdsResource to the DMS. 

	\{
*/

/*!	\brief The session object created from \ref DMU_CreateObjectAndUpload.
 */
typedef void* DMU_Session;

/*!	\brief Enumeration of common DMU error codes.
 */
enum DMU_Errors
{
	/*!	\brief No errors.
	*/
	DMU_ERROR_None					= 0,
	/*!	\brief Transfer aborted or failed.
	*/
	DMU_ERROR_FailedOrAborted		= 1,
	/*!	\brief Resume or retry transfer failed.
	*/
	DMU_ERROR_ResumeOrRetryFailed	= 2,
	/*!	\brief Server for this container is no longer valid.
	*/
	DMU_ERROR_ServerNotExist		= 3,
	/*!	\brief Http errors.
	*/
	DMU_ERROR_HttpError				= 4,
	/*!	\brief Specified container change is not allowed.
	*/
	DMU_ERROR_InvalidContext		= 5,
	/*!	\brief XML returned not well formed.
	*/
	DMU_ERROR_XmlNotWellFormed		= 6,
	/*!	\brief Failed to create object.
	*/
	DMU_ERROR_CreateObjectFailed	= 7,
	/*!	\brief IimportUri is not returned in the response.
	*/
	DMU_ERROR_ImportUriNotAvailble  = 8,
	/*!	\brief Unknowne error.
	*/
	DMU_ERROR_Unknown				= 9,
    DMU_ERROR_RESUMED               = 10,
	DMU_ERROR_FileNotExist			= 11
};

/*!	\brief Initialize the DMU on the UPnP chain. 

	\param[in] chain The UPnP chain of the microstack.
	\param[in] thread_pool The ThreadPool for doing disk I/O processing.

*/
void DMU_Initialize(void* chain, void* thread_pool);

/*! \brief Method signature for handling a completed CDS:CreateObject request and finsihed uploading to the DMS if neccessary.
	\warning both the session object and CDS result returned in this callback will be destroyed and become invalid
	after this callback finishes executing.  Do not reference these values after then.  If You need to resume
	upload on aborted transfers, make sure you call \ref DMU_ResumeTransfer within this callback.  Do not call
	\ref DMU_ResumeTransferByFlowControl, which is invalid for aborted transfers.

	\param[in] session The upload session initiated from\ref DMU_CreateObjectAndUpload.
	\param[in] error_code Error returned for the upload request indicated in \ref DMU_Errors.
	\param[in] http_error_code This is http error code that represents the response of the HTTP POST request
	a sucessful upload would return the 200 http reponse.
	\param[in] dhs_error_code This is the type of error that was encountered. 
	\param[in] result The CDS object that was created on the DMS Server.
	\param[in] user_obj	The user object specified in \ref DMU_CreateObjectAndUpload method.

	\note The meta data of CDS object returned from the DMS server could be different than the CDS object you try to upload, such as the parent id.
*/
typedef void (*DMU_OnCreateObjectUploadResponse)(DMU_Session session, enum DMU_Errors error_code, int http_error_code, struct CdsObject *result, void* user_obj);

/*! \brief Method signature for handling a completed CDS:DestroyObject request.
	\param[in] DMU_Errors error_code Error returned for the upload request indicated in \ref DMU_Errors.
	\param[in] user_obj	The user object specified in \ref DMU_DestroyObject method.

*/
typedef void (*DMU_OnDestroyObjectResponse)(enum DMU_Errors error_code, void* user_obj);

/*!	\brief Allows the application to Create a CDS Object on the DMS and upload to it.

	\param[in] any_container_flag Non-zero value to indicate that the CDS object is going to be created with <b>DLNA.ORG_AnyContainer</b> attribute.
	\param[in] resume_flag Non-zero value to indicate that the CDS object is going to be created with with resume support.
	\param[in] parent_container_or_dms The parent CDS object where the new CDS object is going to be created.

	\warning if <b>any_container_flag</b> is set to 0, then this parameter cannot be NULL.
	\param[in] new_object The new CDS object you are trying to create on the DMS.

	\warning This parameter cannot be NULL.
	\param[in] file_path The local file that needs to be uploaded to the DMS for the created CDS Item.
	
	\warning This parameter cannot be NULL if the CDS object you created is a CDS Item, and the upload destination 
	is the retured importUri attribute from the <b>CreateObject</b> request.

	\param[in] callback_response The callback to execute when the CreateObject request completes or an error occurs. 
	\param[in] user_obj	The user object specified in the application which is passed back on the \ref DMU_OnCreateObjectUploadResponse.

	\returns The \ref DMU_Session instance.	
*/
DMU_Session DMU_CreateObjectAndUpload(int any_container_flag, int resume_flag, const struct CdsObject* parent_container_or_dms, struct CdsObject* new_object, char* file_path, DMU_OnCreateObjectUploadResponse callback_response, void* user_obj);

/*!	\brief Allows the application to Destroy a CDS Object on the DMS.

	\param[in] obj The CDS object you are trying to destroy on the DMS.
	\warning This parameter cannot be NULL.
	\param[in] callback_response The callback to execute when the DestroyObject request completes or an error occurs. 
	\param[in] user_obj	The user object specified in the application which is passed back on the response.

	\returns The \ref DMU_Session object.
*/
void DMU_DestroyObject(struct CdsObject *obj, DMU_OnDestroyObjectResponse callback_response, void* user_obj);

/*!	\brief Allows the application to check whether the server supports resume failed upload content request using
	CONTENT-RANGE requests.
	\param[in] session The upload session initiated from\ref DMU_CreateObjectAndUpload.

	\returns The 1 if resume is supported, 0 otherwise.
*/
int DMU_IsResumeSupported(DMU_Session session);

/*!	\brief Allows the application to get the current uploaded progress in the number of bytes sent.
	\param[in] session The upload session initiated from\ref DMU_CreateObjectAndUpload.

	\returns The number of bytes uploaded.
*/
int DMU_GetBytesSent(DMU_Session session);

/*!	\brief Allows the application to get the total number of bytes needed to be uploaded.
	\param[in] session The upload session initiated from\ref DMU_CreateObjectAndUpload.

	\returns The number of total bytes uploaded, which is the file size.
*/
int DMU_GetTotalBytes(DMU_Session session);

/*!	\brief Find out if the upload session is paused by disconnecting.  Which means the client will disconnect
	from the server and have to resume only using \ref DMU_ResumeTransfer function.
	\param[in] session The upload session initiated from\ref DMU_CreateObjectAndUpload.

	\returns 1 if session is paused by disconnecting, 0 otherwise.
*/
int DMU_IsPaused(DMU_Session session);

/*!	\brief Find out if the upload session is paused using TCP flow control, which it will keep the connection
	to the server but will not send any bytes by stalling.  The client can only resume using \ref
	DMU_ResumeTransferByFlowControl function.
	\param[in] session The upload session initiated from\ref DMU_CreateObjectAndUpload.

	\returns 1 if session is paused using TCP flow control, 0 otherwise.
*/
int DMU_IsPausedByFlowControl(DMU_Session session);

/*!	\brief Pause upload session by disconnecting and resume using CONTENT-RANGE.  Which means the client will disconnect
	from the server and have to resume only using \ref DMU_ResumeTransfer function.
	\warning Do not use \ref DMU_ResumeTransferByFlowControl to resume the connection. Only use
	DMU_ResumeTransfer which will issue a browse request to find out the uploaded size from the server,
	and then resume from the byte position last uploaded.
	\warning This function will only work if the server supports resume capabiliity, which the 
	resumeUpload value must be 1.

	\param[in] session The upload session initiated from\ref DMU_CreateObjectAndUpload.

	\returns A non-zero value if successful.
*/
int	DMU_PauseTransfer(DMU_Session session);

/*!	\brief Pause upload session using TCP flow control.  which it will keep the connection
	to the server but will not send any bytes.
	\warning Do not use \ref DMU_ResumeTransfer to resume the connection. Only use
	DMU_ResumeTransferByFlowControl otherwise you'll be created a new connection to the server. Must resume
	within 5 mins, or the Server could terminiate the connection.

	\param[in] session The upload session initiated from\ref DMU_CreateObjectAndUpload.

	\returns A non-zero value if successful.
*/
int DMU_PauseTransferByFlowControl(DMU_Session session);

/*!	\brief Allows the application to resume the upload session from the last uploaded byte position.  It will issue
	a BrowseMetaData request to the server to find out the uploadedSize value and create a new HTTP-POST
	request to the server using CONTENT-RANGE from the last byte position.
	\warning If the connection is paused using \ref DMU_PauseTransferByFlowControl, then do not use method to resume,
	resume.

	\param[in] session The upload session initiated from\ref DMU_CreateObjectAndUpload.

	\returns A non-zero value if successful.
*/
int DMU_ResumeTransfer(DMU_Session session);

/*!	\brief Allows the application to resume the upload session using TCP flow control. which it will 
	resume the current connection to the server.
	\warning If the connection is paused using \ref DMU_PauseTransfer, then do not use method to resume,
	because the connection is no longer valid after disconnecting.

	\param[in] session The upload session initiated from\ref DMU_CreateObjectAndUpload.

	\returns A non-zero value if successful.
*/
int DMU_ResumeTransferByFlowControl(DMU_Session session);
/*!	\brief Allows the application to abort the transfer progress of the upload session.  This will close
	the connection on the server.  The \ref DMU_OnCreateObjectUploadResponse callback will be called to notify
	the application that the transfer aborted.
	\warning If the session is paused using \ref DMU_PauseTransfer, you still need to call this function
	to abort the transfer, although the connection is already aborted during pause, this will make sure
	that session object is destroyed and the callback is executed.

	\param[in] session The upload session initiated from\ref DMU_CreateObjectAndUpload.

	\returns A non-zero value if successful.
*/
int DMU_AbortTransfer(DMU_Session session);

void DMU_Shutdown(void* chain);
/*! \} */

#endif
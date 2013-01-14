#ifndef _PLAYSINGLEURI_H_
#define _PLAYSINGLEURI_H_

#include "DMRCommon.h"
#include "CdsObject.h"
/**
    \file PlaySingleUri.h
    \ingroup DLNADMR
    \brief DLNA DMR PlaySingleUri API

    This file provides a API to resolve a playsingle URI to a struct CdsObject*
    that contains a list of resources that the caller can pick from to get
    a real media URI.

    \addtogroup DLNADMR DLNA MediaRenderer Microstack (DMR)
    \brief This module defines the component to resolve PlaySingleUris.
    \{
*/


/** \brief This method will take a resource that specified a PlaySingleUri
    and return the same resource with the resolved URI for the media.
    \param MSCPToken [in] This is the token returned from the
        MediaServerCP_CreateControlPoint method in the media server control
        point code.
    \param playSingleUri [in] This is the playSingle Uri as specified in the DLNA spec.
    \returns The struct CdsObject* for the resolved PlaySingle Uri of the if this
        function succeeds or NULL if the function fails.  The caller must pick the
        appropriate URI from the resources in the struct CdsObject*.

    \warning This is a syncronous method that does nework communications.  It
        may take quite a few seconds to execute.
*/
struct CdsObject* ResolvePlaySingleURI(void* MSCPToken, char* playSingleUri);

/* \} */
#endif /* __PLAYSINGLEURI_H__ */

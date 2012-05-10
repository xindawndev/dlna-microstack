
#
# PPTV's UPnP MicroStack/1.0, Sample MakeFile */
# PPTV Device Builder - Build#1.0.2777
#

# List all object files here
OFILES = \
	CdsDidlSerializer.o \
CdsErrors.o \
CdsMediaClass.o \
CdsObject.o \
DlnaHttp.o \
DlnaHttpClient.o \
DlnaHttpServer.o \
MicroAVRCP.o \
FileIoAbstraction.o \
DLNAProtocolInfo.o \
MimeTypes.o \
UTF8Utils.o \
DownloadController.o \
UploadController.o \
PushController.o \
FilteringBrowser.o \
MediaServerControlPoint.o \
BitArray.o \
CircularBuffer.o \
IndexBlocks.o \
PlayListManager.o \
PlaylistManagerPC.o \
PlaylistManagerS.o \
DMR.o \
PlaySingleUri.o \
MediaServerAbstraction.o \
DmsIntegration.o \
FileSystemEnumerator.o \
Main.o \
	ILibParsers.o\
	ILibAsyncSocket.o\
	ILibAsyncUDPSocket.o\
	ILibAsyncServerSocket.o\
	ILibWebClient.o\
	ILibWebServer.o\
	ILibThreadPool.o\
	DMRCP_ControlPoint.o\
DMR_MicroStack.o\
MediaServerCP_ControlPoint.o\
MediaServer_MicroStack.o\
ILibSSDPClient.o\


# Compiler command name
CC = gcc

# Compiler flags applied to all files
CFLAGS = -g -I./ -I../  -I./CdsObjects  -I./HttpFiles  -I./MediaRendererCP  -I./FileAbstractionFiles  -I./ProtocolInfoParser  -I./StringUtils  -I./DownloadController  -I./UploadController  -I./PushController  -I./MediaServerBrowser  -I./PlaylistTrackManager  -I./MediaRenderer  -I./PlaySingleUri  -I./MediaServerAbstraction  -I./DmsIntegration  -I./DmsIntegration/FileSystemEnumerator -DINCLUDE_FEATURE_UPLOAD -DINCLUDE_FEATURE_PLAYSINGLEURI -DINCLUDE_FEATURE_DISPLAY -DINCLUDE_FEATURE_VOLUME -DMICROSTACK_NO_STDAFX -DMSCP -DMICROSTACK_NO_STDAFX -Os -Wall -D_POSIX -D_DEBUG -D_VERBOSE 
 
LIBS = -lpthread -lm 
# Name of the executable
VPATH =  ./ ../  ./CdsObjects  ./HttpFiles  ./MediaRendererCP  ./FileAbstractionFiles  ./ProtocolInfoParser  ./StringUtils  ./DownloadController  ./UploadController  ./PushController  ./MediaServerBrowser  ./PlaylistTrackManager  ./MediaRenderer  ./PlaySingleUri  ./MediaServerAbstraction  ./DmsIntegration  ./DmsIntegration/FileSystemEnumerator 
EXENAME = Main

# Builds all object files and executable
${EXENAME} : $(OFILES)
	$(CC) $(CFLAGS) -o ${EXENAME} $(OFILES) $(LIBS)

# Macro rule for all object files.
$(OFILES) : \
	DMRCP_ControlPoint.h\
MediaServerCP_ControlPoint.h\
MediaServerCP_ControlPoint.h\
DMR_MicroStack.h\
MediaServerCP_ControlPoint.h\
MediaServer_MicroStack.h\
ILibSSDPClient.h\


# Clean up
clean :
	rm -f $(OFILES) ${EXENAME} ${EXENAME}.exe


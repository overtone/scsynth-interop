#include "scsynth-interop.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <memory.h>

#ifdef SC_WIN32
#include <pthread.h>
#include <winsock2.h>
#else
#include <sys/wait.h>
#endif

#include <SC_WorldOptions.h>
#include <SC_SndBuf.h>


/************************************************************************/
// init / cleanup
/************************************************************************/

int scsynth_interop_init()
{
#ifdef SC_WIN32
#ifdef SC_WIN32_STATIC_PTHREADS
  // initialize statically linked pthreads library
  pthread_win32_process_attach_np();
#endif

  // initialize winsock
  WSAData wsaData;
  int nCode;
  if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) {
    scprintf( "WSAStartup() failed with error code %d.\n", nCode );
    return -1;
  }
#else // SC_WIN32

  //catch_crashes();

#endif
  return 0;
}

void scsynth_interop_cleanup()
{
#ifdef SC_WIN32
  // clean up winsock
  WSACleanup();

#ifdef SC_WIN32_STATIC_PTHREADS
  // clean up statically linked pthreads
  pthread_win32_process_detach_np();
#endif
#endif
}

/************************************************************************/
// Server start / quit
/************************************************************************/

#ifdef SC_WIN32
// according to this page: http://www.mkssoftware.com/docs/man3/setlinebuf.3.asp
// setlinebuf is equivalent to the setvbuf call below.
inline int setlinebuf(FILE *stream)
{
  return setvbuf( stream, (char*)0, _IONBF, 0 );
}
#endif

struct ScsynthInteropStartOptions* scsynth_interop_get_default_start_options()
{
  struct ScsynthInteropStartOptions *options = (struct ScsynthInteropStartOptions*)malloc(sizeof(struct ScsynthInteropStartOptions));
  memset(options, 0, sizeof(struct ScsynthInteropStartOptions));
  options->numControlBusChannels =               kDefaultWorldOptions.mNumControlBusChannels;
  options->numAudioBusChannels =                 kDefaultWorldOptions.mNumAudioBusChannels;
  options->numInputBusChannels =                 kDefaultWorldOptions.mNumInputBusChannels;
  options->numOutputBusChannels =                kDefaultWorldOptions.mNumOutputBusChannels;
  options->bufLength =                           kDefaultWorldOptions.mBufLength;
  options->preferredHardwareBufferFrameSize =    kDefaultWorldOptions.mPreferredHardwareBufferFrameSize;
  options->preferredSampleRate =                 kDefaultWorldOptions.mPreferredSampleRate;
  options->numBuffers =                          kDefaultWorldOptions.mNumBuffers;
  options->maxNodes =                            kDefaultWorldOptions.mMaxNodes;
  options->maxGraphDefs =                        kDefaultWorldOptions.mMaxGraphDefs;
  options->realTimeMemorySize =                  kDefaultWorldOptions.mRealTimeMemorySize;
  options->maxWireBufs =                         kDefaultWorldOptions.mMaxWireBufs;
  options->numRGens =                            kDefaultWorldOptions.mNumRGens;

  options->verbosity = 1;
  options->UGensPluginPath = "../server/plugins/";
  options->inDeviceName = "";
  options->outDeviceName = "";
  return options;
}

void* scsynth_interop_start(struct ScsynthInteropStartOptions *inOptions)
{
  setlinebuf(stdout);

  WorldOptions options = kDefaultWorldOptions;

  if( -1 != inOptions->numControlBusChannels) {
    options.mNumControlBusChannels = inOptions->numControlBusChannels;
  }
  if( -1 != inOptions->numAudioBusChannels) {
    options.mNumAudioBusChannels = inOptions->numAudioBusChannels;
  }
  if( -1 != inOptions->numInputBusChannels) {
    options.mNumInputBusChannels= inOptions->numInputBusChannels;
  }
  if( -1 != inOptions->numOutputBusChannels) {
    options.mNumOutputBusChannels = inOptions->numOutputBusChannels;
  }
  if( -1 != inOptions->bufLength) {
    options.mBufLength = inOptions->bufLength;
  }
  if( -1 != inOptions->preferredHardwareBufferFrameSize) {
    options.mPreferredHardwareBufferFrameSize = inOptions->preferredHardwareBufferFrameSize;
  }
  if( -1 != inOptions->preferredSampleRate) {
    options.mPreferredSampleRate = inOptions->preferredSampleRate;
  }
  if( -1 != inOptions->numBuffers) {
    options.mNumBuffers = inOptions->numBuffers;
  }
  if( -1 != inOptions->maxNodes) {
    options.mMaxNodes = inOptions->maxNodes;
  }
  if( -1 != inOptions->maxGraphDefs) {
    options.mMaxGraphDefs = inOptions->maxGraphDefs;
  }
  if( -1 != inOptions->realTimeMemorySize) {
    options.mRealTimeMemorySize = inOptions->realTimeMemorySize;
  }
  if( -1 != inOptions->maxWireBufs) {
    options.mMaxWireBufs = inOptions->maxWireBufs;
  }
  if( -1 != inOptions->numRGens) {
    options.mNumRGens = inOptions->numRGens;
  }
  if( 0 != strcmp("", inOptions->inDeviceName )) { // not empty
    options.mInDeviceName = inOptions->inDeviceName;
  }
  if( 0 != strcmp("", inOptions->outDeviceName )) { // not empty
    options.mOutDeviceName = inOptions->outDeviceName;
  }

  options.mVerbosity = inOptions->verbosity;
  options.mUGensPluginPath = inOptions->UGensPluginPath;

  struct World *world = World_New(&options);
  if (!world) return 0;

  if(options.mVerbosity >=0) {
    scprintf("SuperCollider 3 server ready..\n");
  }

  fflush(stdout);

  return (void*)world;
}

void scsynth_interop_wait_for_quit(void* w)
{
  struct World *inWorld = (struct World*)w;
  World_WaitForQuit(inWorld);
}

/************************************************************************/
// Soundbuffer
/************************************************************************/

void* scsynth_interop_copy_sndbuf(void* w, unsigned index)
{
  struct World *world = (struct World*)w;
  bool didChange;
  struct SndBuf* buf = (struct SndBuf*) malloc(sizeof(struct SndBuf));
  memset(buf, 0, sizeof(struct SndBuf));
  int serverErr = World_CopySndBuf(world, index, buf, false, &didChange);
  return (void*)buf;
}

/************************************************************************/
// Server communication
/************************************************************************/

int scsynth_interop_open_udp(void* world, int inPort)
{
  struct World *inWorld = (struct World*)world;
  return World_OpenUDP(inWorld, inPort);
}

int scsynth_interop_open_tcp(void* world, int inPort, int inMaxConnections, int inBacklog)
{
  struct World *inWorld = (struct World*)world;

  return World_OpenTCP(inWorld, inPort, inMaxConnections, inBacklog);
}

bool scsynth_interop_send_packet(void*world, int inSize, char *inData, void (*func)(void*, void*,int))
{
  struct World *inWorld = (struct World*)world;
  ReplyFunc inFunc = (ReplyFunc)func;

  return World_SendPacket(inWorld, inSize, inData, inFunc);
}


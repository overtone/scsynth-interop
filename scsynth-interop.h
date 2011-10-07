#ifndef _SCSYTNH_INTEROP_
#define _SCSYNTH_INTEROP_

#ifdef SC_WIN32
# define SC_DLLEXPORT __declspec(dllexport)
#else
# define SC_DLLEXPORT 
#endif

struct ScsynthInteropStartOptions
{
  int verbosity;
  const char* UGensPluginPath;
  const char* inDeviceName;
  const char* outDeviceName;
  int numControlBusChannels;
  int numAudioBusChannels;
  int numInputBusChannels;
  int numOutputBusChannels;
  int bufLength;
  int preferredHardwareBufferFrameSize;
  int preferredSampleRate;
  int numBuffers;
  int maxNodes;
  int maxGraphDefs;
  int realTimeMemorySize;
  int maxWireBufs;
  int numRGens;
};

extern "C" {
  SC_DLLEXPORT struct ScsynthInteropStartOptions* scsynth_interop_get_default_start_options();

  SC_DLLEXPORT int scsynth_interop_init();
  SC_DLLEXPORT void* scsynth_interop_start(struct ScsynthInteropStartOptions *options); // returns struct World*
  SC_DLLEXPORT void scsynth_interop_cleanup();
  SC_DLLEXPORT void* scsynth_interop_copy_sndbuf(void* world, unsigned index);  // world is a struct World* , returns struct SndBuf*

  SC_DLLEXPORT void scsynth_interop_wait_for_quit(void* world); // world is a struct World*
  SC_DLLEXPORT int scsynth_interop_open_udp(void* world, int inPort); // world is a struct World*
  SC_DLLEXPORT int scsynth_interop_open_tcp(void* world, int inPort, int inMaxConnections, int inBacklog); // world is a struct World*
  SC_DLLEXPORT bool scsynth_interop_send_packet(void* world, int inSize, char *inData, void (*infunc)(void*, void*,int)); // world is a struct World*, infunc is a ReplyFunc inFunc,

}

#endif // _SCSYNTH_INTEROP_

#ifndef IVP_EXTEND_MUMBLECALLBACKHANDLER_H
#define IVP_EXTEND_MUMBLECALLBACKHANDLER_H

#include <iostream>
#include <portaudio.h>
#include <MOOS/libMOOS/Comms/MOOSAsyncCommClient.h>
#include "mumlib/include/mumlib.hpp"
#include "SimpleRingBuffer.h"

class MumbleCallbackHandler: public mumlib::BasicCallback {
public:
    MumbleCallbackHandler(std::shared_ptr<RingBuffer<int16_t>> pb);

    void audio(int target, int sessionId, int sequenceNumber, int16_t *pcm_data, uint32_t pcm_data_size) override;
    void textMessage(uint32_t actor,
                     std::vector<uint32_t> session,
                     std::vector<uint32_t> channel_id,
                     std::vector<uint32_t> tree_id,
                     std::string message) override;
    void channelState(std::string name,
                      int32_t channel_id,
                      int32_t parent,
                      std::string description,
                      std::vector<uint32_t> links,
                      std::vector<uint32_t> inks_add,
                      std::vector<uint32_t> links_remove,
                      bool temporary,
                      int32_t position) override;
    void serverSync(
      std::string welcome_text,
      int32_t session,
      int32_t max_bandwidth,
      int64_t permissions) override;

    std::string channelList; // Mostly for debugging and setting channel IDs after restructuring Murmur's layout
    bool connectedOnce = false;

protected:
    std::shared_ptr<RingBuffer<int16_t>> playbackBuffer;

};

#endif //IVP_EXTEND_MUMBLECALLBACKHANDLER_H

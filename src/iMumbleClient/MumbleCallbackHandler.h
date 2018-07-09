#ifndef IVP_EXTEND_MUMBLECALLBACKHANDLER_H
#define IVP_EXTEND_MUMBLECALLBACKHANDLER_H

#include <iostream>
#include <portaudio.h>
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

protected:
    std::shared_ptr<RingBuffer<int16_t>> playbackBuffer;

};

#endif //IVP_EXTEND_MUMBLECALLBACKHANDLER_H

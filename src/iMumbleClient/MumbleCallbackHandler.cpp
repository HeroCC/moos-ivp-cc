/************************************************************/
/*    NAME: Conlan Cesar                                    */
/*    ORGN: MIT                                             */
/*    FILE: MumbleCallbackHandler.cpp                       */
/*    DATE: Summer 2018                                     */
/************************************************************/

#include "MumbleCallbackHandler.h"

MumbleCallbackHandler::MumbleCallbackHandler(std::shared_ptr<RingBuffer<int16_t>> pb): playbackBuffer(pb) {

}

void MumbleCallbackHandler::audio(int target, int sessionId, int sequenceNumber, int16_t *pcm_data, uint32_t pcm_data_size) {
  playbackBuffer->push(pcm_data, 0, pcm_data_size);
  //mum->sendAudioData(pcm_data, pcm_data_size);
}

void MumbleCallbackHandler::textMessage(uint32_t actor,
                                        std::vector<uint32_t> session,
                                        std::vector<uint32_t> channel_id,
                                        std::vector<uint32_t> tree_id,
                                        std::string message) {
  //mum->sendTextMessage("someone said: " + message);

}

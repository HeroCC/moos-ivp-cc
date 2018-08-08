/************************************************************/
/*    NAME: Conlan Cesar                                    */
/*    ORGN: MIT                                             */
/*    FILE: MumbleCallbackHandler.cpp                       */
/*    DATE: Summer 2018                                     */
/************************************************************/

#include <MBUtils.h>
#include "MumbleCallbackHandler.h"

MumbleCallbackHandler::MumbleCallbackHandler(std::shared_ptr<RingBuffer<int16_t>> pb): playbackBuffer(pb) {

};

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

void MumbleCallbackHandler::channelState(std::string name, int32_t channel_id, int32_t parent, std::string description,
                  std::vector<uint32_t> links, std::vector<uint32_t> links_add, std::vector<uint32_t> links_remove,
                  bool temporary, int32_t position) {
  if (!strContains(this->channelList, "=" + intToString(channel_id) + ",")) {
    channelList.append(name + '=' + intToString(channel_id) + ',');
  }
}

void MumbleCallbackHandler::serverSync(std::string welcome_text, int32_t session, int32_t max_bandwidth,
                                       int64_t permissions) {
  connectedOnce = true;
}

/************************************************************/
/*    NAME: Conlan Cesar                                    */
/*    ORGN: MIT                                             */
/*    FILE: MumbleCallbackHandler.cpp                       */
/*    DATE: Summer 2018                                     */
/************************************************************/

#include <portaudio.h>
#include <iostream>
#include "mumlib/include/mumlib.hpp"

class MumbleCallbackHandler: public mumlib::BasicCallback {
public:
    mumlib::Mumlib* mum;
    PaStream *stream;

    MumbleCallbackHandler() {
      Pa_Initialize();

      Pa_OpenDefaultStream(&stream,
                           0,
                           1,
                           paInt16,
                           48000, // On some devices, this may be 44100
                           paFramesPerBufferUnspecified,
                           NULL, // No callback function, we are using the Direct Write / Blocking API
                           NULL);

      Pa_StartStream(stream);
    }

    void audio(int target, int sessionId, int sequenceNumber, int16_t *pcm_data, uint32_t pcm_data_size) override {
      auto err = Pa_WriteStream(stream, pcm_data, pcm_data_size);
      if (err) std::cout << Pa_GetErrorText(err) << std::endl;
      //mum->sendAudioData(pcm_data, pcm_data_size);
    }

    void textMessage(uint32_t actor,
                     std::vector<uint32_t> session,
                     std::vector<uint32_t> channel_id,
                     std::vector<uint32_t> tree_id,
                     std::string message) override {
      mum->sendTextMessage("someone said: " + message);
    }
};
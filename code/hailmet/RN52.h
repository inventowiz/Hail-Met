#ifndef RN52_H
#define RN52_H

#include <string.h>
#include <Stream.h>


class RN52 {
  private:
    /*enum ConnectionStatus { 
        LIMBO,
        CONNECTABLE,
        CONNECTABLE_AND_DISCOVERABLE,
        CONNECTED,
        OUTGOING_CALL_ESTABLISHED,
        INCOMING_CALL_ESTABLISHED,
        ACTIVE_CALL_IN_PROGRESS,
        TEST_MODE,
        THREE_WAY_CALL_WAITING,
        THREE_WAY_CALL_MULTI_CALL,
        INCOMING_CALL_ON_HOLD,
        ACTIVE_CALL,
        AUDIO_STREAMING,
        LOW_BATTERY
    };

    static const String ConnectionStatusNames = {

    }*/

    struct Profiles{
      bool iAP;
      bool SPP;
      bool A2DP;
      bool HFP_HSP;
    };

    struct State {
      Profiles activeProfiles; 
      int connectionStatus;
      bool speakerVolumeChange;
      bool microphoneVolumeChange;
    };


    static const char terminatingChar = '\n';

    State prevState;
    State state;
    bool discoverable;
    bool incomingCallActive;
    bool isCommanding;

    int gpio9Pin;
    Stream *serial;

    // This function can be used to get the settings of the RN52.
    // These settings include pin #, module name, etc
    void getSettings();

    int ASCIIToNum(char c);

    // This function will wait for a serial response that ends with the
    // termination character
    String waitForResponse();
    
    void sendCommand(String str);

  public:
    // This will configure the RN52 properly, rebooting if
    // necessary, then return when ready.
    void init(Stream &serial, int gpio9Pin);

    // This function will poll the RN52 for its connection status
    // and update the private "state" variable.
    void updateConnectionStatus();

    // Connection/Discovery commands
    bool isDiscoverable();
    void setDiscoverable(bool discoverable);

    // This attempts to reconnect to the most recently paired device.
    void attemptReconnect();

    void disconnect();

    // Call commands
    // Returns true if there is an incoming call.
    bool incomingCall();

    // Accepts and incoming call.
    void acceptCall();

    // Rejects an incoming call or terminates an active call.
    void rejectCall();


    // Media/AVRCP commands
    void increaseVolume();
    void decreaseVolume();
    void nextTrack();
    void previousTrack();
    void playPause();
    void activateVoiceCommand();

    int getConnectionStatus();
    bool isInCommandMode();
    bool enterCommandMode();
    bool exitCommandMode();
};

#endif

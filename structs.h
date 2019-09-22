#ifndef structs_h
  #define structs_h

struct folderSettings {
    uint8_t folder;
    uint8_t mode;
    uint8_t special;
    uint8_t special2;
    uint8_t current; // add current position
};

// this object stores nfc tag data
struct nfcTagObject {
    uint32_t cookie;
    uint8_t version;
    folderSettings nfcFolderSettings;
};

// admin settings stored in eeprom
struct adminSettings {
    uint32_t cookie;
    byte version;
    uint8_t maxVolume;
    uint8_t minVolume;
    uint8_t initVolume;
    uint8_t eq;
    bool locked;
    long standbyTimer;
    bool invertVolumeButtons;
    folderSettings shortCuts[4];
    uint8_t adminMenuLocked;
    uint8_t adminMenuPin[4];
    bool light;
    uint8_t lightBrightness = 30;
    uint8_t persistedModifier = 0;
};

#endif

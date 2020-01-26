#include <Arduino.h>
#include <DFMiniMp3.h>
#include <EEPROM.h>
#include <JC_Button.h>
#include <MFRC522.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <avr/sleep.h>
#include "constants.h"
#include "structs.h"

#include "platformConfig.h"
#include "KeepAlive.hpp"
#include "ButtonLed.hpp"

/*
   _____         _____ _____ _____ _____
  |_   _|___ ___|  |  |     |   | |     |
	| | | . |   |  |  |-   -| | | |  |  |
	|_| |___|_|_|_____|_____|_|___|_____|
	TonUINO Version 2.1

	created by Thorsten Voß and licensed under GNU/GPL.
	Information and contribution at https://tonuino.de.
*/


static const uint32_t cardCookie = 322417479;
bool configBrightness = false;

#define shutdownPin 7

bool isPowerOff = false;

// DFPlayer Mini
SoftwareSerial mySoftwareSerial(PIN_DFPLAYER_RX, PIN_DFPLAYER_TX); // RX, TX
uint16_t numTracksInFolder;
uint16_t currentTrack;
uint16_t firstTrack;
uint8_t queue[255];
uint8_t volume;

int16_t globalBrightness = 100; // brightness in percent (used for sleep timer)
adminSettings mySettings;
nfcTagObject myCard;
nfcTagObject newCard;
folderSettings *myFolder;
unsigned long sleepAtMillis = 0;
static uint16_t _lastTrackFinished;

// Function forward declarations
void powerOff();

bool askCode(uint8_t *code);
bool ReadFromSerial();
void readButtons();

void setStandbyTimer();
void disableStandbyTimer();
void checkStandbyAtMillis();

void wakeUpCard();
void setupCard();
void resetCard();
bool readCard(nfcTagObject *nfcTag);
void WriteCardDataToSerial();

void playFolder();
bool setupFolder(folderSettings *theFolder);
void playShortCut(uint8_t shortCut);
static void nextTrack(uint16_t track);

uint8_t voiceMenu(int numberOfOptions, int startMessage, int messageOffset, bool preview = false, int previewFromFolder = 0, int defaultValue = 0, bool exitWithLongPress = false);
bool isPlaying();
bool checkTwo(uint8_t a[], uint8_t b[]);
void writeCard(nfcTagObject nfcTag, bool silent = false);
void dump_byte_array(byte *buffer, byte bufferSize, bool noSpace = false);
void adminMenu(bool fromCard = false);
bool knownCard = false;

uint8_t readStatus = 0;
uint8_t rnum = 0;

void waitForTrackToFinish();

void ClearCardData(){
  myCard.cookie = 0;
  myCard.version = 0;
  myCard.nfcFolderSettings.folder = 0;
  myCard.nfcFolderSettings.mode = 0;
  myCard.nfcFolderSettings.special = 0;
  myCard.nfcFolderSettings.special2 = 0;
  myCard.nfcFolderSettings.current = 0;
}

// implement a notification class,
// its member methods will get called
//
class Mp3Notify {
public:
	static void OnError(uint16_t errorCode) {
		// see DfMp3_Error for code meaning
		Serial.println();
		Serial.print("Com Error ");
		Serial.println(errorCode);
		//statusLedUpdate(BURST4, pinkStatusColor, 0);
	}

	static void OnPlayFinished(uint16_t track) {
		nextTrack(track);
	}

	static void OnCardOnline(uint16_t code) {
		Serial.println(F("SD Karte online "));
	}

	static void OnCardInserted(uint16_t code) {
		Serial.println(F("SD Karte bereit "));
	}

	static void OnCardRemoved(uint16_t code) {
		Serial.println(F("SD Karte entfernt "));
	}

	static void OnUsbOnline(uint16_t code) {
		Serial.println(F("USB online "));
	}

	static void OnUsbInserted(uint16_t code) {
		Serial.println(F("USB bereit "));
	}

	static void OnUsbRemoved(uint16_t code) {
		Serial.println(F("USB entfernt "));
	}
};

static DFMiniMp3 <SoftwareSerial, Mp3Notify> mp3(mySoftwareSerial);

void shuffleQueue() {
	// Queue für die Zufallswiedergabe erstellen
	for (uint8_t x = 0; x < numTracksInFolder - firstTrack + 1; x++) {
		queue[x] = x + firstTrack;
	}

	// Rest mit 0 auffüllen
	for (uint8_t x = numTracksInFolder - firstTrack + 1; x < 255; x++) {
		queue[x] = 0;
	}

	// Queue mischen
	for (uint8_t i = 0; i < numTracksInFolder - firstTrack + 1; i++) {
		uint8_t j = random(0, numTracksInFolder - firstTrack + 1);
		uint8_t t = queue[i];
		queue[i] = queue[j];
		queue[j] = t;
	}
}

void writeSettingsToFlash() {
	Serial.println(F("=== writeSettingsToFlash()"));
	int address = sizeof(myFolder->folder) * 100;
	EEPROM.put(address, mySettings);
}

void resetSettings() {
	Serial.println(F("=== resetSettings()"));
	mySettings.cookie = cardCookie;
	mySettings.version = 2;
	mySettings.maxVolume = 25;
	mySettings.minVolume = 5;
	mySettings.initVolume = 15;
	mySettings.eq = 1;
	mySettings.locked = false;
	mySettings.standbyTimer = 0;
	mySettings.invertVolumeButtons = true;
	mySettings.shortCuts[0].folder = 0;
	mySettings.shortCuts[1].folder = 0;
	mySettings.shortCuts[2].folder = 0;
	mySettings.shortCuts[3].folder = 0;
	mySettings.adminMenuLocked = 0;
	mySettings.adminMenuPin[0] = 1;
	mySettings.adminMenuPin[1] = 1;
	mySettings.adminMenuPin[2] = 1;
	mySettings.adminMenuPin[3] = 1;
	mySettings.light = false;
	mySettings.lightBrightness = 30;
	mySettings.persistedModifier = 0;

	writeSettingsToFlash();
}

void migrateSettings(int oldVersion) {
	switch (oldVersion)
	{
		case 1:
		{
			Serial.println(F("=== resetSettings()"));
			Serial.println(F("1 -> 2"));
			mySettings.version = 2;
			mySettings.adminMenuLocked = 0;
			mySettings.adminMenuPin[0] = 1;
			mySettings.adminMenuPin[1] = 1;
			mySettings.adminMenuPin[2] = 1;
			mySettings.adminMenuPin[3] = 1;
			writeSettingsToFlash();
		}
		break;
	
		case 2:
		{
			Serial.println(F("=== resetSettings()"));
			Serial.println(F("2 -> 3"));
			mySettings.version = 3;
			mySettings.lightBrightness = 30;
			mySettings.persistedModifier = false;
			writeSettingsToFlash();
		}
		break;
	}
}

void loadSettingsFromFlash() {
	Serial.println(F("=== loadSettingsFromFlash()"));
	int address = sizeof(myFolder->folder) * 100;
	EEPROM.get(address, mySettings);
	if (mySettings.cookie != cardCookie) {
		resetSettings();
	}
	migrateSettings(mySettings.version);

	Serial.print(F("Version: "));
	Serial.println(mySettings.version);

	Serial.print(F("Maximal Volume: "));
	Serial.println(mySettings.maxVolume);

	Serial.print(F("Minimal Volume: "));
	Serial.println(mySettings.minVolume);

	Serial.print(F("Initial Volume: "));
	Serial.println(mySettings.initVolume);

	Serial.print(F("EQ: "));
	Serial.println(mySettings.eq);

	Serial.print(F("Locked: "));
	Serial.println(mySettings.locked);

	Serial.print(F("Sleep Timer: "));
	Serial.println(mySettings.standbyTimer);

	Serial.print(F("Inverted Volume Buttons: "));
	Serial.println(mySettings.invertVolumeButtons);

	Serial.print(F("Admin Menu locked: "));
	Serial.println(mySettings.adminMenuLocked);

	Serial.print(F("Admin Menu Pin: "));
	Serial.print(mySettings.adminMenuPin[0]);
	Serial.print(mySettings.adminMenuPin[1]);
	Serial.print(mySettings.adminMenuPin[2]);
	Serial.println(mySettings.adminMenuPin[3]);

	Serial.print(F("Deactivate Light: "));
	Serial.println(mySettings.light);

	Serial.print(F("Max Brightness: "));
	Serial.println(mySettings.lightBrightness);
}

class Modifier {
public:
	virtual bool shouldPersistOnPowerOff(){
		return false;
	}

	virtual bool handleShortCut(uint8_t shortcut){
      return false;
    }
	virtual void loop() {}

	virtual bool handlePause() {
		return false;
	}

	virtual bool handleNext() {
		return false;
	}

	virtual bool handlePrevious() {
		return false;
	}

	virtual bool handleNextButton() {
		return false;
	}

	virtual bool handlePreviousButton() {
		return false;
	}

	virtual bool handleVolumeUp() {
		return false;
	}

	virtual bool handleVolumeDown() {
		return false;
	}

	virtual bool handleRFID(nfcTagObject *newCard) {
		return false;
	}

	virtual uint8_t getActive() {
		return MODIFIER_DEFAULT;
	}

	Modifier() {

	}
};

Modifier* activeModifier = NULL;

class LearnToCalculate: public Modifier {
  private:
  uint8_t mode = MODIFIER_CALC_LEARN_MODE_ADD; // learn mode (1 = addition, 2 = subtraction, 3 = multiplication, 4 = division, 5 = mixed)
  uint8_t upperBound = 0; // max number (can not exceed 254)
  uint8_t opA = 0;
  uint8_t opB = 0;
  uint8_t result = 10;
  uint8_t opr = 0; // operator, see mode (0..3)
  unsigned long lastAction = 0;

  void playResult(){
    this->lastAction = millis();
    if(this->result > 255){
      result = 255;
    }
    
    mp3.playMp3FolderTrack(this->result);
    waitForTrackToFinish();
  }

  void nextQuestion(bool repeat = false){
    this->lastAction = millis();
      if(!repeat){
          if(this->mode == MODIFIER_CALC_LEARN_MODE_ALL)
          {
            this->opr = random(MODIFIER_CALC_LEARN_MODE_ADD,MODIFIER_CALC_LEARN_MODE_ALL);
          }
          else{
            this->opr = this->mode;
          }
          this->result = 0;
          
          switch(this->opr){ 
            case MODIFIER_CALC_LEARN_MODE_SUB: // subtraction
            {
              this->opA = random(2, this->upperBound);
              this->opB = random(1, this->opA);
            }
            break;
            case MODIFIER_CALC_LEARN_MODE_MULT: // multiplication
            {
              this->opA = random(1, this->upperBound);
              this->opB = random(1, floor(this->upperBound/this->opA));

            }
            break;
            case MODIFIER_CALC_LEARN_MODE_DIV: // division
            {
              this->opA = random(2, this->upperBound);
              do{
                this->opB = random(1, this->upperBound);
              }while(this->opA % this->opB  != 0);
            }
            break;

			case MODIFIER_CALC_LEARN_MODE_ADD:
            default: // addition
            {
              this->opA = random(1, this->upperBound-1);
              this->opB = random(1, this->upperBound - this->opA);
            }
            break;
          }
      }
        mp3.playMp3FolderTrack(MP3_CALC_HOW_MUCH_IS); // question "how much is"
         waitForTrackToFinish();
        mp3.playMp3FolderTrack(this->opA); // 1..254
         waitForTrackToFinish();
        mp3.playMp3FolderTrack(MP3_CALC_HOW_MUCH_IS + this->opr); // 402, 403, 404, 405
         waitForTrackToFinish();
        mp3.playMp3FolderTrack(this->opB); // 1..254
         waitForTrackToFinish();
  }

  public:
    virtual bool handlePause(){
      this->lastAction = millis();
      if(this->upperBound <= 0){
        this->upperBound = this->result;
        this->result = 0;
        this->nextQuestion();
        return true;
      }
      uint8_t tmpVal = 0;

      switch(this->opr){
        case MODIFIER_CALC_LEARN_MODE_ADD:
        { tmpVal = this->opA + this->opB; }
        break;
        case MODIFIER_CALC_LEARN_MODE_SUB:
        { tmpVal = this->opA - this->opB; }
        break;
        case MODIFIER_CALC_LEARN_MODE_MULT:
        { tmpVal = this->opA * this->opB; }
        break;
        case MODIFIER_CALC_LEARN_MODE_DIV:
        { tmpVal = this->opA / this->opB; }
        break;
      }

      if(tmpVal == this->result){
        mp3.playMp3FolderTrack(MP3_CALC_CORRECT); // richtig
         waitForTrackToFinish();
         this->nextQuestion();
      }
      else{
        mp3.playMp3FolderTrack(MP3_CALC_WRONG); // falsch
         waitForTrackToFinish();
         this->nextQuestion(true); // repeat question
      }
      return true;
    }
    virtual bool handleShortCut(uint8_t shortcut){
      this->lastAction = millis();
      switch (shortcut)
      {
      case 1: // up
        if(result + 10 >= 255){
          result = 255;
        }
        else{
          result += 10;
        }
        break;

        case 2: // down
          if(result - 10 < 1){
            result = 1;
          }
          else{
            result -= 10;
          }
        break;
        case 0: // pause
        {
          mp3.playMp3FolderTrack(802); // cancelled
          waitForTrackToFinish();
          mp3.pause();
          activeModifier = NULL;
          delete this;
          return true;
        }
        break;
      }
      this->playResult();
      return true;
    }

    virtual bool handleNextButton(){
      if(result +1 > 255){
        result = 255;
      }
      else{
        result++;
      }
      this->playResult();
      return true;
    }
    virtual bool handlePreviousButton() {
      if(result -1 < 1){
        result = 1;
      }
      else{
        result--;
      }
      this->playResult();
      return true;
    }
    virtual bool handleVolumeUp()   {
      return this->handleNextButton();
    }
    virtual bool handleVolumeDown() {
      return this->handlePreviousButton();
    }
    virtual bool handleRFID(nfcTagObject *newCard) {
      return true;
    }
    virtual void loop(){
      if(this->upperBound > 0){
        if( millis() - this->lastAction >= 60000){ // check all 60s
          mp3.playMp3FolderTrack(MP3_CALC_CALC_WITH_ME);
          waitForTrackToFinish();
          this->nextQuestion(true);

          this->lastAction = millis();
        }
      }
      return true;
    }
    LearnToCalculate(uint8_t mode = 1) {
      Serial.println(F("=== LearnToCalculate ==="));
      this->mode = mode;

      if(this->mode == 5){
        this->opr = random(1,5);
      }
      else{
        this->opr = this->mode;
      }

      // clear old card
      myCard.nfcFolderSettings.folder = 0;
      myCard.nfcFolderSettings.mode = 0;
      mp3.pause();

      mp3.playMp3FolderTrack(MP3_CALC_INTRO); // intro 
      waitForTrackToFinish();

      mp3.playMp3FolderTrack(MP3_CALC_MAX_NUM); // choose maximum
      waitForTrackToFinish();
      this->lastAction = millis();
    }

    uint8_t getActive() {
      return MODIFIER_LEARN_TO_CALC;
    }
};

class SleepTimer : public Modifier {
private:
	unsigned long sleepAtMillis = 0;
	unsigned long lastCheck = 0;
	unsigned long lastVolCheck = 0;
	uint16_t lastVolume = 0;
public:
	bool shouldPersistOnPowerOff(){
		return true;
	}

	void loop() {
		if (this->sleepAtMillis > 0 && millis() > this->sleepAtMillis) {
			Serial.println(F("=== SleepTimer::loop() -> SLEEP!"));
			mp3.pause();
			globalBrightness = 0;

			setStandbyTimer();
			activeModifier = NULL;
			delete this;
		}
		else {
			if (this->sleepAtMillis > 0) {
				if (millis() > this->lastCheck + 5000) {
					// calculate global brightness
					this->lastCheck = millis();
					float tmp = (float)this->lastCheck / (float)this->sleepAtMillis;
					uint16_t pct = 100 - (int)(tmp * 100.0);
					globalBrightness = min(pct, 100);
				}
				else if (isPlaying() && this->sleepAtMillis > 0 && millis() > this->sleepAtMillis - 20000 && millis() > this->lastVolCheck + 1000) {
					if (this->lastVolume <= 0) {
						this->lastVolume = volume;
					}
					this->lastVolCheck = millis();
					uint16_t val = this->lastVolume - map(this->lastVolCheck, this->sleepAtMillis - 20000, this->sleepAtMillis, 0, this->lastVolume);
					mp3.setVolume(val);
				}
			}
		}
	}

	SleepTimer(uint8_t minutes) {
		Serial.println(F("=== SleepTimer()"));
		Serial.println(minutes);
		this->sleepAtMillis = millis() + minutes * 60000;
		this->lastCheck = millis();
		this->lastVolCheck = millis();
		if (isPlaying()) {
			mp3.playAdvertisement(ADV_SLEEP);
			mp3.playAdvertisement(minutes);
			if(minutes == 1)
			{
				mp3.playAdvertisement(ADV_MINUTE);
			}
			else{
				mp3.playAdvertisement(ADV_MINUTES);
			}
		}
		else {
			mp3.playMp3FolderTrack(MP3_MODIFIER_SLEEP);
			waitForTrackToFinish();
			mp3.playMp3FolderTrack(minutes);
			waitForTrackToFinish();
			if(minutes == 1)
			{
				mp3.playMp3FolderTrack(MP3_MINUTE);
			}
			else{
				mp3.playMp3FolderTrack(MP3_MINUTES);
			}
			waitForTrackToFinish();
			delay(100);
			mp3.pause();
		}

	}

	uint8_t getActive() {
		//Serial.println(F("== SleepTimer::getActive()"));
		return MODIFIER_SLEEP_TIMER;
	}
};

class FreezeDance : public Modifier {
private:
	unsigned long nextStopAtMillis = 0;
	const uint8_t minSecondsBetweenStops = 5;
	const uint8_t maxSecondsBetweenStops = 30;

	void setNextStopAtMillis() {
		uint16_t seconds = random(this->minSecondsBetweenStops, this->maxSecondsBetweenStops + 1);
		this->nextStopAtMillis = millis() + seconds * 1000;
	}

public:
	void loop() {
		if (this->nextStopAtMillis != 0 && millis() > this->nextStopAtMillis) {
			if (isPlaying()) {
				mp3.playAdvertisement(ADV_FREEZE_STOP);
				delay(500);
			}
			setNextStopAtMillis();
		}
	}

	FreezeDance(void) {
		Serial.println(F("=== FreezeDance()"));
		if (isPlaying()) {
			delay(1000);
			mp3.playAdvertisement(ADV_FREEZE_INTRO);
			delay(1000);
		}
		else{
			mp3.playMp3FolderTrack(MP3_FREEZE_INTRO);
			delay(500);
		}
		setNextStopAtMillis();
	}

	uint8_t getActive() {
		return MODIFIER_FREEZE_DANCE;
	}
};

class Locked : public Modifier {
public:
	bool shouldPersistOnPowerOff(){
		return true;
	}

	virtual bool handlePause() {
		return true;
	}

	virtual bool handleNextButton() {
		return true;
	}

	virtual bool handlePreviousButton() {
		return true;
	}

	virtual bool handleVolumeUp() {
		return true;
	}

	virtual bool handleVolumeDown() {
		return true;
	}

	virtual bool handleRFID(nfcTagObject *newCard) {
		return true;
	}

	Locked(void) {
		Serial.println(F("=== Locked()"));
		if (isPlaying()){
              mp3.playAdvertisement(ADV_LOCKED);
		 }
		 else{
			 mp3.playMp3FolderTrack(MP3_MODIFIER_LOCKED);
			 delay(100);
			 mp3.pause();
		 }
	}

	uint8_t getActive() {
		return MODIFIER_LOCKED;
	}
};

class ToddlerMode : public Modifier {
public:
	bool shouldPersistOnPowerOff(){
		return true;
	}

	virtual bool handlePause() {
		return true;
	}

	virtual bool handleNextButton() {
		return true;
	}

	virtual bool handlePreviousButton() {
		return true;
	}

	virtual bool handleVolumeUp() {
		return true;
	}

	virtual bool handleVolumeDown() {
		return true;
	}

	ToddlerMode(void) {
		Serial.println(F("=== ToddlerMode()"));
		if (isPlaying()){
              mp3.playAdvertisement(ADV_BUTTONS_LOCKED);
			  delay(500);
		 }
		 else{
			 mp3.playMp3FolderTrack(MP3_MODIFIER_BUTTONS_LOCKED);
			 delay(100);
			 mp3.pause();
		 }
	}

	uint8_t getActive() {
		return MODIFIER_TODDLER;
	}
};

class KindergardenMode : public Modifier {
private:
	nfcTagObject nextCard;
	bool cardQueued = false;

public:
	bool shouldPersistOnPowerOff(){
		return true;
	}

	virtual bool handleNext() {
		if (this->cardQueued == true) {
			this->cardQueued = false;

			myCard = nextCard;
			myFolder = &myCard.nfcFolderSettings;
			playFolder();
			return true;
		}
		return false;
	}

	virtual bool handleNextButton() {
		return true;
	}

	virtual bool handlePreviousButton() {
		return true;
	}

	virtual bool handleRFID(nfcTagObject *newCard) { // lot of work to do!
		this->nextCard = *newCard;
		this->cardQueued = true;
		if (!isPlaying()) {
			handleNext();
		}
		return true;
	}

	KindergardenMode() {
		Serial.println(F("=== KindergardenMode()"));
		if (isPlaying()){
              mp3.playAdvertisement(ADV_DAYCARE_MODE);
			  delay(500);
		 }
		 else{
			 mp3.playMp3FolderTrack(MP3_MODIFIER_DAYCARE_MODE);
			 delay(100);
			 mp3.pause();
		 }
	}

	uint8_t getActive() {
		return MODIFIER_DAYCARE;
	}
};

class RepeatSingleModifier : public Modifier {
public:
	virtual bool handleNext() {
		delay(50);
		if (isPlaying()) return true;
		mp3.playFolderTrack(myFolder->folder, currentTrack);
		_lastTrackFinished = 0;
		return true;
	}

	RepeatSingleModifier() {
		Serial.println(F("=== RepeatSingleModifier()"));
		if (isPlaying()){
              mp3.playAdvertisement(ADV_REPEAT);
			  delay(500);
		 }
		 else{
			 mp3.playMp3FolderTrack(MP3_MODIFIER_REPEAT);
			 delay(100);
			 mp3.pause();
		 }
	}

	uint8_t getActive() {
		return MODIFIER_REPEAT_SINGLE;
	}
};

MFRC522 mfrc522(PIN_RFID_READER_SS, PIN_RFID_READER_RST); // Create MFRC522
MFRC522::MIFARE_Key key;
bool successRead;
byte sector = 1;
byte blockAddr = 4;
byte trailerBlock = 7;
MFRC522::StatusCode status;
MFRC522::Uid lastCardUid;

bool operator==(const MFRC522::Uid& x, const MFRC522::Uid& other){
	if(x.size != other.size){
		return false;
	}

	for(uint8_t i = 0; i < x.size; i++){
		if(x.uidByte[i] != other.uidByte[i]){
			return false;
		}
	}
	return true;
}


// Leider kann das Modul selbst keine Queue abspielen, daher müssen wir selbst die Queue verwalten
static void nextTrack(uint16_t track) {
	if (activeModifier != NULL) {
		if (activeModifier->handleNext()) {
			return;
		}
	}

	if (track == _lastTrackFinished) {
		return;
	}
	_lastTrackFinished = track;

	if (!knownCard) {
		// Wenn eine neue Karte angelernt wird soll das Ende eines Tracks nicht
		// verarbeitet werden
		return;
	}

	Serial.println(F("=== nextTrack()"));

	switch (myFolder->mode) {
	case MODE_AUDIO_DRAMA:
	case MODE_SPECIAL_AUDIO_DRAMA: {
		if(currentTrack <= numTracksInFolder){
			currentTrack++;
			mp3.playFolderTrack(myFolder->folder, currentTrack);
		}
		else{
			setStandbyTimer();
		}
	}
								   break;

	case MODE_ALBUM:
	case MODE_SPECIAL_ALBUM: {
		if (currentTrack <= numTracksInFolder) {
			currentTrack++;
			mp3.playFolderTrack(myFolder->folder, currentTrack);
		}
		else {
			//      mp3.sleep();   // Je nach Modul kommt es nicht mehr zurück aus dem Sleep!
			setStandbyTimer();
		}
	}
							 break;

	case MODE_PARTY:
	case MODE_SPECIAL_PARTY: {
		if (currentTrack <= (numTracksInFolder - firstTrack + 1)) {
			currentTrack++;
		}
		else {
			currentTrack = 1;

			// Queue am Ende neu mischen ?
#ifdef RECREATE_QUEUE_ON_END
			shuffleQueue();
#endif
		}
		mp3.playFolderTrack(myFolder->folder, queue[currentTrack - 1]);
	}
							 break;

	case MODE_SINGLE_TRACK: {
		//    mp3.sleep();      // Je nach Modul kommt es nicht mehr zurück aus dem Sleep!
		setStandbyTimer();
	}
							break;

	case MODE_AUDIO_BOOK:
	case MODE_SPECIAL_AUDIO_BOOK: {
		if (currentTrack <= numTracksInFolder) {
			currentTrack++;
			mp3.playFolderTrack(myFolder->folder, currentTrack);
			myFolder->current = currentTrack;

			// save progress to card
			wakeUpCard();
			if (mfrc522.PICC_ReadCardSerial()) {
				if(mfrc522.uid == lastCardUid){
					Serial.println("Write progress...");
					writeCard(myCard, true);
					mfrc522.PICC_HaltA();
					mfrc522.PCD_StopCrypto1();
				}
			}
		}
		else {

			// Fortschritt zurück setzen
			if (myFolder->mode == MODE_SPECIAL_AUDIO_BOOK)
			{
				myFolder->current = firstTrack;
			}
			else {
				myFolder->current = 1;
			}

			wakeUpCard();
			if (mfrc522.PICC_ReadCardSerial()) {
				if(mfrc522.uid == lastCardUid){
					Serial.println("Write progress...");
					writeCard(myCard, true);
					mfrc522.PICC_HaltA();
					mfrc522.PCD_StopCrypto1();
				}
			}
			setStandbyTimer();
		}
	}
								  break;
	}
	// @todo check if delay is needed
	delay(500);
}

static void previousTrack() {
	Serial.println(F("=== previousTrack()"));

	switch (myFolder->mode) {
	case MODE_AUDIO_DRAMA:
	case MODE_SPECIAL_AUDIO_DRAMA: {
		if(currentTrack > firstTrack){
			currentTrack--;
		}
		mp3.playFolderTrack(myFolder->folder, currentTrack);
	}
		break;
	case MODE_ALBUM:
	case MODE_SPECIAL_ALBUM: {
		if (currentTrack > firstTrack) {
			currentTrack--;
		}
		mp3.playFolderTrack(myFolder->folder, currentTrack);
	}
							 break;

	case MODE_PARTY:
	case MODE_SPECIAL_PARTY: {
		if (currentTrack > 1) {
			currentTrack--;
		}
		else {
			currentTrack = numTracksInFolder;
		}
		mp3.playFolderTrack(myFolder->folder, queue[currentTrack - 1]);
	}
							 break;

	case MODE_SINGLE_TRACK: {
		mp3.playFolderTrack(myFolder->folder, currentTrack);
	}
							break;

	case MODE_AUDIO_BOOK:
	case MODE_SPECIAL_AUDIO_BOOK: {
		if (myFolder->mode == MODE_AUDIO_BOOK) {
			if (currentTrack > 1) {
				currentTrack--;
			}
		}
		else {
			if (currentTrack > firstTrack)
			{
				currentTrack--;
			}
		}

		mp3.playFolderTrack(myFolder->folder, currentTrack);

		myFolder->current = currentTrack;
		// save progress to card
		wakeUpCard();
		if (mfrc522.PICC_ReadCardSerial()) {
			if(mfrc522.uid == lastCardUid){
				writeCard(myCard, true);
			}
		}
	}
								  break;
	}

	// @todo check if delay is needed
	delay(1000);
}

Button pauseButton(PIN_BUTTON_PAUSE);
Button upButton(PIN_BUTTON_UP);
Button downButton(PIN_BUTTON_DOWN);

bool ignorePauseButton = false;
bool ignoreUpButton = false;
bool ignoreDownButton = false;

/// Funktionen für den Standby Timer (z.B. über Pololu-Switch oder Mosfet)
void setStandbyTimer() {
	Serial.println(F("=== setStandbyTimer()"));
	sleepAtMillis = (mySettings.standbyTimer > 0) ? millis() + (mySettings.standbyTimer * 60000) : 0;
}

void disableStandbyTimer() {
	Serial.println(F("=== disableStandbyTimer()"));
	sleepAtMillis = 0;
}

void checkStandbyAtMillis() {
	if (sleepAtMillis > 0 && millis() > sleepAtMillis) {
		powerOff();
	}
}

void powerOff() {
	Serial.println(F("=== power off!"));
	// enter sleep state
	ButtonLed::set(0); // switch button LEDs off
	KeepAlive::set(false); // disable keep-alive circuit
	digitalWrite(PIN_POWER_SWITCH, !PIN_POWER_SWITCH_ON_STATE);
	//delay(500);
	isPowerOff = true;

	// http://discourse.voss.earth/t/intenso-s10000-powerbank-automatische-abschaltung-software-only/805
	// powerdown to 27mA (powerbank switches off after 30-60s)
	mfrc522.PCD_AntennaOff();
	mfrc522.PCD_SoftPowerDown();
	mp3.sleep();
    mp3.setPlaybackSource(DfMp3_PlaySource_Sleep);
    
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
	cli();  // Disable interrupts
	sleep_mode();
}

bool isPlaying() {
	return !digitalRead(PIN_DFPLAYER_BUSY);
}

void waitForTrackToFinish() {
	long currentTime = millis();

	do {
		mp3.loop();
	} while (!isPlaying() && millis() < currentTime + 1000);

	// @todo check if delay is needed
	delay(1000);
	do {
		mp3.loop();
	} while (isPlaying());
}

void setup() {
	/*
	 * Set power switch pin as soon as possible.
	 * This is especially important if the power switch circuit
	 * is just a simple self locking circuit.
	 */
    pinMode(PIN_POWER_SWITCH, OUTPUT);
	digitalWrite(PIN_POWER_SWITCH, PIN_POWER_SWITCH_ON_STATE);

	/* Init the keep alive circuit - switches it on during start phase */
	KeepAlive::setup();
	/* Init button LED brightness handling */
	ButtonLed::setup();

	Serial.begin(115200); // Es gibt ein paar Debug Ausgaben über die serielle Schnittstelle

	// Wert für randomSeed() erzeugen durch das mehrfache Sammeln von rauschenden LSBs eines offenen Analogeingangs
	uint32_t ADCSeed;
	for (uint8_t i = 0; i < 128; i++) {
		ADCSeed ^= (analogRead(PIN_OPEN_ANALOG) & 0x1) << (i % 32);
	}
	randomSeed(ADCSeed); // Zufallsgenerator initialisieren

	// Dieser Hinweis darf nicht entfernt werden
	Serial.println(F("\n _____         _____ _____ _____ _____"));
	Serial.println(F("|_   _|___ ___|  |  |     |   | |     |"));
	Serial.println(F("  | | | . |   |  |  |-   -| | | |  |  |"));
	Serial.println(F("  |_| |___|_|_|_____|_____|_|___|_____|\n"));
	Serial.println(F("TonUINO Version 2.1"));
	Serial.println(F("created by Thorsten Voß and licensed under GNU/GPL."));
	Serial.println(F("modified version by Peter Pascher"));
	Serial.println(F("Information and contribution at https://tonuino.de.\n"));

	// Busy Pin
	pinMode(PIN_DFPLAYER_BUSY, INPUT);

	// load Settings from EEPROM
	loadSettingsFromFlash();

	// activate standby timer
	setStandbyTimer();

	// DFPlayer Mini initialisieren
	delay(50);
	mp3.begin();

	// Zwei Sekunden warten bis der DFPlayer Mini initialisiert ist
	delay(2000);

	volume = mySettings.initVolume;
	mp3.setVolume(volume);
	mp3.setEq(mySettings.eq - 1);

	// NFC Leser initialisieren
	SPI.begin();        // Init SPI bus
	mfrc522.PCD_Init(); // Init MFRC522
	mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader
	for (byte i = 0; i < 6; i++) {
		key.keyByte[i] = 0xFF;
	}

	pinMode(PIN_BUTTON_PAUSE, INPUT_PULLUP);
	pinMode(PIN_BUTTON_UP,    INPUT_PULLUP);
	pinMode(PIN_BUTTON_DOWN,  INPUT_PULLUP);

	readButtons();
	readButtons();

	// RESET
	if (pauseButton.isPressed() && upButton.isPressed() && downButton.isPressed())
    {
		Serial.println(F("Reset -> EEPROM wird gelöscht"));
		for (int i = 0; i < EEPROM.length(); i++) {
			EEPROM.update(i, 0);
		}
		loadSettingsFromFlash();
        mp3.pause();
		mp3.playMp3FolderTrack(MP3_RESET_OK);
        waitForTrackToFinish();
	}

	mp3.pause();
    mp3.playMp3FolderTrack(MP3_INTRO);
    waitForTrackToFinish();

	// Start Shortcut "at Startup" - e.g. Welcome Sound
	playShortCut(SHORTCUT_STARTUP);

	if(mySettings.persistedModifier > 0){
		switch (mySettings.persistedModifier)
		{
			case MODIFIER_SLEEP_TIMER: {
				activeModifier = new SleepTimer(1); // one minute to deactivate sleep timer
			}
			break;
			case MODIFIER_LOCKED: {
				activeModifier = new Locked();
			}
			break;
			case MODIFIER_TODDLER: {
				activeModifier = new ToddlerMode();
			}
			break;
			case MODIFIER_DAYCARE: {
				activeModifier = new KindergardenMode();
			}
			break;
		}

	}
}

void readButtons() {
	pauseButton.read();
	upButton.read();
	downButton.read();
}

void volumeUpButton() {
	if (activeModifier != NULL) {
		if (activeModifier->handleVolumeUp() == true) {
			return;
		}
	}

	if (volume < mySettings.maxVolume) {
		mp3.setVolume(++volume);
	}
}

void volumeDownButton() {
	if (activeModifier != NULL) {
		if (activeModifier->handleVolumeDown() == true) {
			return;
		}
	}

	if (volume > mySettings.minVolume) {
		mp3.setVolume(--volume);
	}
}

void nextButton() {
	if (activeModifier != NULL)
	{
		if (activeModifier->handleNextButton() == true) {
			return;
		}
	}
	nextTrack(random(65536));
	delay(500);
}

void previousButton() {
	if (activeModifier != NULL) {
		if (activeModifier->handlePreviousButton() == true) {
			return;
		}
	}

	previousTrack();
	delay(500);
}

void playFolder() {
	Serial.println(F("== playFolder()"));
	disableStandbyTimer();
	knownCard = true;
	_lastTrackFinished = 0;
	numTracksInFolder = mp3.getFolderTrackCount(myFolder->folder);
	firstTrack = 1;

	switch (myFolder->mode) {
	case MODE_AUDIO_DRAMA: // Hörspielmodus: eine zufällige Datei aus dem Ordner
	{
		currentTrack = random(1, numTracksInFolder + 1);
		mp3.playFolderTrack(myFolder->folder, currentTrack);
	}
	break;

	case MODE_ALBUM: // Album Modus: kompletten Ordner spielen
	{
		currentTrack = 1;
		mp3.playFolderTrack(myFolder->folder, currentTrack);
	}
	break;

	case MODE_PARTY: // Party Modus: Ordner in zufälliger Reihenfolge
	{
		shuffleQueue();
		currentTrack = 1;
		mp3.playFolderTrack(myFolder->folder, queue[currentTrack - 1]);
	}
	break;

	case MODE_SINGLE_TRACK: // Einzel Modus: eine Datei aus dem Ordner abspielen
	{
		currentTrack = myFolder->special;
		mp3.playFolderTrack(myFolder->folder, currentTrack);
	}
	break;

	case MODE_AUDIO_BOOK: // Hörbuch Modus: kompletten Ordner spielen und Fortschritt merken
	{
		//currentTrack = EEPROM.read(myFolder->folder);
		currentTrack = myFolder->current;

		if (currentTrack == 0 || currentTrack > numTracksInFolder) {
			currentTrack = 1;
		}
		mp3.playFolderTrack(myFolder->folder, currentTrack);
	}
	break;

	case MODE_SPECIAL_AUDIO_BOOK:// Spezialmodus Von-Bin: Hörbuch: Dateien aus Ordner spielen und Fortschritt merken
	{
		numTracksInFolder = myFolder->special2;
		firstTrack = myFolder->special; // set boundary for jump back
		//currentTrack = EEPROM.read(myFolder->folder);
		currentTrack = myFolder->current;
		if (currentTrack > numTracksInFolder || currentTrack < firstTrack) {
			currentTrack = firstTrack;
		}

		mp3.playFolderTrack(myFolder->folder, currentTrack);

	}
	break;

	case MODE_SPECIAL_AUDIO_DRAMA: // Spezialmodus Von-Bis: Hörspiel: eine zufällige Datei aus dem Ordner
	{
		numTracksInFolder = myFolder->special2;
		firstTrack = myFolder->special; // set boundary for jump back mentioned by stockf in #37
		currentTrack = random(myFolder->special, numTracksInFolder + 1);
		mp3.playFolderTrack(myFolder->folder, currentTrack);
	}
	break;

	case MODE_SPECIAL_ALBUM: // Spezialmodus Von-Bis: Album: alle Dateien zwischen Start und Ende spielen
	{
		firstTrack = myFolder->special; // set boundary for jump back mentioned by stockf in #37
		numTracksInFolder = myFolder->special2;
		currentTrack = myFolder->special;
		mp3.playFolderTrack(myFolder->folder, currentTrack);
	}
	break;

	case MODE_SPECIAL_PARTY: // Spezialmodus Von-Bis: Party Ordner in zufälliger Reihenfolge
	{
		firstTrack = myFolder->special;
		numTracksInFolder = myFolder->special2;
		shuffleQueue();
		currentTrack = 1;
		mp3.playFolderTrack(myFolder->folder, queue[currentTrack - 1]);
	}
	break;
	}
}

void playShortCut(uint8_t shortCut) {
	if(activeModifier != NULL){
    	if(activeModifier->handleShortCut(shortCut) == true){
      		return;
		}
  	}
	if (mySettings.shortCuts[shortCut].folder != 0) {
		myFolder = &mySettings.shortCuts[shortCut];
		playFolder();
		disableStandbyTimer();
	}
}

void loop() {
	do {
		checkStandbyAtMillis();
		KeepAlive::loop();
		ButtonLed::loop();
		mp3.loop();

		// Modifier : WIP!
		if (activeModifier != NULL) {
			activeModifier->loop();
		}

		// Buttons werden nun über JC_Button gehandelt, dadurch kann jede Taste doppelt belegt werden
		readButtons();

		// admin menu
		if ((pauseButton.pressedFor(LONG_PRESS) || upButton.pressedFor(LONG_PRESS) || downButton.pressedFor(LONG_PRESS)) && pauseButton.isPressed() && upButton.isPressed() && downButton.isPressed()) {
			mp3.pause();
			do {
				readButtons();
			} while (pauseButton.isPressed() || upButton.isPressed() || downButton.isPressed());
			readButtons();
			adminMenu();
			break;
		}

		if (pauseButton.wasReleased()) {
			if (activeModifier != NULL) {
				if (activeModifier->handlePause() == true) {
					return;
				}
			}

			if (ignorePauseButton == false) {
				if (isPlaying()) {
					mp3.pause();
					setStandbyTimer();
				}
				else if (knownCard) {
					mp3.start();
					disableStandbyTimer();
				}
			}
			ignorePauseButton = false;
		}
		else if (pauseButton.pressedFor(LONG_PRESS) && ignorePauseButton == false) {
			if (activeModifier != NULL) {
				if (activeModifier->handlePause() == true) {
					return;
				}
			}

			if (isPlaying()) {
				uint8_t advertTrack = currentTrack;
				if(myFolder->mode == MODE_PARTY || myFolder->mode == MODE_SPECIAL_PARTY){
					advertTrack = (queue[currentTrack - 1]);
				}

				// Spezialmodus Von-Bis für Album, Party und Hörbuch gibt die Dateinummer relativ zur Startposition wieder
				if(myFolder->mode == MODE_SPECIAL_ALBUM || myFolder->mode == MODE_SPECIAL_PARTY | myFolder->mode == MODE_SPECIAL_AUDIO_BOOK){
					advertTrack = advertTrack - myFolder->special + 1;
				}

				// reset to first track
				if(myFolder->mode == MODE_AUDIO_BOOK)
				{
					currentTrack = 1;
					advertTrack = currentTrack;
				}
				
				// reset to first track
				if(myFolder->mode == MODE_SPECIAL_AUDIO_BOOK){
					currentTrack = firstTrack;
					advertTrack = currentTrack - myFolder->special +1;
				}

				// reset progress
				if(myFolder->mode == MODE_AUDIO_BOOK || myFolder->mode == MODE_SPECIAL_AUDIO_BOOK){
					myFolder->current = currentTrack;
					mp3.playFolderTrack(myFolder->folder, currentTrack);

					wakeUpCard();
					if (mfrc522.PICC_ReadCardSerial()) {
						if(mfrc522.uid == lastCardUid){
							Serial.println("Write progress...");
							writeCard(myCard, true);
							mfrc522.PICC_HaltA();
							mfrc522.PCD_StopCrypto1();
						}
					}
				}

				mp3.playAdvertisement(advertTrack);
			}
			else {
				playShortCut(SHORTCUT_PLAY_BUTTON);
			}
			ignorePauseButton = true;
		}

		if (upButton.pressedFor(LONG_PRESS)) {
			if (isPlaying()) {
				if (!mySettings.invertVolumeButtons) {
					volumeUpButton();
				}
				else {
					nextButton();
				}
			}
			else {
				playShortCut(SHORTCUT_UP_NEXT_BUTTON);
			}
			ignoreUpButton = true;
		}
		else if (upButton.wasReleased()) {
			if (!ignoreUpButton) {
				if (!mySettings.invertVolumeButtons) {
					nextButton();
				}
				else {
					volumeUpButton();
				}
			}
			ignoreUpButton = false;
		}

		if (downButton.pressedFor(LONG_PRESS)) {
			if (isPlaying()) {
				if (!mySettings.invertVolumeButtons) {
					volumeDownButton();
				}
				else {
					previousButton();
				}
			}
			else {
				playShortCut(SHORTCUT_DOWN_PREVIOUS_BUTTON);
			}
			ignoreDownButton = true;
		}
		else if (downButton.wasReleased()) {
			if (!ignoreDownButton) {
				if (!mySettings.invertVolumeButtons) {
					previousButton();
				}
				else {
					volumeDownButton();
				}
			}
			ignoreDownButton = false;
		}

		if (isPlaying() && myFolder->mode != MODE_DEFAULT) {

			if (activeModifier != NULL && activeModifier->getActive() == MODIFIER_SLEEP_TIMER)
			{
			}
			else {
			}
		}
		else {
		}

		// Ende der Buttons
	} while (!mfrc522.PICC_IsNewCardPresent());

	// RFID Karte wurde aufgelegt

	if (!mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	if (readCard(&myCard) == true) {
		if (myCard.cookie == cardCookie && myCard.nfcFolderSettings.folder != 0 && myCard.nfcFolderSettings.mode != MODE_DEFAULT) {
			playFolder();
		}
		else if (myCard.cookie != cardCookie) { // Neue Karte konfigurieren
			knownCard = false;
			mp3.playMp3FolderTrack(MP3_NEW_TAG);
			waitForTrackToFinish();
			setupCard();
		}
	}
	mfrc522.PICC_HaltA();
	mfrc522.PCD_StopCrypto1();
}

void adminMenu(bool fromCard = false) {
	disableStandbyTimer();
	mp3.pause();
	Serial.println(F("=== adminMenu()"));
	knownCard = false;
	int temp;
	nfcTagObject tempCard;
	if (fromCard == false) {
		// Admin menu has been locked - it still can be trigged via admin card
		switch (mySettings.adminMenuLocked) {
		case ADMIN_LOCK_MODE_CARD:
			return;

		case ADMIN_LOCK_MODE_PIN:// Pin check
		{
			uint8_t pin[4];
			mp3.playMp3FolderTrack(MP3_ADMIN_ENTER_PIN);
			if (askCode(pin) == false) {
				return;
			}
			if (checkTwo(pin, mySettings.adminMenuPin) == false) {
				return;
			}
		}
		break;

		case ADMIN_LOCK_MODE_MATH:
		{
			// Math check
			uint8_t a = random(10, 20);
			uint8_t b = random(1, 10);
			uint8_t c;
			mp3.playMp3FolderTrack(MP3_ADMIN_CALC_QUESTION);
			waitForTrackToFinish();
			mp3.playMp3FolderTrack(a);

			if (random(1, 3) == 2) {
				// a + b
				c = a + b;
				waitForTrackToFinish();
				mp3.playMp3FolderTrack(MP3_ADMIN_CALC_PLUS);
			}
			else {
				// a - b
				b = random(1, a);
				c = a - b;
				waitForTrackToFinish();
				mp3.playMp3FolderTrack(MP3_ADMIN_CALC_MINUS);
			}
			waitForTrackToFinish();
			mp3.playMp3FolderTrack(b);
			temp = voiceMenu(255, 0, 0, false);
			if (temp != c) {
				return;
			}
		}
		break;
		}
	}

	int subMenu = voiceMenu(14, MP3_ADMIN_INTRO, MP3_ADMIN_INTRO, false, false, 0, true);

	switch (subMenu) {
	case MENU_SUB_EXIT:
	{
		return;
	}

	case MENU_SUB_RESET_CARD:
	{
		resetCard();
		mfrc522.PICC_HaltA();
		mfrc522.PCD_StopCrypto1();
	}
	break;

	case MENU_SUB_MAX_VOLUME: // Maximum Volume
	{
		mySettings.maxVolume = voiceMenu(30 - mySettings.minVolume, MP3_CONFIGURE_MAX_VOLUME_INTRO, mySettings.minVolume, false, false,
			mySettings.maxVolume - mySettings.minVolume) + mySettings.minVolume;
	}
	break;

	case MENU_SUB_MIN_VOLUME: // Minimum Volume
	{
		mySettings.minVolume = voiceMenu(mySettings.maxVolume - 1, MP3_CONFIGURE_MIN_VOLUME_INTRO, 0, false, false,
			mySettings.minVolume);
	}
	break;

	case MENU_SUB_INIT_VOLUME: // Initial Volume
	{
		mySettings.initVolume = voiceMenu(mySettings.maxVolume - mySettings.minVolume + 1, MP3_CONFIGURE_INIT_VOLUME_INTRO,
			mySettings.minVolume - 1, false, false, mySettings.initVolume - mySettings.minVolume + 1) +
			mySettings.minVolume - 1;
	}
	break;

	case MENU_SUB_LIGHT: {
		temp = voiceMenu(2, MP3_CONFIGURE_LIGHT_INTRO, MP3_CONFIGURE_LIGHT_INTRO, false);
		mySettings.light = (temp == 2);
	}
	break;

	case MENU_SUB_LIGHT_BRIGHTNESS:{
		configBrightness = true;
		mySettings.lightBrightness = voiceMenu(100, MP3_CONFIGURE_LIGHT_BRIGHTNESS_INTRO, 0, false, false, mySettings.lightBrightness);
		configBrightness = false;
	}
	break;

	case MENU_SUB_EQ:
	{
		// EQ
		mySettings.eq = voiceMenu(6, MP3_EQ_INTRO, MP3_EQ_INTRO, false, false, mySettings.eq);
		mp3.setEq(mySettings.eq - 1);
	}
	break;

	case MENU_SUB_MODIFIER: // create modifier card
	{
		tempCard.cookie = cardCookie;
		tempCard.version = 1;
		tempCard.nfcFolderSettings.folder = 0;
		tempCard.nfcFolderSettings.special = 0;
		tempCard.nfcFolderSettings.special2 = 0;
		tempCard.nfcFolderSettings.mode = voiceMenu(7, MP3_CONFIGURE_MODIFIER_INTRO, MP3_CONFIGURE_MODIFIER_INTRO,
			false, false, 0, true);

		if (tempCard.nfcFolderSettings.mode != MODIFIER_DEFAULT) {
			if (tempCard.nfcFolderSettings.mode == MODIFIER_SLEEP_TIMER) {
				switch (voiceMenu(4, MP3_CONFIGURE_TIMER_INTRO, MP3_CONFIGURE_TIMER_INTRO)) {
				case 1:
					tempCard.nfcFolderSettings.special = 5;
					break;
				case 2:
					tempCard.nfcFolderSettings.special = 15;
					break;
				case 3:
					tempCard.nfcFolderSettings.special = 30;
					break;
				case 4:
					tempCard.nfcFolderSettings.special = 60;
					break;
				}
			}
			else if(tempCard.nfcFolderSettings.mode == MODIFIER_LEARN_TO_CALC){
				tempCard.nfcFolderSettings.special = voiceMenu(5, MP3_CALC_ARITH_OP_INTRO, MP3_CALC_ARITH_OP_INTRO);
			}
			mp3.playMp3FolderTrack(MP3_WAITING_FOR_CARD);
			do {
				readButtons();
				if (upButton.wasReleased() || downButton.wasReleased()) {
					Serial.println(F("Abgebrochen!"));
					mp3.playMp3FolderTrack(MP3_RESET_ABORTED);
					return;
				}
			} while (!mfrc522.PICC_IsNewCardPresent());

			// RFID Karte wurde aufgelegt
			if (mfrc522.PICC_ReadCardSerial()) {
				Serial.println(F("schreibe Karte..."));
				writeCard(tempCard);
				delay(100);
				mfrc522.PICC_HaltA();
				mfrc522.PCD_StopCrypto1();
				waitForTrackToFinish();
			}
		}
	}
	break;

	case MENU_SUB_SHORTCUT: {
		uint8_t shortcut = voiceMenu(4, MP3_CONFIGURE_SHORTCUT_INTRO, MP3_CONFIGURE_SHORTCUT_INTRO);
		setupFolder(&mySettings.shortCuts[shortcut - 1]);
		mp3.playMp3FolderTrack(MP3_CARD_CONFIGURED);
	}
							break;

	case MENU_SUB_STANDBY_TIMER: {
		switch (voiceMenu(5, MP3_CONFIGURE_TIMER_INTRO, MP3_CONFIGURE_TIMER_INTRO)) {
		case 1:
			mySettings.standbyTimer = 5;
			break;
		case 2:
			mySettings.standbyTimer = 15;
			break;
		case 3:
			mySettings.standbyTimer = 30;
			break;
		case 4:
			mySettings.standbyTimer = 60;
			break;
		case 5:
			mySettings.standbyTimer = 0;
			break;
		}
	}
								 break;

	case MENU_SUB_CREATE_CARDS_FOR_FOLDER: // Create Cards for Folder
	{
		// Ordner abfragen
		tempCard.cookie = cardCookie;
		tempCard.version = 1;
		tempCard.nfcFolderSettings.mode = MODE_SINGLE_TRACK;
		tempCard.nfcFolderSettings.folder = voiceMenu(99, MP3_SELECT_FOLDER, 0, true);
		uint8_t special = voiceMenu(mp3.getFolderTrackCount(tempCard.nfcFolderSettings.folder), MP3_SELECT_FIRST_FILE,
			0, true, tempCard.nfcFolderSettings.folder);
		uint8_t special2 = voiceMenu(mp3.getFolderTrackCount(tempCard.nfcFolderSettings.folder), MP3_SELECT_LAST_FILE,
			0, true, tempCard.nfcFolderSettings.folder, special);

		mp3.playMp3FolderTrack(MP3_CONFIGURE_BATCH_CARDS_INTRO);
		waitForTrackToFinish();
		for (uint8_t x = special; x <= special2; x++) {
			mp3.playMp3FolderTrack(x);
			tempCard.nfcFolderSettings.special = x;
			do {
				readButtons();
				if (upButton.wasReleased() || downButton.wasReleased()) {
					Serial.println(F("Abgebrochen!"));
					mp3.playMp3FolderTrack(MP3_RESET_ABORTED);
					return;
				}
			} while (!mfrc522.PICC_IsNewCardPresent());

			// RFID Karte wurde aufgelegt
			if (mfrc522.PICC_ReadCardSerial()) {
				Serial.println(F("schreibe Karte..."));
				writeCard(tempCard);
				delay(100);
				mfrc522.PICC_HaltA();
				mfrc522.PCD_StopCrypto1();
				waitForTrackToFinish();
			}
		}
	}
	break;

	case MENU_SUB_INVERT_BUTTONS: // Invert Functions for Up/Down Buttons
	{
		temp = voiceMenu(2, MP3_CONFIGURE_SWITCH_VOLUME_BUTTONS_INTRO, MP3_CONFIGURE_SWITCH_VOLUME_BUTTONS_INTRO, false);
		mySettings.invertVolumeButtons = (temp == 2);
	}
	break;

	case MENU_SUB_RESET: {
		Serial.println(F("Reset -> EEPROM wird gelöscht"));
		for (int i = 0; i < EEPROM.length(); i++) {
			EEPROM.update(i, 0);
		}
		resetSettings();
		mp3.playMp3FolderTrack(MP3_RESET_OK);
	}
						 break;

	case MENU_SUB_LOCK_ADMIN: // lock admin menu
	{
		temp = voiceMenu(4, MP3_CONFIGURE_ADMIN_LOCK_INTRO, MP3_CONFIGURE_ADMIN_LOCK_INTRO, false);

		if (temp == 3) {
			int8_t pin[4];
			mp3.playMp3FolderTrack(MP3_ADMIN_ENTER_PIN);
			if (askCode(pin)) {
				memcpy(mySettings.adminMenuPin, pin, 4);
				mySettings.adminMenuLocked = 2;
			}
			else {
				temp = mySettings.adminMenuLocked + 1;
			}
		}
		mySettings.adminMenuLocked = temp - 1;
	}
	break;

	case MENU_SUB_PROGRAMMING_MODE:
	{
		mp3.playMp3FolderTrack(MP3_PROGRAMMING_MODE_INTRO);
		waitForTrackToFinish();
		Serial.println(F("Programmiermodus aktiviert"));
		
		tempCard.cookie = cardCookie;
		tempCard.version = 1;
		bool cancel = false;
		long lastCheckTime = 0;
		do {
			readButtons();
			if(upButton.wasReleased() || downButton.wasReleased() || pauseButton.wasReleased()){
				cancel = true;
				Serial.println(F("Abgebrochen"));
				mp3.playMp3FolderTrack(MP3_PROGRAMMING_MODE_FINISHED);
				waitForTrackToFinish();
				break;
			}

			Serial.println(F(" Karte auflegen"));
			mp3.playMp3FolderTrack(MP3_WAITING_FOR_CARD);

			// warte auf karte
			do {
				readButtons();
				if(upButton.wasReleased() || downButton.wasReleased() || pauseButton.wasReleased())
				{
					cancel = true;
					Serial.println(F("Abgebrochen!"));
					mp3.playMp3FolderTrack(MP3_RESET_ABORTED);
					return;
				}
				// blink blue, while waiting
			} while (!mfrc522.PICC_IsNewCardPresent());

			// RFID Karte wurde aufgelegt
			if (mfrc522.PICC_ReadCardSerial()) {
				// set status to solid blue
				if(readCard(&newCard) == true){
					WriteCardDataToSerial();
				}

				while(ReadFromSerial() == false){
					readButtons();
					if(upButton.wasReleased() || downButton.wasReleased() || pauseButton.wasReleased())
					{
						cancel = true;
						Serial.println(F("Abgebrochen!"));
						mp3.playMp3FolderTrack(MP3_RESET_ABORTED);
						return;
					}
					delay(10);
				}
				Serial.println(F("schreibe Karte..."));
				writeCard(myCard);
				waitForTrackToFinish();
				delay(100);

				if(readCard(&newCard) == true){
					WriteCardDataToSerial();
				}
				mfrc522.PICC_HaltA();
				mfrc522.PCD_StopCrypto1();				
			}
		}while(!cancel);
	}
	break;
	}

	writeSettingsToFlash();
	setStandbyTimer();
}

bool askCode(uint8_t *code) {
	uint8_t x = 0;
	while (x < 4) {
		readButtons();
		if (pauseButton.pressedFor(LONG_PRESS))
			break;
		if (pauseButton.wasReleased())
			code[x++] = 1;
		if (upButton.wasReleased())
			code[x++] = 2;
		if (downButton.wasReleased())
			code[x++] = 3;
	}
	return true;
}

uint8_t voiceMenu(int numberOfOptions, int startMessage, int messageOffset, bool preview = false, int previewFromFolder = 0, int defaultValue = 0, bool exitWithLongPress = false) {
	uint8_t returnValue = defaultValue;
	uint8_t tmpVal;

	if (startMessage > 0) {
		mp3.playMp3FolderTrack(startMessage);
	}
	do {
		readButtons();
		mp3.loop();
		if(configBrightness){
			tmpVal = mySettings.lightBrightness;
			mySettings.lightBrightness = returnValue;
			mySettings.lightBrightness = tmpVal;
		}
		if (pauseButton.pressedFor(LONG_PRESS)) {
			mp3.playMp3FolderTrack(MP3_RESET_ABORTED);
			ignorePauseButton = true;
			if(configBrightness){
			}
			return defaultValue;
		}
		if (pauseButton.wasReleased()) {
			if(configBrightness){
			}
			if (returnValue != 0) {
				Serial.print(F("=== "));
				Serial.print(returnValue);
				Serial.println(F(" ==="));
				return returnValue;
			}
			// @todo check if delay is needed
			delay(500);
		}

		if (upButton.pressedFor(LONG_PRESS)) {
			returnValue = min(returnValue + 10, numberOfOptions);
			Serial.println(returnValue);
			mp3.playMp3FolderTrack(messageOffset + returnValue);
			if(!configBrightness){
				waitForTrackToFinish();
			}
			else{
				waitForTrackToFinish();
			}
			ignoreUpButton = true;
		}
		else if (upButton.wasReleased()) {
			if (!ignoreUpButton) {
				if(returnValue +1 > numberOfOptions){
					returnValue = 0; // flip over
				}

				returnValue = min(returnValue + 1, numberOfOptions);
				Serial.println(returnValue);
				//mp3.pause();
				mp3.playMp3FolderTrack(messageOffset + returnValue);
				if (preview) {
					waitForTrackToFinish();
					if (previewFromFolder == 0) {
						mp3.playFolderTrack(returnValue, 1);
					}
					else {
						mp3.playFolderTrack(previewFromFolder, returnValue);
					}

					// @todo check if delay is needed
					delay(500);
				}
			}
			else {
				ignoreUpButton = false;
			}
		}

		if (downButton.pressedFor(LONG_PRESS)) {
			returnValue = max(returnValue - 10, 1);
			Serial.println(returnValue);
			mp3.playMp3FolderTrack(messageOffset + returnValue);
			if(!configBrightness){
				waitForTrackToFinish();
			}
			else{
				waitForTrackToFinish();
			}
			ignoreDownButton = true;
		}
		else if (downButton.wasReleased()) {
			if (!ignoreDownButton) {
				if(returnValue-1 < 1){
					returnValue = numberOfOptions +1; // flip over
				}
				returnValue = max(returnValue - 1, 1);
				Serial.println(returnValue);
				mp3.playMp3FolderTrack(messageOffset + returnValue);
				if (preview) {
					waitForTrackToFinish();
					if (previewFromFolder == 0) {
						mp3.playFolderTrack(returnValue, 1);
					}
					else {
						mp3.playFolderTrack(previewFromFolder, returnValue);
					}
					// @todo check if delay is needed
					delay(500);
				}
			}
			else {
				ignoreDownButton = false;
			}
		}
	} while (true);
}

void resetCard() {
	mp3.playMp3FolderTrack(MP3_WAITING_FOR_CARD);
	do {
		pauseButton.read();
		upButton.read();
		downButton.read();

		if (upButton.wasReleased() || downButton.wasReleased()) {
			Serial.print(F("Abgebrochen!"));
			mp3.playMp3FolderTrack(MP3_RESET_ABORTED);
			return;
		}
	} while (!mfrc522.PICC_IsNewCardPresent());

	if (!mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	Serial.print(F("Karte wird neu konfiguriert!"));
	setupCard();
}

bool setupFolder(folderSettings *theFolder) {
	// Ordner abfragen
	theFolder->folder = voiceMenu(99, MP3_SELECT_FOLDER, 0, true, 0, 0, true);

	if (theFolder->folder == 0) return false;

	// Wiedergabemodus abfragen
	theFolder->mode = voiceMenu(10, MP3_SELECT_MODE, MP3_SELECT_MODE, false, 0, 0, true);

	switch (theFolder->mode) {
	case MODE_DEFAULT:
		return false;

	case MODE_SINGLE_TRACK: // Einzelmodus -> Datei abfragen
		theFolder->special = voiceMenu(mp3.getFolderTrackCount(theFolder->folder), MP3_SELECT_FILE, 0, true, theFolder->folder);
		break;

	case MODE_ADMIN: // Admin Funktionen
		theFolder->folder = 0;
		theFolder->mode = MODIFIER_ADMIN_MENU;
		break;

	case MODE_SPECIAL_AUDIO_DRAMA: // Spezialmodus Von-Bis
	case MODE_SPECIAL_AUDIO_BOOK: // Spezialmodus Von-Bis
	case MODE_SPECIAL_ALBUM:
	case MODE_SPECIAL_PARTY:
		theFolder->special = voiceMenu(mp3.getFolderTrackCount(theFolder->folder), MP3_SELECT_FIRST_FILE, 0, true, theFolder->folder);
		theFolder->special2 = voiceMenu(mp3.getFolderTrackCount(theFolder->folder), MP3_SELECT_LAST_FILE, 0, true, theFolder->folder, theFolder->special);
		theFolder->current = theFolder->special;
		break;
	}
	return true;
}

void setupCard() {
	mp3.pause();
	Serial.println(F("=== setupCard()"));
	nfcTagObject newCard;
	if (setupFolder(&newCard.nfcFolderSettings) == true) {
		// Karte ist konfiguriert -> speichern
		mp3.pause();
		do {
		} while (isPlaying());
		writeCard(newCard);
	}

	// @todo check if delay is needed
	delay(1000);
}

bool readCard(nfcTagObject *nfcTag) {
	nfcTagObject tempCard;
	lastCardUid.size = 0; // clean last card

	// Show some details of the PICC (that is: the tag/card)
	Serial.print(F("Card UID:"));
	dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
	Serial.println();
	Serial.print(F("PICC type: "));
	MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
	Serial.println(mfrc522.PICC_GetTypeName(piccType));

	byte buffer[18];
	byte size = sizeof(buffer);

	// Authenticate using key A
	if ((piccType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
		(piccType == MFRC522::PICC_TYPE_MIFARE_1K) ||
		(piccType == MFRC522::PICC_TYPE_MIFARE_4K)) {
		Serial.println(F("Authenticating Classic using key A..."));
		status = mfrc522.PCD_Authenticate(
			MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
	}
	else if (piccType == MFRC522::PICC_TYPE_MIFARE_UL) {
		byte pACK[] = { 0, 0 }; //16 bit PassWord ACK returned by the tempCard

		// Authenticate using key A
		Serial.println(F("Authenticating MIFARE UL..."));
		status = mfrc522.PCD_NTAG216_AUTH(key.keyByte, pACK);
	}

	if (status != MFRC522::STATUS_OK) {
		Serial.print(F("PCD_Authenticate() failed: "));
		Serial.println(mfrc522.GetStatusCodeName(status));
		return false;
	}

	// Show the whole sector as it currently is
	// Serial.println(F("Current data in sector:"));
	// mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
	// Serial.println();

	// Read data from the block
	if ((piccType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
		(piccType == MFRC522::PICC_TYPE_MIFARE_1K) ||
		(piccType == MFRC522::PICC_TYPE_MIFARE_4K)) {
		Serial.print(F("Reading data from block "));
		Serial.print(blockAddr);
		Serial.println(F(" ..."));
		status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
		if (status != MFRC522::STATUS_OK) {
			Serial.print(F("MIFARE_Read() failed: "));
			Serial.println(mfrc522.GetStatusCodeName(status));
			return false;
		}
	}
	else if (piccType == MFRC522::PICC_TYPE_MIFARE_UL) {
		byte buffer2[18];
		byte size2 = sizeof(buffer2);
		uint8_t x = 0;
		for (uint8_t i = 0; i <= 12; i += 4)
		{
			status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(8 + x++, buffer2, &size2);
			if (status != MFRC522::STATUS_OK) {
				Serial.print(F("MIFARE_Read_"));
				Serial.print(x);
				Serial.print(F(" failed: "));
				Serial.println(mfrc522.GetStatusCodeName(status));
				return false;
			}
			memcpy(buffer + i, buffer2, 4);
		}
	}

	Serial.print(F("Data on Card "));
	Serial.println(F(":"));
	dump_byte_array(buffer, 16);
	Serial.println();
	Serial.println();

	uint32_t tempCookie;
	tempCookie = (uint32_t)buffer[0] << 24;
	tempCookie += (uint32_t)buffer[1] << 16;
	tempCookie += (uint32_t)buffer[2] << 8;
	tempCookie += (uint32_t)buffer[3];

	tempCard.cookie = tempCookie;
	tempCard.version = buffer[4];
	tempCard.nfcFolderSettings.folder = buffer[5];
	tempCard.nfcFolderSettings.mode = buffer[6];
	tempCard.nfcFolderSettings.special = buffer[7];
	tempCard.nfcFolderSettings.special2 = buffer[8];
	tempCard.nfcFolderSettings.current = buffer[9];

	if (tempCard.cookie == cardCookie) {

		if (activeModifier != NULL && tempCard.nfcFolderSettings.folder != 0) {
			if (activeModifier->handleRFID(&tempCard) == true) {
				return false;
			}
		}

		if (tempCard.nfcFolderSettings.folder == 0) {
			if (activeModifier != NULL) {
				if (activeModifier->getActive() == tempCard.nfcFolderSettings.mode) {
					// @todo check if correct, should show unlock led when modifier deleted
					switch (tempCard.nfcFolderSettings.mode) {
					case MODIFIER_LOCKED: {
					}
										  break;

					case MODIFIER_REPEAT_SINGLE: {
					}
												 break;
					}
					delete activeModifier;
					if(mySettings.persistedModifier  > 0){
						mySettings.persistedModifier = 0;
						writeSettingsToFlash();
					}

					activeModifier = NULL;
					Serial.println(F("modifier removed"));
					if (isPlaying()) {
						mp3.playAdvertisement(ADV_MODIFIER_REMOVED_SOUND);
					}
					else {
						mp3.playMp3FolderTrack(MP3_MODIFIER_REMOVED_SOUND);
						waitForTrackToFinish();
						delay(100);
						mp3.pause();
					}
					return false;
				}
			}
			switch (tempCard.nfcFolderSettings.mode) {
			case MODIFIER_DEFAULT:
			case MODIFIER_ADMIN_MENU: {
				mfrc522.PICC_HaltA();
				mfrc522.PCD_StopCrypto1();
				adminMenu(true);
			}
									  break;
			case MODIFIER_SLEEP_TIMER: {
				activeModifier = new SleepTimer(tempCard.nfcFolderSettings.special);
			}
									   break;
			case MODIFIER_FREEZE_DANCE:
			{
				activeModifier = new FreezeDance();
			}
			break;
			case MODIFIER_LOCKED: {
				activeModifier = new Locked();
			}
								  break;
			case MODIFIER_TODDLER: {
				activeModifier = new ToddlerMode();
			}
								   break;
			case MODIFIER_DAYCARE: {
				activeModifier = new KindergardenMode();
			}
								   break;
			case MODIFIER_REPEAT_SINGLE: {
				//@todo check if correct
				activeModifier = new RepeatSingleModifier();
			}
										 break;
			
			case MODIFIER_LEARN_TO_CALC: {
				activeModifier = new LearnToCalculate(tempCard.nfcFolderSettings.special); break;
			}
			break;
			}

			if(activeModifier != NULL){
				if(activeModifier->shouldPersistOnPowerOff()){
					mySettings.persistedModifier = tempCard.nfcFolderSettings.mode; // save current modifier
					writeSettingsToFlash();
				}
			}
			return false;
		}
		else {
			memcpy(nfcTag, &tempCard, sizeof(nfcTagObject));
			Serial.println(nfcTag->nfcFolderSettings.folder);
			myFolder = &nfcTag->nfcFolderSettings;
			Serial.println(myFolder->folder);

			lastCardUid = mfrc522.uid;
		}
		return true;
	}
	else {
		memcpy(nfcTag, &tempCard, sizeof(nfcTagObject));
		lastCardUid = mfrc522.uid;
		return true;
	}
}

void wakeUpCard() {
	byte bufferATQA[2];
	byte bufferSize = sizeof(bufferATQA);
	mfrc522.PICC_WakeupA(bufferATQA, &bufferSize);
}

void writeCard(nfcTagObject nfcTag, bool silent = false) {
	MFRC522::PICC_Type mifareType;
	byte buffer[16] = { 0x13, 0x37, 0xb3, 0x47, // 0x1337 0xb347 magic cookie to
			// identify our nfc tags
					   0x02,                   // version 1
					   nfcTag.nfcFolderSettings.folder,          // the folder picked by the user
					   nfcTag.nfcFolderSettings.mode,    // the playback mode picked by the user
					   nfcTag.nfcFolderSettings.special, // track or function for admin cards
					   nfcTag.nfcFolderSettings.special2,
					   nfcTag.nfcFolderSettings.current,
		/*0x00,*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	byte size = sizeof(buffer);

	mifareType = mfrc522.PICC_GetType(mfrc522.uid.sak);

	// Authenticate using key B
	//authentificate with the card and set card specific parameters
	if ((mifareType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
		(mifareType == MFRC522::PICC_TYPE_MIFARE_1K) ||
		(mifareType == MFRC522::PICC_TYPE_MIFARE_4K)) {
		Serial.println(F("Authenticating again using key A..."));
		status = mfrc522.PCD_Authenticate(
			MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
	}
	else if (mifareType == MFRC522::PICC_TYPE_MIFARE_UL) {
		byte pACK[] = { 0, 0 }; //16 bit PassWord ACK returned by the NFCtag

		// Authenticate using key A
		Serial.println(F("Authenticating UL..."));
		status = mfrc522.PCD_NTAG216_AUTH(key.keyByte, pACK);
	}

	if (status != MFRC522::STATUS_OK) {
		Serial.print(F("PCD_Authenticate() failed: "));
		Serial.println(mfrc522.GetStatusCodeName(status));
		if (!silent) {
			mp3.playMp3FolderTrack(MP3_ERROR);
		}
		return;
	}

	// Write data to the block
	Serial.print(F("Writing data into block "));
	Serial.print(blockAddr);
	Serial.println(F(" ..."));
	dump_byte_array(buffer, 16);
	Serial.println();

	if ((mifareType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
		(mifareType == MFRC522::PICC_TYPE_MIFARE_1K) ||
		(mifareType == MFRC522::PICC_TYPE_MIFARE_4K)) {
		status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, buffer, 16);
	}
	else if (mifareType == MFRC522::PICC_TYPE_MIFARE_UL) {
		byte buffer2[16];
		byte size2 = sizeof(buffer2);

		uint8_t x = 0;
		for (uint8_t i = 0; i <= 12; i += 4)
		{
			memset(buffer2, 0, size2);
			memcpy(buffer2, buffer + i, 4);
			status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(8 + x++, buffer2, 16);
		}
	}

	if (status != MFRC522::STATUS_OK) {
		Serial.print(F("MIFARE_Write() failed: "));
		Serial.println(mfrc522.GetStatusCodeName(status));
		if (!silent) {
			mp3.playMp3FolderTrack(MP3_ERROR);
		}
	}
	else {
		if (!silent) {
			mp3.playMp3FolderTrack(MP3_CARD_CONFIGURED);
		}
	}
	Serial.println();

	// @todo check if delay is needed
	if (!silent) {
		delay(2000);
	}
}


/**
  Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte *buffer, byte bufferSize, bool noSpace = false) {
	for (byte i = 0; i < bufferSize; i++) {
		if(noSpace){
			if(buffer[i]< 0x10){
				Serial.print("0");
			}
		}
		else{
			Serial.print(buffer[i] < 0x10 ? " 0" : " ");
		}
		
		Serial.print(buffer[i], HEX);
	}
}

///////////////////////////////////////// Check Bytes   ///////////////////////////////////
bool checkTwo(uint8_t a[], uint8_t b[]) {
	for (uint8_t k = 0; k < 4; k++) {   // Loop 4 times
		if (a[k] != b[k]) {     // IF a != b then false, because: one fails, all fail
			return false;
		}
	}
	return true;
}

uint8_t GetByte(char c)
{
  if (c > '9') 
  {
    return (c&0x0F)+9;
  }
  else {
    return (c&0x0F);
  }
}

bool ReadFromSerial()
{
    if (Serial.available() > 0) {
      char c = Serial.read();
      if (c != '#' && (c < '0' || c > 'F')) return false;
      switch (readStatus) {
      case 0: {
              //warte auf Startzeichen
              if (c == '#') {
                rnum = 0;
                readStatus = 1;
                ClearCardData();
              }
            }
            return false;
      case 1: {
              //Magic lesen, 8 Hex Zeichen
              if (c == '#') {
                rnum = 0;
                ClearCardData();
                return false;
              }
              myCard.cookie = myCard.cookie*16+GetByte(c);
              rnum++;
              if (rnum > 7) {
                rnum = 0;
                readStatus = 2;
              }
            }
            return false;
      case 2: {
              //Version lesen, 2 Zeichen
              if (c == '#') {
                rnum = 0;
                ClearCardData();
                readStatus = 1;
                return false;
              }
              myCard.version = myCard.version*16+GetByte(c);
              rnum++;
              if (rnum > 1) {
                rnum = 0;
                readStatus = 3;
              }
            }
            return false;
      case 3: {
              //folder lesen, 2 Zeichen
              if (c == '#') {
                rnum = 0;
                ClearCardData();
                readStatus = 1;
                return false;
              }
              myCard.nfcFolderSettings.folder = myCard.nfcFolderSettings.folder*16+GetByte(c);
              rnum++;
              if (rnum > 1) {
                rnum = 0;
                readStatus = 4;
              }
            }
            return false;
      case 4: {
              //mode lesen, 2 Zeichen
              if (c == '#') {
                rnum = 0;
                ClearCardData();
                readStatus = 1;
                return false;
              }
              myCard.nfcFolderSettings.mode = myCard.nfcFolderSettings.mode*16+GetByte(c);
              rnum++;
              if (rnum > 1) {
                rnum = 0;
                readStatus = 5;
              }
            }
            return false;
      case 5: {
              //von track lesen, 3 Zeichen
              if (c == '#') {
                rnum = 0;
                ClearCardData();
                readStatus = 1;
                return false;
              }
              myCard.nfcFolderSettings.special = myCard.nfcFolderSettings.special*16+GetByte(c);
              rnum++;
              if (rnum > 1) {
                rnum = 0;
                readStatus = 6;
              }
            }
            return false;
      case 6: {
              //bis track lesen, 3 Zeichen
              if (c == '#') {
                rnum = 0;
                ClearCardData();
                readStatus = 1;
                return false;
              }
              myCard.nfcFolderSettings.special2 = myCard.nfcFolderSettings.special2*16+GetByte(c);
              rnum++;
              if (rnum > 1) {
                rnum = 0;
                readStatus = 0;
                return true;
              }
            }
    }
  }
  return false;
}

void WriteCardDataToSerial()
{
  // Daten aus newCar an PC senden mit führenden '#'
    byte buffer[9] = {0x13, 0x37, 0xb3, 0x47, // 0x1337 0xb347 magic cookie to
                     // identify our nfc tags
                     0x02,                   // version 1
                     newCard.nfcFolderSettings.folder,          // the folder picked by the user
                     newCard.nfcFolderSettings.mode,    // the playback mode picked by the user
                     newCard.nfcFolderSettings.special, // track or function for admin cards
                     newCard.nfcFolderSettings.special2 // to track
                    };

  byte size = sizeof(buffer);
  Serial.print('#');
  dump_byte_array(buffer, size, true);
  Serial.print('\n');
}

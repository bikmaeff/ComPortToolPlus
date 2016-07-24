#ifndef QTHINKGEAR_H
#define QTHINKGEAR_H

#include <QByteArray>

#define SYNC_MARK 0xAA
#define POOR_SIGNAL_MARK 0x20
#define ATTENTION_MARK 0x04
#define MEDITATION_MARK 0x05
#define BLINK_STRENTH_MARK 0x16
#define RAW_MARK 0x80
#define ASIC_EEG_POWER 0x83

class QThinkGear
{
public:
    QThinkGear();
    ~QThinkGear();
    void addByteToPackage(char byte);

    bool isNewRawReady();
    bool isNewpoorSignalQualityReady();
    bool isNewAttentionReady();
    bool isNewMeditationReay();
    bool isNewBlinkStrengthReady();
    bool isPowerReady();

    unsigned char getpoorSignalQuality();
    unsigned char getAttention();
    unsigned char getMeditation();
    unsigned char getBlinkStrength();
    short getRaw();



private:
    QByteArray payload;

    bool isPoorSignalQualityRefreshed;
    bool isAttentionRefreshed;
    bool isMeditationRefreshed;
    bool isBlinkStrengthRefreshed;
    //bool isPowerRefreshed; not parsed yet
    bool isRawDataRefreshed;

    bool isFirstSyncMarkDetected;
    bool isSecondSyncMarkDetected;
    bool isPayloadLengthStored;

    bool isChecksumOK;

    unsigned char payloadLength;
    unsigned char poorSignalQuality;
    unsigned char attention;
    unsigned char meditation;
    unsigned char blinkStrength;
    short raw;

};

#endif // QTHINKGEAR_H

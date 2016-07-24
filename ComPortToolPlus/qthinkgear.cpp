#include "qthinkgear.h"

QThinkGear::QThinkGear()
{
    isPoorSignalQualityRefreshed = false;
    isAttentionRefreshed = false;
    isMeditationRefreshed = false;
    isBlinkStrengthRefreshed = false;
    //isPowerRefreshed = false;
    isRawDataRefreshed = false;

    isFirstSyncMarkDetected = false;
    isSecondSyncMarkDetected = false;
    isPayloadLengthStored = false;

    isChecksumOK = false;
}
QThinkGear::~QThinkGear()
{

}
bool QThinkGear::isNewRawReady()
{
    return isRawDataRefreshed;
}
bool QThinkGear::isNewAttentionReady()
{
    return isPoorSignalQualityRefreshed;
}
bool QThinkGear::isNewMeditationReay()
{
    return isMeditationRefreshed;
}
bool QThinkGear::isNewBlinkStrengthReady()
{
    return isBlinkStrengthRefreshed;
}
unsigned char QThinkGear::getpoorSignalQuality()
{
    isPoorSignalQualityRefreshed = false;
    return poorSignalQuality;
}

unsigned char QThinkGear::getAttention()
{
    isAttentionRefreshed = false;
    return attention;
}
unsigned char QThinkGear::getMeditation()
{
    isMeditationRefreshed = false;
    return meditation;
}

unsigned char QThinkGear::getBlinkStrength()
{
    isBlinkStrengthRefreshed = false;
    return blinkStrength;
}

short QThinkGear::getRaw()
{
    isRawDataRefreshed = false;
    return raw;
}

void QThinkGear::addByteToPackage(char byte)
{
    //check for synchronization code
    if(isFirstSyncMarkDetected == false)//first SYNC_MARK detected
    {
        if((unsigned char)byte == SYNC_MARK)
        {
            isFirstSyncMarkDetected = true;
            return;
        }
    }
    else //first SYNC_MARK detected
    {
        if((unsigned char)byte == SYNC_MARK && isSecondSyncMarkDetected == false)//second SYNC_MARK detected right after the first SYNC_MARK
        {
            isSecondSyncMarkDetected = true;
            return;
        }
    }

    //sychronized, store payload
    if(isFirstSyncMarkDetected && isSecondSyncMarkDetected)
    {
        if(isPayloadLengthStored == false)
        {
            payloadLength = (unsigned char) (byte) + 1; //including checksum
            isPayloadLengthStored = true;
            return;
        }
        else
        {
            payload.clear();//clear last payload
            if(payloadLength !=0 )//store new payload
            {
                payload.append(byte);
                payloadLength--;
            }
            else //payload stored completely
            {
                //reset flags for next synchronization
                isFirstSyncMarkDetected = false;
                isSecondSyncMarkDetected = false;
                isPayloadLengthStored = false;
                //checksum
                for(QByteArray::Iterator iter = payload.begin(); iter != payload.end(); iter++)
                {
                    unsigned char checksum = 0;
                    if(iter != payload.end() - 1)
                    {
                        checksum = (unsigned char)(checksum + (unsigned char) (*iter));
                    }
                    else //checksum complete
                    {
                        checksum = ~checksum;
                        if(checksum == *iter)
                        {
                            isChecksumOK = true;
                        }
                    }

                }
                //parse all kinds of data, only if checksum is verified
                if(isChecksumOK)
                {
                    isChecksumOK = false;
                    for(QByteArray::Iterator iter = payload.begin(); iter != payload.end() - 1; )
                    {
                        switch ((unsigned char)*iter)
                        {
                            case POOR_SIGNAL_MARK: //1 byte signal quality mark
                                iter++; //go to data
                                poorSignalQuality =(unsigned char)*iter;
                                isPoorSignalQualityRefreshed = true;
                                iter++;
                                break;
                            case ATTENTION_MARK: //1 byte signal quality data
                                iter++;//go to data
                                attention = (unsigned char)*iter;
                                isAttentionRefreshed = true;
                                iter++;
                                break;
                            case MEDITATION_MARK: //1 byte signal quality data
                                iter++;//go to data
                                meditation = (unsigned char)*iter;
                                isMeditationRefreshed = true;
                                iter++;
                                break;
                            case BLINK_STRENTH_MARK: //1 byte bink strenth data
                                iter++;//go to data
                                blinkStrength = (unsigned char)*iter;
                                isBlinkStrengthRefreshed = true;
                                iter++;
                                break;
                            case RAW_MARK: //2 bytes raw eeg data
                                iter++;//go to data
                                raw = ((*iter)<<8 | (*(iter+1)));
                                isRawDataRefreshed = true;
                                iter = iter + 2;
                                break;
                            case ASIC_EEG_POWER: // 24 bytes eeg power data


                                break;
                        }
                    }

                }
            }
        }
    }
}

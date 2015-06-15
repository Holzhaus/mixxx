#ifndef MIDIMASTER_H
#define MIDIMASTER_H

#include <QObject>
#include <QString>
#include <QScopedPointer>

#include "engine/sync/clock.h"
#include "engine/sync/syncable.h"
#include "engine/enginechannel.h"

class ControlObject;
class ControlPushButton;
class EngineSync;

class MidiMasterClock : public QObject, public Clock, public Syncable {
    Q_OBJECT
  public:
    MidiMasterClock(const char* pGroup, SyncableListener* pEngineSync);
    virtual ~MidiMasterClock();

    const QString& getGroup() const {
        return m_group;
    }
    EngineChannel* getChannel() const {
        return NULL;
    }

    void notifySyncModeChanged(SyncMode mode);
    void notifyOnlyPlayingSyncable();
    void requestSyncPhase();
    SyncMode getSyncMode() const {
        return m_mode;
    }

    bool isPlaying() const {
        // midi running / not running state
        return true;
    }

    double getBeatDistance() const;
    void setMasterBeatDistance(double beatDistance);

    double getBaseBpm() const;
    void setMasterBaseBpm(double);
    void setMasterBpm(double bpm);
    double getBpm() const;
    void setInstantaneousBpm(double bpm);
    void setMasterParams(double beatDistance, double baseBpm, double bpm);

    void onCallbackStart(int sampleRate, int bufferSize);
    void onCallbackEnd(int sampleRate, int bufferSize);

  private slots:
    void slotSyncMasterEnabledChangeRequest(double state);

  private:
    QString m_group;
    SyncableListener* m_pEngineSync;
    QScopedPointer<ControlPushButton> m_pSyncMasterEnabled;

    QScopedPointer<ControlObject> m_pMidiClockBpm;
    QScopedPointer<ControlObject> m_pMidiClockLastBeatTime;

    SyncMode m_mode;
};

#endif   // MIDIMASTER_H

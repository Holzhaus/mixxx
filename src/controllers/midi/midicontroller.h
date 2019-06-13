/**
* @file midicontroller.h
* @author Sean Pappalardo spappalardo@mixxx.org
* @date Tue 7 Feb 2012
* @brief MIDI Controller base class
*
* This is a base class representing a MIDI controller.
*   It must be inherited by a class that implements it on some API.
*
*   Note that the subclass' destructor should call close() at a minimum.
*/

#ifndef MIDICONTROLLER_H
#define MIDICONTROLLER_H

#include "controllers/controller.h"
#include "controllers/midi/midiclockgenerator.h"
#include "controllers/midi/midicontrollerpreset.h"
#include "controllers/midi/midicontrollerpresetfilehandler.h"
#include "controllers/midi/midimessage.h"
#include "controllers/midi/midioutputhandler.h"
#include "controllers/softtakeover.h"

class MidiController : public Controller {
    Q_OBJECT
  public:
    MidiController();
    ~MidiController() override;

    QString presetExtension() override;

    ControllerPresetPointer getPreset() const override {
        MidiControllerPreset* pClone = new MidiControllerPreset();
        *pClone = m_preset;
        return ControllerPresetPointer(pClone);
    }

    bool savePreset(const QString fileName) const override;

    void visit(const MidiControllerPreset* preset) override;
    void visit(const HidControllerPreset* preset) override;

    void accept(ControllerVisitor* visitor) override {
        if (visitor) {
            visitor->visit(this);
        }
    }

    bool isMappable() const override {
        return m_preset.isMappable();
    }

    bool matchPreset(const PresetInfo& preset)  override;

    inline void sendClockMsg() {
        sendShortMsg(MIDI_TIMING_CLK, 0, 0);
    }

  signals:
    void messageReceived(unsigned char status, unsigned char control,
                         unsigned char value);

  protected:
    Q_INVOKABLE virtual void sendShortMsg(unsigned char status,
                                          unsigned char byte1, unsigned char byte2) = 0;

    // Alias for send()
    // The length parameter is here for backwards compatibility for when scripts
    // were required to specify it.
    Q_INVOKABLE inline void sendSysexMsg(QList<int> data, unsigned int length = 0) {
        Q_UNUSED(length);
        send(data);
    }

  protected slots:
    virtual void receive(unsigned char status, unsigned char control,
                         unsigned char value, mixxx::Duration timestamp);
    // For receiving System Exclusive messages
    void receive(const QByteArray data, mixxx::Duration timestamp) override;
    int close() override;

  private slots:
    // Initializes the engine and static output mappings.
    bool applyPreset(QList<QString> scriptPaths, bool initializeScripts) override;

    void learnTemporaryInputMappings(const MidiInputMappings& mappings);
    void clearTemporaryInputMappings();
    void commitTemporaryInputMappings();

  private:
    void startClockGenerator() override;
    void stopClockGenerator() override;

    void processInputMapping(const MidiInputMapping& mapping,
                             unsigned char status,
                             unsigned char control,
                             unsigned char value,
                             mixxx::Duration timestamp);
    void processInputMapping(const MidiInputMapping& mapping,
                             const QByteArray& data,
                             mixxx::Duration timestamp);

    double computeValue(MidiOptions options, double _prevmidivalue, double _newmidivalue);
    void createOutputHandlers();
    void updateAllOutputs();
    void destroyOutputHandlers();

    // Returns a pointer to the currently loaded controller preset. For internal
    // use only.
    ControllerPreset* preset() override {
        return &m_preset;
    }

    QHash<uint16_t, MidiInputMapping> m_temporaryInputMappings;
    QList<MidiOutputHandler*> m_outputs;
    MidiControllerPreset m_preset;
    SoftTakeoverCtrl m_st;
    QList<QPair<MidiInputMapping, unsigned char> > m_fourteen_bit_queued_mappings;
    MidiClockGenerator *m_clockGenerator;


    // So it can access sendShortMsg()
    friend class MidiOutputHandler;
    friend class MidiControllerTest;
};

#endif

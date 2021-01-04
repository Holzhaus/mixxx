#include "macrocontrol.h"

#include "track/track.h"

namespace {
constexpr uint kRecordingTimerInterval = 100;
constexpr size_t kRecordingQueueSize = kRecordingTimerInterval / 10;

const QString recordingSuffix = QStringLiteral(" [Recording]");
const QString interruptedSuffix = QStringLiteral(" [Interrupted]");
} // namespace

ConfigKey MacroControl::getConfigKey(const QString& name) {
    return ConfigKey(m_group, m_controlPattern.arg(name));
}

MacroControl::MacroControl(const QString& group, UserSettingsPointer pConfig, int slot)
        : EngineControl(group, pConfig),
          m_slot(slot),
          m_controlPattern(QString("macro_%1_%2").arg(slot)),
          m_queuedJumpTarget(-1),
          m_recordedActions(kRecordingQueueSize),
          m_iNextAction(0),
          m_COStatus(getConfigKey("status")),
          m_CORecord(getConfigKey("record")),
          m_COPlay(getConfigKey("play")),
          m_COEnable(getConfigKey("enable")),
          m_COLoop(getConfigKey("loop")),
          m_activate(getConfigKey("activate")),
          m_clear(getConfigKey("clear")) {
    m_COStatus.setReadOnly();
    setStatus(Status::NoTrack);

    m_updateRecordingTimer.moveToThread(qApp->thread());
    connect(&m_updateRecordingTimer,
            &QTimer::timeout,
            this,
            &MacroControl::updateRecording);

    m_CORecord.setButtonMode(ControlPushButton::TOGGLE);
    connect(&m_CORecord,
            &ControlObject::valueChanged,
            this,
            &MacroControl::slotRecord);
    m_COPlay.setButtonMode(ControlPushButton::TOGGLE);
    connect(&m_COPlay,
            &ControlObject::valueChanged,
            this,
            &MacroControl::slotPlay);
    m_COEnable.setButtonMode(ControlPushButton::TOGGLE);
    connect(&m_COEnable,
            &ControlObject::valueChanged,
            this,
            &MacroControl::slotEnable);
    m_COLoop.setButtonMode(ControlPushButton::TOGGLE);
    connect(&m_COLoop,
            &ControlObject::valueChanged,
            this,
            &MacroControl::slotLoop);
    m_clear.setButtonMode(ControlPushButton::TRIGGER);
    connect(&m_clear,
            &ControlObject::valueChanged,
            this,
            &MacroControl::slotClear);
    m_activate.setButtonMode(ControlPushButton::TRIGGER);
    connect(&m_activate,
            &ControlObject::valueChanged,
            this,
            &MacroControl::slotActivate);
}

void MacroControl::process(const double dRate, const double dCurrentSample, const int iBufferSize) {
    Q_UNUSED(dRate);
    if (m_queuedJumpTarget >= 0) {
        // if a cue press doesn't change the position, notifySeek isn't called, thus m_queuedJumpTarget isn't reset
        if (getStatus() == Status::Armed) {
            // start the recording on a cue press even when there is no jump
            notifySeek(dCurrentSample);
        }
        m_queuedJumpTarget = -1;
    }
    if (getStatus() != Status::Playing) {
        return;
    }
    double framePos = dCurrentSample / mixxx::kEngineChannelCount;
    const MacroAction& nextAction = m_pMacro->getActions().at(m_iNextAction);
    double nextActionPos = nextAction.sourceFrame;
    int bufFrames = iBufferSize / 2;
    // The process method is called roughly every iBufferSize/2 samples, the
    // tolerance range is double that to be safe. It is ahead of the position
    // because the seek is executed in the next EngineBuffer process cycle.
    if (framePos > nextActionPos - bufFrames && framePos < nextActionPos + bufFrames) {
        seekExact(nextAction.getTargetPositionSample());
        m_iNextAction++;
        if (m_iNextAction == m_pMacro->size()) {
            if (m_pMacro->isLooped()) {
                m_iNextAction = 0;
            } else {
                setStatus(Status::Recorded);
            }
        }
    }
}

void MacroControl::trackLoaded(TrackPointer pNewTrack) {
    if (isRecording()) {
        stopRecording();
        m_pMacro->setLabel(m_pMacro->getLabel().append(interruptedSuffix));
    }
    if (!pNewTrack) {
        m_pMacro = nullptr;
        setStatus(Status::NoTrack);
        return;
    }
    m_pMacro = pNewTrack->getMacros().value(m_slot);
    if (!m_pMacro) {
        m_pMacro = std::make_shared<Macro>();
        pNewTrack->addMacro(m_slot, m_pMacro);
    }
    if (m_pMacro->isEmpty()) {
        setStatus(Status::Empty);
    } else if (m_pMacro->isEnabled()) {
        play();
    } else {
        stop();
    }
}

void MacroControl::notifySeek(double dNewPlaypos) {
    if (m_queuedJumpTarget < 0) {
        return;
    }
    double originalDestFramePos = m_queuedJumpTarget;
    m_queuedJumpTarget = -1;
    if (getStatus() == Status::Armed) {
        setStatus(Status::Recording);
    }
    if (getStatus() != Status::Recording) {
        return;
    }
    double sourceFramePos = getSampleOfTrack().current / mixxx::kEngineChannelCount;
    double destFramePos = dNewPlaypos / mixxx::kEngineChannelCount;
    double diff = originalDestFramePos - destFramePos;
    m_recordedActions.try_emplace(sourceFramePos + diff, originalDestFramePos);
}

void MacroControl::slotJumpQueued(double samplePos) {
    m_queuedJumpTarget = samplePos / mixxx::kEngineChannelCount;
}

MacroControl::Status MacroControl::getStatus() const {
    return Status(m_COStatus.get());
}

void MacroControl::setStatus(Status status) {
    m_COStatus.forceSet(static_cast<int>(status));
    m_COPlay.set(status == Status::Playing ? 1 : 0);
    m_CORecord.set(isRecording() ? 1 : 0);
    // add blinking for Status::Recording & Status::Playing
    //m_COIndicator.forceSet(status > Status::Empty ? 1 : 0);
}

MacroPointer MacroControl::getMacro() const {
    return m_pMacro;
}

bool MacroControl::isRecording() const {
    return getStatus() == Status::Armed || getStatus() == Status::Recording;
}

void MacroControl::play() {
    DEBUG_ASSERT(m_pMacro);
    m_iNextAction = 1;
    setStatus(Status::Playing);
    m_pMacro->setState(Macro::StateFlag::Enabled);
}

void MacroControl::stop() {
    DEBUG_ASSERT(m_pMacro);
    m_iNextAction = INT_MAX;
    setStatus(Status::Recorded);
    m_pMacro->setState(Macro::StateFlag::Enabled, false);
}

void MacroControl::updateRecording() {
    //qCDebug(macroLoggingCategory) << QThread::currentThread() << QTime::currentTime() << "Update recording status:" << getStatus() << "recording:" << isRecording();
    VERIFY_OR_DEBUG_ASSERT(isRecording()) {
        return;
    }
    bool actionsRecorded = false;
    while (MacroAction* action = m_recordedActions.front()) {
        m_pMacro->addAction(*action);
        m_recordedActions.pop();
        actionsRecorded = true;
    }
    if (actionsRecorded && getStatus() == Status::Armed) {
        setStatus(Status::Recording);
    }
}

void MacroControl::stopRecording() {
    VERIFY_OR_DEBUG_ASSERT(isRecording()) {
        return;
    }
    m_updateRecordingTimer.stop();
    updateRecording();
    m_pMacro->setLabel(m_pMacro->getLabel().remove(recordingSuffix));
    if (getStatus() == Status::Armed) {
        setStatus(Status::Empty);
    } else {
        // This will still be the position of the previous track when called from trackLoaded
        // since trackLoaded is invoked before the SampleOfTrack of the controls is updated.
        m_pMacro->setEnd(getSampleOfTrack().current / mixxx::kEngineChannelCount);
        if (m_pMacro->getLabel().isEmpty()) {
            // Automatically set the start position in seconds as label if there
            // is no user-defined one
            double secPos = m_pMacro->getStartSamplePos() /
                    mixxx::kEngineChannelCount / getSampleOfTrack().rate;
            m_pMacro->setLabel(QString::number(secPos, 'f', 2));
        }
        setStatus(Status::Recorded);
        if (m_pMacro->isEnabled()) {
            slotGotoPlay();
        }
    }
}

void MacroControl::slotRecord(double value) {
    if (value == 0) {
        if (isRecording()) {
            stopRecording();
        }
        return;
    }
    switch (getStatus()) {
    case Status::Empty:
        setStatus(Status::Armed);
        break;
    case Status::Recorded:
        setStatus(Status::Recording);
        break;
    default:
        return;
    }
    DEBUG_ASSERT(m_updateRecordingTimer.thread() == QThread::currentThread());
    m_updateRecordingTimer.start(kRecordingTimerInterval);
    m_pMacro->setLabel(m_pMacro->getLabel().append(recordingSuffix));
}

void MacroControl::slotPlay(double value) {
    if (static_cast<bool>(value) == (getStatus() == Status::Playing)) {
        return;
    }
    if (value > 0) {
        play();
    } else {
        stop();
    }
}

void MacroControl::slotEnable(double value) {
    m_pMacro->setState(Macro::StateFlag::Enabled, value != 0);
}

void MacroControl::slotLoop(double value) {
    m_pMacro->setState(Macro::StateFlag::Looped, value != 0);
}

void MacroControl::slotClear(double value) {
    if (value == 0) {
        return;
    }
    if (getStatus() == Status::Recorded) {
        qCDebug(macroLoggingCategory) << "Clearing" << m_slot;
        m_pMacro->clear();
        if (QRegExp("[0-9. ]+(\\[\\w*\\])?").exactMatch(m_pMacro->getLabel())) {
            m_pMacro->setLabel("");
        }
        setStatus(Status::Empty);
    }
}

void MacroControl::slotActivate(double value) {
    if (value == 0) {
        return;
    }
    if (getStatus() < Status::Recorded) {
        slotRecord(!isRecording());
    } else if (getStatus() == Status::Playing) {
        slotGotoPlay();
    } else {
        play();
    }
}

void MacroControl::slotGotoPlay(double value) {
    if (value > 0 && getStatus() > Status::Recording) {
        seekExact(m_pMacro->getStartSamplePos());
        play();
    }
}
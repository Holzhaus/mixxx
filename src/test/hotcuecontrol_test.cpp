#include "engine/controls/cuecontrol.h"
#include "test/signalpathtest.h"

namespace {
mixxx::audio::FrameDiff_t getBeatLengthFrames(TrackPointer pTrack) {
    return (60.0 / pTrack->getBpm()) * pTrack->getSampleRate();
}

} // anonymous namespace

class HotcueControlTest : public BaseSignalPathTest {
  protected:
    void SetUp() override {
        BaseSignalPathTest::SetUp();

        m_pPlay = std::make_unique<ControlProxy>(ConfigKey(m_sGroup1, "play"));
        m_pBeatloopActivate = std::make_unique<ControlFramePosProxy>(
                ConfigKey(m_sGroup1, "beatloop_activate"));
        m_pBeatloopSize = std::make_unique<ControlProxy>(ConfigKey(m_sGroup1, "beatloop_size"));
        m_pLoopStartPosition = std::make_unique<ControlFramePosProxy>(
                ConfigKey(m_sGroup1, "loop_start_position"));
        m_pLoopEndPosition = std::make_unique<ControlFramePosProxy>(
                ConfigKey(m_sGroup1, "loop_end_position"));
        m_pLoopEnabled = std::make_unique<ControlProxy>(ConfigKey(m_sGroup1, "loop_enabled"));
        m_pLoopDouble = std::make_unique<ControlProxy>(ConfigKey(m_sGroup1, "loop_double"));
        m_pLoopHalve = std::make_unique<ControlProxy>(ConfigKey(m_sGroup1, "loop_halve"));
        m_pLoopMove = std::make_unique<ControlProxy>(ConfigKey(m_sGroup1, "loop_move"));
        m_pHotcue1Activate = std::make_unique<ControlProxy>(
                ConfigKey(m_sGroup1, "hotcue_1_activate"));
        m_pHotcue1ActivateCue = std::make_unique<ControlProxy>(
                ConfigKey(m_sGroup1, "hotcue_1_activatecue"));
        m_pHotcue1ActivateLoop = std::make_unique<ControlProxy>(
                ConfigKey(m_sGroup1, "hotcue_1_activateloop"));
        m_pHotcue1Set = std::make_unique<ControlProxy>(ConfigKey(m_sGroup1, "hotcue_1_set"));
        m_pHotcue1SetCue = std::make_unique<ControlProxy>(ConfigKey(m_sGroup1, "hotcue_1_setcue"));
        m_pHotcue1SetLoop = std::make_unique<ControlProxy>(
                ConfigKey(m_sGroup1, "hotcue_1_setloop"));
        m_pHotcue1Goto = std::make_unique<ControlProxy>(ConfigKey(m_sGroup1, "hotcue_1_goto"));
        m_pHotcue1GotoAndPlay = std::make_unique<ControlProxy>(
                ConfigKey(m_sGroup1, "hotcue_1_gotoandplay"));
        m_pHotcue1GotoAndLoop = std::make_unique<ControlProxy>(
                ConfigKey(m_sGroup1, "hotcue_1_gotoandloop"));
        m_pHotcue1CueLoop = std::make_unique<ControlProxy>(
                ConfigKey(m_sGroup1, "hotcue_1_cueloop"));
        m_pHotcue1Position = std::make_unique<ControlFramePosProxy>(
                ConfigKey(m_sGroup1, "hotcue_1_position"));
        m_pHotcue1EndPosition = std::make_unique<ControlFramePosProxy>(
                ConfigKey(m_sGroup1, "hotcue_1_endposition"));
        m_pHotcue1Enabled = std::make_unique<ControlProxy>(
                ConfigKey(m_sGroup1, "hotcue_1_enabled"));
        m_pHotcue1Clear = std::make_unique<ControlProxy>(ConfigKey(m_sGroup1, "hotcue_1_clear"));
        m_pQuantizeEnabled = std::make_unique<ControlProxy>(ConfigKey(m_sGroup1, "quantize"));
    }

    TrackPointer createTestTrack() const {
        const QString kTrackLocationTest = QDir::currentPath() + "/src/test/sine-30.wav";
        const auto pTrack = Track::newTemporary(
                mixxx::FileAccess(mixxx::FileInfo(kTrackLocationTest)));
        pTrack->setAudioProperties(
                mixxx::audio::ChannelCount(2),
                mixxx::audio::SampleRate(44100),
                mixxx::audio::Bitrate(),
                mixxx::Duration::fromSeconds(180));
        return pTrack;
    }

    void loadTrack(TrackPointer pTrack) {
        BaseSignalPathTest::loadTrack(m_pMixerDeck1, pTrack);
        ProcessBuffer();
    }

    TrackPointer loadTestTrackWithBpm(double bpm) {
        DEBUG_ASSERT(!m_pPlay->toBool());
        // Setup fake track with 120 bpm can calculate loop size
        TrackPointer pTrack = createTestTrack();
        pTrack->trySetBpm(bpm);

        loadTrack(pTrack);
        ProcessBuffer();

        return pTrack;
    }

    TrackPointer createAndLoadFakeTrack() {
        TrackPointer pTrack = m_pMixerDeck1->loadFakeTrack(false, 0.0);
        ProcessBuffer();
        return pTrack;
    }

    void unloadTrack() {
        m_pMixerDeck1->slotLoadTrack(TrackPointer(), false);
    }

    mixxx::audio::FramePos currentFramePosition() {
        return m_pChannel1->getEngineBuffer()->m_pCueControl->frameInfo().currentPosition;
    }

    void setCurrentFramePosition(mixxx::audio::FramePos position) {
        m_pChannel1->getEngineBuffer()->queueNewPlaypos(position, EngineBuffer::SEEK_STANDARD);
        ProcessBuffer();
    }

    std::unique_ptr<ControlProxy> m_pPlay;
    std::unique_ptr<ControlProxy> m_pBeatloopActivate;
    std::unique_ptr<ControlProxy> m_pBeatloopSize;
    std::unique_ptr<ControlFramePosProxy> m_pLoopStartPosition;
    std::unique_ptr<ControlFramePosProxy> m_pLoopEndPosition;
    std::unique_ptr<ControlProxy> m_pLoopEnabled;
    std::unique_ptr<ControlProxy> m_pLoopDouble;
    std::unique_ptr<ControlProxy> m_pLoopHalve;
    std::unique_ptr<ControlProxy> m_pLoopMove;
    std::unique_ptr<ControlProxy> m_pHotcue1Activate;
    std::unique_ptr<ControlProxy> m_pHotcue1ActivateCue;
    std::unique_ptr<ControlProxy> m_pHotcue1ActivateLoop;
    std::unique_ptr<ControlProxy> m_pHotcue1Set;
    std::unique_ptr<ControlProxy> m_pHotcue1SetCue;
    std::unique_ptr<ControlProxy> m_pHotcue1SetLoop;
    std::unique_ptr<ControlProxy> m_pHotcue1Goto;
    std::unique_ptr<ControlProxy> m_pHotcue1GotoAndPlay;
    std::unique_ptr<ControlProxy> m_pHotcue1GotoAndLoop;
    std::unique_ptr<ControlProxy> m_pHotcue1CueLoop;
    std::unique_ptr<ControlFramePosProxy> m_pHotcue1Position;
    std::unique_ptr<ControlFramePosProxy> m_pHotcue1EndPosition;
    std::unique_ptr<ControlProxy> m_pHotcue1Enabled;
    std::unique_ptr<ControlProxy> m_pHotcue1Clear;
    std::unique_ptr<ControlProxy> m_pQuantizeEnabled;
};

TEST_F(HotcueControlTest, DefautltControlValues) {
    TrackPointer pTrack = createTestTrack();

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    loadTrack(pTrack);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());
}

TEST_F(HotcueControlTest, NoTrackLoaded) {
    TrackPointer pTrack = createTestTrack();

    m_pHotcue1Set->set(1);
    m_pHotcue1Set->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    m_pHotcue1SetCue->set(1);
    m_pHotcue1SetCue->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    m_pHotcue1Activate->set(1);
    m_pHotcue1Activate->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    m_pHotcue1ActivateCue->set(1);
    m_pHotcue1ActivateCue->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    m_pHotcue1ActivateLoop->set(1);
    m_pHotcue1ActivateLoop->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());
}

TEST_F(HotcueControlTest, SetCueAuto) {
    createAndLoadFakeTrack();

    constexpr mixxx::audio::FramePos cuePosition(100);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    m_pQuantizeEnabled->set(0);
    setCurrentFramePosition(cuePosition);
    ProcessBuffer();

    m_pHotcue1Set->set(1);
    m_pHotcue1Set->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());
}

TEST_F(HotcueControlTest, SetCueManual) {
    createAndLoadFakeTrack();

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    constexpr mixxx::audio::FramePos hotcuePosition(100);

    m_pQuantizeEnabled->set(0);
    setCurrentFramePosition(hotcuePosition);

    m_pHotcue1SetCue->set(1);
    m_pHotcue1SetCue->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(hotcuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());
}

TEST_F(HotcueControlTest, SetLoopAuto) {
    createAndLoadFakeTrack();

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    constexpr mixxx::audio::FramePos loopStartPosition(100);
    constexpr mixxx::audio::FramePos loopEndPosition(200);
    m_pChannel1->getEngineBuffer()->setLoop(loopStartPosition, loopEndPosition, true);

    m_pHotcue1Set->set(1);
    m_pHotcue1Set->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopEndPosition, m_pHotcue1EndPosition->toFramePos());
}

TEST_F(HotcueControlTest, SetLoopManualWithLoop) {
    createAndLoadFakeTrack();

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    constexpr mixxx::audio::FramePos loopStartPosition(100);
    constexpr mixxx::audio::FramePos loopEndPosition(200);
    m_pChannel1->getEngineBuffer()->setLoop(loopStartPosition, loopEndPosition, true);

    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopEndPosition, m_pHotcue1EndPosition->toFramePos());
}

TEST_F(HotcueControlTest, SetLoopManualWithoutLoop) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    const mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(pTrack);
    m_pBeatloopSize->set(4);
    EXPECT_EQ(4, m_pBeatloopSize->get());
    const double beatloopLengthFrames = m_pBeatloopSize->get() * getBeatLengthFrames(pTrack);

    const auto loopStartPosition = mixxx::audio::FramePos(8 * beatLengthFrames);
    setCurrentFramePosition(loopStartPosition);
    ProcessBuffer();

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, currentFramePosition());
    EXPECT_FRAMEPOS_EQ(currentFramePosition(), m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(currentFramePosition() + beatloopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());
}

TEST_F(HotcueControlTest, SetLoopManualWithoutLoopOrBeats) {
    createAndLoadFakeTrack();

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());
}

TEST_F(HotcueControlTest, CueGoto) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    const auto cuePosition = mixxx::audio::FramePos(8 * getBeatLengthFrames(pTrack));

    // Seek to cue Position (8th beat)
    setCurrentFramePosition(cuePosition);
    ProcessBuffer();
    EXPECT_FRAMEPOS_EQ(cuePosition, currentFramePosition());

    m_pHotcue1SetCue->set(1);
    m_pHotcue1SetCue->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Seek to start of track
    setCurrentFramePosition(mixxx::audio::kStartFramePos);
    ProcessBuffer();
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, currentFramePosition());

    m_pHotcue1Goto->set(1);
    m_pHotcue1Goto->set(0);
    ProcessBuffer();
    EXPECT_FRAMEPOS_EQ(cuePosition, currentFramePosition());

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());
    EXPECT_DOUBLE_EQ(0.0, m_pLoopEnabled->get());
}

TEST_F(HotcueControlTest, CueGotoAndPlay) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    const auto cuePosition = mixxx::audio::FramePos(8 * getBeatLengthFrames(pTrack));

    // Seek to cue Position (8th beat)
    setCurrentFramePosition(cuePosition);
    ProcessBuffer();
    EXPECT_FRAMEPOS_EQ(cuePosition, currentFramePosition());

    m_pHotcue1SetCue->set(1);
    m_pHotcue1SetCue->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Seek to start of track
    setCurrentFramePosition(mixxx::audio::kStartFramePos);
    ProcessBuffer();
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, currentFramePosition());

    m_pHotcue1GotoAndPlay->set(1);
    m_pHotcue1GotoAndPlay->set(0);
    ProcessBuffer();
    EXPECT_LE(cuePosition, currentFramePosition());

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());
    EXPECT_DOUBLE_EQ(0.0, m_pLoopEnabled->get());
}

TEST_F(HotcueControlTest, CueGotoAndLoop) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    const mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(pTrack);
    const auto cuePosition = mixxx::audio::FramePos(8 * beatLengthFrames);
    m_pBeatloopSize->set(4);
    const double beatloopLengthFrames = m_pBeatloopSize->get() * getBeatLengthFrames(pTrack);

    // Seek to cue Position (8th beat)
    setCurrentFramePosition(cuePosition);
    ProcessBuffer();
    EXPECT_FRAMEPOS_EQ(cuePosition, currentFramePosition());

    m_pHotcue1SetCue->set(1);
    m_pHotcue1SetCue->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Seek to start of track
    setCurrentFramePosition(mixxx::audio::kStartFramePos);
    ProcessBuffer();
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, currentFramePosition());

    m_pHotcue1GotoAndLoop->set(1);
    m_pHotcue1GotoAndLoop->set(0);
    ProcessBuffer();
    EXPECT_LE(cuePosition, currentFramePosition());

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    EXPECT_DOUBLE_EQ(1.0, m_pLoopEnabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pLoopStartPosition->toFramePos());
    EXPECT_FRAMEPOS_EQ(cuePosition + beatloopLengthFrames, m_pLoopEndPosition->toFramePos());
}

TEST_F(HotcueControlTest, SavedLoopGoto) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    m_pBeatloopSize->set(4);
    const mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(pTrack);
    const mixxx::audio::FrameDiff_t loopLengthFrames = m_pBeatloopSize->get() * beatLengthFrames;
    const auto cuePosition = mixxx::audio::FramePos(8 * beatLengthFrames);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Seek to cue Position (8th beat)
    setCurrentFramePosition(cuePosition);
    ProcessBuffer();
    EXPECT_FRAMEPOS_EQ(cuePosition, currentFramePosition());

    // Set a beatloop this position
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);
    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    EXPECT_DOUBLE_EQ(1.0, m_pLoopEnabled->get());

    // Save loop to hotcue slot 1
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(cuePosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());

    // Disable loop
    m_pLoopEnabled->set(0);
    EXPECT_DOUBLE_EQ(0.0, m_pLoopEnabled->get());
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());

    // Seek to start of track
    setCurrentFramePosition(mixxx::audio::kStartFramePos);
    ProcessBuffer();
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, currentFramePosition());

    m_pHotcue1Goto->set(1);
    m_pHotcue1Goto->set(0);
    ProcessBuffer();
    EXPECT_FRAMEPOS_EQ(cuePosition, currentFramePosition());

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(cuePosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());
    EXPECT_DOUBLE_EQ(0.0, m_pLoopEnabled->get());
}

TEST_F(HotcueControlTest, SavedLoopGotoAndPlay) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    m_pBeatloopSize->set(4);
    const mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(pTrack);
    const mixxx::audio::FrameDiff_t loopLengthFrames = m_pBeatloopSize->get() * beatLengthFrames;
    const auto cuePosition = mixxx::audio::FramePos(8 * beatLengthFrames);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Seek to cue Position (8th beat)
    setCurrentFramePosition(cuePosition);
    ProcessBuffer();
    EXPECT_FRAMEPOS_EQ(cuePosition, currentFramePosition());

    // Set a beatloop this position
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);
    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    EXPECT_DOUBLE_EQ(1.0, m_pLoopEnabled->get());

    // Save loop to hotcue slot 1
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(cuePosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());

    // Disable loop
    m_pLoopEnabled->set(0);
    EXPECT_DOUBLE_EQ(0.0, m_pLoopEnabled->get());
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());

    // Seek to start of track
    setCurrentFramePosition(mixxx::audio::kStartFramePos);
    ProcessBuffer();
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, currentFramePosition());

    m_pHotcue1GotoAndPlay->set(1);
    m_pHotcue1GotoAndPlay->set(0);
    ProcessBuffer();
    EXPECT_LE(cuePosition, currentFramePosition());

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(cuePosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());
    EXPECT_DOUBLE_EQ(0.0, m_pLoopEnabled->get());
}

TEST_F(HotcueControlTest, SavedLoopGotoAndLoop) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    m_pBeatloopSize->set(4);
    const mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(pTrack);
    const mixxx::audio::FrameDiff_t loopLengthFrames = m_pBeatloopSize->get() * beatLengthFrames;
    const auto cuePosition = mixxx::audio::FramePos(8 * beatLengthFrames);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Seek to cue Position (8th beat)
    setCurrentFramePosition(cuePosition);
    ProcessBuffer();
    EXPECT_FRAMEPOS_EQ(cuePosition, currentFramePosition());

    // Set a beatloop this position
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);
    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    EXPECT_DOUBLE_EQ(1.0, m_pLoopEnabled->get());

    // Save loop to hotcue slot 1
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(cuePosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());

    // Disable loop
    m_pLoopEnabled->set(0);
    EXPECT_DOUBLE_EQ(0.0, m_pLoopEnabled->get());
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());

    // Seek to start of track
    setCurrentFramePosition(mixxx::audio::kStartFramePos);
    ProcessBuffer();
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, currentFramePosition());

    m_pHotcue1GotoAndLoop->set(1);
    m_pHotcue1GotoAndLoop->set(0);
    ProcessBuffer();
    EXPECT_LE(cuePosition, currentFramePosition());

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(cuePosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());
    EXPECT_DOUBLE_EQ(1.0, m_pLoopEnabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pLoopStartPosition->toFramePos());
    EXPECT_FRAMEPOS_EQ(cuePosition + loopLengthFrames, m_pLoopEndPosition->toFramePos());
}

TEST_F(HotcueControlTest, SavedLoopStatus) {
    createAndLoadFakeTrack();

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    constexpr auto loopStartPositon = mixxx::audio::FramePos(100);
    constexpr auto loopEndPosition = mixxx::audio::FramePos(200);

    m_pChannel1->getEngineBuffer()->setLoop(loopStartPositon, loopEndPosition, true);

    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    EXPECT_DOUBLE_EQ(1.0, m_pLoopEnabled->get());
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPositon, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopEndPosition, m_pHotcue1EndPosition->toFramePos());

    // Disable Loop
    m_pLoopEnabled->set(0);

    EXPECT_DOUBLE_EQ(0.0, m_pLoopEnabled->get());
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPositon, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopEndPosition, m_pHotcue1EndPosition->toFramePos());

    // Re-Enable Loop
    m_pLoopEnabled->set(1);

    EXPECT_DOUBLE_EQ(1.0, m_pLoopEnabled->get());
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPositon, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopEndPosition, m_pHotcue1EndPosition->toFramePos());

    m_pHotcue1Clear->set(1);
    m_pHotcue1Clear->set(0);

    EXPECT_DOUBLE_EQ(1.0, m_pLoopEnabled->get());
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());
}

TEST_F(HotcueControlTest, SavedLoopScale) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    m_pBeatloopSize->set(4);
    const mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(pTrack);
    const mixxx::audio::FrameDiff_t loopLengthFrames = m_pBeatloopSize->get() * beatLengthFrames;

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Set a beatloop (4 beats)
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);
    m_pPlay->set(1);
    ProcessBuffer();

    // Save currently active loop to hotcue slot 1
    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    // Double loop size (4 => 8 beats)
    m_pLoopDouble->set(1);
    m_pLoopDouble->set(0);
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + 2 * loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    // Halve loop size (8 => 4 beats)
    m_pLoopHalve->set(1);
    m_pLoopHalve->set(0);
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    // Halve loop size (4 => 2 beats)
    m_pLoopHalve->set(1);
    m_pLoopHalve->set(0);
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames / 2,
            m_pHotcue1EndPosition->toFramePos());

    m_pPlay->set(0);
}

TEST_F(HotcueControlTest, SavedLoopMove) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    constexpr double loopSize = 4;
    m_pBeatloopSize->set(loopSize);
    const mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(pTrack);
    const mixxx::audio::FrameDiff_t loopLengthFrames = loopSize * beatLengthFrames;

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Set a beatloop at position 0
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);
    m_pPlay->set(1);

    // Save currently active loop to hotcue slot 1
    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    ProcessBuffer();
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    // Move loop right (0 => 4 beats)
    m_pLoopMove->set(loopSize);
    ProcessBuffer();
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + 2 * loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    // Move loop left (4 => 0 beats)
    m_pLoopMove->set(-loopSize);
    ProcessBuffer();
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    // Move loop left (0 => -4 beats)
    m_pLoopMove->set(-loopSize);
    ProcessBuffer();
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos - loopLengthFrames,
            m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1EndPosition->toFramePos());

    m_pPlay->set(0);
}

TEST_F(HotcueControlTest, SavedLoopNoScaleIfDisabled) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    m_pBeatloopSize->set(4);
    const mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(pTrack);
    const mixxx::audio::FrameDiff_t loopLengthFrames = m_pBeatloopSize->get() * beatLengthFrames;

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Set a beatloop (4 beats)
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);
    m_pPlay->set(1);
    ProcessBuffer();

    // Save currently active loop to hotcue slot 1
    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    // Disable loop
    m_pLoopEnabled->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());

    // Double loop size (4 => 8 beats) while saved loop is disabled
    m_pLoopDouble->set(1);
    m_pLoopDouble->set(0);
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    // Halve loop size (8 => 4 beats) while saved loop is disabled
    m_pLoopHalve->set(1);
    m_pLoopHalve->set(0);
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    // Halve loop size (4 => 2 beats) while saved loop is disabled
    m_pLoopHalve->set(1);
    m_pLoopHalve->set(0);
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    m_pPlay->set(0);
}

TEST_F(HotcueControlTest, SavedLoopNoMoveIfDisabled) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    constexpr double loopSize = 4;
    m_pBeatloopSize->set(loopSize);
    const mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(pTrack);
    const mixxx::audio::FrameDiff_t loopLengthFrames = loopSize * beatLengthFrames;

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Set a beatloop at position 0
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);
    m_pPlay->set(1);

    // Save currently active loop to hotcue slot 1
    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    // Disable Loop
    m_pLoopEnabled->set(0);
    ProcessBuffer();
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());

    // Move loop right (0 => 4 beats) while saved loop is disabled
    m_pLoopMove->set(loopSize);
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    // Move loop left (4 => 0 beats) while saved loop is disabled
    m_pLoopMove->set(-loopSize);
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    // Move loop left (0 => -4 beats) while saved loop is disabled
    m_pLoopMove->set(-loopSize);
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    m_pPlay->set(0);
}

TEST_F(HotcueControlTest, SavedLoopReset) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    m_pBeatloopSize->set(4);
    const mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(pTrack);
    const mixxx::audio::FrameDiff_t loopLengthFrames = m_pBeatloopSize->get() * beatLengthFrames;

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Set a beatloop
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);
    ProcessBuffer();

    // Save currently active loop to hotcue slot 1
    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    const auto loopEndPosition = mixxx::audio::kStartFramePos + loopLengthFrames;

    // Set a new beatloop
    setCurrentFramePosition(loopEndPosition);
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);
    ProcessBuffer();

    // Check if setting the new beatloop disabled the current saved loop
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopEndPosition, m_pHotcue1EndPosition->toFramePos());
}

TEST_F(HotcueControlTest, SavedLoopCueLoopWithExistingLoop) {
    createAndLoadFakeTrack();

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    constexpr auto loopStartPosition = mixxx::audio::FramePos(100);
    constexpr auto loopEndPosition = mixxx::audio::FramePos(200);

    m_pChannel1->getEngineBuffer()->setLoop(loopStartPosition, loopEndPosition, true);

    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    EXPECT_DOUBLE_EQ(1.0, m_pLoopEnabled->get());
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopEndPosition, m_pHotcue1EndPosition->toFramePos());

    // Disable Loop
    m_pHotcue1CueLoop->set(1);
    m_pHotcue1CueLoop->set(0);

    EXPECT_DOUBLE_EQ(0.0, m_pLoopEnabled->get());
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopEndPosition, m_pHotcue1EndPosition->toFramePos());

    // Re-Enable Loop
    m_pHotcue1CueLoop->set(1);
    m_pHotcue1CueLoop->set(0);

    EXPECT_DOUBLE_EQ(1.0, m_pLoopEnabled->get());
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopEndPosition, m_pHotcue1EndPosition->toFramePos());
}

TEST_F(HotcueControlTest, CueLoopWithoutHotcueSetsHotcue) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    m_pHotcue1CueLoop->set(1);
    m_pHotcue1CueLoop->set(0);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());
    EXPECT_TRUE(m_pLoopEnabled->toBool());
}

TEST_F(HotcueControlTest, CueLoopWithSavedLoopToggles) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_NE(Cue::kNoPosition, m_pHotcue1EndPosition->get());
    EXPECT_TRUE(m_pLoopEnabled->toBool());

    m_pHotcue1CueLoop->set(1);
    m_pHotcue1CueLoop->set(0);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_NE(Cue::kNoPosition, m_pHotcue1EndPosition->get());
    EXPECT_FALSE(m_pLoopEnabled->toBool());

    m_pHotcue1CueLoop->set(1);
    m_pHotcue1CueLoop->set(0);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_NE(Cue::kNoPosition, m_pHotcue1EndPosition->get());
    EXPECT_TRUE(m_pLoopEnabled->toBool());
}

TEST_F(HotcueControlTest, CueLoopWithoutLoopOrBeats) {
    createAndLoadFakeTrack();

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    m_pHotcue1CueLoop->set(1);
    m_pHotcue1CueLoop->set(0);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());
    EXPECT_FALSE(m_pLoopEnabled->toBool());
}

TEST_F(HotcueControlTest, SavedLoopToggleDoesNotSeek) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    constexpr double loopSize = 4;
    const mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(pTrack);
    const mixxx::audio::FrameDiff_t loopLengthFrames = loopSize * beatLengthFrames;

    const auto beforeLoopPosition = mixxx::audio::kStartFramePos;
    const auto loopStartPosition = mixxx::audio::FramePos(8 * beatLengthFrames);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Seek to loop start position
    setCurrentFramePosition(loopStartPosition);
    ProcessBuffer();

    m_pPlay->set(1);

    // Set a beatloop
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);

    // Save currently active loop to hotcue slot 1
    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopStartPosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());

    // Seek to start of track
    setCurrentFramePosition(beforeLoopPosition);
    EXPECT_NEAR(beforeLoopPosition.value(), currentFramePosition().value(), 1024);

    // Check that the previous seek disabled the loop
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopStartPosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());

    // Re-Enable loop
    m_pHotcue1Activate->set(1);
    m_pHotcue1Activate->set(0);
    ProcessBuffer();
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopStartPosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());

    // Check that re-enabling loop didn't seek
    EXPECT_NEAR(beforeLoopPosition.value(), currentFramePosition().value(), 1024);

    // Disable loop
    m_pHotcue1Activate->set(1);
    m_pHotcue1Activate->set(0);
    ProcessBuffer();
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopStartPosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());
}

TEST_F(HotcueControlTest, SavedLoopActivate) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    m_pBeatloopSize->set(4);
    const mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(pTrack);
    const mixxx::audio::FrameDiff_t loopLengthFrames = m_pBeatloopSize->get() * beatLengthFrames;

    const auto beforeLoopPosition = mixxx::audio::kStartFramePos;
    const auto loopStartPosition = mixxx::audio::FramePos(8 * beatLengthFrames);
    const auto afterLoopPosition = mixxx::audio::FramePos(16 * beatLengthFrames);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Seek to loop start position
    setCurrentFramePosition(loopStartPosition);
    ProcessBuffer();

    // Set a beatloop
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);

    m_pPlay->set(1);

    // Save currently active loop to hotcue slot 1
    m_pHotcue1Activate->set(1);
    m_pHotcue1Activate->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopStartPosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());

    // Seek to start of track
    setCurrentFramePosition(beforeLoopPosition);
    auto positionBeforeActivate = currentFramePosition();
    EXPECT_NEAR(beforeLoopPosition.value(), currentFramePosition().value(), 1000);

    // Check that the previous seek disabled the loop
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopStartPosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());

    // Activate saved loop (does not imply seeking to loop start)
    m_pHotcue1Activate->set(1);
    m_pHotcue1Activate->set(0);
    ProcessBuffer();
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopStartPosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());
    EXPECT_NEAR(positionBeforeActivate.value(), currentFramePosition().value(), 1000);

    // Seek to position after saved loop
    setCurrentFramePosition(afterLoopPosition);
    ProcessBuffer();

    // Check that the previous seek disabled the loop
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopStartPosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());

    positionBeforeActivate = currentFramePosition();

    // Activate saved loop (usually doesn't imply seeking to loop start, but in this case it does
    // because the play position is behind the loop end position)
    m_pHotcue1Activate->set(1);
    m_pHotcue1Activate->set(0);
    ProcessBuffer();
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopStartPosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());
    EXPECT_NEAR(loopStartPosition.value(), currentFramePosition().value(), 1000);
}

TEST_F(HotcueControlTest, SavedLoopActivateWhilePlayingTogglesLoop) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    m_pBeatloopSize->set(4);
    const mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(pTrack);
    const mixxx::audio::FrameDiff_t loopLengthFrames = m_pBeatloopSize->get() * beatLengthFrames;
    const auto loopStartPosition = mixxx::audio::FramePos(8 * beatLengthFrames);
    const auto loopEndPosition = loopStartPosition + loopLengthFrames;

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Set a beatloop
    setCurrentFramePosition(loopStartPosition);
    ProcessBuffer();
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);

    m_pQuantizeEnabled->set(1);
    m_pPlay->set(1);
    ProcessBuffer();

    // Save currently active loop to hotcue slot 1
    m_pHotcue1Activate->set(1);
    m_pHotcue1Activate->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(loopStartPosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopEndPosition, m_pHotcue1EndPosition->toFramePos());
    EXPECT_DOUBLE_EQ(1.0, m_pLoopEnabled->get());
    EXPECT_DOUBLE_EQ(m_pHotcue1Position->get(), m_pLoopStartPosition->get());
    EXPECT_DOUBLE_EQ(m_pHotcue1EndPosition->get(), m_pLoopEndPosition->get());

    m_pHotcue1Activate->set(1);
    m_pHotcue1Activate->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_DOUBLE_EQ(0.0, m_pLoopEnabled->get());

    m_pHotcue1Activate->set(1);
    m_pHotcue1Activate->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_DOUBLE_EQ(1.0, m_pLoopEnabled->get());
}

TEST_F(HotcueControlTest, SavedLoopBeatLoopSizeRestore) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    constexpr double savedLoopSize = 8;
    m_pBeatloopSize->set(savedLoopSize);
    const mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(pTrack);
    const mixxx::audio::FrameDiff_t loopLengthFrames = m_pBeatloopSize->get() * beatLengthFrames;

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Set a beatloop
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);

    m_pPlay->set(1);

    // Save currently active loop to hotcue slot 1
    m_pHotcue1ActivateLoop->set(1);
    m_pHotcue1ActivateLoop->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    // Disable loop
    m_pLoopEnabled->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    // Set new beatloop size
    m_pBeatloopSize->set(savedLoopSize / 2);

    // Re-enabled saved loop
    m_pHotcue1ActivateLoop->set(1);
    m_pHotcue1ActivateLoop->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos + loopLengthFrames,
            m_pHotcue1EndPosition->toFramePos());

    // Check that saved loop's beatloop size has been restored
    EXPECT_DOUBLE_EQ(savedLoopSize, m_pBeatloopSize->get());
}

TEST_F(HotcueControlTest, SavedLoopBeatLoopSizeRestoreDoesNotJump) {
    // Setup fake track with 120 bpm and calculate loop size
    TrackPointer pTrack = loadTestTrackWithBpm(120.0);

    constexpr double savedLoopSize = 4;
    m_pBeatloopSize->set(savedLoopSize);
    const mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(pTrack);
    const mixxx::audio::FrameDiff_t loopLengthFrames = m_pBeatloopSize->get() * beatLengthFrames;
    const auto cuePosition = mixxx::audio::FramePos(8 * beatLengthFrames);
    const auto beforeLoopPosition = mixxx::audio::kStartFramePos;
    const auto afterLoopPosition = mixxx::audio::FramePos(beatLengthFrames);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Seek to cue Position (8th beat)
    setCurrentFramePosition(cuePosition);
    ProcessBuffer();
    EXPECT_FRAMEPOS_EQ(cuePosition, currentFramePosition());

    // Set a beatloop
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);

    // Save currently active loop to hotcue slot 1
    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(cuePosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());

    m_pPlay->set(1);

    // Check 1: Play position before saved loop

    // Disable loop
    m_pLoopEnabled->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(cuePosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());

    // Set new beatloop size
    m_pBeatloopSize->set(m_pBeatloopSize->get() / 2);

    // Seek to position before saved loop
    setCurrentFramePosition(beforeLoopPosition);

    // Re-enable saved loop
    m_pHotcue1Activate->set(1);
    m_pHotcue1Activate->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(cuePosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());

    // Check that saved loop's beatloop size has been restored
    EXPECT_DOUBLE_EQ(savedLoopSize, m_pBeatloopSize->get());

    // Check that enabling the loop didn't cause a jump
    EXPECT_NEAR(beforeLoopPosition.value(), currentFramePosition().value(), 1000);

    // Check 2: Play position after saved loop

    // Disable loop
    m_pLoopEnabled->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Set), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(cuePosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());

    // Set new beatloop size
    m_pBeatloopSize->set(m_pBeatloopSize->get() / 2);

    // Seek to position after saved loop
    setCurrentFramePosition(afterLoopPosition);

    // Re-enable saved loop
    m_pHotcue1Activate->set(1);
    m_pHotcue1Activate->set(0);
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(cuePosition, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(cuePosition + loopLengthFrames, m_pHotcue1EndPosition->toFramePos());

    // Check that saved loop's beatloop size has been restored
    EXPECT_DOUBLE_EQ(savedLoopSize, m_pBeatloopSize->get());

    // Check that enabling the loop didn't cause a jump
    EXPECT_NEAR(afterLoopPosition.value(), currentFramePosition().value(), 1000);
}

TEST_F(HotcueControlTest, SavedLoopUnloadTrackWhileActive) {
    // Setup fake track with 120 bpm
    qWarning() << "Loading first track";
    loadTestTrackWithBpm(120.0);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Set a beatloop
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);
    ProcessBuffer();

    // Save currently active loop to hotcue slot 1
    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    ProcessBuffer();
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_NE(Cue::kNoPosition, m_pHotcue1Position->get());
    EXPECT_NE(Cue::kNoPosition, m_pHotcue1EndPosition->get());

    // Setup another fake track with 130 bpm
    unloadTrack();
    qWarning() << "Loading second track";
    loadTestTrackWithBpm(130.0);
    ProcessBuffer();

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());
}

TEST_F(HotcueControlTest, SavedLoopUseLoopInOutWhileActive) {
    std::unique_ptr<ControlProxy> pLoopIn = std::make_unique<ControlProxy>(m_sGroup1, "loop_in");
    std::unique_ptr<ControlProxy> pLoopOut = std::make_unique<ControlProxy>(m_sGroup1, "loop_out");

    // Setup fake track with 120 bpm
    loadTestTrackWithBpm(120.0);

    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Empty), m_pHotcue1Enabled->get());
    EXPECT_FALSE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_FALSE(m_pHotcue1EndPosition->toFramePos().isValid());

    // Set a beatloop
    m_pBeatloopActivate->set(1);
    m_pBeatloopActivate->set(0);
    ProcessBuffer();

    // Save currently active loop to hotcue slot 1
    m_pHotcue1SetLoop->set(1);
    m_pHotcue1SetLoop->set(0);
    ProcessBuffer();
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_TRUE(m_pHotcue1Position->toFramePos().isValid());
    EXPECT_TRUE(m_pHotcue1EndPosition->toFramePos().isValid());

    constexpr auto loopEndPosition = mixxx::audio::FramePos(1000);

    setCurrentFramePosition(mixxx::audio::kStartFramePos);

    pLoopIn->set(1);
    pLoopIn->set(0);
    ProcessBuffer();

    setCurrentFramePosition(loopEndPosition);

    pLoopOut->set(1);
    pLoopOut->set(0);

    ProcessBuffer();
    EXPECT_DOUBLE_EQ(static_cast<double>(HotcueControl::Status::Active), m_pHotcue1Enabled->get());
    EXPECT_FRAMEPOS_EQ(mixxx::audio::kStartFramePos, m_pHotcue1Position->toFramePos());
    EXPECT_FRAMEPOS_EQ(loopEndPosition, m_pHotcue1EndPosition->toFramePos());
}

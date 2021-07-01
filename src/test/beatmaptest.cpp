#include <gtest/gtest.h>
#include <memory.h>

#include <QtDebug>

#include "track/beatmap.h"
#include "track/track.h"

using namespace mixxx;

namespace {

class BeatMapTest : public testing::Test {
  protected:
    BeatMapTest()
            : m_pTrack(Track::newTemporary()),
              m_iSampleRate(10000),
              m_iFrameSize(2) {
        m_pTrack->setAudioProperties(
                mixxx::audio::ChannelCount(2),
                mixxx::audio::SampleRate(m_iSampleRate),
                mixxx::audio::Bitrate(),
                mixxx::Duration::fromSeconds(180));
    }

    mixxx::audio::FrameDiff_t getBeatLengthFrames(mixxx::Bpm bpm) {
        return (60.0 * m_iSampleRate / bpm.getValue());
    }

    double getBeatLengthSamples(mixxx::Bpm bpm) {
        return getBeatLengthFrames(bpm) * m_iFrameSize;
    }

    QVector<mixxx::audio::FramePos> createBeatVector(mixxx::audio::FramePos first_beat,
            unsigned int num_beats,
            mixxx::audio::FrameDiff_t beat_length) {
        QVector<mixxx::audio::FramePos> beats;
        for (unsigned int i = 0; i < num_beats; ++i) {
            beats.append(first_beat + i * beat_length);
        }
        return beats;
    }

    TrackPointer m_pTrack;
    int m_iSampleRate;
    int m_iFrameSize;
};

TEST_F(BeatMapTest, Scale) {
    constexpr mixxx::Bpm bpm(60.0);
    m_pTrack->trySetBpm(bpm.getValue());
    mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(bpm);
    const auto startOffsetFrames = mixxx::audio::FramePos(7);
    const int numBeats = 100;
    // Note beats must be in frames, not samples.
    QVector<mixxx::audio::FramePos> beats =
            createBeatVector(startOffsetFrames, numBeats, beatLengthFrames);
    auto pMap = BeatMap::makeBeatMap(m_pTrack->getSampleRate(), QString(), beats);

    EXPECT_DOUBLE_EQ(bpm.getValue(), pMap->getBpm().getValue());
    pMap = pMap->scale(Beats::DOUBLE);
    EXPECT_DOUBLE_EQ(2 * bpm.getValue(), pMap->getBpm().getValue());

    pMap = pMap->scale(Beats::HALVE);
    EXPECT_DOUBLE_EQ(bpm.getValue(), pMap->getBpm().getValue());

    pMap = pMap->scale(Beats::TWOTHIRDS);
    EXPECT_DOUBLE_EQ(bpm.getValue() * 2 / 3, pMap->getBpm().getValue());

    pMap = pMap->scale(Beats::THREEHALVES);
    EXPECT_DOUBLE_EQ(bpm.getValue(), pMap->getBpm().getValue());

    pMap = pMap->scale(Beats::THREEFOURTHS);
    EXPECT_DOUBLE_EQ(bpm.getValue() * 3 / 4, pMap->getBpm().getValue());

    pMap = pMap->scale(Beats::FOURTHIRDS);
    EXPECT_DOUBLE_EQ(bpm.getValue(), pMap->getBpm().getValue());
}

TEST_F(BeatMapTest, TestNthBeat) {
    constexpr mixxx::Bpm bpm(60.0);
    m_pTrack->trySetBpm(bpm.getValue());
    mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(bpm);
    const auto startOffsetFrames = mixxx::audio::FramePos(7);
    double beatLengthSamples = getBeatLengthSamples(bpm);
    const double startOffsetSamples = startOffsetFrames.toEngineSamplePos();
    const int numBeats = 100;
    // Note beats must be in frames, not samples.
    QVector<mixxx::audio::FramePos> beats =
            createBeatVector(startOffsetFrames, numBeats, beatLengthFrames);
    auto pMap = BeatMap::makeBeatMap(m_pTrack->getSampleRate(), QString(), beats);

    // Check edge cases
    double firstBeat = startOffsetSamples + beatLengthSamples * 0;
    double lastBeat = startOffsetSamples + beatLengthSamples * (numBeats - 1);
    EXPECT_EQ(lastBeat, pMap->findNthBeat(lastBeat, 1));
    EXPECT_EQ(lastBeat, pMap->findNextBeat(lastBeat));
    EXPECT_EQ(-1, pMap->findNthBeat(lastBeat, 2));
    EXPECT_EQ(firstBeat, pMap->findNthBeat(firstBeat, -1));
    EXPECT_EQ(firstBeat, pMap->findPrevBeat(firstBeat));
    EXPECT_EQ(-1, pMap->findNthBeat(firstBeat, -2));

    double prevBeat, nextBeat;
    pMap->findPrevNextBeats(lastBeat, &prevBeat, &nextBeat, true);
    EXPECT_EQ(lastBeat, prevBeat);
    EXPECT_EQ(-1, nextBeat);

    pMap->findPrevNextBeats(firstBeat, &prevBeat, &nextBeat, true);
    EXPECT_EQ(firstBeat, prevBeat);
    EXPECT_EQ(firstBeat + beatLengthSamples, nextBeat);
}

TEST_F(BeatMapTest, TestNthBeatWhenOnBeat) {
    constexpr mixxx::Bpm bpm(60.0);
    m_pTrack->trySetBpm(bpm.getValue());
    mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(bpm);
    const auto startOffsetFrames = mixxx::audio::FramePos(7);
    double beatLengthSamples = getBeatLengthSamples(bpm);
    const double startOffsetSamples = startOffsetFrames.toEngineSamplePos();
    const int numBeats = 100;
    // Note beats must be in frames, not samples.
    QVector<mixxx::audio::FramePos> beats =
            createBeatVector(startOffsetFrames, numBeats, beatLengthFrames);
    auto pMap = BeatMap::makeBeatMap(m_pTrack->getSampleRate(), QString(), beats);

    // Pretend we're on the 20th beat;
    const int curBeat = 20;
    double position = startOffsetSamples + beatLengthSamples * curBeat;

    // The spec dictates that a value of 0 is always invalid and returns -1
    EXPECT_EQ(-1, pMap->findNthBeat(position, 0));

    // findNthBeat should return exactly the current beat if we ask for 1 or
    // -1. For all other values, it should return n times the beat length.
    for (int i = 1; i < curBeat; ++i) {
        EXPECT_DOUBLE_EQ(position + beatLengthSamples*(i-1), pMap->findNthBeat(position, i));
        EXPECT_DOUBLE_EQ(position + beatLengthSamples*(-i+1), pMap->findNthBeat(position, -i));
    }

    // Also test prev/next beat calculation.
    double prevBeat, nextBeat;
    pMap->findPrevNextBeats(position, &prevBeat, &nextBeat, true);
    EXPECT_EQ(position, prevBeat);
    EXPECT_EQ(position + beatLengthSamples, nextBeat);

    // Also test prev/next beat calculation without snapping tolerance
    pMap->findPrevNextBeats(position, &prevBeat, &nextBeat, false);
    EXPECT_EQ(position, prevBeat);
    EXPECT_EQ(position + beatLengthSamples, nextBeat);

    // Both previous and next beat should return the current position.
    EXPECT_EQ(position, pMap->findNextBeat(position));
    EXPECT_EQ(position, pMap->findPrevBeat(position));
}

TEST_F(BeatMapTest, TestNthBeatWhenOnBeat_BeforeEpsilon) {
    constexpr mixxx::Bpm bpm(60.0);
    m_pTrack->trySetBpm(bpm.getValue());
    mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(bpm);
    const auto startOffsetFrames = mixxx::audio::FramePos(7);
    double beatLengthSamples = getBeatLengthSamples(bpm);
    const double startOffsetSamples = startOffsetFrames.toEngineSamplePos();
    const int numBeats = 100;
    // Note beats must be in frames, not samples.
    QVector<mixxx::audio::FramePos> beats =
            createBeatVector(startOffsetFrames, numBeats, beatLengthFrames);
    auto pMap = BeatMap::makeBeatMap(m_pTrack->getSampleRate(), QString(), beats);

    // Pretend we're just before the 20th beat;
    const int curBeat = 20;
    const double kClosestBeat = startOffsetSamples + curBeat * beatLengthSamples;
    double position = kClosestBeat - beatLengthSamples * 0.005;

    // The spec dictates that a value of 0 is always invalid and returns -1
    EXPECT_EQ(-1, pMap->findNthBeat(position, 0));

    // findNthBeat should return exactly the current beat if we ask for 1 or
    // -1. For all other values, it should return n times the beat length.
    for (int i = 1; i < curBeat; ++i) {
        EXPECT_DOUBLE_EQ(kClosestBeat + beatLengthSamples*(i-1), pMap->findNthBeat(position, i));
        EXPECT_DOUBLE_EQ(kClosestBeat + beatLengthSamples*(-i+1), pMap->findNthBeat(position, -i));
    }

    // Also test prev/next beat calculation
    double prevBeat, nextBeat;
    pMap->findPrevNextBeats(position, &prevBeat, &nextBeat, true);
    EXPECT_EQ(kClosestBeat, prevBeat);
    EXPECT_EQ(kClosestBeat + beatLengthSamples, nextBeat);

    // Also test prev/next beat calculation without snapping tolerance
    pMap->findPrevNextBeats(position, &prevBeat, &nextBeat, false);
    EXPECT_EQ(kClosestBeat - beatLengthSamples, prevBeat);
    EXPECT_EQ(kClosestBeat, nextBeat);

    // Both previous and next beat should return the closest beat.
    EXPECT_EQ(kClosestBeat, pMap->findNextBeat(position));
    EXPECT_EQ(kClosestBeat, pMap->findPrevBeat(position));

}

TEST_F(BeatMapTest, TestNthBeatWhenOnBeat_AfterEpsilon) {
    constexpr mixxx::Bpm bpm(60.0);
    m_pTrack->trySetBpm(bpm.getValue());
    mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(bpm);
    const auto startOffsetFrames = mixxx::audio::FramePos(7);
    double beatLengthSamples = getBeatLengthSamples(bpm);
    const double startOffsetSamples = startOffsetFrames.toEngineSamplePos();
    const int numBeats = 100;
    // Note beats must be in frames, not samples.
    QVector<mixxx::audio::FramePos> beats =
            createBeatVector(startOffsetFrames, numBeats, beatLengthFrames);
    auto pMap = BeatMap::makeBeatMap(m_pTrack->getSampleRate(), QString(), beats);

    // Pretend we're just after the 20th beat;
    const int curBeat = 20;
    const double kClosestBeat = startOffsetSamples + curBeat * beatLengthSamples;
    double position = kClosestBeat + beatLengthSamples * 0.005;

    // The spec dictates that a value of 0 is always invalid and returns -1
    EXPECT_EQ(-1, pMap->findNthBeat(position, 0));

    EXPECT_EQ(kClosestBeat, pMap->findClosestBeat(position));

    // findNthBeat should return exactly the current beat if we ask for 1 or
    // -1. For all other values, it should return n times the beat length.
    for (int i = 1; i < curBeat; ++i) {
        EXPECT_DOUBLE_EQ(kClosestBeat + beatLengthSamples*(i-1), pMap->findNthBeat(position, i));
        EXPECT_DOUBLE_EQ(kClosestBeat + beatLengthSamples*(-i+1), pMap->findNthBeat(position, -i));
    }

    // Also test prev/next beat calculation.
    double prevBeat, nextBeat;
    pMap->findPrevNextBeats(position, &prevBeat, &nextBeat, true);
    EXPECT_EQ(kClosestBeat, prevBeat);
    EXPECT_EQ(kClosestBeat + beatLengthSamples, nextBeat);

    // Also test prev/next beat calculation without snapping tolerance
    pMap->findPrevNextBeats(position, &prevBeat, &nextBeat, false);
    EXPECT_EQ(kClosestBeat, prevBeat);
    EXPECT_EQ(kClosestBeat + beatLengthSamples, nextBeat);

    // Both previous and next beat should return the closest beat.
    EXPECT_EQ(kClosestBeat, pMap->findNextBeat(position));
    EXPECT_EQ(kClosestBeat, pMap->findPrevBeat(position));
}

TEST_F(BeatMapTest, TestNthBeatWhenNotOnBeat) {
    constexpr mixxx::Bpm bpm(60.0);
    m_pTrack->trySetBpm(bpm.getValue());
    mixxx::audio::FrameDiff_t beatLengthFrames = getBeatLengthFrames(bpm);
    const auto startOffsetFrames = mixxx::audio::FramePos(7);
    double beatLengthSamples = getBeatLengthSamples(bpm);
    const double startOffsetSamples = startOffsetFrames.toEngineSamplePos();
    const int numBeats = 100;
    // Note beats must be in frames, not samples.
    QVector<mixxx::audio::FramePos> beats =
            createBeatVector(startOffsetFrames, numBeats, beatLengthFrames);
    auto pMap = BeatMap::makeBeatMap(m_pTrack->getSampleRate(), QString(), beats);

    // Pretend we're half way between the 20th and 21st beat
    double previousBeat = startOffsetSamples + beatLengthSamples * 20.0;
    double nextBeat = startOffsetSamples + beatLengthSamples * 21.0;
    double position = (nextBeat + previousBeat) / 2.0;

    // The spec dictates that a value of 0 is always invalid and returns -1
    EXPECT_EQ(-1, pMap->findNthBeat(position, 0));

    // findNthBeat should return multiples of beats starting from the next or
    // previous beat, depending on whether N is positive or negative.
    for (int i = 1; i < 20; ++i) {
        EXPECT_DOUBLE_EQ(nextBeat + beatLengthSamples*(i-1),
                         pMap->findNthBeat(position, i));
        EXPECT_DOUBLE_EQ(previousBeat - beatLengthSamples*(i-1),
                         pMap->findNthBeat(position, -i));
    }

    // Also test prev/next beat calculation
    double foundPrevBeat, foundNextBeat;
    pMap->findPrevNextBeats(position, &foundPrevBeat, &foundNextBeat, true);
    EXPECT_EQ(previousBeat, foundPrevBeat);
    EXPECT_EQ(nextBeat, foundNextBeat);

    // Also test prev/next beat calculation without snapping tolerance
    pMap->findPrevNextBeats(position, &foundPrevBeat, &foundNextBeat, false);
    EXPECT_EQ(previousBeat, foundPrevBeat);
    EXPECT_EQ(nextBeat, foundNextBeat);
}

TEST_F(BeatMapTest, TestBpmAround) {
    constexpr mixxx::Bpm filebpm(60.0);
    double approx_beat_length = getBeatLengthSamples(filebpm);
    m_pTrack->trySetBpm(filebpm.getValue());
    const int numBeats = 64;

    QVector<mixxx::audio::FramePos> beats;
    mixxx::audio::FramePos beat_pos = mixxx::audio::kStartFramePos;
    for (unsigned int i = 0, bpmValue = 60; i < numBeats; ++i, ++bpmValue) {
        const mixxx::audio::FrameDiff_t beat_length = getBeatLengthFrames(mixxx::Bpm(bpmValue));
        beats.append(beat_pos);
        beat_pos += beat_length;
    }

    auto pMap = BeatMap::makeBeatMap(m_pTrack->getSampleRate(), QString(), beats);

    // The average of the first 8 beats should be different than the average
    // of the last 8 beats.
    EXPECT_DOUBLE_EQ(63.937645572318047,
            pMap->getBpmAroundPosition(4 * approx_beat_length, 4).getValue());
    EXPECT_DOUBLE_EQ(118.96668932698844,
            pMap->getBpmAroundPosition(60 * approx_beat_length, 4).getValue());
    // Also test at the beginning and end of the track
    EXPECT_DOUBLE_EQ(62.937377309576974,
            pMap->getBpmAroundPosition(0, 4).getValue());
    EXPECT_DOUBLE_EQ(118.96668932698844,
            pMap->getBpmAroundPosition(65 * approx_beat_length, 4).getValue());

    // Try a really, really short track
    constexpr auto startFramePos = mixxx::audio::FramePos(10);
    beats = createBeatVector(startFramePos, 3, getBeatLengthFrames(filebpm));
    pMap = BeatMap::makeBeatMap(m_pTrack->getSampleRate(), QString(), beats);
    EXPECT_DOUBLE_EQ(filebpm.getValue(),
            pMap->getBpmAroundPosition(1 * approx_beat_length, 4).getValue());
}

}  // namespace

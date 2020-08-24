#pragma once

#include <QByteArray>
#include <QList>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QVector>
#include <memory>

#include "proto/beats.pb.h"
#include "track/beat.h"
#include "track/bpm.h"
#include "track/frame.h"
#include "util/types.h"

namespace mixxx {

class Beats;
class BeatIterator;
using BeatsPointer = std::shared_ptr<Beats>;
} // namespace mixxx

#include "track/beatiterator.h"
#include "track/timesignature.h"

class Track;

namespace mixxx {
/// This is an intermediate class which encapsulates the beats into a
/// plain copyable, movable object.
class BeatsInternal {
  public:
    BeatsInternal();
    void initWithProtobuf(const QByteArray& byteArray);
    void initWithAnalyzer(const QVector<FramePos>& beats,
            const QVector<track::io::TimeSignatureMarker>& timeSignatureMarkers);

    enum BPMScale {
        DOUBLE,
        HALVE,
        TWOTHIRDS,
        THREEFOURTHS,
        FOURTHIRDS,
        THREEHALVES,
    };

    using iterator = BeatIterator;

    static const QString BEAT_MAP_VERSION;
    static const QString BEAT_GRID_1_VERSION;
    static const QString BEAT_GRID_2_VERSION;
    static const QString BEATS_VERSION;
    Beat findNthBeat(FramePos frame, int offset) const;
    Beat findNextBeat(FramePos frame) const;
    Beat findPrevBeat(FramePos frame) const;
    Bpm getBpm() const;
    bool isValid() const;
    void setSampleRate(int sampleRate);
    void setDurationSeconds(double duration) {
        m_dDurationSeconds = duration;
    }
    int numBeatsInRange(FramePos startFrame, FramePos endFrame) const;
    QByteArray toProtobuf() const;
    QString getVersion() const;
    QString getSubVersion() const;
    void setSubVersion(const QString& subVersion);
    Bpm calculateBpm(const Beat& startBeat,
            const Beat& stopBeat) const;
    void scale(enum BPMScale scale);
    FramePos findNBeatsFromFrame(FramePos fromFrame, double beats) const;
    bool findPrevNextBeats(FramePos frame,
            FramePos* pPrevBeatFrame,
            FramePos* pNextBeatFrame) const;
    void setGrid(Bpm dBpm, FramePos firstBeatFrame = kStartFramePos);
    FramePos findClosestBeat(FramePos frame) const;
    std::unique_ptr<BeatsInternal::iterator> findBeats(
            FramePos startFrame, FramePos stopFrame) const;
    Bpm getBpmAroundPosition(FramePos curFrame, int n) const;
    void setSignature(TimeSignature sig, int downbeatIndex);
    void translate(FrameDiff_t numFrames);
    void setBpm(Bpm bpm, int beatIndex = 0);
    inline int size() {
        return m_beats.size();
    }
    FramePos getFirstBeatPosition() const;
    FramePos getLastBeatPosition() const;
    Beat getBeatAtIndex(int index) {
        return m_beats.at(index);
    }
    void setAsDownbeat(int beatIndex);

  private:
    void updateBpm();
    void scaleDouble();
    void scaleTriple();
    void scaleQuadruple();
    void scaleHalve();
    void scaleThird();
    void scaleFourth();
    void scaleMultiple(uint multiple);
    void scaleFraction(uint fraction);
    void generateBeatsFromMarkers();
    void clearMarkers();

    QString m_subVersion;
    Bpm m_bpm;
    BeatList m_beats;
    track::io::Beats m_beatsProto;
    int m_iSampleRate;
    double m_dDurationSeconds;
    friend QDebug operator<<(QDebug dbg, const BeatsInternal& arg);
};

/// Beats is a class for BPM and beat management classes.
/// It stores beats information including beats position, down beats position,
/// phrase beat position and changes in tempo.
class Beats final : public QObject {
    Q_OBJECT
  public:
    /// Initialize beats with only the track pointer.
    explicit Beats(const Track* track);
    /// The source of this byte array is the serialized representation of beats
    /// generated by the protocol buffer and stored in the database.
    Beats(const Track* track, const QByteArray& byteArray);
    /// A list of beat locations in audio frames may be provided.
    /// The source of this data is the analyzer.
    Beats(const Track* track,
            const QVector<FramePos>& beats,
            const QVector<track::io::TimeSignatureMarker>&
                    timeSignatureMarkers =
                            QVector<track::io::TimeSignatureMarker>());
    ~Beats() override = default;

    using iterator = BeatIterator;

    // TODO(JVC) Is a copy constructor needed? of we can force a move logic??
    Beats(const mixxx::Beats& other);

    /// Serializes into a protobuf.
    QByteArray toProtobuf() const;
    BeatsPointer clone() const;

    /// Returns a string representing the version of the beat-processing code that
    /// produced this Beats instance. Used by BeatsFactory for associating a
    /// given serialization with the version that produced it.
    QString getVersion() const;
    /// Return a string that represent the preferences used to generate
    /// the beats object.
    QString getSubVersion() const;
    void setSubVersion(const QString& subVersion);

    /// Initializes the BeatGrid to have a BPM of dBpm and the first beat offset
    /// of firstBeatFrame. Does not generate an updated() signal, since it is
    /// meant for initialization.
    void setGrid(Bpm dBpm, FramePos firstBeatFrame = kStartFramePos);

    // TODO: We may want to implement these with common code that returns
    //       the triple of closest, next, and prev.

    /// Starting from frame, return the next beat
    /// in the track, or invalid beat if none exists. If frame refers to the location
    /// of a beat, the same beat is returned.
    Beat findNextBeat(FramePos frame) const;

    /// Starting from frame frame, return the previous
    /// beat in the track, or invalid beat if none exists. If frame refers to the
    /// location of beat, the same beat is returned.
    Beat findPrevBeat(FramePos frame) const;

    /// Starting from frame, fill the frame numbers of the previous beat
    /// and next beat.  Either can be -1 if none exists.  If frame refers
    /// to the location of the beat, the first value is frame, and the second
    /// value is the next beat position.  Non- -1 values are guaranteed to be
    /// even.  Returns false if *at least one* frame is -1.  (Can return false
    /// with one beat successfully filled)
    bool findPrevNextBeats(FramePos frame,
            FramePos* pPrevBeatFrame,
            FramePos* pNextBeatFrame) const;

    /// Starting from frame, return the frame number of the closest beat
    /// in the track, or -1 if none exists.  Non- -1 values are guaranteed to be
    /// even.
    FramePos findClosestBeat(FramePos frame) const;

    /// Find the Nth beat from frame. Works with both positive and
    /// negative values of n. If frame refers to the location of a beat,
    /// then the same beat is returned. If no beat can be found, returns kInvalidBeat.
    Beat findNthBeat(FramePos frame, int offset) const;

    int numBeatsInRange(FramePos startFrame, FramePos endFrame);

    /// Find the frame N beats away from frame. The number of beats may be
    /// negative and does not need to be an integer.
    FramePos findNBeatsFromFrame(FramePos fromFrame, double beats) const;

    /// Return an iterator to a container of Beats containing the Beats
    /// between startFrameNum and endFrameNum. THe BeatIterator must be iterated
    /// while a strong reference to the Beats object to ensure that the Beats
    /// object is not deleted. Caller takes ownership of the returned BeatsIterator
    std::unique_ptr<Beats::iterator> findBeats(FramePos startFrame,
            FramePos stopFrame) const;

    /**
     * Return Beat at (0 based) index
     * @param index
     * @return Beat object
     */
    Beat getBeatAtIndex(int index) {
        return m_beatsInternal.getBeatAtIndex(index);
    }

    /// Return the average BPM over the entire track if the BPM is
    /// valid, otherwise returns -1
    Bpm getBpm() const;

    /// Return the average BPM over the range of n*2 beats centered around
    /// curFrameNum.  (An n of 4 results in an averaging of 8 beats).  Invalid
    /// BPM returns -1.
    Bpm getBpmAroundPosition(FramePos curFrame, int n) const;

    /// Sets the track signature starting at specified bar
    void setSignature(TimeSignature sig, int downbeatIndex);

    /// Sets the nearest beat as a downbeat

    /// Translate all beats in the song by numFrames. Beats that lie
    /// before the start of the track or after the end of the track are not
    /// removed.
    void translate(FrameDiff_t numFrames);

    /// Scale the position of every beat in the song by dScalePercentage.
    void scale(enum BeatsInternal::BPMScale scale);

    /// Set bpm marker at a beat
    void setBpm(Bpm bpm, int beatIndex = 0);

    /// Returns the number of beats
    inline int size() {
        return m_beatsInternal.size();
    }

    /// Returns the frame number for the first beat, -1 is no beats
    FramePos getFirstBeatPosition() const;

    /// Returns the frame number for the last beat, -1 if no beats
    FramePos getLastBeatPosition() const;

    /// Return the sample rate
    SINT getSampleRate() const;

    /**
     * Convert a non-downbeat to a downbeat shifting all downbeats
     * @param beatIndex Index of the beat to be converted to downbeat
     */
    void setAsDownbeat(int beatIndex);

    /// Prints debugging information in stderr
    friend QDebug operator<<(QDebug dbg, const BeatsPointer& arg);

  private slots:
    void slotTrackBeatsUpdated();
  private:
    mutable QMutex m_mutex;
    const Track* m_track;
    BeatsInternal m_beatsInternal;

  signals:
    void updated();
};
} // namespace mixxx

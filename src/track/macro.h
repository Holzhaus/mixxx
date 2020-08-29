#pragma once

#include <QtCore>

#include "engine/engine.h"
#include "proto/macro.pb.h"
namespace proto = mixxx::track::io;

const QLoggingCategory macroLoggingCategory("macros");
constexpr int kMacrosPerTrack = 16;

/// A MacroAction is the building block of a Macro.
/// It contains a position as well as the action to be taken at that position.
///
/// Note that currently only jumps to a target position are available, but that
/// is subject to change.
struct MacroAction {
    enum class Type : uint32_t {
        Jump = 0
    };

    MacroAction(double position, double target)
            : position(position), target(target), type(Type::Jump){};
    // use FramePos once https://github.com/mixxxdj/mixxx/pull/2961 is merged
    const double position;
    const double target;
    const Type type;

    double getSamplePos() const {
        return position * mixxx::kEngineChannelCount;
    }
    double getTargetSamplePos() const {
        return target * mixxx::kEngineChannelCount;
    }

    bool operator==(const MacroAction& other) const {
        return position == other.position && target == other.target;
    }
    inline bool operator!=(const MacroAction& other) const {
        return !operator==(other);
    }

    proto::Macro_Action* serialize() const;
};

/// A Macro stores a list of MacroActions as well as its current state and label.
class Macro {
  public:
    static QByteArray serialize(QList<MacroAction> actions);
    static QList<MacroAction> deserialize(QByteArray serialized);
    static int getFreeSlot(QList<int> taken);

    enum class StateFlag {
        Enabled = 1u,
        Looped = 2u,
    };
    Q_DECLARE_FLAGS(State, StateFlag);

    explicit Macro(QList<MacroAction> actions = {},
            QString label = "",
            State state = State(StateFlag::Enabled),
            int dbId = -1);

    bool isDirty() const;
    int getId() const;

    QString getLabel() const;
    void setLabel(QString);

    bool isEnabled() const;
    bool isLooped() const;
    const State& getState() const;
    void setState(StateFlag flag, bool enable = true);

    bool isEmpty() const;
    unsigned int size() const;

    const QList<MacroAction>& getActions() const;
    void addAction(const MacroAction& action);
    /// Sets the end of the Macro (relevant for looping)
    void setEnd(double framePos);

    void clear();

  private:
    void setDirty(bool dirty);
    void setId(int id);

    bool m_bDirty;
    int m_iId;

    /// The list of actions. The first action marks the jump from end to start.
    QList<MacroAction> m_actions;
    QString m_label;
    State m_state;

    friend class MacroDAO;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Macro::State)

typedef std::shared_ptr<Macro> MacroPtr;

bool operator==(const Macro& m1, const Macro& m2);
inline bool operator!=(const Macro& m1, const Macro& m2) {
    return !(m1 == m2);
}
QDebug operator<<(QDebug debug, const MacroAction& action);
QDebug operator<<(QDebug debug, const Macro& macro);

#pragma once

#include <QtQml>
#include <QtQuick/QQuickItem>

#include "track/track.h"
namespace mixxx {
namespace qml {

class QmlPlayerProxy;

class QmlWaveform : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(mixxx::qml::QmlPlayerProxy* player READ getPlayer WRITE setPlayer
                    NOTIFY playerChanged REQUIRED)
    QML_NAMED_ELEMENT(Waveform)

  public:
    QmlWaveform(QQuickItem* parent = 0);
    ~QmlWaveform();

    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*);

    void setPlayer(QmlPlayerProxy* player);
    QmlPlayerProxy* getPlayer() const;

  private slots:
    void slotTrackLoaded(TrackPointer pLoadedTrack);
    void slotTrackLoading(TrackPointer pNewTrack, TrackPointer pOldTrack);
    void slotTrackUnloaded();
    void slotWaveformUpdated();

  signals:
    void playerChanged();

  private:
    void setCurrentTrack(TrackPointer pTrack);

    QPointer<QmlPlayerProxy> m_pPlayer;
    TrackPointer m_pCurrentTrack;
};

} // namespace qml
} // namespace mixxx

#include "qml/qmlwaveform.h"

#include <QtQuick/qsgflatcolormaterial.h>
#include <QtQuick/qsgnode.h>

#include "mixer/basetrackplayer.h"
#include "qml/qmlplayerproxy.h"

namespace mixxx {
namespace qml {

QmlWaveform::QmlWaveform(QQuickItem* parent)
        : QQuickItem(parent),
          m_pPlayer(nullptr) {
}

QmlWaveform::~QmlWaveform() {
}

QmlPlayerProxy* QmlWaveform::getPlayer() const {
    return m_pPlayer;
}

void QmlWaveform::setPlayer(QmlPlayerProxy* pPlayer) {
    if (m_pPlayer == pPlayer) {
        return;
    }

    if (m_pPlayer != nullptr) {
        m_pPlayer->internalTrackPlayer()->disconnect(this);
    }

    m_pPlayer = pPlayer;

    if (pPlayer != nullptr) {
        connect(m_pPlayer->internalTrackPlayer(),
                &BaseTrackPlayer::newTrackLoaded,
                this,
                &QmlWaveform::slotTrackLoaded);
        connect(m_pPlayer->internalTrackPlayer(),
                &BaseTrackPlayer::loadingTrack,
                this,
                &QmlWaveform::slotTrackLoading);
        connect(m_pPlayer->internalTrackPlayer(),
                &BaseTrackPlayer::playerEmpty,
                this,
                &QmlWaveform::slotTrackUnloaded);
    }

    emit playerChanged();
    update();
}

void QmlWaveform::slotTrackLoaded(TrackPointer pTrack) {
    // TODO: Investigate if it's a bug that this debug assertion fails when
    // passing tracks on the command line
    // DEBUG_ASSERT(m_pCurrentTrack == pTrack);
    setCurrentTrack(pTrack);
}

void QmlWaveform::slotTrackLoading(TrackPointer pNewTrack, TrackPointer pOldTrack) {
    Q_UNUSED(pOldTrack); // only used in DEBUG_ASSERT
    DEBUG_ASSERT(m_pCurrentTrack == pOldTrack);
    setCurrentTrack(pNewTrack);
}

void QmlWaveform::slotTrackUnloaded() {
    setCurrentTrack(nullptr);
}

void QmlWaveform::setCurrentTrack(TrackPointer pTrack) {
    // TODO: Check if this is actually possible
    if (m_pCurrentTrack == pTrack) {
        return;
    }

    if (m_pCurrentTrack != nullptr) {
        disconnect(m_pCurrentTrack.get(), nullptr, this, nullptr);
    }

    m_pCurrentTrack = pTrack;
    if (pTrack != nullptr) {
        connect(pTrack.get(),
                &Track::waveformUpdated,
                this,
                &QmlWaveform::slotWaveformUpdated);
    }
    slotWaveformUpdated();
}

void QmlWaveform::slotWaveformUpdated() {
    bool hasContents = false;

    const TrackPointer pTrack = m_pCurrentTrack;
    if (pTrack != nullptr) {
        const ConstWaveformPointer pWaveform = pTrack->getWaveform();
        hasContents = (pWaveform != nullptr);
    }

    setFlag(ItemHasContents, hasContents);
    update();
}

QSGNode* QmlWaveform::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {
    QSGGeometryNode* node = nullptr;
    QSGGeometry* geometry = nullptr;

    const TrackPointer pTrack = m_pCurrentTrack;
    if (pTrack == nullptr) {
        return oldNode;
    }

    const ConstWaveformPointer pWaveform = pTrack->getWaveform();
    if (pWaveform == nullptr) {
        return oldNode;
    }

    // Add first and and last position.
    const int numberOfElementsTotal = pWaveform->getDataSize() + 2;
    const int numberOfElementsComplete = pWaveform->getCompletion();

    if (!oldNode) {
        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), numberOfElementsTotal);
        geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        QSGFlatColorMaterial* material = new QSGFlatColorMaterial;
        material->setColor(QColor(255, 0, 0));
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
        node = static_cast<QSGGeometryNode*>(oldNode);
        geometry = node->geometry();
        geometry->allocate(numberOfElementsTotal);
    }

    QSizeF itemSize = size();
    const float halfHeight = static_cast<float>(itemSize.height()) / 2.0f;
    const float width = static_cast<float>(itemSize.width());
    QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();

    vertices[0].set(0, halfHeight);
    for (int i = 1; i < numberOfElementsComplete + 1; i++) {
        const bool isLeftChannel = i & 1;
        float amplitude = pWaveform->getAll(i) / 255.0f;
        float position = i / static_cast<float>(numberOfElementsTotal);

        float x = position * width;
        float y = halfHeight - (isLeftChannel ? -amplitude : amplitude) * halfHeight;
        vertices[i].set(x, y);
    }

    for (int i = numberOfElementsComplete + 1; i < numberOfElementsTotal; i++) {
        float position = i / static_cast<float>(numberOfElementsTotal);
        float x = position * width;
        vertices[i].set(x, halfHeight);
    }

    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}

} // namespace qml
} // namespace mixxx

#include "waveform/renderers/waveformbeatmarker.h"

#include "waveform/renderers/geometryutils.h"

namespace {
constexpr int kTriangleEdgeLength = 15;
const auto labelBackgroundColor = QColor(10, 100, 200, 75);
} // namespace

WaveformBeatMarker::WaveformBeatMarker()
        : m_orientation(Qt::Horizontal) {
}

void WaveformBeatMarker::draw(QPainter* painter) const {
    painter->setBrush(Qt::white);
    painter->setPen((QPen(Qt::white, 2)));
    if (m_orientation == Qt::Horizontal) {
        painter->drawLine(QPointF(m_position, 0), QPoint(m_position, m_length));
        painter->drawPolygon(getEquilateralTriangle(
                kTriangleEdgeLength, QPointF(m_position, 0), Direction::DOWN));
        QString labelText;
        float maxWidth = 0;
        for (int i = 0; i < m_textDisplayItems.size(); i++) {
            labelText += m_textDisplayItems.at(i) + "\n";
            if (m_textDisplayItems.at(i).length() * painter->font().pointSize() > maxWidth) {
                maxWidth = m_textDisplayItems.at(i).size() * painter->font().pointSize();
            }
        }
        QRect textBoundingRect(m_position + kTriangleEdgeLength / 2,
                kTriangleEdgeLength / 2 + painter->font().pointSizeF(),
                maxWidth,
                painter->font().pointSize() * m_textDisplayItems.size() * 1.5);
        painter->setBrush(QBrush(labelBackgroundColor));
        painter->setPen(Qt::transparent);
        painter->drawRect(textBoundingRect);
        painter->setPen(Qt::white);
        painter->drawText(textBoundingRect, Qt::TextWordWrap, labelText);
    } else {
        painter->drawLine(QPointF(0, m_position), QPoint(m_length, m_position));
        painter->setBrush(Qt::white);
        painter->drawPolygon(getEquilateralTriangle(
                kTriangleEdgeLength, QPointF(0, m_position), Direction::RIGHT));
    }
}
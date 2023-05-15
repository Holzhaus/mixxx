#pragma once

#include "waveform/renderers/glwaveformrenderersignal.h"
#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_2)

#include "waveform/renderers/glwaveformrenderersignal.h"

QT_FORWARD_DECLARE_CLASS(QDomNode)
class ControlObject;

class GLWaveformRendererFilteredSignal : public GLWaveformRendererSignal {
  public:
    explicit GLWaveformRendererFilteredSignal(
            WaveformWidgetRenderer* waveformWidgetRenderer);
    virtual ~GLWaveformRendererFilteredSignal();

    virtual void onSetup(const QDomNode &node);
    virtual void draw(QPainter* painter, QPaintEvent* event);
};

#endif // QT_NO_OPENGL && !QT_OPENGL_ES_2

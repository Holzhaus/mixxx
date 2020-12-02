#ifndef QTVSYNCTESTWIDGET_H
#define QTVSYNCTESTWIDGET_H

#include <QByteArrayData>
#include <QGLWidget>
#include <QString>
#include <QStringBuilder>

#include "util/duration.h"
#include "waveform/widgets/waveformwidgettype.h"
#include "waveformwidgetabstract.h"

class QObject;
class QPaintEvent;
class QWidget;

class QtVSyncTestWidget : public QGLWidget, public WaveformWidgetAbstract {
    Q_OBJECT
  public:
    QtVSyncTestWidget(const QString& group, QWidget* parent);
    virtual ~QtVSyncTestWidget();

    virtual WaveformWidgetType::Type getType() const { return WaveformWidgetType::QtVSyncTest; }

    static inline QString getWaveformWidgetName() { return tr("VSyncTest") + " - Qt"; }
    static inline bool useOpenGl() { return true; }
    static inline bool useOpenGles() { return true; }
    static inline bool useOpenGLShaders() { return false; }
    static inline bool developerOnly() { return true; }

  protected:
    virtual void castToQWidget();
    virtual void paintEvent(QPaintEvent* event);
    virtual mixxx::Duration render();

  private:
    friend class WaveformWidgetFactory;
};

#endif // QTVSYNCTESTWIDGET_H

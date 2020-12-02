// wrecordingduration.h
// WRecordingDuration is a widget showing the duration of running recoding
// In skin it is represented by a <RecordingDuration> node.

#ifndef WRECORDINGDURATION_H
#define WRECORDINGDURATION_H

#include <QByteArrayData>
#include <QDomNode>
#include <QString>

#include "recording/recordingmanager.h"
#include "skin/skincontext.h"
#include "widget/wlabel.h"

class QObject;
class QWidget;
class RecordingManager;

class WRecordingDuration: public WLabel {
    Q_OBJECT
  public:
    WRecordingDuration(QWidget* parent, RecordingManager* pRecordingManager);
    ~WRecordingDuration() override;

    void setup(const QDomNode& node, const SkinContext& context) override;

  private slots:
    void refreshLabel(const QString&);
    void slotRecordingInactive(bool);

  private:
    RecordingManager* m_pRecordingManager;
    QString m_inactiveText;
};

#endif /* WRECORDINGDURATION_H */

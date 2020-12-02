#ifndef DLGPREFRECORD_H
#define DLGPREFRECORD_H

#include <QButtonGroup>
#include <QByteArrayData>
#include <QList>
#include <QRadioButton>
#include <QString>
#include <QWidget>

#include "encoder/encoder.h"
#include "preferences/dialog/ui_dlgprefrecorddlg.h"
#include "preferences/dlgpreferencepage.h"
#include "preferences/usersettings.h"

class ControlObject;
class ControlProxy;
class QAbstractButton;
class QObject;
class QRadioButton;
class QWidget;

class DlgPrefRecord : public DlgPreferencePage, public Ui::DlgPrefRecordDlg  {
    Q_OBJECT
  public:
    DlgPrefRecord(QWidget *parent, UserSettingsPointer _config);
    virtual ~DlgPrefRecord();

  public slots:
    // Apply changes to widget
    void slotApply() override;
    void slotUpdate() override;
    void slotResetToDefaults() override;

    // Dialog to browse for recordings directory
    void slotBrowseRecordingsDir();

    void slotFormatChanged();
    void slotSliderQuality();
    void slotSliderCompression();
    void slotGroupChanged();

  signals:
    void apply(const QString &);

  private:
    void setupEncoderUI();
    void loadMetaData();
    void updateTextQuality();
    void updateTextCompression();
    void saveRecordingFolder();
    void saveMetaData();
    void saveEncoding();
    void saveUseCueFile();
    void saveSplitSize();

    // Pointer to config object
    UserSettingsPointer m_pConfig;
    Encoder::Format m_selFormat;
    QButtonGroup encodersgroup;
    QButtonGroup optionsgroup;
    QList<QRadioButton*> m_formatButtons;
    QList<QAbstractButton*> m_optionWidgets;
};

#endif

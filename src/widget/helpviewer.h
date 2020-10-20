#include <QFileInfo>
#include <QString>
#include <QWidget>

#include "preferences/usersettings.h"

// forward declarations
QT_FORWARD_DECLARE_CLASS(QHelpEngine);
QT_FORWARD_DECLARE_CLASS(QTabWidget);
class HelpBrowser;

/// The help viewer class represents the whole help window, consisting of a
/// tabbed sidebar (content/index/search) and the HelpBrowser (i.e. the HTML
/// renderer).
class HelpViewer : public QWidget {
  public:
    explicit HelpViewer(const UserSettingsPointer& helpFile, QWidget* parent = nullptr);

    void open(const QString& documentPath);

  private:
    void openDocument(const QString& documentPath);

    QHelpEngine* m_pHelpEngine;
    HelpBrowser* m_pHelpBrowser;
    QTabWidget* m_pTabWidget;

    QString m_documentUrlPrefix;
    QString m_language;
};

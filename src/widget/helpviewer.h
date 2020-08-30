#include <QFileInfo>
#include <QString>
#include <QWidget>

// forward declarations
QT_FORWARD_DECLARE_CLASS(QHelpEngine);
QT_FORWARD_DECLARE_CLASS(QTabWidget);
class HelpBrowser;

/// The help viewer class represents the whole help window, consisting of a
/// tabbed sidebar (content/index/search) and the HelpBrowser (i.e. the HTML
/// renderer).
class HelpViewer : public QWidget {
  public:
    explicit HelpViewer(const QFileInfo& helpFile, QWidget* parent = nullptr);

    void openDocument(const QString& documentPath);

  private:
    QHelpEngine* m_pHelpEngine;
    HelpBrowser* m_pHelpBrowser;
    QTabWidget* m_pTabWidget;

    QString m_documentUrlPrefix;
};

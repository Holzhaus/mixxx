#include <QFileInfo>
#include <QString>
#include <QWidget>

// forward declarations
class HelpBrowser;
class QHelpEngine;

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

    QString m_documentUrlPrefix;
};

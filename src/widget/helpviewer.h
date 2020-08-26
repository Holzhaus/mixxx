#include <QFileInfo>
#include <QString>
#include <QWidget>

// forward declarations
class HelpBrowser;
class QHelpEngine;

class HelpViewer : public QWidget {
  public:
    explicit HelpViewer(const QFileInfo& helpFile, QWidget* parent = nullptr);

    void openDocument(const QString& documentPath);

  private:
    QHelpEngine* m_pHelpEngine;
    HelpBrowser* m_pHelpBrowser;

    QString m_documentUrlPrefix;
};

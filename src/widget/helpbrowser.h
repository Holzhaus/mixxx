#include <QTextBrowser>
#include <QUrl>
#include <QVariant>

// forward declarations
class QWidget;
class QHelpEngine;

class HelpBrowser : public QTextBrowser {
  public:
    HelpBrowser(QHelpEngine* pHelpEngine, QWidget* parent = nullptr);
    QVariant loadResource(int type, const QUrl& name);

  private:
    QHelpEngine* m_pHelpEngine;
};

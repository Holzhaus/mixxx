#include <QPoint>
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

  private slots:
    void slotCustomContextMenu(const QPoint& pos);

  private:
    QHelpEngine* m_pHelpEngine;
};

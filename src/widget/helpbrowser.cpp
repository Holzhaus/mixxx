#include "widget/helpbrowser.h"

#include <QHelpEngine>

namespace {

const QString kHelpUrlScheme = QStringLiteral("qthelp");

} // anonymous namespace

HelpBrowser::HelpBrowser(QHelpEngine* pHelpEngine, QWidget* parent)
        : QTextBrowser(parent),
          m_pHelpEngine(pHelpEngine) {
}

QVariant HelpBrowser::loadResource(int type, const QUrl& name) {
    if (name.scheme() == kHelpUrlScheme) {
        return QVariant(m_pHelpEngine->fileData(name));
    }

    return QTextBrowser::loadResource(type, name);
}

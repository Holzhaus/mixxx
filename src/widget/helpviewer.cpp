#include "widget/helpviewer.h"

#include <QHBoxLayout>
#include <QHelpContentWidget>
#include <QHelpEngine>
#include <QHelpIndexWidget>
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#include <QHelpLink>
#endif
#include <QSplitter>
#include <QTabWidget>

#include "util/assert.h"
#include "widget/helpbrowser.h"

HelpViewer::HelpViewer(const QFileInfo& helpPath, QWidget* parent)
        : QWidget(parent),
          m_pHelpEngine(new QHelpEngine(helpPath.filePath())) {
    DEBUG_ASSERT(helpPath.exists());
    m_pHelpEngine->setupData();

    QString namespaceName(m_pHelpEngine->namespaceName(helpPath.filePath()));
    DEBUG_ASSERT(!namespaceName.isEmpty());
    m_documentUrlPrefix = QStringLiteral("qthelp://") + namespaceName + QStringLiteral("/doc");

    QTabWidget* pWidget = new QTabWidget(this);
    pWidget->setMaximumWidth(200);
    pWidget->addTab(m_pHelpEngine->contentWidget(), tr("Contents"));
    pWidget->addTab(m_pHelpEngine->indexWidget(), tr("Index"));

    m_pHelpBrowser = new HelpBrowser(m_pHelpEngine, this);
    openDocument("/index.html");

    connect(m_pHelpEngine->contentWidget(),
            &QHelpContentWidget::linkActivated,
            [this](QUrl url) { m_pHelpBrowser->setSource(url); });

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(m_pHelpEngine->indexWidget(),
            &QHelpIndexWidget::documentActivated,
            [this](const QHelpLink& document, const QString& keyword) {
                Q_UNUSED(keyword);
                m_pHelpBrowser->setSource(document.url);
            });
#else
    connect(m_pHelpEngine->indexWidget(),
            &QHelpIndexWidget::linkActivated,
            [this](QUrl url, QString) { m_pHelpBrowser->setSource(url); });
#endif

    QSplitter* pSplitter = new QSplitter(Qt::Horizontal);
    pSplitter->insertWidget(0, pWidget);
    pSplitter->insertWidget(1, m_pHelpBrowser);

    QHBoxLayout* pLayout = new QHBoxLayout();
    pLayout->addWidget(pSplitter);
    setLayout(pLayout);

    setWindowFlags(Qt::Window);
}

void HelpViewer::openDocument(const QString& documentPath) {
    m_pHelpBrowser->setSource(QUrl(m_documentUrlPrefix + documentPath));
}

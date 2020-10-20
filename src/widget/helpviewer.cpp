#include "widget/helpviewer.h"

#include <QAction>
#include <QHBoxLayout>
#include <QHelpContentWidget>
#include <QHelpEngine>
#include <QHelpIndexWidget>
#include <QHelpSearchEngine>
#include <QHelpSearchQueryWidget>
#include <QHelpSearchResultWidget>
#include <QRegExp>
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#include <QHelpLink>
#endif
#include <QSplitter>
#include <QTabWidget>
#include <QToolBar>

#include "defs_urls.h"
#include "util/assert.h"
#include "widget/helpbrowser.h"

namespace {
const QRegExp kSlugifyRegExp("[\\W+]");
const QString kSlugifyReplacement = QStringLiteral("-");
const QString kIndexDocument = QStringLiteral(MIXXX_MANUAL_INDEX_PATH);
} // namespace

HelpViewer::HelpViewer(const QFileInfo& helpPath, QWidget* parent)
        : QWidget(parent),
          m_pHelpEngine(new QHelpEngine(helpPath.filePath())) {
    DEBUG_ASSERT(helpPath.exists());
    m_pHelpEngine->setAutoSaveFilter(false);
    DEBUG_ASSERT(m_pHelpEngine->setupData());
    m_pHelpEngine->searchEngine()->reindexDocumentation();

    QString namespaceName(m_pHelpEngine->namespaceName(helpPath.filePath()));
    DEBUG_ASSERT(!namespaceName.isEmpty());
    m_documentUrlPrefix = QStringLiteral("qthelp://") + namespaceName + QStringLiteral("/doc");

    m_language = QStringLiteral("en");
    {
        QStringList customFilters = m_pHelpEngine->customFilters();
        QLocale locale;
        QList<QString> languages;
        for (const QString& language : locale.uiLanguages()) {
            if (customFilters.contains(language)) {
                m_language = language;
                break;
            }
        }
    }
    m_pHelpEngine->setCurrentFilter(m_language);

    QVBoxLayout* pSidebarLayout = new QVBoxLayout(this);
    pSidebarLayout->addWidget(m_pHelpEngine->searchEngine()->queryWidget());

    m_pTabWidget = new QTabWidget(this);
    m_pTabWidget->addTab(m_pHelpEngine->contentWidget(), tr("Contents"));
    m_pTabWidget->addTab(m_pHelpEngine->indexWidget(), tr("Index"));
    m_pTabWidget->addTab(m_pHelpEngine->searchEngine()->resultWidget(), tr("Search"));
    pSidebarLayout->addWidget(m_pTabWidget);

    QWidget* pSidebarWidget = new QWidget(this);
    pSidebarWidget->setLayout(pSidebarLayout);

    m_pHelpBrowser = new HelpBrowser(m_pHelpEngine, this);

    QToolBar* pToolBar = new QToolBar(this);
    pToolBar->addAction(QIcon::fromTheme("go-home"), tr("Main Page"), [this] {
        openDocument(kIndexDocument);
    });
    QAction* pBackwardAction = pToolBar->addAction(
            QIcon::fromTheme("go-previous"), tr("Back"), [this] {
                m_pHelpBrowser->backward();
            });
    connect(m_pHelpBrowser, &HelpBrowser::backwardAvailable, [pBackwardAction](bool available) {
        pBackwardAction->setEnabled(available);
    });
    QAction* pForwardAction = pToolBar->addAction(QIcon::fromTheme("go-next"),
            tr("Forward"),
            [this] { m_pHelpBrowser->forward(); });
    connect(m_pHelpBrowser, &HelpBrowser::forwardAvailable, [pForwardAction](bool available) {
        pForwardAction->setEnabled(available);
    });

    QVBoxLayout* pBrowserLayout = new QVBoxLayout(this);
    pBrowserLayout->addWidget(pToolBar);
    pBrowserLayout->addWidget(m_pHelpBrowser);

    QWidget* pBrowserWidget = new QWidget(this);
    pBrowserWidget->setLayout(pBrowserLayout);

    QSplitter* pSplitter = new QSplitter(Qt::Horizontal);
    pSplitter->insertWidget(0, pSidebarWidget);
    pSplitter->insertWidget(1, pBrowserWidget);

    QHBoxLayout* pLayout = new QHBoxLayout();
    pLayout->addWidget(pSplitter);
    setLayout(pLayout);
    setWindowFlags(Qt::Window);

    connect(m_pHelpEngine->searchEngine(),
            &QHelpSearchEngine::searchingFinished,
            [this](int searchResultCount) {
                Q_UNUSED(searchResultCount);
                m_pTabWidget->setCurrentWidget(m_pHelpEngine->searchEngine()->resultWidget());
            });
    connect(m_pHelpEngine->searchEngine()->queryWidget(),
            &QHelpSearchQueryWidget::search,
            [this] {
                m_pHelpEngine->searchEngine()->search(
                        m_pHelpEngine->searchEngine()
                                ->queryWidget()
                                ->searchInput());
            });
    connect(m_pHelpEngine->searchEngine()->resultWidget(),
            &QHelpSearchResultWidget::requestShowLink,
            [this](const QUrl& url) { m_pHelpBrowser->setSource(url); });

    connect(m_pHelpEngine->contentWidget(),
            &QHelpContentWidget::linkActivated,
            [this](const QUrl& url) { m_pHelpBrowser->setSource(url); });

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    // FIXME: This is a very ugly hack that works around the non-working
    // documentActivated/documentsActivate signals. See QTBUG-84727.
    connect(m_pHelpEngine->indexWidget(),
            &QHelpIndexWidget::activated,
            [this](const QModelIndex& index) {
                QString keyword = m_pHelpEngine->indexModel()->data(index).toString();
                m_pHelpBrowser->setSource(QUrl(m_documentUrlPrefix +
                        QStringLiteral("/glossary.") + m_language + QStringLiteral(".html#term-") +
                        keyword.replace(kSlugifyRegExp, kSlugifyReplacement)));
            });
    // connect(m_pHelpEngine->indexWidget(),
    //         &QHelpIndexWidget::documentActivated,
    //         [this](const QHelpLink& document, const QString& keyword) {
    //             Q_UNUSED(keyword);
    //             m_pHelpBrowser->setSource(document.url);
    //         });
    // connect(m_pHelpEngine->indexWidget(),
    //         &QHelpIndexWidget::documentsActivated,
    //         [this](const QList<QHelpLink>& documents, const QString& keyword) {
    //             Q_UNUSED(keyword);
    //             if (documents.isEmpty()) {
    //                 return;
    //             }
    //             m_pHelpBrowser->setSource(documents.first().url);
    //         });
#else
    connect(m_pHelpEngine->indexWidget(),
            &QHelpIndexWidget::linkActivated,
            [this](const QUrl& url, QString) { m_pHelpBrowser->setSource(url); });
#endif

    openDocument(kIndexDocument);
}

void HelpViewer::openDocument(const QString& documentPath) {
    QUrl url(m_documentUrlPrefix + documentPath);
    QString path = url.path();
    if (path.endsWith(QStringLiteral(".html"))) {
        path.chop(4);
        url.setPath(path + m_language + QStringLiteral(".html"));
    }
    m_pHelpBrowser->setSource(url);
}

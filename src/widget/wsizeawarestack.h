#ifndef WSIZEAWARESTACK_H
#define WSIZEAWARESTACK_H

#include <QByteArrayData>
#include <QEvent>
#include <QString>
#include <QWidget>

#include "widget/wbasewidget.h"

class SizeAwareLayout;
class QEvent;
class QObject;
class QResizeEvent;

class WSizeAwareStack : public QWidget, public WBaseWidget {
    Q_OBJECT
  public:
    explicit WSizeAwareStack(QWidget* parent = nullptr);

    int addWidget(QWidget* widget);

  protected:
    void resizeEvent(QResizeEvent* event) override;
    bool event(QEvent* pEvent) override;

  private:
    SizeAwareLayout* m_layout;
};

#endif /* WSIZEAWARESTACK_H */

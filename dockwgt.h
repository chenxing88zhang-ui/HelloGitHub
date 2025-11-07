#ifndef DOCKWGT_H
#define DOCKWGT_H

#include <QDockWidget>

class DockWgt : public QDockWidget
{
    Q_OBJECT
public:
    explicit DockWgt(QWidget *parent = nullptr , const QString& name = "");

signals:
};

#endif // DOCKWGT_H

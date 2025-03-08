#ifndef FUNCWINDOW_H
#define FUNCWINDOW_H
#include <QListWidgetItem>
#include <QTableWidget>
#include <QWidget>
#include "caculate_table_item.h"

namespace Ui {
class funcwindow;
}

class funcwindow : public QWidget {
    Q_OBJECT
public:
    explicit funcwindow(const QPair<int, int>&, CaculateTable*, QWidget* = nullptr);
    ~funcwindow();

private slots:
    void math_list_clicked(QListWidgetItem*);
    void correct_btn_clicked();

private:
    Ui::funcwindow *ui;
    QPair<int, int> posi_;
    CaculateTable* table_;
    QString func_;
};

#endif // FUNCWINDOW_H

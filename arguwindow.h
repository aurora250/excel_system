#ifndef ARGUWINDOW_H
#define ARGUWINDOW_H
#include <QLineEdit>
#include <QWidget>
#include <QTableWidget>
#include "caculate_table_item.h"

class ChangedEdit : public QLineEdit{
    Q_OBJECT;
private:
    bool changed_ = false;

public:
    ChangedEdit() : QLineEdit() {}
    ~ChangedEdit() = default;

    bool is_changed() const { return changed_; }
    void set_change(bool state) { changed_ = state; }
};

namespace Ui {
class arguwindow;
}

class arguwindow : public QWidget {
    Q_OBJECT
public:
    explicit arguwindow(QString*, QPair<int, int>*, CaculateTable*, QWidget* = nullptr);
    ~arguwindow();

    void add_labels(const QString& = "");

private slots:
    void argu_label_changed(QString);
    void correct_btn_clicked();

private:
    Ui::arguwindow *ui;
    QWidget* widget_;
    QPair<int, int>* posi_;
    CaculateTable* table_;
    uint32_t count_;
    uint32_t max_count_;
    QString* str_;
    QMap<uint32_t, bool> changed_;
};

#endif // ARGUWINDOW_H

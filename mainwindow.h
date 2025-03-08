#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QVector>
#include <QTableWidgetItem>
#include <caculate.h>
#include "setting.h"
#include "caculate_table_item.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void set_connect();
    CaculateTable* add_panel(const QString& = "");
    void add_menu();

    CaculateTable* find_tab(const QString&);

    void expand_vtable(CaculateTable*, uint32_t = setting::default_row);
    void expand_htable(CaculateTable*, uint32_t = setting::default_column);

    bool read_binary(const QString&);
    bool save_binary(const QString&);
    bool read_mcsv(const QString&);
    bool save_mcsv(const QString&);
    bool read_csv(CaculateTable*, const QString&);
    bool save_csv(const QString&);
    bool read_excel(const QString&);
    bool save_excel(const QString&);

private slots:
    // button:
    void func_btn_clicked();
    void correct_btn_clicked();
    void cancel_btn_clicked();
    void add_table_btn_clicked();
    // label:
    void func_lab_changed(QString);
    void position_lab_changed(QString);
    // tab widget:
    void tab_double_clicked(int);
    // table widget:
    void table_view(QTableWidgetItem*);
    void table_item_changed(QTableWidgetItem*);
    void table_vertical_bar_changed(int);
    void table_horizontal_bar_changed(int);
    // file:
    void read_binary_triggered(bool);
    void save_binary_triggered(bool);
    void read_mcsv_triggered(bool);
    void save_mcsv_triggered(bool);
    void read_csv_triggered(bool);
    void save_csv_triggered(bool);
    void read_excel_triggered(bool);
    void save_excel_triggered(bool);

private:
    Ui::MainWindow *ui = nullptr;
    QVector<CaculateTable*> tables_{};
};

#endif // MAINWINDOW_H

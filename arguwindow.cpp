#include "arguwindow.h"
#include "ui_arguwindow.h"
#include "setting.h"
#include "caculate.h"
#include "caculate_table_item.h"
#include <QLabel>
#include <QMessageBox>

arguwindow::arguwindow(QString* str, QPair<int, int>* posi, CaculateTable* table, QWidget *parent)
    : QWidget(parent), ui(new Ui::arguwindow), widget_(new QWidget),
    posi_(posi), table_(table), count_(0), str_(str) {
    ui->setupUi(this);
    this->setWindowTitle("设置参数");
    this->setWindowIcon(QIcon(setting::title_icon));
    widget_->setLayout(ui->form_layout);
    ui->scroll_area->setWidget(widget_);
    ui->scroll_area->setWidgetResizable(true);

    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    Qt::WindowFlags flags = windowFlags();
    flags &= ~(Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
    setWindowFlags(flags);
    max_count_ = setting::math_describes[*str_].second;

    ui->func_name_label->setText(*str_ + ":");
    ui->argu_count_label->setText("最大参数数目: " + QString::number(max_count_));
    add_labels();

    connect(ui->correct_btn, SIGNAL(clicked(bool)), this, SLOT(correct_btn_clicked()));
}

void arguwindow::add_labels(const QString& str){
    count_++;
    QLabel* label = new QLabel("参数: " + QString::number(count_), this);
    QLineEdit* edit = new ChangedEdit();
    ui->form_layout->addRow(label, edit);
    edit->setText(str);
    connect(edit, SIGNAL(textChanged(QString)), this, SLOT(argu_label_changed(QString)));
}

void arguwindow::argu_label_changed(QString str){
    if(str.isEmpty()) return;
    if(count_ == max_count_) return;
    QObject *send = sender();
    if (send) {
        ChangedEdit *edit = dynamic_cast<ChangedEdit*>(qobject_cast<QLineEdit*>(send));
        if(edit && !edit->is_changed()) {
            add_labels();
            edit->set_change(true);
        }
    }
}

void arguwindow::correct_btn_clicked(){
    bool oped = false;
    for(uint32_t i = 0; i<count_;i++){
        QLayoutItem* item = ui->form_layout->itemAt(i, QFormLayout::FieldRole);
        QString edit;
        if (item)
            edit = qobject_cast<QLineEdit*>(item->widget())->text();
        if(!edit.isEmpty()) {
            oped = true;
            if(i==0) *str_ += ("(" + edit);
            if(i!=0) *str_ += ("," + edit);
        }
        if(oped && i==count_-1) {
            *str_ += ")";
            break;
        }
    }
    table_->blockSignals(true);
    CaculateTableItem* item_ = dynamic_cast<CaculateTableItem*>(table_->item(posi_->first, posi_->second));
    QString func =  "=" + *str_;
    auto final = Parser().parse(*posi_, table_, func);
    if(final.first){
        item_->update_func(func);
        item_->update_caculate(final.second);
        item_->setText(QString::number(final.second));
    }
    else{
        item_->setText(func);
    }
    table_->blockSignals(false);
    this->close();
}

arguwindow::~arguwindow() {
    delete widget_;
    delete ui;
}

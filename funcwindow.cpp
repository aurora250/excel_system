#include "funcwindow.h"
#include "arguwindow.h"
#include "ui_funcwindow.h"
#include "setting.h"

funcwindow::funcwindow(const QPair<int, int>& posi, CaculateTable* table, QWidget *parent)
    : QWidget(parent), ui(new Ui::funcwindow), posi_(posi), table_(table), func_("NULL") {
    ui->setupUi(this);
    this->setWindowTitle("插入函数");
    this->setWindowIcon(QIcon(setting::title_icon));
    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    Qt::WindowFlags flags = windowFlags();
    flags &= ~(Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
    setWindowFlags(flags);

    ui->math_list->addItems(setting::math_fcns);

    for (int i = 0; i < ui->math_list->count(); ++i) {
        QListWidgetItem *item = ui->math_list->item(i);
        Qt::ItemFlags flags = item->flags();
        flags &= ~Qt::ItemIsEditable;
        item->setFlags(flags);
    }
    connect(ui->math_list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(math_list_clicked(QListWidgetItem*)));
    connect(ui->correct_btn, SIGNAL(clicked(bool)), this, SLOT(correct_btn_clicked()));
}

void funcwindow::math_list_clicked(QListWidgetItem* item) {
    func_ = item->text();
    ui->interact_lab->setText(func_ + " : " + setting::math_describes[func_].first);
}

void funcwindow::correct_btn_clicked(){
    if(func_ == "NULL") {
        this->close();
        return;
    }
    arguwindow* w = new arguwindow(&func_, &posi_, table_);
    this->hide();
    w->setWindowModality(Qt::ApplicationModal);
    w->show();
    this->close();
}

funcwindow::~funcwindow() {
    delete ui;
}

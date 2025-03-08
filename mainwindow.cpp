#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "caculate_table_item.h"
#include "funcwindow.h"
#include "setting.h"

#include <QVBoxLayout>
#include <QInputDialog>
#include <QScrollBar>
#include <QIODevice>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenuBar>

// QXlsx:
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxchart.h"
#include "xlsxworkbook.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->scroll_area->setWidgetResizable(true);
    ui->main_tab->setTabPosition(QTabWidget::South);

    add_panel();
    add_menu();
    set_connect();
}

MainWindow::~MainWindow() {
    hide();
    for(int i = 0; i < tables_.size(); i++) {
        CaculateTable* table = tables_.at(i);
        for(int row=0; row!=table->rowCount();row++){
            for(int col=0;col!=table->columnCount();col++){
                delete table->item(row, col);
            }
        }
        delete table;
    }
    delete ui;
}

void MainWindow::set_connect(){
    // button:
    connect(ui->correct_btn, SIGNAL(clicked(bool)), this, SLOT(correct_btn_clicked()));
    connect(ui->cancel_btn, SIGNAL(clicked(bool)), this, SLOT(cancel_btn_clicked()));
    connect(ui->add_table_btn, SIGNAL(clicked(bool)), this, SLOT(add_table_btn_clicked()));
    connect(ui->func_btn, SIGNAL(clicked(bool)), this, SLOT(func_btn_clicked()));
    // label:
    connect(ui->func_label, SIGNAL(textChanged(QString)), this, SLOT(func_lab_changed(QString)));
    connect(ui->position_lab, SIGNAL(textChanged(QString)), this, SLOT(position_lab_changed(QString)));
    // tab widget:
    connect(ui->main_tab, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(tab_double_clicked(int)));
}

CaculateTable* MainWindow::add_panel(const QString& tab_name){
    CaculateTable* table = new CaculateTable();
    table->setRowCount(setting::default_row);
    table->setColumnCount(setting::default_column);
    for(int i=0;i<setting::default_row;i++){
        for(int j=0;j<setting::default_column;j++){
            CaculateTableItem* item = new CaculateTableItem();
            table->setItem(i, j, item);
        }
        QTableWidgetItem *header = new QTableWidgetItem(CaculateTableItem::column_alpha(i));
        table->setHorizontalHeaderItem(i, header);
    }
    tables_.push_back(table);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(table);
    layout->setContentsMargins(0, 0, 0, 0);
    QWidget* widget = new QWidget();
    widget->setLayout(layout);
    QString str = tab_name.isEmpty() ? ("Table " + QString::number(tables_.size())) : tab_name;
    ui->main_tab->addTab(widget, str);

    // table widget:
    connect(table, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(table_view(QTableWidgetItem*)));
    connect(table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(table_item_changed(QTableWidgetItem*)));
    connect(table->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(table_vertical_bar_changed(int)));
    connect(table->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(table_horizontal_bar_changed(int)));
    return table;
}

void MainWindow::add_menu(){
    QMenuBar* form_menu = new QMenuBar(this);
    setMenuBar(form_menu);
    QMenu *menu_file = new QMenu("文件", this);

    QMenu *menu_import=new QMenu("导入");
    QAction *import_excel=new QAction("Excel", this);
    connect(import_excel, SIGNAL(triggered(bool)), this, SLOT(read_excel_triggered(bool)));
    menu_import->addAction(import_excel);
    QAction *import_csv=new QAction("Csv", this);
    connect(import_csv, SIGNAL(triggered(bool)), this, SLOT(read_csv_triggered(bool)));
    menu_import->addAction(import_csv);
    QAction *import_bin=new QAction("Txt", this);
    connect(import_bin, SIGNAL(triggered(bool)), this, SLOT(read_binary_triggered(bool)));
    menu_import->addAction(import_bin);
    QAction *import_mcsv=new QAction("Mcsv", this);
    connect(import_mcsv, SIGNAL(triggered(bool)), this, SLOT(read_mcsv_triggered(bool)));
    menu_import->addAction(import_mcsv);
    menu_file->addMenu(menu_import);

    QMenu *menu_export=new QMenu("导出");
    QAction *export_excel=new QAction("Excel", this);
    menu_export->addAction(export_excel);
    connect(export_excel, SIGNAL(triggered(bool)), this, SLOT(save_excel_triggered(bool)));
    QAction *export_csv=new QAction("Csv", this);
    menu_export->addAction(export_csv);
    connect(export_csv, SIGNAL(triggered(bool)), this, SLOT(save_csv_triggered(bool)));
    QAction *export_binary=new QAction("Txt", this);
    menu_export->addAction(export_binary);
    connect(export_binary, SIGNAL(triggered(bool)), this, SLOT(save_binary_triggered(bool)));
    QAction *export_mcsv=new QAction("Mcsv", this);
    menu_export->addAction(export_mcsv);
    connect(export_mcsv, SIGNAL(triggered(bool)), this, SLOT(save_mcsv_triggered(bool)));
    menu_file->addMenu(menu_export);

    form_menu->addMenu(menu_file);
}

CaculateTable* MainWindow::find_tab(const QString& tab){
    int count = ui->main_tab->count();
    for (int i = 0; i < count; ++i) {
        if (ui->main_tab->tabText(i) == tab)
            return tables_[i];
    }
    return nullptr;
}

void MainWindow::expand_vtable(CaculateTable* table, uint32_t row){
    uint32_t old_row = table->rowCount();
    uint32_t cur_column = table->columnCount();
    for(uint32_t i=0;i<row;i++){
        table->insertRow(old_row + i);
        for(uint32_t j=0;j<cur_column;j++){
            CaculateTableItem* item = new CaculateTableItem();
            table->setItem(old_row + i, j, item);
        }
    }
}
void MainWindow::expand_htable(CaculateTable* table, uint32_t column){
    uint32_t old_column = table->columnCount();
    uint32_t cur_row = table->rowCount();
    for(uint32_t i=0;i<column;i++){
        table->insertColumn(old_column + i);
        for(uint32_t j=0;j<cur_row;j++){
            CaculateTableItem* item = new CaculateTableItem();
            table->setItem(j, old_column + i, item);
        }
        QTableWidgetItem *header = new QTableWidgetItem(CaculateTableItem::column_alpha(old_column + i));
        table->setHorizontalHeaderItem(old_column + i, header);
    }
}

bool MainWindow::read_binary(const QString& file_path) {
    auto start = std::chrono::high_resolution_clock().now();
    blockSignals(true);
    QFile file(file_path);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        while (!in.atEnd()) {
            int row, column;
            QString tab;
            in >> row >> column >> tab;
            CaculateTable* table = find_tab(tab);
            if(table == nullptr)
                table = add_panel(tab);
            if(row > table->rowCount()) expand_vtable(table, row - table->rowCount());
            if(column > table->columnCount()) expand_htable(table, column - table->columnCount());

            for (int i = 0; i < row; i++) {
                for (int j = 0; j < column; j++) {
                    QString func;
                    in >> func;
                    QPair<int, int> pir = { i, j };
                    CaculateTableItem* item = dynamic_cast<CaculateTableItem*>(table->item(i, j));

                    auto final = Parser().parse(pir, table, func);
                    if (final.first == STRING_STATE::INVALID || final.first == STRING_STATE::HAS_CYCLE
                        || final.first == STRING_STATE::NOT_NUMBER) {
                        item->setText(func);
                    }
                    else {
                        item->update_caculate(final.second);
                        item->update_func(func);
                        item->setText(QString::number(final.second));
                        table->gph_->traverse(pir, [table](const QPair<int,int>& value) mutable -> void {
                            CaculateTableItem* item = dynamic_cast<CaculateTableItem*>
                                (table->item(value.first, value.second));
                            auto final = Parser().parse(value, table, item->get_func());
                            item->update_caculate(final.second);
                            item->setText(QString::number(final.second));
                        });
                    }
                }
            }
        }
        file.close();
        blockSignals(false);
        auto last = std::chrono::high_resolution_clock().now();
        qDebug() << std::chrono::duration_cast<std::chrono::milliseconds>(last - start) << '\n';
        return true;
    }
    else {
        qDebug() << u8"Unable to open file for reading.";
        blockSignals(false);
        return false;
    }
}

bool MainWindow::save_binary(const QString& file_path) {
    blockSignals(true);
    auto start = std::chrono::high_resolution_clock::now();
    QFile file(file_path);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        for(int i=0;i<tables_.size();i++){
            QTableWidget* table = tables_.at(i);
            uint32_t row = table->rowCount();
            uint32_t col = table->columnCount();
            out << row << col << ui->main_tab->tabText(i);
            for (uint32_t i = 0; i < row; i++) {
                for (uint32_t j = 0; j < col; j++) {
                    CaculateTableItem* item = dynamic_cast<CaculateTableItem*>(table->item(i, j));
                    QString func = item->get_func();
                    QString str = (func.isEmpty() ? item->text() : func);
                    out << str;
                }
            }
        }
        file.close();
        auto last = std::chrono::high_resolution_clock().now();
        qDebug() << std::chrono::duration_cast<std::chrono::milliseconds>(last - start) << '\n';
        blockSignals(false);
        return true;
    }
    else {
        qDebug() << u8"Unable to open file for writing.";
        blockSignals(false);
        return false;
    }
}

bool MainWindow::read_mcsv(const QString& file_path) {
    blockSignals(true);
    auto start = std::chrono::high_resolution_clock::now();
    QFile file(file_path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.isEmpty()) continue;
            QStringList parts = line.split(',');
            if (parts.size() < 3) continue;
            int row = parts[0].toInt();
            int column = parts[1].toInt();
            if(row==0&&column==0) continue;
            QString tab = parts[2];

            CaculateTable* table = find_tab(tab);
            if (table == nullptr)
                table = add_panel(tab);
            if (row > table->rowCount()) expand_vtable(table, row);
            if (column > table->columnCount()) expand_htable(table, column);

            for (int i = 0; i < row; i++) {
                line = in.readLine();
                QStringList cells = line.split(',');
                int col = cells.size();
                for (int j = 0; j < col; j++) {
                    QString cell = cells[j];
                    if (cell.startsWith('"') && cell.endsWith('"'))
                        cell = cell.mid(1, cell.length() - 2);

                    QPair<int, int> pir = {i, j};
                    CaculateTableItem* item = dynamic_cast<CaculateTableItem*>(table->item(i, j));
                    auto final = Parser().parse(pir, table, cell);
                    if(final.first == STRING_STATE::INVALID || final.first == STRING_STATE::HAS_CYCLE
                        || final.first == STRING_STATE::NOT_NUMBER)
                        item->setText(cell);
                    else{
                        item->update_caculate(final.second);
                        item->update_func(cell);
                        item->setText(QString::number(final.second));
                        table->gph_->traverse(pir, [table](const QPair<int,int>& value) mutable -> void {
                            CaculateTableItem* item = dynamic_cast<CaculateTableItem*>
                                (table->item(value.first, value.second));
                            auto final = Parser().parse(value, table, item->get_func());
                            item->update_caculate(final.second);
                            item->setText(QString::number(final.second));
                        });
                    }
                }
            }
        }
        file.close();
        auto last = std::chrono::high_resolution_clock().now();
        qDebug() << std::chrono::duration_cast<std::chrono::milliseconds>(last - start) << '\n';
        blockSignals(false);
        return true;
    } else {
        qDebug() << u8"Unable to open file for reading.";
        blockSignals(false);
        return false;
    }
}

bool MainWindow::save_mcsv(const QString& file_path) {
    blockSignals(true);
    auto start = std::chrono::high_resolution_clock().now();
    QFile file(file_path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (int i = 0; i < tables_.size(); i++) {
            CaculateTable* table = tables_.at(i);
            int row = table->rowCount();
            int column = table->columnCount();
            QString tab = ui->main_tab->tabText(i);
            out << row << "," << column << "," << tab << "\n";

            for(int i=0;i<row;i++){
                for(int j=0;j<column;j++){
                    CaculateTableItem* item = dynamic_cast<CaculateTableItem*>(table->item(i,j));
                    out << (item->get_func().isEmpty() ? item->text() : ('"' + item->get_func() + '"'));
                    if(j!=column-1) out << ",";
                }
                out << "\n";
            }
        }
        file.close();
        auto last = std::chrono::high_resolution_clock().now();
        qDebug() << std::chrono::duration_cast<std::chrono::milliseconds>(last - start) << '\n';
        blockSignals(false);
        return true;
    } else {
        qDebug() << u8"Unable to open file for writing.";
        blockSignals(false);
        return false;
    }
}

bool MainWindow::read_csv(CaculateTable* table, const QString& path) {
    auto start = std::chrono::high_resolution_clock().now();
    table->blockSignals(true);
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int row=0;
        while (!in.atEnd()) {
            if(row > table->rowCount()) expand_vtable(table);
            QString line = in.readLine();
            QStringList list;
            int start = 0;
            bool inQuotes = false;

            for (int i = 0; i < line.length(); ++i) {
                if (line[i] == '"') {
                    inQuotes = !inQuotes;
                }
                else if (line[i] == ',' && !inQuotes) {
                    QString cell = line.mid(start, i - start);
                    if (cell.startsWith('"') && cell.endsWith('"'))
                        cell = cell.mid(1, cell.length() - 2);
                    list.append(cell);
                    start = i + 1;
                }
            }
            QString lastCell = line.mid(start);
            if (lastCell.startsWith('"') && lastCell.endsWith('"'))
                lastCell = lastCell.mid(1, lastCell.length() - 2);
            list.append(lastCell);

            int column = list.size();
            if(column > table->columnCount()) expand_htable(table, column);

            for(int i=0;i<column;i++){
                QString str = list.at(i);
                QPair<int,int> pir = { row, i };
                auto final = Parser().parse(pir, table, str);
                CaculateTableItem* item = dynamic_cast<CaculateTableItem*>(table->item(row, i));
                if(!item) {
                    item = new CaculateTableItem();
                    table->setItem(row, i, item);
                }
                if(final.first == STRING_STATE::INVALID || final.first == STRING_STATE::HAS_CYCLE
                    || final.first == STRING_STATE::NOT_NUMBER)
                    item->setText(str);
                else{
                    item->update_caculate(final.second);
                    item->update_func(str);
                    item->setText(QString::number(final.second));
                    table->gph_->traverse(pir, [table](const QPair<int,int>& value) mutable -> void {
                        CaculateTableItem* item = dynamic_cast<CaculateTableItem*>
                            (table->item(value.first, value.second));
                        auto final = Parser().parse(value, table, item->get_func());
                        item->update_caculate(final.second);
                        item->setText(QString::number(final.second));
                    });
                }
            }
            row++;
        }
        file.close();
        table->blockSignals(false);
        auto last = std::chrono::high_resolution_clock().now();
        qDebug() << std::chrono::duration_cast<std::chrono::milliseconds>(last - start) << '\n';
        return true;
    }
    else {
        qDebug() << u8"Unable to open file for reading.";
        table->blockSignals(false);
        return false;
    }
}

bool MainWindow::save_csv(const QString& exc_path){
    blockSignals(true);
    auto start = std::chrono::high_resolution_clock().now();
    QFile file(exc_path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        QTableWidget* table = tables_.at(0);
        uint32_t row = table->rowCount();
        uint32_t col = table->columnCount();
        for(uint32_t i=0;i<row;i++){
            for(uint32_t j=0;j<col;j++){
                CaculateTableItem* item = dynamic_cast<CaculateTableItem*>(table->item(i,j));
                out << (item->get_func().isEmpty() ? item->text() : '"' + item->get_func() + '"');
                if(j!=col-1) out << ",";
            }
            out << "\n";
        }
        file.close();
        qDebug() << u8"CSV file written successfully.";
        auto last = std::chrono::high_resolution_clock().now();
        qDebug() << std::chrono::duration_cast<std::chrono::milliseconds>(last - start) << '\n';
        blockSignals(false);
        return true;
    }
    else {
        qDebug() << u8"Unable to open file for writing.";
        blockSignals(false);
        return false;
    }
}

bool MainWindow::read_excel(const QString& path){
    auto start = std::chrono::high_resolution_clock().now();
    blockSignals(true);
    QXlsx::Document* xlsx = new QXlsx::Document(path);
    if(!xlsx->load()){
        qDebug() << u8"打开失败";
        blockSignals(false);
        delete xlsx;
        return false;
    }
    QStringList sheets = xlsx->sheetNames();
    for(auto tab = sheets.begin(); tab!=sheets.end();++tab){
        QXlsx::AbstractSheet *csheet = xlsx->sheet(*tab);
        if (!csheet) continue;
        csheet->workbook()->setActiveSheet(sheets.indexOf(*tab));
        QXlsx::Worksheet *wsheet = dynamic_cast<QXlsx::Worksheet*>(csheet->workbook()->activeSheet());
        if (!wsheet) continue;

        int row = wsheet->dimension().rowCount();
        int column = wsheet->dimension().columnCount();
        CaculateTable* table = find_tab(*tab);
        if(table == nullptr)
            table = add_panel(*tab);
        if(row > table->rowCount()) expand_vtable(table, row);
        if(column > table->columnCount()) expand_htable(table, column);

        for(int i=0;i<row;i++){
            for(int j=0;j<column;j++){
                QVariant value = wsheet->read(i+1, j+1);
                if(!value.isNull()) {
                    QString str = value.toString();
                    QPair<int,int> pir = { i, j };
                    auto final = Parser().parse(pir, table, str);
                    if(final.first == STRING_STATE::VALID || final.first == STRING_STATE::IS_EMPTY){
                        CaculateTableItem* item = dynamic_cast<CaculateTableItem*>(table->item(i, j));
                        item->update_caculate(final.second);
                        item->update_func(str);
                        item->setText(QString::number(final.second));
                        table->gph_->traverse(pir, [table](const QPair<int,int>& value) mutable -> void {
                            CaculateTableItem* item = dynamic_cast<CaculateTableItem*>
                                (table->item(value.first, value.second));
                            auto final = Parser().parse(value, table, item->get_func());
                            item->update_caculate(final.second);
                            item->setText(QString::number(final.second));
                        });
                    }
                    else {
                        dynamic_cast<CaculateTableItem*>(table->item(i, j))->setText(str);
                    }
                }
                else {
                    table->item(i, j)->setText("");
                }
            }
        }
    }
    delete xlsx;
    blockSignals(false);
    auto last = std::chrono::high_resolution_clock::now();
    qDebug() << std::chrono::duration_cast<std::chrono::milliseconds>(last - start) << '\n';
    return true;
}

bool MainWindow::save_excel(const QString& path){
    blockSignals(true);
    auto start = std::chrono::high_resolution_clock::now();
    QXlsx::Document* xlsx = new QXlsx::Document(path);
    for(uint32_t idx = 0; idx<tables_.size();idx++){
        QString var = ui->main_tab->tabText(idx);
        CaculateTable* table = tables_[idx];
        xlsx->addSheet(var);
        QXlsx::AbstractSheet *csheet=xlsx->sheet(var);
        csheet->workbook()->setActiveSheet(idx);
        QXlsx::Worksheet *wsheet = dynamic_cast<QXlsx::Worksheet*>(csheet->workbook()->activeSheet());
        if (!wsheet) continue;
        uint32_t row = table->rowCount();
        uint32_t column = table->columnCount();
        for(uint32_t i=0;i<row;i++){
            for(uint32_t j=0;j<column;j++){
                CaculateTableItem* item = dynamic_cast<CaculateTableItem*>(table->item(i, j));
                if(!item) continue;
                QString func = item->get_func();
                wsheet->write(i+1, j+1, (func.isEmpty() ? item->text() : func));
            }
        }
    }
    if(!xlsx->save()) {
        delete xlsx;
        blockSignals(false);
        return false;
    }
    else {
        delete xlsx;
        auto last = std::chrono::high_resolution_clock().now();
        qDebug() << std::chrono::duration_cast<std::chrono::milliseconds>(last - start) << '\n';
        blockSignals(false);
        return true;
    }
}

void MainWindow::func_btn_clicked(){
    auto check = Parser::get_position(ui->position_lab->text());
    if(!check.first) return;
    QPair<int, int> pir = check.second;
    int idx = ui->main_tab->currentIndex();
    funcwindow* w = new funcwindow(pir, tables_.at(idx));
    w->setWindowModality(Qt::ApplicationModal);
    w->show();
}

void MainWindow::correct_btn_clicked(){
    auto check = Parser::get_position(ui->position_lab->text());
    if(!check.first) return;
    QPair<int, int> pir = check.second;
    int idx = ui->main_tab->currentIndex();
    CaculateTable* table = tables_.at(idx);
    CaculateTableItem* item = dynamic_cast<CaculateTableItem*>(table->item(pir.first, pir.second));
    QString str = ui->func_label->text();
    auto final = Parser().parse(pir, table, str);
    if(final.first == STRING_STATE::VALID || final.first == STRING_STATE::IS_EMPTY) {
        item->update_func(str);
        item->update_caculate(final.second);
        blockSignals(true);
        table->gph_->traverse(pir, [table](const QPair<int,int>& value) mutable -> void {
            CaculateTableItem* item = dynamic_cast<CaculateTableItem*>
                (table->item(value.first, value.second));
            auto final = Parser().parse(value, table, item->get_func());
            item->update_caculate(final.second);
            item->setText(QString::number(final.second));
        });
        item->setText(QString::number(final.second));
    }
    else{
        if(final.first == STRING_STATE::HAS_CYCLE)
            QMessageBox::warning(this, u8"警告", u8"插入的公式将造成环，此公式将不会被执行");
        else
            QMessageBox::warning(this, u8"警告", u8"插入的公式非法，此公式将不会被执行");
        blockSignals(true);
        item->setText(str);
    }
    ui->func_label->setText(str);
    blockSignals(false);
}

void MainWindow::cancel_btn_clicked(){
    auto check = Parser::get_position(ui->position_lab->text());
    if(!check.first) return;
    QPair<int, int> pir = check.second;
    int idx = ui->main_tab->currentIndex();
    CaculateTable* table = tables_.at(idx);
    CaculateTableItem* item = dynamic_cast<CaculateTableItem*>(table->item(pir.first, pir.second));
    blockSignals(true);
    if(!item->get_func().isEmpty()) {
        table->gph_->delete_income_edges(pir);
        table->gph_->delete_outgo_edges(pir);
        item->clear();
        item->setText("");
    }
    ui->func_label->clear();
    blockSignals(false);
}

void MainWindow::add_table_btn_clicked(){
    add_panel()->setFocus();
    ui->main_tab->setCurrentIndex(tables_.size()-1);
}

void MainWindow::func_lab_changed(QString str){
    auto check = Parser::get_position(ui->position_lab->text());
    if(!check.first) return;
    QPair<int, int> pir = check.second;
    int idx = ui->main_tab->currentIndex();
    CaculateTable* table = tables_.at(idx);
    CaculateTableItem* item = dynamic_cast<CaculateTableItem*>(table->item(pir.first, pir.second));
    if(!item) return;
    table->blockSignals(true);
    auto final = Parser().parse(pir, table, str);
    if(final.first == STRING_STATE::VALID || final.first == STRING_STATE::IS_EMPTY){
        item->update_func(str);
        item->update_caculate(final.second);
        item->setText(QString::number(final.second));
        table->gph_->traverse(pir, [table](const QPair<int,int>& value) mutable -> void {
            CaculateTableItem* item = dynamic_cast<CaculateTableItem*>
                (table->item(value.first, value.second));
            auto final = Parser().parse(value, table, item->get_func());
            item->update_caculate(final.second);
            table->blockSignals(true);
            table->item(value.first, value.second)->setText(QString::number(final.second));
            table->blockSignals(false);
        });
    }
    else{
        if(!item->get_func().isEmpty()){
            item->clear();
        }
        item->setText(str);
    }
    table->blockSignals(false);
}

void MainWindow::position_lab_changed(QString str){
    blockSignals(true);
    auto check = Parser::get_position(str);
    if(!check.first) return;
    QPair<int, int> pir = check.second;
    int idx = ui->main_tab->currentIndex();
    CaculateTable* table = tables_.at(idx);
    table->setCurrentCell(pir.first, pir.second);
    CaculateTableItem* item = dynamic_cast<CaculateTableItem*>(table->item(pir.first, pir.second));
    table_view(item);
    blockSignals(false);
}

void MainWindow::tab_double_clicked(int index){
    QObject *send = sender();
    if (send) {
        QTabWidget *tab = qobject_cast<QTabWidget*>(send);
        QString cur_text = tab->tabText(index);
        bool ok;
        QString new_text = QInputDialog::getText(this, u8"更改文本", u8"输入文本：",
                                                 QLineEdit::Normal, cur_text, &ok);
        if (ok && !new_text.isEmpty())
            tab->setTabText(index, new_text);
    }
}

void MainWindow::table_view(QTableWidgetItem* item){
    blockSignals(true);
    CaculateTableItem* ptr = dynamic_cast<CaculateTableItem*>(item);
    if(!ptr) return;
    ui->position_lab->setText(CaculateTableItem::column_alpha(ptr->column()) + QString::number(ptr->row() + 1));
    QString func = ptr->get_func();
    if(func.isEmpty()) ui->func_label->setText(ptr->text());
    else {
        ui->func_label->setText(ptr->get_func());
        ptr->setText(QString::number(ptr->get_caculate()));
    }
    blockSignals(false);
}


void MainWindow::table_item_changed(QTableWidgetItem* item){
    blockSignals(true);
    QPair<int, int> pir = {item->row(), item->column()};
    CaculateTableItem* ptr = dynamic_cast<CaculateTableItem*>(item);
    int idx = ui->main_tab->currentIndex();
    CaculateTable* table = tables_.at(idx);
    if(!ptr) return;
    ui->position_lab->setText(CaculateTableItem::column_alpha(ptr->column()) + QString::number(ptr->row() + 1));
    QString str = item->text();
    auto final = Parser().parse(pir, table, str);
    if(final.first == STRING_STATE::VALID || final.first == STRING_STATE::IS_EMPTY){
        ptr->update_func(str);
        ptr->update_caculate(final.second);
        ptr->setText(QString::number(final.second));
        table->gph_->traverse(pir, [table](const QPair<int,int>& value) mutable -> void {
            CaculateTableItem* item = dynamic_cast<CaculateTableItem*>
                (table->item(value.first, value.second));
            auto final = Parser().parse(value, table, item->get_func());
            item->update_caculate(final.second);
            table->item(value.first, value.second)->setText(QString::number(final.second));
        });
    }
    else{
        if(!ptr->get_func().isEmpty()){
            ptr->clear();
        }
        ptr->setText(str);
    }
    blockSignals(false);
}

void MainWindow::table_vertical_bar_changed(int value){
    QObject *send = sender();
    if (send) {
        QScrollBar *bar = qobject_cast<QScrollBar*>(send);
        if (value == bar->maximum()) {
            CaculateTable* table = tables_.at(ui->main_tab->currentIndex());
            expand_vtable(table);
        }
    }
}

void MainWindow::table_horizontal_bar_changed(int value){
    QObject *send = sender();
    if (send) {
        QScrollBar *bar = qobject_cast<QScrollBar*>(send);
        if (value == bar->maximum()) {
            CaculateTable* table = tables_.at(ui->main_tab->currentIndex());
            expand_htable(table);
        }
    }
}

void MainWindow::read_binary_triggered(bool) {
    QString exc_path = QFileDialog::getOpenFileName(this, QStringLiteral("选择载入文件")
                                                    , setting::desk_path, "Txt Files(*.txt)");
    if(exc_path.isEmpty()) return;
    QMessageBox msg;
    msg.setInformativeText(u8"若载入，当前工作区成员会与载入文件成员合并。是否继续？");
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Yes);
    int ret = msg.exec();
    if(ret==QMessageBox::Cancel) return;
    if(!read_binary(exc_path)){
        QMessageBox msgBox;
        msgBox.setText(u8"载入失败，请检查路径");
        msgBox.exec();
    }
}

void MainWindow::save_binary_triggered(bool){
    QString exc_path = QFileDialog::getSaveFileName(this, QStringLiteral("选择导出路径")
                                                    , setting::desk_path, "Txt Files(*.txt)");
    if(exc_path.isEmpty()) return;
    save_binary(exc_path);
}

void MainWindow::read_mcsv_triggered(bool) {
    QString exc_path = QFileDialog::getOpenFileName(this, QStringLiteral("选择载入文件")
                                                    , setting::desk_path, "Mcsv Files(*.mcsv)");
    if(exc_path.isEmpty()) return;
    QMessageBox msg;
    msg.setInformativeText(u8"若载入，当前工作区成员会与载入文件成员合并。是否继续？");
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Yes);
    int ret = msg.exec();
    if(ret==QMessageBox::Cancel) return;
    if(!read_mcsv(exc_path)){
        QMessageBox msgBox;
        msgBox.setText(u8"载入失败，请检查路径");
        msgBox.exec();
    }
}

void MainWindow::save_mcsv_triggered(bool){
    QString exc_path = QFileDialog::getSaveFileName(this, QStringLiteral("选择导出路径")
                                                    , setting::desk_path, "Mcsv Files(*.mcsv)");
    if(exc_path.isEmpty()) return;
    save_mcsv(exc_path);
}

void MainWindow::read_csv_triggered(bool) {
    QString exc_path = QFileDialog::getOpenFileName(this, QStringLiteral("选择载入文件")
                                                    , setting::desk_path, "Csv Files(*.csv)");
    if(exc_path.isEmpty()) return;
    QMessageBox msg;
    msg.setInformativeText(u8"若载入，当前工作区成员会与载入文件成员合并。是否继续？");
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Yes);
    int ret = msg.exec();
    if(ret==QMessageBox::Cancel) return;
    CaculateTable* table = tables_.at(ui->main_tab->currentIndex());
    if(!read_csv(table, exc_path)){
        QMessageBox msgBox;
        msgBox.setText(u8"载入失败，请检查路径");
        msgBox.exec();
    }
}

void MainWindow::save_csv_triggered(bool){
    QString exc_path = QFileDialog::getSaveFileName(this, QStringLiteral("选择导出路径")
                                                    , setting::desk_path, "Csv Files(*.csv)");
    if(exc_path.isEmpty()) return;
    save_csv(exc_path);
}

void MainWindow::read_excel_triggered(bool){
    QString exc_path = QFileDialog::getOpenFileName(this, QStringLiteral("选择载入文件")
                                                    , setting::desk_path, "Xlsx Files(*.xlsx)");
    if(exc_path.isEmpty()) return;
    QMessageBox msg;
    msg.setInformativeText(u8"若载入，当前工作区成员会与载入文件成员合并。是否继续？");
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Yes);
    int ret = msg.exec();
    if(ret==QMessageBox::Cancel) return;
    if(!read_excel(exc_path)){
        QMessageBox msgBox;
        msgBox.setText(u8"载入失败，请检查路径");
        msgBox.exec();
    }
}

void MainWindow::save_excel_triggered(bool){
    QString exc_path = QFileDialog::getSaveFileName(this, QStringLiteral("选择导出路径")
                                                    , setting::desk_path, "Xlsx Files(*.xlsx)");
    if(exc_path.isEmpty()) return;
    save_excel(exc_path);
}

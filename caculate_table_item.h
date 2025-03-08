#ifndef CACULATE_TABLE_ITEM_H
#define CACULATE_TABLE_ITEM_H
#include <QTableWidgetItem>
#include "graph.h"

class CaculateTableItem : public QTableWidgetItem{
public:
    QString func_{};
    double caculate_final_=0.0;

public:
    CaculateTableItem() : QTableWidgetItem() {}
    CaculateTableItem(const QString& str) : QTableWidgetItem(str) {}
    ~CaculateTableItem() = default;

    static QString column_alpha(int column) {
        QString fin;
        while (column >= 0) {
            int remainder = column % 26;
            fin = QChar('A' + remainder) + fin;
            column = (column / 26) - 1;
        }
        return fin;
    }

    QString get_func() const {
        return func_;
    }
    void update_func(const QString& str){
        func_ = str;
    }
    double get_caculate() const { return caculate_final_; }
    void update_caculate(double x) { caculate_final_ = x;  }

    void clear() {
        func_ = "";
        caculate_final_ = 0.0;
    }
};

class CaculateTable : public QTableWidget{
public:
    graph<QPair<int, int>>* gph_;

    CaculateTable() : gph_(new graph<QPair<int, int>>) {}
    ~CaculateTable() { delete gph_; }
};

#endif // CACULATE_TABLE_ITEM_H

#ifndef CACULATE_H
#define CACULATE_H
#include <QString>
#include <QVector>
#include <QDebug>
#include <QTableWidget>
#include <QString>
#include <QVector>
#include <deque>
#include <QDebug>
#include "mathlib.h"
#include "caculate_table_item.h"

inline bool is_number(const std::string& str) {
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return!str.empty();
}

enum class STRING_STATE{
    VALID, INVALID, HAS_CYCLE, NOT_NUMBER, IS_EMPTY
};

struct Token {
    enum TYPE { Number, Operator, Function1, Function2, Cell, Comma,Colon, LeftParen, RightParen, End };
    TYPE type;
    QString str_val;
    double num_val;
};

class Parser {
private:
    const QPair<int, int>* posi_;
    const QString* expr_;
    int index_;
    Token current_token;
    CaculateTable* table_;

    void get_next_token() {
        while (index_ < expr_->size() && expr_->at(index_).isSpace()) {
            index_++;
        }
        if (index_ >= expr_->size()) {
            current_token.type = Token::End;
            current_token.str_val = "";
            return;
        }

        QChar c = expr_->at(index_);
        if (c.isDigit() || c == '.') {
            QString num_str;
            bool has_dot = false;
            while (index_ < expr_->size() && (expr_->at(index_).isDigit() || expr_->at(index_) == '.')) {
                if (expr_->at(index_) == '.') {
                    if (has_dot) break;
                    has_dot = true;
                }
                num_str += expr_->at(index_);
                index_++;
            }
            current_token.type = Token::Number;
            current_token.num_val = num_str.toDouble();
        } else if (c.isLetter()) {
            bool f = false;
            QString func_str;
            while (index_ < expr_->size() && expr_->at(index_).isLetter()) {
                func_str += expr_->at(index_).toLower();
                index_++;
            }
            while (index_ < expr_->size() && expr_->at(index_).isDigit()) {
                f = true;
                func_str += expr_->at(index_);
                index_++;
            }
            if (func_str == "sum" || func_str == "avg" || func_str == "gcd" || func_str == "lcm" ||
                func_str == "power" || func_str == "log" || func_str == "log10" || func_str == "log2" ||
                func_str == "loge") {
                current_token.type = Token::Function2;
            }
            else if(func_str=="abs" || func_str=="sin" || func_str=="cos" || func_str=="tan" ||
                       func_str=="asin" || func_str=="acos" || func_str=="atan" || func_str=="angular" ||
                       func_str=="radian" || func_str=="sqrt" || func_str=="cbrt" || func_str=="fact" ||
                       func_str=="floor" || func_str=="ceil" || func_str=="round"){
                current_token.type = Token::Function1;
            }
            else if(f){
                current_token.type = Token::Cell;
            }
            else{
                current_token.type=Token::End;
            }
            current_token.str_val = func_str;
        } else if (c == '+' || c == '-' || c == '*' || c == '/'||c=='%') {
            current_token.type = Token::Operator;
            current_token.str_val = c;
            index_++;
        } else if (c == '(') {
            current_token.type = Token::LeftParen;
            index_++;
        } else if (c == ')') {
            current_token.type = Token::RightParen;
            index_++;
        } else if (c == ',') {
            current_token.type = Token::Comma;
            index_++;
        } else if(c==':'){
            current_token.type = Token::Colon;
            index_++;
        }else {
            current_token.type = Token::End;
        }
    }

    QPair<STRING_STATE, double> parse_expression() {
        QPair<STRING_STATE, double> result = parse_term();
        if (!(result.first == STRING_STATE::VALID || result.first == STRING_STATE::IS_EMPTY)) {
            return result;
        }
        while (current_token.type == Token::Operator &&
               (current_token.str_val == "+" || current_token.str_val == "-")) {
            QString op = current_token.str_val;
            get_next_token();
            QPair<STRING_STATE, double> term = parse_term();
            if (!(term.first == STRING_STATE::VALID || term.first == STRING_STATE::IS_EMPTY)) {
                return term;
            }
            if (op == "+") {
                result.second += term.second;
            } else {
                result.second -= term.second;
            }
        }
        return result;
    }

    QPair<STRING_STATE, double> parse_term() {
        QPair<STRING_STATE, double> result = parse_factor();
        if (!(result.first == STRING_STATE::VALID || result.first == STRING_STATE::IS_EMPTY)) {
            return result;
        }
        while (current_token.type == Token::Operator &&
               (current_token.str_val == "*" || current_token.str_val == "/"|| current_token.str_val == "%")) {
            QString op = current_token.str_val;
            get_next_token();
            QPair<STRING_STATE, double> factor = parse_factor();
            if (!(factor.first == STRING_STATE::VALID || factor.first == STRING_STATE::IS_EMPTY)) {
                return factor;
            }
            if (op == "*") {
                result.second *= factor.second;
            } else if(op=="/"){
                result.second /= factor.second;
            }else{
                result.second = std::fmod(result.second,factor.second);
            }
        }
        return result;
    }

    QPair<STRING_STATE, double> parse_factor() {
        if (current_token.type == Token::Number) {
            double value = current_token.num_val;
            get_next_token();
            return { STRING_STATE::VALID, value };
        } else if (current_token.type == Token::Function1) {
            QString func = current_token.str_val;
            get_next_token();
            if (current_token.type != Token::LeftParen) {
                return { STRING_STATE::INVALID, 0.0 };
            }
            get_next_token();
            QPair<STRING_STATE, double> arg = parse_expression();
            if (!(arg.first == STRING_STATE::VALID || arg.first == STRING_STATE::IS_EMPTY)) {
                return arg;
            }
            if (current_token.type != Token::RightParen) {
                return { STRING_STATE::INVALID, 0.0 };
            }
            get_next_token();
            return { STRING_STATE::VALID, apply_function1(func, arg.second) };
        } else if(current_token.type == Token::Function2){
            QString func = current_token.str_val;
            std::deque<double> args;
            get_next_token();
            if (current_token.type != Token::LeftParen) {
                return { STRING_STATE::INVALID, 0.0 };
            }
            get_next_token();
            while(index_ < expr_->size() && (expr_->at(index_)==',' || expr_->at(index_)==':')){
                if(expr_->at(index_)==':'){
                    QPair<int,int> begin_position;
                    if(current_token.type!=Token::Cell){
                        return { STRING_STATE::INVALID, 0.0 };
                    }
                    QString cell=current_token.str_val;
                    begin_position = get_position(cell).second;
                    if(begin_position.first >= table_->rowCount() ||begin_position.second>= table_->columnCount()){
                        return { STRING_STATE::INVALID, 0.0 };
                    }
                    get_next_token();
                    get_next_token();
                    if(current_token.type!=Token::Cell){
                        return { STRING_STATE::INVALID, 0.0 };
                    }
                    cell=current_token.str_val;
                    QPair<int,int> end_position=get_position(cell).second;
                    if(end_position.first >= table_->rowCount() || end_position.second>= table_->columnCount()){
                        return { STRING_STATE::INVALID, 0.0 };
                    }
                    if(begin_position.first>end_position.first)
                    {
                        std::swap(begin_position.first,end_position.first);
                    }
                    if(begin_position.second>end_position.second)
                    {
                        std::swap(begin_position.second,end_position.second);
                    }
                    for(int i=begin_position.first;i<=end_position.first;i++){
                        for(int j=begin_position.second;j<=end_position.second;j++){
                            args.push_back(stod(table_->item(i, j)->text().toStdString()));
                        }
                    }
                    get_next_token();
                    if(current_token.type==Token::Comma){
                        get_next_token();
                    }
                }else if(expr_->at(index_)==','){
                    QPair<STRING_STATE, long double> arg=parse_expression();
                    if (!(arg.first == STRING_STATE::VALID || arg.first == STRING_STATE::IS_EMPTY)) {
                        return { STRING_STATE::INVALID, 0.0 };
                    }
                    args.push_back(arg.second);
                    get_next_token();
                }
            }
            if (current_token.type != Token::RightParen) {
                QPair<STRING_STATE, long double> tem=parse_expression();
                if (!(tem.first == STRING_STATE::VALID || tem.first == STRING_STATE::IS_EMPTY)) {
                    return { STRING_STATE::INVALID, 0.0 };
                }
                args.push_back(tem.second);
            }
            if (current_token.type != Token::RightParen){
                return { STRING_STATE::INVALID, 0.0 };
            }
            get_next_token();
            return { STRING_STATE::VALID, apply_function2(func, args)};
        } else if (current_token.type == Token::Cell) {
            QString cell = current_token.str_val;
            QPair<int, int> tem = get_position(cell).second;
            int row = tem.first;
            int column = tem.second;
            if(row >= table_->rowCount() || column >= table_->columnCount()) {
                get_next_token();
                return { STRING_STATE::IS_EMPTY, 0.0 };
            }
            CaculateTableItem* item = dynamic_cast<CaculateTableItem*>(table_->item(row, column));
            auto gph_ = table_->gph_;
            gph_->insert_node(tem);
            gph_->insert_edge(tem, *posi_);
            if (gph_->depth_first_search()) {
                gph_->delete_edge(tem, *posi_);
                get_next_token();
                return { STRING_STATE::HAS_CYCLE, 0.0 };
            }
            if(!item->get_func().isEmpty()){
                get_next_token();
                return { STRING_STATE::VALID, item->get_caculate() };
            }
            QString str = item->text();
            if(str.isEmpty()) {
                get_next_token();
                return { STRING_STATE::IS_EMPTY, 0.0 };
            }
            else if (is_number(str.toStdString())) {
                double result = std::stod(str.toStdString());
                get_next_token();
                return { STRING_STATE::VALID, result };
            }
            else {
                return { STRING_STATE::NOT_NUMBER, 0.0 };
            }
        } else if (current_token.type == Token::LeftParen) {
            get_next_token();
            QPair<STRING_STATE, double> value = parse_expression();
            if (!(value.first == STRING_STATE::VALID || value.first == STRING_STATE::IS_EMPTY)) {
                return value;
            }
            if (current_token.type != Token::RightParen) {
                return { STRING_STATE::INVALID, 0.0 };
            }
            get_next_token();
            return { STRING_STATE::VALID, value.second };
        } else if (current_token.type == Token::Operator &&
                   (current_token.str_val == "+" || current_token.str_val == "-")) {
            QString op = current_token.str_val;
            get_next_token();
            QPair<STRING_STATE, double> factor = parse_factor();
            if (!(factor.first == STRING_STATE::VALID || factor.first == STRING_STATE::IS_EMPTY)) {
                return factor;
            }
            if (op == "-") {
                return { STRING_STATE::VALID, -factor.second };
            } else {
                return factor;
            }
        } else {
            return { STRING_STATE::INVALID, 0.0 };
        }
    }

    double apply_function1(const QString& func, double arg) {
        if (func == "abs")
            return absolute(arg);
        else if (func == "sin")
            return sine(arg);
        else if (func == "cos")
            return cosine(arg);
        else if (func == "tan")
            return tangent(arg);
        else if (func == "asin")
            return arcsine(arg);
        else if (func == "acos")
            return arccosine(arg);
        else if (func == "atan")
            return arctangent(arg);
        else if (func == "sqrt")
            return square_root(arg);
        else if (func == "cbrt")
            return cube_root(arg);
        else if (func == "fact")
            return factorial((mathui_t)arg);
        else if (func == "floor")
            return floor(arg);
        else if (func == "ceil")
            return ceil(arg);
        else if (func == "round")
            return round(arg);
        return 0.0;
    }

    double apply_function2(const QString& func, const std::deque<double>& args) {
        if (func == "sum") {
            double sum = 0;
            for (double arg : args) {
                sum += arg;
            }
            return sum;
        }
        else if (func == "avg") {
            double sum = 0;
            for (double arg : args) {
                sum += arg;
            }
            return sum / args.size();
        }
        else if (func == "gcd")
            return gcd((size_t)args[0], (size_t)args[1]);
        else if (func == "lcm")
            return lcm((size_t)args[0], (size_t)args[1]);
        else if (func == "power")
            return power(args[0], args[1]);
        else if (func == "log")
            return logarithm(args[0], args[1]);
        else if (func == "log2")
            return logarithm_2(args[0]);
        else if (func == "log10")
            return logarithm_10(args[0]);
        else if (func == "loge")
            return logarithm_e(args[0]);
        return 0.0;
    }

public:
    static QPair<bool, QPair<int, int>> get_position(const QString& cell) {
        int count = 0, column = 0, row = 0;

        if (cell.isEmpty() ||!cell[0].isLetter())
            return { false, { -1, -1 } };

        while (count < cell.size() && cell[count].isLetter()) {
            column = column * 26 + (cell[count].toLower().unicode() - 'a' + 1);
            count++;
        }

        if (count == cell.size())
            return { false, { -1, -1 } };
        for (int i = count; i < cell.size(); ++i) {
            if (!cell[i].isDigit())
                return { false, { -1, -1 } };
        }
        row = cell.mid(count).toInt();
        if (row <= 0 || column <= 0)
            return { false, { -1, -1 } };

        return { true, { row - 1, column - 1 } };
    }

    QPair<STRING_STATE, double> parse(QPair<int, int> posi, CaculateTable* table, const QString& expr) {
        if(!expr.startsWith('='))
            return { STRING_STATE::INVALID, 0.0 };
        table_ = table;
        posi_ = &posi;
        expr_ = &expr;
        index_ = 1;
        get_next_token();
        QPair<STRING_STATE, double> result = parse_expression();
        if (current_token.str_val.isEmpty())
            return result;
        else
            return { STRING_STATE::INVALID, 0.0 };
    }
};

#endif // CACULATE_H

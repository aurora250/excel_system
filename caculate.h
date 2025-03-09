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
#include <cmath>
#include <string>
#include "mathlib.h"
#include "caculate_table_item.h"

using namespace std;

inline bool is_number(const std::string& str) {
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return!str.empty();
}

inline double sum(const std::vector<double>& args) noexcept {
    double result=0;
    for(int i=0;i<args.size();i++)
    {
        result+=args[i];
    }
    return result;
}

constexpr int lcm(int m, int n) noexcept {
    return (m * n) / gcd(m, n);
}

constexpr double log(double x, double base) noexcept {
    return std::log(x) / std::log(base);
}

constexpr double angular(double radians) noexcept {
    return radians * (constants::SEMI_CIRCLE / constants::PI);
}

constexpr double radian(double angular) noexcept {
    return (angular * constants::PI / constants::SEMI_CIRCLE);
}

constexpr size_t fact(int n) noexcept {
    size_t h = 1;
    for (int i = 1; i <= n; i++)
        h *= i;
    return h;
}

enum class STRING_STATE{
    VALID, INVALID, HAS_CYCLE, NOT_NUMBER, IS_EMPTY
};

struct Token {
    enum Type { Number, Operator, Function1, Function2, Cell, Comma, Colon, LeftParen, RightParen, End };
    Type type;
    QString str_val;
    double num_val;
};

class Parser {
private:
    QPair<int, int> current_position;
    const QString* expr;
    int index;
    Token current_token;
    CaculateTable* grid;

    void get_next_token() {
        while (index < expr->size() && expr->at(index).isSpace()) {
            index++;
        }
        if (index >= expr->size()) {
            current_token.type = Token::End;
            current_token.str_val = "";
            return;
        }

        QChar c = expr->at(index);
        if (c.isDigit() || c == '.') {
            QString num_str;
            bool has_dot = false;
            while (index < expr->size() && (expr->at(index).isDigit() || expr->at(index) == '.')) {
                if (expr->at(index) == '.') {
                    if (has_dot) break;
                    has_dot = true;
                }
                num_str += expr->at(index);
                index++;
            }
            current_token.type = Token::Number;
            current_token.num_val = num_str.toDouble();
        } else if (c.isLetter()) {
            bool f = false;
            QString func_str;
            while (index < expr->size() && expr->at(index).isLetter()) {
                func_str += expr->at(index).toLower();
                index++;
            }
            while (index < expr->size() && expr->at(index).isDigit()) {
                f = true;
                func_str += expr->at(index);
                index++;
            }
            if (func_str == "sum" || func_str == "avg" || func_str == "gcd" || func_str == "lcm" ||
                func_str == "power" || func_str == "log" || func_str == "log10" || func_str == "log2" || func_str == "loge") {
                current_token.type = Token::Function2;
            } else if (func_str == "abs" || func_str == "sin" || func_str == "cos" || func_str == "tan" ||
                       func_str == "asin" || func_str == "acos" || func_str == "atan" || func_str == "angular" ||
                       func_str == "radian" || func_str == "sqrt" || func_str == "cbrt" || func_str == "fact" ||
                       func_str == "floor" || func_str == "ceil" || func_str == "round") {
                current_token.type = Token::Function1;
            } else if (f) {
                current_token.type = Token::Cell;
            } else {
                current_token.type = Token::End;
            }
            current_token.str_val = func_str;
        } else if (c == '+' || c == '-' || c == '*' || c == '/') {
            current_token.type = Token::Operator;
            current_token.str_val = c;
            index++;
        } else if (c == '(') {
            current_token.type = Token::LeftParen;
            index++;
        } else if (c == ')') {
            current_token.type = Token::RightParen;
            index++;
        } else if (c == ',') {
            current_token.type = Token::Comma;
            index++;
        } else if (c == ':') {
            current_token.type = Token::Colon;
            index++;
        } else {
            current_token.str_val = c;
            current_token.type = Token::End;
        }
    }

    QPair<bool, double> parse_expression() {
        QPair<bool, double> result = parse_term();
        if (!result.first) {
            return { false, 0.0 };
        }
        while (current_token.type == Token::Operator && (current_token.str_val == "+" || current_token.str_val == "-")) {
            QString op = current_token.str_val;
            get_next_token();
            QPair<bool, double> term = parse_term();
            if (!term.first) {
                return { false, 0.0 };
            } else if (op == "+") {
                result.second += term.second;
            } else {
                result.second -= term.second;
            }
        }
        return result;
    }

    QPair<bool, double> parse_term() {
        QPair<bool, double> result = parse_factor();
        if (!result.first) {
            return { false, 0.0 };
        }
        while (current_token.type == Token::Operator && (current_token.str_val == "*" || current_token.str_val == "/")) {
            QString op = current_token.str_val;
            get_next_token();
            QPair<bool, double> factor = parse_factor();
            if (!factor.first) {
                return { false, 0.0 };
            }
            if (op == "*") {
                result.second *= factor.second;
            } else {
                result.second /= factor.second;
            }
        }
        return result;
    }

    QPair<bool, double> parse_factor() {
        if (current_token.type == Token::Number) {
            double value = current_token.num_val;
            get_next_token();
            return { true, value };
        } else if (current_token.type == Token::Function1) {
            QString func = current_token.str_val;
            get_next_token();
            if (current_token.type != Token::LeftParen) {
                return { false, 0.0 };
            }
            get_next_token();
            QPair<bool, double> arg = parse_expression();
            if (!arg.first) {
                return { false, 0.0 };
            }
            if (current_token.type != Token::RightParen) {
                return { false, 0.0 };
            }
            get_next_token();
            return { true, apply_function1(func, arg.second) };
        } else if (current_token.type == Token::Function2) {
            QString func = current_token.str_val;
            std::vector<double> args;
            get_next_token();
            if (current_token.type != Token::LeftParen) {
                return { false, 0.0 };
            }
            get_next_token();
            while (index < expr->size() && (expr->at(index) == ',' || expr->at(index) == ':')) {
                if (expr->at(index) == ':') {
                    QPair<int, int> begin_position;
                    if (current_token.type != Token::Cell) {
                        return { false, 0.0 };
                    }
                    QString cell = current_token.str_val;
                    begin_position = getposition(cell).second;
                    if (begin_position.first >= grid->rowCount() || begin_position.second >= grid->columnCount()) {
                        return { false, 0.0 };
                    }
                    get_next_token();
                    get_next_token();
                    if (current_token.type != Token::Cell) {
                        return { false, 0.0 };
                    }
                    cell = current_token.str_val;
                    QPair<int, int> end_position = getposition(cell).second;
                    if (end_position.first >= grid->rowCount() || end_position.second >= grid->columnCount()) {
                        return { false, 0.0 };
                    }
                    if (begin_position.first > end_position.first) {
                        std::swap(begin_position.first, end_position.first);
                    }
                    if (begin_position.second > end_position.second) {
                        std::swap(begin_position.second, end_position.second);
                    }
                    for (int i = begin_position.first; i <= end_position.first; i++) {
                        for (int j = begin_position.second; j <= end_position.second; j++) {
                            args.push_back(grid->item(i, j)->text().toDouble());
                        }
                    }
                    get_next_token();
                    if (current_token.type == Token::Comma) {
                        get_next_token();
                    }
                } else if (expr->at(index) == ',') {
                    QPair<bool, double> arg = parse_expression();
                    if (!arg.first) {
                        return { false, 0.0 };
                    }
                    args.push_back(arg.second);
                    get_next_token();
                }
            }
            if (current_token.type != Token::RightParen) {
                QPair<bool, double> tem = parse_expression();
                if (!tem.first) {
                    return { false, 0.0 };
                }
                args.push_back(tem.second);
            }
            if (current_token.type != Token::RightParen) {
                return { false, 0.0 };
            }
            get_next_token();
            return { true, apply_function2(func, args) };
        } else if (current_token.type == Token::Cell) {
            QString cell = current_token.str_val;
            QPair<int, int> position_tem;
            QPair<int, int> tem = getposition(cell).second;
            if (tem.first >= grid->rowCount() || tem.second >= grid->columnCount() || tem == current_position) {
                return { false, 0.0 };
            }
            grid->gph_->insert_node(tem);
            grid->gph_->insert_edge(tem, current_position);
            //grid->gph_->print();
            if (grid->gph_->depth_first_search()) {
                grid->gph_->delete_edge(tem, current_position);
                return { false, 0.0 };
            }
            int column = tem.second;
            int row = tem.first;
            CaculateTableItem* item = dynamic_cast<CaculateTableItem*>(grid->item(row, column));
            if(!item->get_func().isEmpty()){
                get_next_token();
                return { true, item->get_caculate() };
            }
            QString str = item->text();
            const QString* tem_expr = expr;
            int tem_index = index;
            position_tem = current_position;
            auto final = parse(tem, grid, str);
            if(final.first){
                item->update_caculate(final.second);
                item->update_func(str);
                item->setText(QString::number(final.second));
            }
            current_position = position_tem;

            expr = tem_expr;
            index = tem_index;
            get_next_token();
            return { true, item->text().toDouble() };
        } else if (current_token.type == Token::LeftParen) {
            get_next_token();
            QPair<bool, double> value = parse_expression();
            if (!value.first) {
                return value;
            }
            if (current_token.type != Token::RightParen) {
                return { false, 0.0 };
            }
            get_next_token();
            return value;
        } else if (current_token.type == Token::Operator && (current_token.str_val == "+" || current_token.str_val == "-")) {
            QString op = current_token.str_val;
            get_next_token();
            QPair<bool, double> factor = parse_factor();
            if (!factor.first) {
                return { false, 0.0 };
            } else if (op == "-") {
                return { true, -factor.second };
            } else {
                return factor;
            }
        } else {
            return { false, 0.0 };
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

    double apply_function2(const QString& func, const vector<double>& args) {
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
    static QPair<bool, QPair<int, int>> getposition(const QString& _cell) {
        int count = 0, column = 0, row = 0;
        if (_cell.isEmpty() ||!_cell[0].isLetter())
            return { false, { -1, -1 } };

        while (count < _cell.size() && _cell[count].isLetter()) {
            column = column * 26 + (_cell[count].toLower().unicode() - 'a' + 1);
            count++;
        }

        if (count == _cell.size())
            return { false, { -1, -1 } };
        for (int i = count; i < _cell.size(); ++i) {
            if (!_cell[i].isDigit())
                return { false, { -1, -1 } };
        }
        row = _cell.mid(count).toInt();
        if (row <= 0 || column <= 0)
            return { false, { -1, -1 } };

        return { true, { row - 1, column - 1 } };
    }

    QPair<bool, double> parse(const QPair<int, int>& p, CaculateTable* g, const QString& e) {
        if (!e.startsWith('='))
            return { false, 0.0 };
        grid = g;
        current_position = p;
        expr = &e;
        index = 1;
        get_next_token();
        QPair<bool, double> result = parse_expression();
        if (current_token.str_val.isEmpty())
            return result;
        else
            return { false, 0.0 };
    }
};

#endif // CACULATE_H

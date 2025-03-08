#ifndef SETTING_H
#define SETTING_H
#include <QString>
#include <QDir>
#include <QStandardPaths>

namespace setting{
enum class LANGUAGE{
    CHINESE, ENGLISH
};

static const QString current_path = "D:/Workspace/Cpp Workspace/Qt Workspace/excel_system";
static const LANGUAGE current_language = LANGUAGE::CHINESE;
static const QString resource_path = current_path + "/resource";
static const QString title_icon = resource_path + "/title_icon.png";
static const QString desk_path=QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).at(0);
static const int default_column = 100;
static const int default_row = 100;

static const QStringList math_fcns =
    { "ABS", "AVG", "SUM", "SIN", "COS", "TAN", "ASIN", "ACOS", "ATAN", "ANGULAR",
    "RADIAN", "GCD", "LCM", "POWER", "LOG", "LOGE", "LOG2", "LOG10", "SQRT", "CBRT",
    "FACT", "FLOOR", "CEIL", "ROUND" };
static const QMap<QString, QPair<QString, int>> math_describes =
    {
        {"ABS", {"绝对值", 1}},
        {"AVG", {"平均值", 255}},
        {"SUM", {"求和", 255}},
        {"SIN", {"正弦", 1}},
        {"COS", {"余弦", 1}},
        {"TAN", {"正切", 1}},
        {"ASIN", {"反正弦", 1}},
        {"ACOS", {"反余弦", 1}},
        {"ATAN", {"反正切", 1}},
        {"ANGULAR", {"弧度转换为角度", 1}},
        {"RADIAN", {"角度转换为弧度", 1}},
        {"GCD", {"最大公约数", 2}},
        {"LCM", {"最小公倍数", 2}},
        {"POWER", {"幂运算", 2}},
        {"LOG", {"对数", 2}},
        {"LOGE", {"自然对数", 1}},
        {"LOG2", {"以2为底的对数", 1}},
        {"LOG10", {"以10为底的对数", 1}},
        {"SQRT", {"平方根", 1}},
        {"CBRT", {"立方根", 1}},
        {"FACT", {"阶乘", 1}},
        {"FLOOR", {"向下取整", 1}},
        {"CEIL", {"向上取整", 1}},
        {"ROUND", {"四舍五入", 1}}
};

}

#endif // SETTING_H

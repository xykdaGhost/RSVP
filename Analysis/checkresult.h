#ifndef CHECKRESULT_H
#define CHECKRESULT_H
#include <QString>
#include <QImage>
#include <QTableWidget>

typedef struct rect
{
    double x[2];
    double y[2];
}Rect;

inline double area(const rect &a) { return (a.x[1] - a.x[0]) * (a.y[1] - a.y[0]); }   // 计算矩形面积

inline double unionarea(const rect &a, const rect  &b, double overlap) { return area(a) + area(b) - overlap; }   // 计算两矩形并集面积

inline double overlaparea(const rect &a, const rect &b)  // 计算两矩形交集的面积
{
    double difX = std::min(a.x[1], b.x[1]) - std::max(a.x[0], b.x[0]);
    double difY = std::min(a.y[1], b.y[1]) - std::max(a.y[0], b.y[0]);
    return (difX < 0 || difY < 0) ? 0 : difX * difY;
}

void checkResult(QString mlabeltxtname, QString ylabeltxtname, QImage* image, QTableWidget* tabwidget);

#endif // CHECKRESULT_H

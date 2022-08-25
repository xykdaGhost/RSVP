#include "checkresult.h"
#include <string>
#include <fstream>
#include <sstream>
#include <QPainter>
#include <assert.h>
void compareLabel(QString& mlabeltxtname, QString& ylabeltxtname, QImage* image, std::vector<int>& correctnums, std::vector<int>& wrongnums,
                    std::vector<int>& missnums, std::vector<int>& maxarea, std::vector<int>& minarea, int setIOU = 0);

void refreshTab(std::vector<int>& correctnums, std::vector<int>& wrongnums, std::vector<int>& missnums, std::vector<int>& maxarea,
                    std::vector<int>& minarea, QTableWidget *tabwidget);


void checkResult(QString mlabeltxtname, QString ylabeltxtname, QImage* image, QTableWidget* tabwidget) {
    std::vector<int> correctnums(10); // 正确检测数量数组
    std::vector<int> wrongnums(10);   // 误检数组
    std::vector<int> missnums(10);    // 漏检数组
    std::vector<int> maxarea(10);     // 最大目标面积数组
    std::vector<int> minarea(10);     // 最小目标面积数组

    // 比较label文件得出核对结果并在图片中画出检测框
    compareLabel(mlabeltxtname, ylabeltxtname, image, correctnums, wrongnums, missnums, maxarea, minarea);

    // 刷新表格内容显示统计信息
    refreshTab(correctnums, wrongnums, missnums, maxarea, minarea, tabwidget);
}

void compareLabel(QString& mlabeltxtname, QString& ylabeltxtname, QImage* image, std::vector<int>& correctnums, std::vector<int>& wrongnums,
                  std::vector<int>& missnums, std::vector<int>& maxarea, std::vector<int>& minarea, int setIOU) {
    std::vector<std::vector<double>> labels;  // 标记文件数组

    // 将人工标记的label信息读入labels数组中
    std::ifstream infile1;
    double l_id1;
    double x1,y1,w1,h1;
    infile1.open(mlabeltxtname.toStdString());
    assert(infile1.is_open());
    std::string line1,s1;
    while (getline(infile1, line1)) {
        std::istringstream label1(line1);
        label1 >> l_id1;
        label1 >> x1;
        label1 >> y1;
        label1 >> w1;
        label1 >> h1;
        labels.push_back({l_id1, x1, y1, w1, h1, 0});
    }
    infile1.close();

    int W = image->width();   // 图片宽度
    int H = image->height();  // 图片高度
    std::ifstream infile;
    int l_id;
    double x,y,w,h;
    infile.open(ylabeltxtname.toStdString());
    assert(infile.is_open());
    std::string line, s;
    rect a, b;
    double overlapa, uniona, iou;
    int flag;
    while (getline(infile, line)) { //一层循环，读取每个预测标记框
        flag = 0;
        std::istringstream label(line);
        label >> l_id;
        label >> x;
        label >> y;
        label >> w;
        label >> h;
        a.x[0] = x - w / 2;
        a.x[1] = x + w / 2;
        a.y[0] = y - h / 2;
        a.y[1] = y + h / 2;
        for (int i = 0; i < labels.size(); ++i) { // 二层循环，对于每个预测标记框扫描存放人工标记label信息的labels数组，寻找与之匹配的人工标记框
            int l_id0 = (int)labels[i][0];
            x1 = labels[i][1];
            y1 = labels[i][2];
            w1 = labels[i][3];
            h1 = labels[i][4];
            b.x[0] = x1 - w1 / 2;
            b.x[1] = x1 + w1 / 2;
            b.y[0] = y1 - h1 / 2;
            b.y[1] = y1 + h1 / 2;

            //计算两标记框的iou
            overlapa = overlaparea(a, b);
            uniona = unionarea(a, b, overlapa);
            iou = overlapa / uniona;

            if (iou > 0)
                ++labels[i][5];
            if (iou > setIOU && l_id == l_id0) {
                flag = 1;
                break;
            }
        }

        a.x[0] = a.x[0] * W;
        a.y[0] = a.y[0] * H;
        w = w * W;
        h = h * H;

        QPainter painter(image);
        if (flag == 1) { // 如果找到了，且类别正确则为正确检测
            painter.setPen(QPen(Qt::green,5));
            painter.drawRect(QRect(a.x[0], a.y[0], w, h));
            painter.drawText(a.x[0], a.y[0], "正确");

            ++correctnums[l_id];

            int s = w * h;

            if (s > maxarea[l_id])
                maxarea[l_id] = s;
            if (minarea[l_id] == 0)
                minarea[l_id] = s;   // 给min一个初值
            if (s < minarea[l_id])
                minarea[l_id] = s;

        }
        else {  // 如果没找到或者类别错误，则为误检
            painter.setPen(QPen(Qt::red,5));
            painter.drawRect(QRect(a.x[0], a.y[0], w, h));
            painter.drawText(a.x[0], a.y[0], "误检");
            ++wrongnums[l_id];
        }

    }
    infile.close();
    for (int i = 0; i < labels.size(); ++i) { // 再扫描一遍人工标记label数组，如果存在标记框不与任何预测的标记框匹配则为漏检
        QPainter painter(image);
        if (labels[i][5] < 0.5) {
            x1 = labels[i][1];
            y1 = labels[i][2];
            w1 = labels[i][3];
            h1 = labels[i][4];
            x1 = x1 - w1 / 2;
            y1 = y1 - h1 / 2;
            x1 = x1 * W;
            y1 = y1 * H;
            w1 = w1 * W;
            h1 = h1 * H;
            painter.setPen(QPen(Qt::yellow,5));
            painter.drawRect(QRect(x1, y1, w1, h1));
            painter.drawText(x1, y1, "漏检");
            ++missnums[size_t(labels[i][0])];
        }
    }
    labels.clear();
}

void refreshTab(std::vector<int>& correctnums, std::vector<int>& wrongnums, std::vector<int>& missnums, std::vector<int>& maxarea,
                std::vector<int>& minarea, QTableWidget *tabwidget) {
    int correctsum = 0; // 所有类别正确检测总数
    int wrongsum = 0;   // 所有类别误检总数
    int misssum = 0;    // 所有类别漏检总数
    int allnum = 0;     //以上三者之和视为目标总数
    int maxsum = 0;     // 所有类别中的最大目标面积
    int minsum = 0;     // 所有类别中的最小目标面积
    QString correct, wrong, miss;
    double corr, wro, mis;

    // 填入每种类别的统计信息
    for (int i = 0; i < 10; ++i)
    {
        int co = correctnums[i];
        int wr = wrongnums[i];
        int ms = missnums[i];
        correctsum += co;
        wrongsum += wr;
        misssum += ms;
        allnum = co + wr + ms;
        correct = QString::number(co);
        wrong = QString::number(wr);
        miss = QString::number(ms);
        if (co != 0)
        {
            corr = (double)co / allnum * 100;
            correct = correct + "(" + QString::number(corr, 'f', 1) + ")";
        }
        if (wr != 0)
        {
            wro = (double)wr / allnum * 100;
            wrong = wrong + "(" + QString::number(wro, 'f', 1) + ")";
        }
        if (ms != 0)
        {
            mis = (double)ms / allnum * 100;
            miss = miss + "(" + QString::number(mis, 'f', 1) + ")";
        }

        if (!tabwidget->item(i, 0)) tabwidget->setItem(i, 0, new QTableWidgetItem(QString::number(allnum)));
        if (!tabwidget->item(i, 1)) tabwidget->setItem(i, 1, new QTableWidgetItem(correct));
        if (!tabwidget->item(i, 2)) tabwidget->setItem(i, 2, new QTableWidgetItem(wrong));
        if (!tabwidget->item(i, 3)) tabwidget->setItem(i, 3, new QTableWidgetItem(miss));
        if (!tabwidget->item(i, 4)) tabwidget->setItem(i, 4, new QTableWidgetItem(QString::number(maxarea[i])));
        if (!tabwidget->item(i, 5)) tabwidget->setItem(i, 5, new QTableWidgetItem(QString::number(minarea[i])));
        tabwidget->item(i, 0)->setText(QString::number(allnum));
        tabwidget->item(i, 1)->setText(correct);
        tabwidget->item(i, 2)->setText(wrong);
        tabwidget->item(i, 3)->setText(miss);
        tabwidget->item(i, 4)->setText(QString::number(maxarea[i]) + "(" + QString::number((float)maxarea[i] / 2432 / 896 * 1000, 'f', 2) + "‰)");
        tabwidget->item(i, 5)->setText(QString::number(minarea[i]) + "(" + QString::number((float)minarea[i] / 2432 / 896 * 1000, 'f', 2) + "‰)");
        if (maxarea[i] > maxsum)
                maxsum = maxarea[i];
        if (minarea[i] > 0)
        {
            if (minsum == 0)
                minsum = minarea[i];
            if (minarea[i] < minsum)
                minsum = minarea[i];
        }
    }

    // 填入类别总计的统计信息
    allnum = correctsum + wrongsum + misssum;
    correct = QString::number(correctsum);
    wrong = QString::number(wrongsum);
    miss = QString::number(misssum);
    if (correctsum != 0)
    {
        corr = (double)correctsum / allnum * 100;
        correct = correct + "(" + QString::number((float)corr, 'f', 1) + ")";
    }
    if (wrongsum != 0)
    {
        wro = (double)wrongsum / allnum * 100;
        wrong = wrong + "(" + QString::number((float)wro, 'f', 1) + ")";
    }
    if (misssum != 0)
    {
        mis = (double)misssum / allnum * 100;
        miss = miss + "(" + QString::number((float)mis, 'f', 1) + ")";
    }
    int crow = tabwidget->rowCount();

    if (!tabwidget->item(crow - 1, 0)) tabwidget->setItem(crow - 1, 0, new QTableWidgetItem(QString::number(allnum)));
    if (!tabwidget->item(crow - 1, 1)) tabwidget->setItem(crow - 1, 1, new QTableWidgetItem(correct));
    if (!tabwidget->item(crow - 1, 2)) tabwidget->setItem(crow - 1, 2, new QTableWidgetItem(wrong));
    if (!tabwidget->item(crow - 1, 3)) tabwidget->setItem(crow - 1, 3, new QTableWidgetItem(miss));
    if (!tabwidget->item(crow - 1, 4)) tabwidget->setItem(crow - 1, 4, new QTableWidgetItem(QString::number(maxsum)));
    if (!tabwidget->item(crow - 1, 5)) tabwidget->setItem(crow - 1, 5, new QTableWidgetItem(QString::number(minsum)));
    tabwidget->item(crow - 1, 0)->setText(QString::number(allnum));
    tabwidget->item(crow - 1, 1)->setText(correct);
    tabwidget->item(crow - 1, 2)->setText(wrong);
    tabwidget->item(crow - 1, 3)->setText(miss);
    tabwidget->item(crow - 1, 4)->setText(QString::number(maxsum) + "(" + QString::number((float)maxsum / 2432 / 896 * 1000, 'f', 2) + "‰)");
    tabwidget->item(crow - 1, 5)->setText(QString::number(minsum) + "(" + QString::number((float)maxsum / 2432 / 896 * 1000, 'f', 2) + "‰)");

    for(int i = 0; i < tabwidget->rowCount(); i++)
        for(int j = 0; j < tabwidget->columnCount(); j++)
            tabwidget->item(i, j)->setTextAlignment(Qt::AlignCenter);
}

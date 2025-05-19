#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTableView>
#include<QStandardItemModel>
#include<QString>
#include<QGraphicsWidget>
#include<QVBoxLayout>
#include<QMenuBar>
#include<QAction>
#include<QWidget>
#include<QAbstractItemView>
#include<QTextCharFormat>
#include<QLabel>
#include<QInputDialog>
#include<QStringList>
#include<QMessageBox>
#include<QFileDialog>
#include<QFile>
#include <QClipboard>
#include <QGuiApplication>
#include <QItemSelectionModel>
#include <QMenu>
#include <QColorDialog>
#include <QFont>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    //基本声明
    void createMenuBar();//创建上方菜单栏
    void createTable(int rows,int cols);//动态创建表格

    QTableView *tableview=nullptr;//表格视图控件声明
    QStandardItemModel *model=nullptr;//数据模型

    //其他功能相关声明
    QString currentFilePath;//当前文件地址
    QStringList parseCSVLine(const QString &line); // 辅助 CSV 解析
    QList<double> getSelectedNumbers();//提取选中单元格中有效数字
    QLabel *resultLabel;//计算结果显示标签
    void showContextMenu(const QPoint &pos);//鼠标右键菜单栏
    QString copiedValue;//要复制的数据的内容
    QModelIndex copiedIndex;//要复制的数据的位置

    //“文件”菜单下选项声明
    QAction *newAction;
    QAction *saveAction;
    QAction *importAction;
    QAction *saveasAction;

    //“编辑”菜单下选项声明
    QAction *copyAction;
    QAction *cutAction;
    QAction *pasteAction;
    QAction *clearAction;
    QAction *specopyAction;
    QAction *spepasteAction;

    //“计算”菜单下选项声明
    QAction *sumAction;
    QAction *avgAction;
    QAction *maxAction;
    QAction *minAction;

    //"排序"菜单下选项声明
    QAction *sortAscAction;
    QAction *sortDesAction;

private slots:
    void newfile();//新建文件函数
    void savefile();//保存文件槽函数
    void importfile();//导入文件槽函数
    void saveasfile(); // 另存为
    void copyCells();    // 复制
    void pasteCells();   // 粘贴
    void cutCells();//剪切
    void clearCells();//清除
    void calculateSum();
    void calculateAverage();
    void calculateMax();
    void calculateMin();
    void specialCopy();
    void specialPaste();

};
#endif // MAINWINDOW_H

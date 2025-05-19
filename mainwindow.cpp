#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("简易电子表格");//表格标题
    resize(800,600);//表格大小

    QWidget* centralWidget= new QWidget(this);//创建中央部件
    centralWidget->setLayout(new QVBoxLayout());
    createMenuBar();//创建菜单栏
    setCentralWidget(centralWidget);//设置中央部件
}

MainWindow::~MainWindow()
{
    delete ui;
}

//菜单栏
void MainWindow::createMenuBar(){
    QMenu *fileMenu=menuBar()->addMenu(tr("文件(&F)"));//声明文件菜单
    //添加文件菜单子项
    newAction=fileMenu->addAction(tr("新建"));
    connect(newAction,&QAction::triggered,this,&MainWindow::newfile);//连接单击“新建”按钮和新建文件函数
    saveAction=fileMenu->addAction(tr("保存"));
    connect(saveAction,&QAction::triggered,this,&MainWindow::savefile);//连接单击“保存”按钮和保存文件函数
    importAction=fileMenu->addAction(tr("导入"));
    connect(importAction,&QAction::triggered,this,&MainWindow::importfile);
    QAction *saveAsAction = fileMenu->addAction(tr("另存为"));
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveasfile);

    QMenu *editMenu=menuBar()->addMenu(tr("编辑(&E)"));//声明编辑菜单
    //添加编辑菜单动作项
    copyAction=editMenu->addAction(tr("复制"));
    cutAction=editMenu->addAction(tr("剪切"));
    pasteAction=editMenu->addAction(tr("粘贴"));
    connect(copyAction, &QAction::triggered, this, &MainWindow::copyCells);
    connect(pasteAction, &QAction::triggered, this, &MainWindow::pasteCells);
    connect(cutAction, &QAction::triggered, this, &MainWindow::cutCells);
    editMenu->addSeparator();//添加分割线
    clearAction=editMenu->addAction(tr("清除"));
    connect(clearAction, &QAction::triggered, this, &MainWindow::clearCells);
    editMenu->addSeparator();//添加分割线
    specopyAction=editMenu->addAction(tr("特殊复制"));
    spepasteAction=editMenu->addAction(tr("特殊粘贴"));
    connect(specopyAction, &QAction::triggered, this, &MainWindow::specialCopy);
    connect(spepasteAction, &QAction::triggered, this, &MainWindow::specialPaste);



    QMenu *calculateMenu=menuBar()->addMenu(tr("计算(&C)"));//声明计算菜单
    //添加计算菜单动作项
    sumAction=calculateMenu->addAction(tr("合计"));
    avgAction=calculateMenu->addAction(tr("平均值"));
    maxAction=calculateMenu->addAction(tr("最大值"));
    minAction=calculateMenu->addAction(tr("最小值"));
    connect(sumAction, &QAction::triggered, this, &MainWindow::calculateSum);
    connect(avgAction, &QAction::triggered, this, &MainWindow::calculateAverage);
    connect(maxAction, &QAction::triggered, this, &MainWindow::calculateMax);
    connect(minAction, &QAction::triggered, this, &MainWindow::calculateMin);

    resultLabel = new QLabel(this);
    resultLabel->setMinimumWidth(200);  // 设置宽度，可以根据需要调节
    resultLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);  // 右对齐
    statusBar()->addPermanentWidget(resultLabel);

    QMenu *sortMenu=menuBar()->addMenu(tr("排序(&S)"));//声明排序菜单
    //添加排序菜单动作项
    sortAscAction=sortMenu->addAction(tr("顺序"));
    sortDesAction=sortMenu->addAction(tr("倒序"));


}

//新建文件槽函数
void MainWindow::newfile(){
    bool ok;
    int rows=QInputDialog::getInt(this,tr("新建表格"),tr("行数"),5,1,100,1,&ok);
    if(!ok) return;
    int cols=QInputDialog::getInt(this,tr("新建表格"),tr("列数"),5,1,100,1,&ok);
    if(!ok) return;
    createTable(rows,cols);
}

//创建表格，初始化函数
void MainWindow::createTable(int rows,int cols){
    //如果已经有表格，新建前需要清除
    if(tableview){
        centralWidget()->layout()->removeWidget(tableview);//清除表格
        delete tableview;
        delete model;
    }

    //下面四行用于创建表格
    tableview=new QTableView(this);//创建tableview对象
    model=new QStandardItemModel(rows,cols,this);//创建表格数据模型对象
    tableview->setModel(model);//让tableview具有model的属性
    centralWidget()->layout()->addWidget(tableview);//添加表格至中央部件

    //设置表格列名行名，要放在创建表格后，不然进程和奔溃
    QStringList colheaders;
    for(int i=0;i<cols;i++){
        colheaders<<QString::number(i+1);
    }
    model->setVerticalHeaderLabels(colheaders);//注意该函数针对QStandardItemModel，是model
    QStringList rowheaders;
    for(int i=0;i<rows;i++){
        rowheaders<<QString(QChar('A'+i));
    }
    model->setHorizontalHeaderLabels(rowheaders);

    //设置鼠标右键菜单栏
    tableview->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tableview, &QTableView::customContextMenuRequested, this, &MainWindow::showContextMenu);

}
//鼠标右键设置数据属性
void MainWindow::showContextMenu(const QPoint &pos) {
    QModelIndex index = tableview->indexAt(pos);
    if (!index.isValid()) return;

    QMenu contextMenu(this);
    QAction *changeFontSizeAction = contextMenu.addAction("更改字体大小");
    QAction *changeFontColorAction = contextMenu.addAction("更改字体颜色");
    QAction *changeBackgroundColorAction = contextMenu.addAction("更改背景颜色");

    QAction *selectedAction = contextMenu.exec(tableview->viewport()->mapToGlobal(pos));
    if (!selectedAction) return;

    QItemSelectionModel *selectModel = tableview->selectionModel();
    QModelIndexList selectedIndexes = selectModel->selectedIndexes();

    if (selectedAction == changeFontSizeAction) {
        bool ok;
        int size = QInputDialog::getInt(this, "字体大小", "请输入字体大小（例如 12）", 12, 1, 100, 1, &ok);
        if (ok) {
            for (const QModelIndex &idx : selectedIndexes) {
                QFont font = model->itemFromIndex(idx)->font();
                font.setPointSize(size);
                model->itemFromIndex(idx)->setFont(font);
            }
        }
    } else if (selectedAction == changeFontColorAction) {
        QColor color = QColorDialog::getColor(Qt::black, this, "选择字体颜色");
        if (color.isValid()) {
            for (const QModelIndex &idx : selectedIndexes) {
                model->itemFromIndex(idx)->setForeground(color);
            }
        }
    } else if (selectedAction == changeBackgroundColorAction) {
        QColor color = QColorDialog::getColor(Qt::white, this, "选择背景颜色");
        if (color.isValid()) {
            for (const QModelIndex &idx : selectedIndexes) {
                model->itemFromIndex(idx)->setBackground(color);
            }
        }
    }
}

//保存函数槽函数
void MainWindow::savefile() {
    if (!model) return;  // 没有表格则返回

    // 若未选择路径，则弹出“另存为”对话框
    if (currentFilePath.isEmpty()) {
        currentFilePath = QFileDialog::getSaveFileName(this, tr("保存文件"), "", tr("CSV 文件 (*.csv);;文本文件 (*.txt)"));
        if (currentFilePath.isEmpty()) return;
    }

    QFile file(currentFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("错误"), tr("无法保存文件"));
        return;
    }

    QTextStream out(&file);

    int rows = model->rowCount();
    int cols = model->columnCount();

    for (int i = 0; i < rows; ++i) {
        QStringList rowContent;
        for (int j = 0; j < cols; ++j) {
            QStandardItem *item = model->item(i, j);
            if (item) {
                // 使用双引号包裹以防包含逗号
                rowContent << "\"" + item->text() + "\"";
            } else {
                rowContent << "";
            }
        }
        out << rowContent.join(",") << "\n";
    }

    file.close();
    QMessageBox::information(this, tr("成功"), tr("文件已保存"));
}
//另存为槽函数
void MainWindow::saveasfile() {
    QString savePath = QFileDialog::getSaveFileName(this, tr("另存为"), "", tr("CSV 文件 (*.csv);;文本文件 (*.txt)"));
    if (savePath.isEmpty()) return;

    currentFilePath = savePath;
    savefile(); // 复用保存逻辑
}
//导入槽函数
void MainWindow::importfile() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("导入 CSV 文件"), "", tr("CSV 文件 (*.csv)"));
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("错误"), tr("无法打开文件"));
        return;
    }

    QTextStream in(&file);
    QList<QStringList> rows;  // 临时存储所有行

    bool firstLine = true;
    while (!in.atEnd()) {
        QString line = in.readLine();

        if (firstLine) {
            firstLine = false;      // 跳过第一行标题
            continue;
        }

        QStringList items = parseCSVLine(line);
        rows.append(items);
    }
    file.close();

    if (rows.isEmpty()) return;

    int rowCount = rows.size();
    int colCount = rows[0].size();

    createTable(rowCount, colCount);

    // 写入 model 数据
    for (int i = 0; i < rowCount; ++i) {
        for (int j = 0; j < colCount; ++j) {
            model->setItem(i, j, new QStandardItem(rows[i].value(j)));
        }
    }

    currentFilePath = filePath;  // 更新当前文件路径
}
//csv解析函数
QStringList MainWindow::parseCSVLine(const QString &line) {
    QStringList result;
    QString value;
    bool insideQuote = false;

    for (int i = 0; i < line.length(); ++i) {
        QChar c = line[i];

        if (c == '\"') {
            insideQuote = !insideQuote;
        } else if (c == ',' && !insideQuote) {
            result << value.trimmed();
            value.clear();
        } else {
            value.append(c);
        }
    }

    result << value.trimmed();  // 最后一列
    return result;
}
//复制槽函数
void MainWindow::copyCells() {
    if (!tableview || !model) return;

    QItemSelectionModel *select = tableview->selectionModel();
    QModelIndexList indexes = select->selectedIndexes();

    if (indexes.isEmpty()) return;

    // 排序选中的索引（按行列）
    std::sort(indexes.begin(), indexes.end());

    int currentRow = indexes.first().row();
    int currentCol = indexes.first().column();

    QString copiedText;
    int prevRow = currentRow;

    for (const QModelIndex &index : indexes) {
        int row = index.row();
        int col = index.column();

        if (row != prevRow) {
            copiedText += "\n";
            prevRow = row;
        } else if (index != indexes.first()) {
            copiedText += "\t";
        }

        copiedText += index.data().toString();
    }

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(copiedText);

}
//粘贴槽函数
void MainWindow::pasteCells() {
    if (!tableview || !model) return;

    QClipboard *clipboard = QGuiApplication::clipboard();
    QString clipboardText = clipboard->text();
    if (clipboardText.isEmpty()) return;

    QModelIndex startIndex = tableview->currentIndex();
    int startRow = startIndex.row();
    int startCol = startIndex.column();

    QStringList rows = clipboardText.split('\n');
    for (int i = 0; i < rows.size(); ++i) {
        QStringList cols = rows[i].split('\t');
        for (int j = 0; j < cols.size(); ++j) {
            int targetRow = startRow + i;
            int targetCol = startCol + j;
            if (targetRow < model->rowCount() && targetCol < model->columnCount()) {
                model->setItem(targetRow, targetCol, new QStandardItem(cols[j]));
            }
        }
    }

}
//剪切槽函数
void MainWindow::cutCells() {
    if (!tableview || !model) return;

    // 获取当前的选中区域
    QItemSelectionModel *select = tableview->selectionModel();
    QModelIndexList indexes = select->selectedIndexes();
    if (indexes.isEmpty()) return;

    // 先执行复制操作
    copyCells();

    // 清空选中区域的内容
    for (const QModelIndex &index : indexes) {
        model->setData(index, QString("")); // 设置为空字符串
    }
}
//清除槽函数
void MainWindow::clearCells() {
    if (!tableview || !model) return;

    // 获取当前选中的单元格索引列表
    QItemSelectionModel *selection = tableview->selectionModel();
    QModelIndexList indexes = selection->selectedIndexes();
    if (indexes.isEmpty()) return;

    // 遍历所有选中的单元格，将其内容设为空字符串
    for (const QModelIndex &index : indexes) {
        model->setData(index, QString(""));
    }
}
//特殊复制槽函数
void MainWindow::specialCopy() {
    copiedIndex = tableview->currentIndex();
    if (!copiedIndex.isValid()) return;

    copiedValue = copiedIndex.data().toString();
}
//特殊粘贴槽函数
void MainWindow::specialPaste() {
    QModelIndex targetIndex = tableview->currentIndex();
    if (!copiedIndex.isValid() || !targetIndex.isValid()) return;

    QString targetValue = targetIndex.data().toString();

    bool ok1, ok2;
    double sourceNum = copiedValue.toDouble(&ok1);
    double targetNum = targetValue.toDouble(&ok2);

    if (ok1 && ok2) {
        // 都是数字，弹出粘贴选项窗口
        QStringList options = {"替代", "相加", "相减"};
        bool ok;
        QString choice = QInputDialog::getItem(this, tr("选择粘贴方式"),
                                               tr("源值：%1\n目标值：%2").arg(copiedValue, targetValue),
                                               options, 0, false, &ok);
        if (!ok) return;

        double result;
        if (choice == "替代") {
            result = sourceNum;
        } else if (choice == "相加") {
            result = targetNum + sourceNum;
        } else if (choice == "相减") {
            result = targetNum - sourceNum;
        } else {
            return;
        }

        model->setData(targetIndex, QString::number(result));
    } else {
        // 任一不是数字，直接替代
        model->setData(targetIndex, copiedValue);
    }
}


// 通用函数：提取选中单元格中有效数字
QList<double> MainWindow::getSelectedNumbers() {
    QList<double> numbers;
    if (!tableview || !model) return numbers;

    QItemSelectionModel *select = tableview->selectionModel();
    QModelIndexList indexes = select->selectedIndexes();
    if (indexes.isEmpty()) return numbers;

    for (const QModelIndex &index : indexes) {
        QString text = index.data().toString();
        bool ok;
        double val = text.toDouble(&ok);
        if (ok) {
            numbers.append(val);
        }
    }
    return numbers;
}
//计算合计槽函数
void MainWindow::calculateSum() {
    QList<double> numbers = getSelectedNumbers();
    if (numbers.isEmpty()) {
        resultLabel->setText("错误：所选单元格无有效数字");
        return;
    }
    double sum = std::accumulate(numbers.begin(), numbers.end(), 0.0);
    resultLabel->setText(QString("合计：%1").arg(sum));
}
//计算平均值槽函数
void MainWindow::calculateAverage() {
    QList<double> numbers = getSelectedNumbers();
    if (numbers.isEmpty()) {
        resultLabel->setText("错误：所选单元格无有效数字");
        return;
    }
    double avg = std::accumulate(numbers.begin(), numbers.end(), 0.0) / numbers.size();
    resultLabel->setText(QString("平均值：%1").arg(avg));
}
//计算最大值槽函数
void MainWindow::calculateMax() {
    QList<double> numbers = getSelectedNumbers();
    if (numbers.isEmpty()) {
        resultLabel->setText("错误：所选单元格无有效数字");
        return;
    }
    double maxVal = *std::max_element(numbers.begin(), numbers.end());
    resultLabel->setText(QString("最大值：%1").arg(maxVal));
}
//计算最小值槽函数
void MainWindow::calculateMin() {
    QList<double> numbers = getSelectedNumbers();
    if (numbers.isEmpty()) {
        resultLabel->setText("错误：所选单元格无有效数字");
        return;
    }
    double minVal = *std::min_element(numbers.begin(), numbers.end());
    resultLabel->setText(QString("最小值：%1").arg(minVal));
}

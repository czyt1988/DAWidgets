#include "DAFilePathEditWidget.h"
#include "ui_DAFilePathEditWidget.h"
#include <QFileDialog>
namespace DA
{

class DAFilePathEditWidget::PrivateData
{
    DA_DECLARE_PUBLIC(DAFilePathEditWidget)
public:
    PrivateData(DAFilePathEditWidget* p);

public:
    QStringList mNameFilters;
};

DAFilePathEditWidget::PrivateData::PrivateData(DAFilePathEditWidget* p) : q_ptr(p)
{
}

//===============================================================
// DAFilePathEditWidget
//===============================================================

/**
 * @brief 构造文件路径编辑窗口
 * @param parent 父控件
 * @details 加载 ui，连接打开按钮和行编辑器的编辑完成信号
 */
DAFilePathEditWidget::DAFilePathEditWidget(QWidget* parent)
    : QWidget(parent), DA_PIMPL_CONSTRUCT, ui(new Ui::DAFilePathEditWidget)
{
    Q_INIT_RESOURCE(dawidgetsicons);  // 静态库需显式注册 qrc 资源,否则被链接器丢弃
    ui->setupUi(this);
    connect(ui->toolButtonOpen, &QToolButton::clicked, this, &DAFilePathEditWidget::onToolButtonOpenClicked);
    connect(ui->lineEdit, &QLineEdit::editingFinished, this, &DAFilePathEditWidget::onLineEditEditingFinished);
}

/**
 * @brief 析构函数
 * @details 释放 ui 指针
 */
DAFilePathEditWidget::~DAFilePathEditWidget()
{
    delete ui;
}

/**
 * @brief 设置单个文件名过滤器
 * @param filter 文件名过滤器字符串
 * @details 内部转换为 QStringList 后委托给 setNameFilters
 */
void DAFilePathEditWidget::setNameFilter(const QString& filter)
{
    QStringList filters;
    filters.append(filter);
    setNameFilters(filters);
}

/**
 * @brief 设置文件名过滤器列表
 * @param filters 文件名过滤器字符串列表
 * @details 过滤器存储在私有数据中，打开文件对话框时生效
 */
void DAFilePathEditWidget::setNameFilters(const QStringList& filters)
{
    d_ptr->mNameFilters = filters;
}

/**
 * @brief 获取用户输入框的路径
 * @return
 */
QString DAFilePathEditWidget::getFilePath() const
{
    return ui->lineEdit->text();
}

/**
 * @brief 设置输入框的路径
 * @param v
 */
void DAFilePathEditWidget::setFilePath(const QString& v)
{
    ui->lineEdit->setText(v);
}

void DAFilePathEditWidget::onToolButtonOpenClicked()
{
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    if (!d_ptr->mNameFilters.isEmpty()) {
        fileDialog.setNameFilters(d_ptr->mNameFilters);
    }
    if (fileDialog.exec()) {
        auto files = fileDialog.selectedFiles();
        if (!files.isEmpty()) {
            QString p = files.back();
            ui->lineEdit->setText(p);
            emit selectedPath(p);
        }
    }
}

void DAFilePathEditWidget::onLineEditEditingFinished()
{
    QString p = getFilePath();
    emit selectedPath(p);
}

void DAFilePathEditWidget::changeEvent(QEvent* e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

}

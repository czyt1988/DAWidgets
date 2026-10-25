#include "DAFilePathEditWidget.h"
#include "ui_DAFilePathEditWidget.h"
#include <QFileDialog>
#include <QFileInfo>
namespace DA
{

class DAFilePathEditWidget::PrivateData
{
    DA_DECLARE_PUBLIC(DAFilePathEditWidget)
public:
    PrivateData(DAFilePathEditWidget* p);

public:
    QStringList mNameFilters;
    Mode mMode { Mode::OpenFile };
    QString mStartDirectory;
    QString mDefaultSuffix;
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
    connect(ui->lineEdit, &QLineEdit::textChanged, this, [ this ](const QString& t) { Q_EMIT pathChanged(t); });
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
 * @brief 获取文件名过滤器列表
 * @return 过滤器列表
 */
QStringList DAFilePathEditWidget::nameFilters() const
{
    return d_ptr->mNameFilters;
}

/**
 * @brief 设置浏览模式（OpenFile/SaveFile/ExistingDirectory）
 * @param m 模式
 */
void DAFilePathEditWidget::setMode(Mode m)
{
    d_ptr->mMode = m;
}

/**
 * @brief 获取当前浏览模式
 * @return 模式
 */
DAFilePathEditWidget::Mode DAFilePathEditWidget::mode() const
{
    return d_ptr->mMode;
}

/**
 * @brief 设置文件对话框起始目录
 * @param dir 目录（空表示由对话框决定）
 */
void DAFilePathEditWidget::setStartDirectory(const QString& dir)
{
    d_ptr->mStartDirectory = dir;
}

/**
 * @brief 获取文件对话框起始目录
 * @return 目录
 */
QString DAFilePathEditWidget::startDirectory() const
{
    return d_ptr->mStartDirectory;
}

/**
 * @brief 设置 SaveFile 模式的默认后缀
 * @param suffix 后缀（不带点，如 "txt"）
 */
void DAFilePathEditWidget::setDefaultSuffix(const QString& suffix)
{
    d_ptr->mDefaultSuffix = suffix;
}

/**
 * @brief 获取 SaveFile 模式的默认后缀
 * @return 后缀
 */
QString DAFilePathEditWidget::defaultSuffix() const
{
    return d_ptr->mDefaultSuffix;
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

/**
 * @brief 设置文本框占位提示
 * @param t 提示文本
 */
void DAFilePathEditWidget::setPlaceholderText(const QString& t)
{
    ui->lineEdit->setPlaceholderText(t);
}

void DAFilePathEditWidget::onToolButtonOpenClicked()
{
    QFileDialog fileDialog(this);
    const QString current = getFilePath().trimmed();
    // 起始目录优先级：显式设置 > 当前路径所在目录
    if (!d_ptr->mStartDirectory.isEmpty()) {
        fileDialog.setDirectory(d_ptr->mStartDirectory);
    } else if (!current.isEmpty()) {
        const QFileInfo info(current);
        const QString dir = info.isDir() ? info.absoluteFilePath() : info.absolutePath();
        if (!dir.isEmpty()) {
            fileDialog.setDirectory(dir);
        }
    }
    switch (d_ptr->mMode) {
    case Mode::OpenFile: {
        fileDialog.setFileMode(QFileDialog::ExistingFile);
        if (!d_ptr->mNameFilters.isEmpty()) {
            fileDialog.setNameFilters(d_ptr->mNameFilters);
        }
        if (!current.isEmpty()) {
            fileDialog.selectFile(current);
        }
        break;
    }
    case Mode::SaveFile: {
        fileDialog.setAcceptMode(QFileDialog::AcceptSave);
        if (!d_ptr->mDefaultSuffix.isEmpty()) {
            fileDialog.setDefaultSuffix(d_ptr->mDefaultSuffix);
        }
        if (!d_ptr->mNameFilters.isEmpty()) {
            fileDialog.setNameFilters(d_ptr->mNameFilters);
        }
        if (!current.isEmpty()) {
            fileDialog.selectFile(current);
        }
        break;
    }
    case Mode::ExistingDirectory: {
        fileDialog.setFileMode(QFileDialog::Directory);
        fileDialog.setOption(QFileDialog::ShowDirsOnly, true);
        if (!current.isEmpty()) {
            fileDialog.selectFile(current);
        }
        break;
    }
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

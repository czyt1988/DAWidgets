#include "DAFilePathEditWidget.h"
#include "ui_DAFilePathEditWidget.h"
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QMenu>
#include <QMimeData>
#include <QStyle>
#include <QUrl>
namespace DA
{

class DAFilePathEditWidget::PrivateData
{
    DA_DECLARE_PUBLIC(DAFilePathEditWidget)
public:
    PrivateData(DAFilePathEditWidget* p);

    enum class ValidationState
    {
        Hidden,
        Valid,
        Invalid
    };

    bool computeValid(const QString& path) const;

public:
    QStringList mNameFilters;
    Mode mMode { Mode::OpenFile };
    QString mStartDirectory;
    QString mDefaultSuffix;
    bool mDropEnabled { false };
    bool mValidationEnabled { false };
    bool mHasValidation { false };
    bool mLastValid { false };
    ValidationState mState { ValidationState::Hidden };
    QStringList mRecentPaths;
    QMenu* mRecentMenu { nullptr };
};

DAFilePathEditWidget::PrivateData::PrivateData(DAFilePathEditWidget* p) : q_ptr(p)
{
}

/**
 * @brief 按当前模式判断路径是否可用
 * @details SaveFile 允许目标文件不存在，只要求父目录存在且可写
 */
bool DAFilePathEditWidget::PrivateData::computeValid(const QString& path) const
{
    const QString p = path.trimmed();
    if (p.isEmpty())
        return false;
    const QFileInfo fi(p);
    switch (mMode) {
    case Mode::OpenFile:
        return fi.isFile();
    case Mode::ExistingDirectory:
        return fi.isDir();
    case Mode::SaveFile:
        if (fi.exists())
            return !fi.isDir();
        // 不存在的保存目标是正常输入：只要求父目录存在且可写
        return QFileInfo(fi.absolutePath()).isDir() && QFileInfo(fi.absolutePath()).isWritable();
    }
    return false;
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
    ensureResourcesInitialized();  // 静态库需显式注册 qrc 资源,否则被链接器丢弃
    ui->setupUi(this);
    // 新控件默认隐藏；不依赖 .ui 的 visible 属性
    ui->toolButtonRecent->setVisible(false);
    ui->widgetStatus->setVisible(false);
    ui->labelDropHint->setVisible(false);
    ui->lineEdit->installEventFilter(this);
    d_ptr->mRecentMenu = new QMenu(this);
    ui->toolButtonRecent->setMenu(d_ptr->mRecentMenu);
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
    // 析构时 QWidget 隐藏子控件还会触发 focusOut → editingFinished，
    // 必须先断开，否则槽里访问已 delete 的 ui 会崩
    ui->lineEdit->disconnect(this);
    ui->toolButtonOpen->disconnect(this);
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
            const QString p = files.back();
            setPathAndValidate(p);
            Q_EMIT selectedPath(p);
        }
    }
}

void DAFilePathEditWidget::onLineEditEditingFinished()
{
    if (d_ptr->mValidationEnabled)
        validateNow();
    Q_EMIT selectedPath(getFilePath());
}

/**
 * @brief 打开/关闭拖放
 * @param on 是否启用（默认 false）
 */
void DAFilePathEditWidget::setDropEnabled(bool on)
{
    d_ptr->mDropEnabled = on;
    setAcceptDrops(on);
    ui->labelDropHint->setVisible(on);
    if (!on)
        resetDragActive();
}

/** @brief 拖放是否启用 */
bool DAFilePathEditWidget::isDropEnabled() const
{
    return d_ptr->mDropEnabled;
}

/**
 * @brief 打开/关闭路径校验
 * @param on 是否启用（默认 false）
 */
void DAFilePathEditWidget::setValidationEnabled(bool on)
{
    d_ptr->mValidationEnabled = on;
    ui->lineEdit->setClearButtonEnabled(on);
    if (on) {
        if (d_ptr->mState != PrivateData::ValidationState::Hidden)
            ui->widgetStatus->setVisible(true);
    } else {
        ui->widgetStatus->setVisible(false);
        ui->lineEdit->setProperty("invalid", false);
        ui->widgetStatus->setProperty("invalid", false);
        ui->lineEdit->style()->unpolish(ui->lineEdit);
        ui->lineEdit->style()->polish(ui->lineEdit);
        ui->lineEdit->update();
    }
}

/** @brief 校验是否启用 */
bool DAFilePathEditWidget::isValidationEnabled() const
{
    return d_ptr->mValidationEnabled;
}

/**
 * @brief 立即校验当前路径
 * @return 是否合法；空路径返回 false，且不发信号、不显示状态行
 */
bool DAFilePathEditWidget::validateNow()
{
    const QString path = getFilePath().trimmed();
    if (path.isEmpty()) {
        d_ptr->mState = PrivateData::ValidationState::Hidden;
        d_ptr->mHasValidation = false;
        ui->widgetStatus->setVisible(false);
        ui->lineEdit->setProperty("invalid", false);
        ui->lineEdit->style()->unpolish(ui->lineEdit);
        ui->lineEdit->style()->polish(ui->lineEdit);
        ui->lineEdit->update();
        return false;
    }
    const bool valid = d_ptr->computeValid(path);
    const bool changed = !d_ptr->mHasValidation || d_ptr->mLastValid != valid;
    d_ptr->mHasValidation = true;
    d_ptr->mLastValid = valid;
    d_ptr->mState = valid ? PrivateData::ValidationState::Valid : PrivateData::ValidationState::Invalid;
    if (d_ptr->mValidationEnabled)
        refreshValidationVisual(valid);
    if (changed)
        Q_EMIT validationChanged(valid);
    return valid;
}

/**
 * @brief 设置最近路径
 * @param paths 入参顺序为“最近 → 最旧”，去重后截断到 5
 */
void DAFilePathEditWidget::setRecentPaths(const QStringList& paths)
{
    d_ptr->mRecentPaths.clear();
    for (const QString& raw : paths) {
        const QString p = raw.trimmed();
        if (p.isEmpty() || d_ptr->mRecentPaths.contains(p))
            continue;
        d_ptr->mRecentPaths << p;
        if (d_ptr->mRecentPaths.size() >= 5)
            break;
    }
    rebuildRecentMenu();
    ui->toolButtonRecent->setVisible(!d_ptr->mRecentPaths.isEmpty());
}

/** @brief 当前最近路径（最近优先） */
QStringList DAFilePathEditWidget::recentPaths() const
{
    return d_ptr->mRecentPaths;
}

void DAFilePathEditWidget::rebuildRecentMenu()
{
    d_ptr->mRecentMenu->clear();
    for (const QString& path : d_ptr->mRecentPaths) {
        QAction* action = d_ptr->mRecentMenu->addAction(path);
        connect(action, &QAction::triggered, this, [ this, path ]() { onRecentPathActivated(path); });
    }
}

void DAFilePathEditWidget::onRecentPathActivated(const QString& path)
{
    setPathAndValidate(path);
    Q_EMIT recentPathActivated(path);
    Q_EMIT selectedPath(path);
}

void DAFilePathEditWidget::setPathAndValidate(const QString& path)
{
    ui->lineEdit->setText(path);
    if (d_ptr->mValidationEnabled)
        validateNow();
}

void DAFilePathEditWidget::refreshValidationVisual(bool valid)
{
    ui->widgetStatus->setVisible(true);
    ui->labelStatusIcon->setPixmap(style()
                                       ->standardIcon(valid ? QStyle::SP_DialogApplyButton
                                                            : QStyle::SP_MessageBoxCritical)
                                       .pixmap(13, 13));
    ui->labelStatusText->setText(valid ? tr("Path exists")       //cn:文件存在
                                       : tr("Path not found"));  //cn:路径不存在
    ui->lineEdit->setProperty("invalid", !valid);
    ui->widgetStatus->setProperty("invalid", !valid);
    for (QWidget* w : { static_cast< QWidget* >(ui->lineEdit), ui->widgetStatus }) {
        w->style()->unpolish(w);
        w->style()->polish(w);
        w->update();
    }
}

void DAFilePathEditWidget::resetDragActive()
{
    ui->labelDropHint->setProperty("dragActive", false);
    ui->labelDropHint->style()->unpolish(ui->labelDropHint);
    ui->labelDropHint->style()->polish(ui->labelDropHint);
    ui->labelDropHint->update();
}

void DAFilePathEditWidget::dragEnterEvent(QDragEnterEvent* e)
{
    if (!d_ptr->mDropEnabled || !e->mimeData()->hasUrls()) {
        e->ignore();
        return;
    }
    e->acceptProposedAction();
    ui->labelDropHint->setProperty("dragActive", true);
    ui->labelDropHint->style()->unpolish(ui->labelDropHint);
    ui->labelDropHint->style()->polish(ui->labelDropHint);
    ui->labelDropHint->update();
}

void DAFilePathEditWidget::dragLeaveEvent(QDragLeaveEvent* e)
{
    Q_UNUSED(e);
    resetDragActive();
}

void DAFilePathEditWidget::dropEvent(QDropEvent* e)
{
    resetDragActive();
    if (!d_ptr->mDropEnabled || !e->mimeData()->hasUrls()) {
        e->ignore();
        return;
    }
    const QList< QUrl > urls = e->mimeData()->urls();
    if (urls.isEmpty()) {
        e->ignore();
        return;
    }
    const QString path = urls.first().toLocalFile();
    if (path.isEmpty()) {  // 远程 URL 不接管
        e->ignore();
        return;
    }
    setPathAndValidate(path);
    Q_EMIT selectedPath(path);
    e->acceptProposedAction();
}

bool DAFilePathEditWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->lineEdit && d_ptr->mDropEnabled) {
        switch (event->type()) {
        case QEvent::DragEnter: {
            auto* de = static_cast< QDragEnterEvent* >(event);
            if (de->mimeData()->hasUrls()) {
                dragEnterEvent(de);
                return true;
            }
            break;
        }
        case QEvent::DragMove: {
            auto* dm = static_cast< QDragMoveEvent* >(event);
            if (dm->mimeData()->hasUrls()) {
                dm->acceptProposedAction();
                return true;
            }
            break;
        }
        case QEvent::DragLeave:
            resetDragActive();
            return true;
        case QEvent::Drop: {
            auto* de = static_cast< QDropEvent* >(event);
            if (de->mimeData()->hasUrls()) {
                dropEvent(de);
                return true;
            }
            break;
        }
        default:
            break;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void DAFilePathEditWidget::changeEvent(QEvent* e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        if (d_ptr->mValidationEnabled && d_ptr->mState != PrivateData::ValidationState::Hidden) {
            refreshValidationVisual(d_ptr->mState == PrivateData::ValidationState::Valid);
        }
        break;
    default:
        break;
    }
}

}

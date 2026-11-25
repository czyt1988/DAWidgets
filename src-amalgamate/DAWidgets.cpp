
/*** Start of inlined file: DAWidgetsAmalgamTemplateHeaderGlue.h ***/
// This file provides an extra level of indirection for the @remap in the template.
// 合并 cpp 时，模板中的 @remap 会把下面的公共头文件清单替换为已生成的
// DAWidgets.h，避免把所有头文件内容在 .cpp 里再内联一份。
#include "DAWidgets.h"

/*** End of inlined file: DAWidgetsAmalgamTemplateHeaderGlue.h ***/

// -----------------------------------------------------------------------------
// DAWidgets single-file amalgamation (implementation).
//
// 本文件由 tools/Amalgamate.ps1 自动生成，包含 DAWidgets 全部实现。
// 需要与同目录下的 DAWidgets.h 一起编译：两者合起来就是整个库。
// 生成前脚本会用 uic 生成 ui_*.h、用 rcc 生成 qrc_dawidgetsicons.cpp，
// 再由 Amalgamate 一并内联，因此生成后的两个文件没有任何私有头文件依赖。
// Do not edit this generated file by hand.
// -----------------------------------------------------------------------------

// disable warnings about unsafe standard library calls
#ifdef _MSC_VER
#pragma push_macro("_CRT_SECURE_NO_WARNINGS")
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma warning(push)
#pragma warning(disable: 4996) // deprecated POSIX names
#endif

// ---- Selector 选择器家族 ----

/*** Start of inlined file: DAFlowLayout.cpp ***/
#include <QWidget>
#include <QWidgetItem>

namespace DA
{
class DAFlowLayout::PrivateData
{
public:
	PrivateData(DAFlowLayout *) {}
	QList<QLayoutItem *> items;
	int hSpace { -1 };
	int vSpace { -1 };
	bool trailingExpanding { true };
};

/**
 * @brief 构造流式布局并挂载到父控件
 * @param parent 父控件
 * @param margin 外边距，-1 表示使用 style 默认值
 * @param hSpacing 水平间距，-1 表示使用 style 默认值
 * @param vSpacing 垂直间距，-1 表示使用 style 默认值
 */
DAFlowLayout::DAFlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
	: QLayout(parent), DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	d->hSpace = (hSpacing >= 0 ? hSpacing : -1);
	d->vSpace = (vSpacing >= 0 ? vSpacing : -1);
	setContentsMargins(margin, margin, margin, margin);
}

/**
 * @brief 构造流式布局（无父控件）
 * @param margin 外边距，-1 表示使用 style 默认值
 * @param hSpacing 水平间距，-1 表示使用 style 默认值
 * @param vSpacing 垂直间距，-1 表示使用 style 默认值
 */
DAFlowLayout::DAFlowLayout(int margin, int hSpacing, int vSpacing)
	: DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	d->hSpace = (hSpacing >= 0 ? hSpacing : -1);
	d->vSpace = (vSpacing >= 0 ? vSpacing : -1);
	setContentsMargins(margin, margin, margin, margin);
}

/**
 * @brief 析构，释放所有布局项
 */
DAFlowLayout::~DAFlowLayout()
{
	while (auto *item = takeAt(0))
		delete item;
}

/**
 * @brief 添加布局项到末尾
 * @param item 布局项
 */
void DAFlowLayout::addItem(QLayoutItem *item)
{
	DA_D(d);
	d->items.append(item);
}

/**
 * @brief 在指定位置插入控件
 * @param index 插入位置，负数追加到末尾
 * @param widget 要插入的控件
 * @details 负索引自动修正为末尾，越界索引被 qBound 钳制到合法范围
 */
void DAFlowLayout::insertWidget(int index, QWidget *widget)
{
	DA_D(d);
	addChildWidget(widget);
	if (index < 0)
		index = d->items.size();
	index = qBound(0, index, d->items.size());
	d->items.insert(index, new QWidgetItem(widget));
	invalidate();
}

/**
 * @brief 获取水平间距
 * @return 间距值，-1 时回退到 style 默认值
 */
int DAFlowLayout::horizontalSpacing() const
{
	DA_DC(d);
	return d->hSpace >= 0 ? d->hSpace : smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

/**
 * @brief 获取垂直间距
 * @return 间距值，-1 时回退到 style 默认值
 */
int DAFlowLayout::verticalSpacing() const
{
	DA_DC(d);
	return d->vSpace >= 0 ? d->vSpace : smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}

/**
 * @brief 获取布局项数量
 * @return 布局项数量
 */
int DAFlowLayout::count() const
{
	DA_DC(d);
	return d->items.size();
}

/**
 * @brief 获取指定索引的布局项
 * @param index 索引
 * @return 布局项指针，越界返回 nullptr
 */
QLayoutItem *DAFlowLayout::itemAt(int index) const
{
	DA_DC(d);
	return d->items.value(index);
}

/**
 * @brief 移除并返回指定索引的布局项
 * @param index 索引
 * @return 被移除的布局项指针，越界返回 nullptr
 */
QLayoutItem *DAFlowLayout::takeAt(int index)
{
	DA_D(d);
	return (index >= 0 && index < d->items.size()) ? d->items.takeAt(index) : nullptr;
}

/**
 * @brief 返回布局可扩展的方向
 * @return 空方向集——流式布局不主动扩展
 */
Qt::Orientations DAFlowLayout::expandingDirections() const
{
	return {};
}

/**
 * @brief 是否支持 height-for-width 布局
 * @return 固定返回 true——流式布局高度取决于可用宽度
 */
bool DAFlowLayout::hasHeightForWidth() const
{
	return true;
}

/**
 * @brief 根据给定宽度计算所需高度
 * @param width 可用宽度
 * @return 该宽度下排列所有子项所需的高度
 * @details 以 testOnly 模式调用 doLayout 进行测量计算
 */
int DAFlowLayout::heightForWidth(int width) const
{
	return doLayout(QRect(0, 0, width, 0), true);
}

/**
 * @brief 执行布局，排列所有子项
 * @param rect 布局区域
 * @details 先调用 QLayout::setGeometry 处理缓存，再以非测试模式调用 doLayout 实际排列
 */
void DAFlowLayout::setGeometry(const QRect &rect)
{
	QLayout::setGeometry(rect);
	doLayout(rect, false);
}

/**
 * @brief 获取推荐尺寸
 * @return 与 minimumSize 相同
 */
QSize DAFlowLayout::sizeHint() const
{
	return minimumSize();
}

/**
 * @brief 获取最小尺寸
 * @return 所有子项最小尺寸的扩展最大值加上 contentsMargins
 */
QSize DAFlowLayout::minimumSize() const
{
	DA_DC(d);
	QSize size;
	for (auto *item : d->items)
		size = size.expandedTo(item->minimumSize());
	const auto m = contentsMargins();
	size += QSize(m.left() + m.right(), m.top() + m.bottom());
	return size;
}

/**
 * @brief 设置末尾项是否占满剩余宽度
 * @param on 是否启用
 * @details 用于 DAChipsEdit 末尾内嵌输入框自适应宽度。变化时调用 invalidate 刷新布局缓存
 */
void DAFlowLayout::setTrailingExpanding(bool on)
{
	DA_D(d);
	if (d->trailingExpanding == on)
		return;
	d->trailingExpanding = on;
	invalidate();
}

/**
 * @brief 获取末尾项是否占满剩余宽度
 * @return 启用返回 true
 */
bool DAFlowLayout::trailingExpanding() const
{
	DA_DC(d);
	return d->trailingExpanding;
}

/**
 * @brief 核心流式排列算法
 * @param rect 可用布局区域
 * @param testOnly true 时仅计算高度不实际 setGeometry，false 时执行排列
 * @return 排列后所需总高度
 * @details 逐项按行排列，超出右边界自动换行。末尾项在 trailingExpanding 开启时
 *          占满当前行剩余宽度，不足最小宽度则换行占满整行
 */
int DAFlowLayout::doLayout(const QRect &rect, bool testOnly) const
{
	DA_DC(d);
	int left, top, right, bottom;
	getContentsMargins(&left, &top, &right, &bottom);
	const QRect eff = rect.adjusted(left, top, -right, -bottom);
	int x = eff.x();
	int y = eff.y();
	int lineHeight = 0;

	const int hsp = horizontalSpacing();
	const int vsp = verticalSpacing();
	const int n = d->items.size();

	for (int i = 0; i < n; ++i) {
		QLayoutItem *item = d->items.at(i);
		// 隐藏控件(例如折叠的标签 chip)不占位,也不吃水平/垂直间距
		if (item->isEmpty())
			continue;
		const bool trailing = d->trailingExpanding && (i == n - 1);

		if (trailing) {
			const int minW = item->minimumSize().width();
			const int hintH = item->sizeHint().height();
			int remaining = eff.right() - x; // 剩余宽度（不含尾部 hsp）
			if (remaining < minW && lineHeight > 0) {
				// 换行：占满整行宽度
				y = y + lineHeight + vsp;
				x = eff.x();
				lineHeight = 0;
				remaining = eff.width();
			}
			const int w = qMax(remaining, minW);
			const int h = qMax(hintH, item->minimumSize().height());
			if (!testOnly)
				item->setGeometry(QRect(x, y, w, h));
			x += w + hsp;
			lineHeight = qMax(lineHeight, h);
			continue;
		}

		const QSize is = item->sizeHint();
		int nextX = x + is.width() + hsp;
		if (nextX - hsp > eff.right() && lineHeight > 0) {
			x = eff.x();
			y = y + lineHeight + vsp;
			nextX = x + is.width() + hsp;
			lineHeight = 0;
		}
		if (!testOnly)
			item->setGeometry(QRect(QPoint(x, y), is));
		x = nextX;
		lineHeight = qMax(lineHeight, is.height());
	}
	return y + lineHeight - rect.y() + bottom;
}

/**
 * @brief 从父控件或父布局获取 style 默认间距
 * @param pm 要查询的 PixelMetric
 * @return 间距值，无父对象返回 -1
 * @details 父对象为控件时查询其 style，为布局时返回 layout spacing
 */
int DAFlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
	QObject *p = parent();
	if (!p)
		return -1;
	if (p->isWidgetType()) {
		auto *w = static_cast<QWidget *>(p);
		return w->style()->pixelMetric(pm, nullptr, w);
	}
	return static_cast<QLayout *>(p)->spacing();
}
}  // namespace DA

/*** End of inlined file: DAFlowLayout.cpp ***/



/*** Start of inlined file: DAChip.cpp ***/
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QMouseEvent>
#include <QToolButton>

namespace DA
{
class DAChip::PrivateData
{
public:
	PrivateData(DAChip *) {}
	QString text;
	bool closable { true };
	int elideWidth { 0 };
	QLabel *label { nullptr };
	QToolButton *closeBtn { nullptr };
};

/**
 * @brief 构造带文本的标签胶囊
 * @param text 标签显示文本
 * @param parent 父控件
 */
DAChip::DAChip(const QString &text, QWidget *parent) : QFrame(parent), DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	d->text = text;
	init();
}

/**
 * @brief 构造空文本的标签胶囊
 * @param parent 父控件
 */
DAChip::DAChip(QWidget *parent) : QFrame(parent), DA_PIMPL_CONSTRUCT
{
	init();
}

/**
 * @brief 析构
 * @details PIMPL 指针由 DA_PIMPL_CONSTRUCT 宏自动释放
 */
DAChip::~DAChip() = default;

/**
 * @brief 初始化标签胶囊内部结构
 * @details 创建水平布局（QLabel + QToolButton），设置对象名与 qss 选择器，
 *          连接关闭按钮 clicked 到 closeRequested 信号
 */
void DAChip::init()
{
	DA_D(d);
	setObjectName(QStringLiteral("DAChip"));
	setFrameShape(QFrame::NoFrame);

	auto *lay = new QHBoxLayout(this);
	lay->setContentsMargins(6, 0, 4, 0);
	lay->setSpacing(4);

	d->label = new QLabel(d->text, this);
	d->label->setObjectName(QStringLiteral("DAChipLabel"));
	lay->addWidget(d->label);

	d->closeBtn = new QToolButton(this);
	d->closeBtn->setObjectName(QStringLiteral("DAChipCloseButton"));
	d->closeBtn->setText(QStringLiteral("×"));  // "×"
	d->closeBtn->setAutoRaise(true);
	d->closeBtn->setFocusPolicy(Qt::NoFocus);
	d->closeBtn->setCursor(Qt::ArrowCursor);
	d->closeBtn->setFixedSize(16, 16);
	lay->addWidget(d->closeBtn);

	connect(d->closeBtn, &QToolButton::clicked, this, &DAChip::closeRequested);
	d->closeBtn->setVisible(d->closable);

	setAttribute(Qt::WA_LayoutUsesWidgetRect);
	applyElide();
}

/**
 * @brief 获取标签显示文本
 * @return 当前文本
 */
QString DAChip::text() const
{
	DA_DC(d);
	return d->text;
}

/**
 * @brief 设置标签显示文本
 * @param text 新文本
 * @details 文本变化时更新 QLabel 并发射 textChanged
 */
void DAChip::setText(const QString &text)
{
	DA_D(d);
	if (d->text == text)
		return;
	d->text = text;
	applyElide();
	Q_EMIT textChanged(text);
}

/**
 * @brief 是否显示关闭按钮
 * @return 可关闭返回 true
 */
bool DAChip::closable() const
{
	DA_DC(d);
	return d->closable;
}

/**
 * @brief 设置是否显示关闭按钮
 * @param on 是否可关闭
 * @details 变化时切换关闭按钮可见性并发射 closableChanged
 */
void DAChip::setClosable(bool on)
{
	DA_D(d);
	if (d->closable == on)
		return;
	d->closable = on;
	d->closeBtn->setVisible(on);
	// 关闭按钮显隐会改变 label 可用宽度，省略结果需要重算
	applyElide();
	Q_EMIT closableChanged(on);
}

/**
 * @brief 设置标签最大宽度
 * @param px 最大宽度（像素），0 表示不省略
 * @details 超过宽度时 label 以 … 结尾显示，全文进入 tooltip；text() 仍返回全文。
 *          tooltip 由本控件接管，会覆盖调用方通过 setToolTip() 设置的内容。
 */
void DAChip::setElideWidth(int px)
{
	DA_D(d);
	px = qMax(0, px);
	if (d->elideWidth == px)
		return;
	d->elideWidth = px;
	applyElide();
}

/**
 * @brief 获取标签最大宽度
 * @return 最大宽度（像素），0 表示不省略
 */
int DAChip::elideWidth() const
{
	DA_DC(d);
	return d->elideWidth;
}

/**
 * @brief 按 elideWidth 重新计算显示文本
 * @details 关闭时恢复全文、清空 tooltip 与最大宽度；开启时按布局可用宽度做右侧省略。
 */
void DAChip::applyElide()
{
	DA_D(d);
	if (!d->label)
		return;
	if (d->elideWidth <= 0) {
		d->label->setText(d->text);
		d->label->setToolTip(QString());
		setToolTip(QString());
		setMaximumWidth(QWIDGETSIZE_MAX);
		updateGeometry();
		return;
	}
	QLayout *lay = layout();
	const QMargins m = lay->contentsMargins();
	int avail = d->elideWidth - m.left() - m.right() - lay->spacing();
	// 用 closable 而不是 closeBtn->isVisible()：控件 show() 之前 isVisible() 恒为 false，
	// 会算出偏大的可用宽度，导致第一次布局时省略结果不准
	if (d->closable && d->closeBtn)
		avail -= d->closeBtn->sizeHint().width();
	// 宽度过小时至少给关闭按钮留出空间，避免负宽度
	avail = qMax(avail, 0);
	const QFontMetrics fm(d->label->font());
	d->label->setText(fm.elidedText(d->text, Qt::ElideRight, avail));
	d->label->setToolTip(d->text);
	setToolTip(d->text);
	setMaximumWidth(d->elideWidth);
	updateGeometry();
}

/**
 * @brief 鼠标按下事件
 * @param e 鼠标事件
 * @details 左键点击时发射 clicked 信号
 */
void DAChip::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
		Q_EMIT clicked();
	QFrame::mousePressEvent(e);
}
}  // namespace DA

/*** End of inlined file: DAChip.cpp ***/


/*** Start of inlined file: DAChipsEdit.cpp ***/
#include <QClipboard>
#include <QColor>
#include <QFocusEvent>
#include <QGuiApplication>
#include <QInputMethodEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QSet>

namespace DA
{
class DAChipsEdit::PrivateData
{
public:
	PrivateData(DAChipsEdit *) {}
	QStringList tags;
	DAFlowLayout *layout { nullptr };
	QLineEdit *input { nullptr };
	QList<DAChip *> chips;
	QString placeholder;
	bool allowCreate { true };
	bool allowDuplicate { false };
	int maxCount { -1 };
	QList<Separator> separators;
	QSet<QChar> extraSeparators;
	bool pasteSplitEnabled { false };
	bool imeComposing { false };
	int tagElideWidth { 0 };
	bool showCount { false };
	QLabel *countLabel { nullptr };
	// 打开计数前 layout 的底部 margin，关闭时原样恢复
	int countMarginBackup { -1 };
};

/**
 * @brief 构造流式多标签输入文本框
 * @param parent 父控件
 * @details 创建 DAFlowLayout 流式布局和内嵌 QLineEdit 输入框，输入框设为布局末尾项
 *          以实现 trailing expanding 自适应宽度。安装事件过滤器拦截按键与焦点事件。
 *          默认分隔符为逗号和回车。
 */
DAChipsEdit::DAChipsEdit(QWidget *parent) : QFrame(parent), DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	setObjectName(QStringLiteral("DAChipsEdit"));
	setFrameShape(QFrame::StyledPanel);
	setFocusPolicy(Qt::StrongFocus);

	d->layout = new DAFlowLayout(this, 2, 4, 4);
	d->layout->setContentsMargins(4, 4, 4, 4);

	d->input = new QLineEdit(this);
	d->input->setObjectName(QStringLiteral("DAChipsEditInput"));
	d->input->setFrame(false);
	d->input->setClearButtonEnabled(false);
	d->input->setPlaceholderText(d->placeholder);
	d->input->installEventFilter(this);
	d->layout->addWidget(d->input);  // 末尾，trailing expanding

	d->separators = { Separator::Comma, Separator::Enter };

	connect(d->input, &QLineEdit::returnPressed, this, &DAChipsEdit::commitInput);
	connect(d->input, &QLineEdit::textEdited, this, &DAChipsEdit::onInputEdited);
	connect(d->input, &QLineEdit::textChanged, this, &DAChipsEdit::inputTextChanged);
	// 计数标签文本由标签列表统一驱动，覆盖 append/insert/remove/clear/setTags/截断
	connect(this, &DAChipsEdit::tagsChanged, this, [this] { updateCountLabel(); });
}

/**
 * @brief 析构
 * @details PIMPL 指针由 DA_PIMPL_CONSTRUCT 宏自动释放
 */
DAChipsEdit::~DAChipsEdit() = default;

/**
 * @brief 获取当前所有标签文本列表
 * @return 标签字符串列表
 */
QStringList DAChipsEdit::tags() const
{
	DA_DC(d);
	return d->tags;
}

/**
 * @brief 设置标签列表
 * @param tags 标签字符串列表
 * @details 替换当前所有标签，自动去空白、去重（若不允许重复）、截断至 maxCount，
 *          然后重建所有 DAChip 子控件并发射 tagsChanged
 */
void DAChipsEdit::setTags(const QStringList &tags)
{
	DA_D(d);
	QStringList clean;
	clean.reserve(tags.size());
	for (const QString &t : tags) {
		const QString s = t.trimmed();
		if (s.isEmpty())
			continue;
		if (!d->allowDuplicate && clean.contains(s))
			continue;
		clean << s;
	}
	if (d->maxCount >= 0 && clean.size() > d->maxCount)
		clean = clean.mid(0, d->maxCount);
	d->tags = clean;
	rebuildChips();
	Q_EMIT tagsChanged(d->tags);
}

/**
 * @brief 追加一个标签到末尾
 * @param text 标签文本
 * @details 空文本、达到 maxCount 上限、重复（若不允许重复）时静默忽略。
 *          成功时创建 DAChip 子控件并发射 tagAdded 与 tagsChanged
 */
void DAChipsEdit::appendTag(const QString &text)
{
	DA_D(d);
	const QString t = text.trimmed();
	if (t.isEmpty())
		return;
	if (d->maxCount >= 0 && d->tags.size() >= d->maxCount)
		return;
	if (!d->allowDuplicate && d->tags.contains(t))
		return;
	d->tags.append(t);
	addChipWidget(t);
	Q_EMIT tagAdded(t);
	Q_EMIT tagsChanged(d->tags);
}

/**
 * @brief 在指定位置插入标签
 * @param index 插入位置
 * @param text 标签文本
 * @return 成功返回 true，失败（空文本/超限/重复/越界）返回 false
 * @details 成功后重建所有 DAChip 子控件并发射 tagAdded 与 tagsChanged
 */
bool DAChipsEdit::insertTag(int index, const QString &text)
{
	DA_D(d);
	const QString t = text.trimmed();
	if (t.isEmpty())
		return false;
	if (d->maxCount >= 0 && d->tags.size() >= d->maxCount)
		return false;
	if (!d->allowDuplicate && d->tags.contains(t))
		return false;
	index = qBound(0, index, d->tags.size());
	d->tags.insert(index, t);
	rebuildChips();
	Q_EMIT tagAdded(t);
	Q_EMIT tagsChanged(d->tags);
	return true;
}

/**
 * @brief 移除第一个匹配文本的标签
 * @param text 要移除的标签文本
 */
void DAChipsEdit::removeTag(const QString &text)
{
	DA_D(d);
	const int idx = d->tags.indexOf(text);
	if (idx < 0)
		return;
	removeAt(idx);
}

/**
 * @brief 移除指定索引处的标签
 * @param index 标签索引
 * @details 移除对应的 DAChip 子控件并发射 tagRemoved 与 tagsChanged
 */
void DAChipsEdit::removeAt(int index)
{
	DA_D(d);
	if (index < 0 || index >= d->tags.size())
		return;
	const QString t = d->tags.takeAt(index);
	if (index < d->chips.size()) {
		DAChip *chip = d->chips.takeAt(index);
		d->layout->removeWidget(chip);
		delete chip;
	}
	Q_EMIT tagRemoved(t);
	Q_EMIT tagsChanged(d->tags);
}

/**
 * @brief 清空所有标签
 * @details 若已无标签则不触发信号，否则重建子控件并发射 tagsChanged
 */
void DAChipsEdit::clear()
{
	DA_D(d);
	if (d->tags.isEmpty())
		return;
	d->tags.clear();
	rebuildChips();
	Q_EMIT tagsChanged(d->tags);
}

/**
 * @brief 获取占位提示文本
 * @return 占位提示文本
 */
QString DAChipsEdit::placeholder() const
{
	DA_DC(d);
	return d->placeholder;
}

/**
 * @brief 设置占位提示文本
 * @param text 占位提示文本
 * @details 同步更新内嵌输入框的 placeholder
 */
void DAChipsEdit::setPlaceholder(const QString &text)
{
	DA_D(d);
	d->placeholder = text;
	d->input->setPlaceholderText(text);
}

/**
 * @brief 是否允许添加非候选列表中的文本作为标签
 * @return 允许创建返回 true
 */
bool DAChipsEdit::allowCreate() const
{
	DA_DC(d);
	return d->allowCreate;
}

/**
 * @brief 设置是否允许创建新标签
 * @param on 是否允许
 */
void DAChipsEdit::setAllowCreate(bool on)
{
	DA_D(d);
	d->allowCreate = on;
}

/**
 * @brief 是否允许重复标签
 * @return 允许重复返回 true
 */
bool DAChipsEdit::allowDuplicate() const
{
	DA_DC(d);
	return d->allowDuplicate;
}

/**
 * @brief 设置是否允许重复标签
 * @param on 是否允许
 */
void DAChipsEdit::setAllowDuplicate(bool on)
{
	DA_D(d);
	d->allowDuplicate = on;
}

/**
 * @brief 打开/关闭粘贴批量拆分
 * @param on 是否开启（默认 false）
 * @details 关闭时不拦截任何按键、不读剪贴板，行为与旧版本一致
 */
void DAChipsEdit::setPasteSplitEnabled(bool on)
{
	DA_D(d);
	d->pasteSplitEnabled = on;
}

/**
 * @brief 粘贴批量拆分是否开启
 * @return 开启返回 true
 */
bool DAChipsEdit::isPasteSplitEnabled() const
{
	DA_DC(d);
	return d->pasteSplitEnabled;
}

/**
 * @brief 粘贴的唯一入口
 * @param text 待粘贴文本
 * @details 开关关闭或 allowCreate 为 false 时退化为普通插入输入框；
 *          开启时按分隔符拆成 token，先发 pasteSplit，再逐个追加为标签。
 *          快捷键、单测与 demo 的"模拟粘贴"共用本函数。
 */
void DAChipsEdit::pasteText(const QString &text)
{
	DA_D(d);
	if (text.trimmed().isEmpty())
		return;
	if (!d->allowCreate || !d->pasteSplitEnabled) {
		d->input->insert(text);
		return;
	}
	const QStringList tokens = splitForPaste(text);
	if (tokens.isEmpty())
		return;
	Q_EMIT pasteSplit(tokens);
	applyPastedTokens(tokens);
}

/**
 * @brief 设置 chip 最大宽度
 * @param px 最大宽度（像素），0 表示不省略（默认）
 */
void DAChipsEdit::setTagElideWidth(int px)
{
	DA_D(d);
	px = qMax(0, px);
	d->tagElideWidth = px;
	for (DAChip *chip : d->chips)
		chip->setElideWidth(px);
	d->layout->invalidate();
}

/**
 * @brief 获取 chip 最大宽度
 * @return 最大宽度（像素），0 表示不省略
 */
int DAChipsEdit::tagElideWidth() const
{
	DA_DC(d);
	return d->tagElideWidth;
}

/**
 * @brief 显示/隐藏右下角数量计数
 * @param on 是否显示（默认 false）
 * @details 开启时创建私有 QLabel 并预留底部 margin，标签不进 DAFlowLayout，
 *          因此不会夺走末尾输入框的自动伸展；关闭时删除标签并恢复原 margin。
 */
void DAChipsEdit::setShowCount(bool on)
{
	DA_D(d);
	if (d->showCount == on)
		return;
	d->showCount = on;
	QMargins m = d->layout->contentsMargins();
	if (on) {
		if (!d->countLabel) {
			d->countLabel = new QLabel(this);
			d->countLabel->setObjectName(QStringLiteral("DAChipsEditCount"));
			d->countLabel->setFocusPolicy(Qt::NoFocus);
			d->countLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
			QFont f = d->countLabel->font();
			f.setPixelSize(11);
			d->countLabel->setFont(f);
			QPalette pal = d->countLabel->palette();
			pal.setColor(QPalette::WindowText, QColor(0xA8, 0xAB, 0xB2));
			d->countLabel->setPalette(pal);
		}
		// 记下调用方当前 margin，关闭时原样恢复，不写死 4
		d->countMarginBackup = m.bottom();
		m.setBottom(m.bottom() + d->countLabel->sizeHint().height() + 2);
		d->layout->setContentsMargins(m);
		updateCountLabel();
		d->countLabel->show();
		d->countLabel->raise();
	} else {
		if (d->countLabel) {
			d->countLabel->hide();
			d->countLabel->deleteLater();
			d->countLabel = nullptr;
		}
		if (d->countMarginBackup >= 0) {
			m.setBottom(d->countMarginBackup);
			d->layout->setContentsMargins(m);
			d->countMarginBackup = -1;
		}
	}
}

/**
 * @brief 是否显示右下角数量计数
 * @return 显示返回 true
 */
bool DAChipsEdit::showCount() const
{
	DA_DC(d);
	return d->showCount;
}

/**
 * @brief 获取标签数量上限
 * @return 上限值，-1 表示无限制
 */
int DAChipsEdit::maxCount() const
{
	DA_DC(d);
	return d->maxCount;
}

/**
 * @brief 设置标签数量上限
 * @param count 上限值，-1 表示无限制
 * @details 若当前标签数超过新上限则截断并重建子控件，发射 tagsChanged
 */
void DAChipsEdit::setMaxCount(int count)
{
	DA_D(d);
	d->maxCount = count;
	if (d->maxCount >= 0 && d->tags.size() > d->maxCount) {
		d->tags = d->tags.mid(0, d->maxCount);
		rebuildChips();
		Q_EMIT tagsChanged(d->tags);
	}
	// 未触发截断时没有 tagsChanged，计数标签仍要跟着换分母
	updateCountLabel();
}

/**
 * @brief 设置分隔符枚举列表
 * @param separators 分隔符枚举列表
 * @details 指定哪些字符/按键触发标签提交，Enter 由回车键触发不参与文本内分隔检测
 */
void DAChipsEdit::setSeparators(const QList<Separator> &separators)
{
	DA_D(d);
	d->separators = separators;
}

/**
 * @brief 获取当前分隔符枚举列表
 * @return 分隔符枚举列表
 */
QList<DAChipsEdit::Separator> DAChipsEdit::separators() const
{
	DA_DC(d);
	return d->separators;
}

/**
 * @brief 以字符串形式设置额外分隔符
 * @param chars 分隔符字符集合
 * @details 字符串中任意字符均可触发标签提交，补充枚举之外的运行时分隔符
 */
void DAChipsEdit::setSeparators(const QString &chars)
{
	DA_D(d);
	d->extraSeparators.clear();
	for (const QChar &c : chars)
		d->extraSeparators.insert(c);
}

/**
 * @brief 获取内嵌输入框当前文本
 * @return 输入框文本
 */
QString DAChipsEdit::inputText() const
{
	DA_DC(d);
	return d->input->text();
}

/**
 * @brief 清空内嵌输入框文本
 */
void DAChipsEdit::clearInput()
{
	DA_D(d);
	d->input->clear();
}

/**
 * @brief 计算单行高度
 * @return 输入框 sizeHint 高度 + layout 上下 margin
 * @details 供 DASelectComboBox 固定高度模式估算多行高度
 */
int DAChipsEdit::rowHeightHint() const
{
	DA_DC(d);
	const auto m = d->layout->contentsMargins();
	return d->input->sizeHint().height() + m.top() + m.bottom();
}

/**
 * @brief 计算推荐尺寸
 * @return 推荐的 QSize
 */
QSize DAChipsEdit::sizeHint() const
{
	return QFrame::sizeHint();
}

/**
 * @brief 计算最小推荐尺寸
 * @return 最小 QSize
 */
QSize DAChipsEdit::minimumSizeHint() const
{
	return QFrame::minimumSizeHint();
}

/**
 * @brief 事件过滤器，拦截内嵌输入框的焦点与按键事件
 * @param watched 被监听的对象
 * @param event 事件
 * @return 是否拦截事件
 * @details FocusIn/FocusOut 发射 inputFocusChanged；KeyPress 先做 IME 组字与粘贴
 *          拦截，再发射 inputKeyEvent 供外部拦截，未拦截时处理 Backspace 删末尾标签、
 *          Tab 提交、Escape 清空
 */
bool DAChipsEdit::eventFilter(QObject *watched, QEvent *event)
{
	DA_D(d);
	if (watched == d->input) {
		const QEvent::Type t = event->type();
		if (t == QEvent::FocusIn) {
			Q_EMIT inputFocusChanged(true);
		} else if (t == QEvent::FocusOut) {
			Q_EMIT inputFocusChanged(false);
		} else if (t == QEvent::InputMethod) {
			// 只观察不消费：组字中（preedit 非空且无 commit）与组字结束的判定
			auto *ime = static_cast< QInputMethodEvent * >(event);
			if (!ime->commitString().isEmpty() || ime->replacementLength() > 0)
				d->imeComposing = false;
			else if (!ime->preeditString().isEmpty())
				d->imeComposing = true;
		} else if (t == QEvent::KeyPress) {
			auto *ke = static_cast< QKeyEvent * >(event);
			const int key = ke->key();
			// IME 守卫必须在 inputKeyEvent 之前：否则 DASelectComboBox 已经选中候选
			if (d->imeComposing && (key == Qt::Key_Return || key == Qt::Key_Enter))
				return true;
			// 粘贴拦截同样先于 inputKeyEvent，避免外部先消费 Ctrl+V
			if (d->pasteSplitEnabled && ke->matches(QKeySequence::Paste)) {
				const QString clip = QGuiApplication::clipboard()->text();
				if (clip.isEmpty())
					Q_EMIT pasteRejected(QStringLiteral("clipboard"));
				else
					pasteText(clip);
				return true;
			}
			bool handled = false;
			Q_EMIT inputKeyEvent(ke, &handled);
			if (handled)
				return true;
			if (key == Qt::Key_Backspace && d->input->text().isEmpty() && !d->chips.isEmpty()) {
				removeAt(d->chips.size() - 1);
				return true;
			}
			if (key == Qt::Key_Tab && d->separators.contains(Separator::Tab)) {
				commitInput();
				return true;
			}
			if (key == Qt::Key_Escape) {
				d->input->clear();
				return true;
			}
		}
	}
	return QFrame::eventFilter(watched, event);
}

/**
 * @brief 获焦时将焦点转发到内嵌输入框
 * @param event 焦点事件
 */
void DAChipsEdit::focusInEvent(QFocusEvent *event)
{
	QFrame::focusInEvent(event);
	DA_D(d);
	d->input->setFocus();
}

/**
 * @brief 尺寸变化时重新定位右下角计数标签
 * @param event 尺寸事件
 * @details 只做 setGeometry，不改 margin，避免与流式布局互相触发
 */
void DAChipsEdit::resizeEvent(QResizeEvent *event)
{
	QFrame::resizeEvent(event);
	positionCountLabel();
}

/**
 * @brief 重建所有 DAChip 子控件
 * @details 先删除现有所有 chip，再按 d->tags 逐一创建。用于标签列表整体替换后刷新视图
 */
void DAChipsEdit::rebuildChips()
{
	DA_D(d);
	while (!d->chips.isEmpty()) {
		DAChip *chip = d->chips.takeLast();
		d->layout->removeWidget(chip);
		delete chip;
	}
	for (const QString &t : d->tags)
		addChipWidget(t);
}

/**
 * @brief 创建并插入一个 DAChip 子控件
 * @param text 标签文本
 * @details 插入到布局末尾输入框之前，连接 closeRequested 信号到移除逻辑。
 *          需显式 show()：parent 可能已可见，未 show 的 QWidgetItem sizeHint 为 0
 */
void DAChipsEdit::addChipWidget(const QString &text)
{
	DA_D(d);
	auto *chip = new DAChip(text, this);
	chip->setClosable(true);
	chip->setElideWidth(d->tagElideWidth);
	const int idx = d->chips.size();  // 插在末尾输入框之前
	d->layout->insertWidget(idx, chip);
	d->chips.append(chip);
	chip->show();  // parent 可能已可见，需显式 show，否则 QWidgetItem sizeHint 为 0
	connect(chip, &DAChip::closeRequested, this, [this, chip]() {
		DA_D(d);
		const int i = d->chips.indexOf(chip);
		if (i >= 0)
			removeAt(i);
	});
}

/**
 * @brief 提交输入框文本为标签
 * @details 由回车键触发。空文本不操作，allowCreate 为 false 时不创建标签，
 *          否则清空输入框并调用 appendTag
 */
void DAChipsEdit::commitInput()
{
	DA_D(d);
	// 第二道闸：returnPressed 可能绕过事件过滤器
	if (d->imeComposing)
		return;
	const QString raw = d->input->text();
	if (raw.isEmpty())
		return;
	d->input->clear();
	if (!d->allowCreate)
		return;
	appendTag(raw);
}

/**
 * @brief 输入框文本编辑时检测内嵌分隔符
 * @param text 当前输入框文本
 * @details 若文本包含分隔符字符则按分隔符拆分并逐个提交标签，清空输入框。
 *          allowCreate 为 false 时不处理
 */
void DAChipsEdit::onInputEdited(const QString &text)
{
	DA_D(d);
	if (!d->allowCreate)
		return;
	const QSet< QChar > seps = separatorChars();
	if (seps.isEmpty())
		return;
	bool has = false;
	for (const QChar &c : text) {
		if (seps.contains(c)) {
			has = true;
			break;
		}
	}
	if (!has)
		return;
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
	const QStringList parts = text.split(separatorRegex(), Qt::SkipEmptyParts);
#else
	const QStringList parts = text.split(separatorRegex(), QString::SkipEmptyParts);
#endif
	d->input->clear();
	for (const QString &p : parts)
		appendTag(p);
}

/**
 * @brief 获取所有分隔符字符集合
 * @return 分隔符 QSet
 * @details 合并枚举分隔符（Comma/Semicolon/Space/Tab，Enter 除外）与额外字符串分隔符
 */
QSet< QChar > DAChipsEdit::separatorChars() const
{
	DA_DC(d);
	QSet< QChar > s = d->extraSeparators;
	for (Separator sep : d->separators) {
		switch (sep) {
		case Separator::Comma: s << QLatin1Char(','); break;
		case Separator::Semicolon: s << QLatin1Char(';'); break;
		case Separator::Space: s << QLatin1Char(' '); break;
		case Separator::Tab: s << QLatin1Char('\t'); break;
		case Separator::Enter: break;  // 由 returnPressed 处理
		}
	}
	return s;
}

/**
 * @brief 构造分隔符正则表达式
 * @return 匹配任意分隔符字符的 QRegularExpression
 * @details 用于 split 拆分输入文本，正则字符集内特殊字符已转义
 */
QRegularExpression DAChipsEdit::separatorRegex() const
{
	QString pattern = QStringLiteral("[");
	for (const QChar &c : separatorChars()) {
		if (c == QLatin1Char(']') || c == QLatin1Char('\\') || c == QLatin1Char('^') || c == QLatin1Char('-'))
			pattern += QLatin1Char('\\');
		pattern += c;
	}
	pattern += QStringLiteral("]");
	return QRegularExpression(pattern);
}

/**
 * @brief 按分隔符拆分待粘贴文本
 * @param text 原始粘贴文本
 * @return 已 trim、已丢弃空串的 token 列表
 * @details 换行先归一为 \n；仅当分隔符集合含 Enter 时把 \n 当作分隔符，
 *          避免 DASelectComboBox（无分隔符）里粘贴一行文字被拆成多个候选过滤词。
 *          上界：分隔符集合为空时整段作为一个 token。
 */
QStringList DAChipsEdit::splitForPaste(const QString &text) const
{
	DA_DC(d);
	QString normalized = text;
	normalized.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
	normalized.replace(QLatin1Char('\r'), QLatin1Char('\n'));

	// 字符集来自 QSet，天然去重；空集合不能构造 "[]" 这种非法正则
	QString chars;
	for (const QChar &c : separatorChars())
		chars += c;
	if (d->separators.contains(Separator::Enter))
		chars += QLatin1Char('\n');
	if (chars.isEmpty()) {
		const QString whole = normalized.trimmed();
		return whole.isEmpty() ? QStringList() : QStringList { whole };
	}

	QString pattern = QStringLiteral("[");
	for (const QChar &c : chars) {
		if (c == QLatin1Char(']') || c == QLatin1Char('\\') || c == QLatin1Char('^')
			|| c == QLatin1Char('-') || c == QLatin1Char('['))
			pattern += QLatin1Char('\\');
		pattern += c;
	}
	pattern += QStringLiteral("]");

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
	const QStringList parts = normalized.split(QRegularExpression(pattern), Qt::SkipEmptyParts);
#else
	const QStringList parts = normalized.split(QRegularExpression(pattern), QString::SkipEmptyParts);
#endif
	QStringList out;
	out.reserve(parts.size());
	for (const QString &p : parts) {
		const QString s = p.trimmed();
		if (!s.isEmpty())
			out << s;
	}
	return out;
}

/**
 * @brief 把粘贴拆出的 token 逐个追加为标签
 * @param tokens splitForPaste() 的输出
 * @details 去重与 maxCount 仍由 appendTag() 判定；只有被 maxCount 挡住时才发
 *          pasteRejected("maxCount")，重复项按既有语义静默丢弃。
 */
void DAChipsEdit::applyPastedTokens(const QStringList &tokens)
{
	DA_D(d);
	bool hitMaxCount = false;
	for (const QString &t : tokens) {
		if (d->maxCount >= 0 && d->tags.size() >= d->maxCount) {
			hitMaxCount = true;
			break;
		}
		appendTag(t);
	}
	if (hitMaxCount)
		Q_EMIT pasteRejected(QStringLiteral("maxCount"));
}

/**
 * @brief 刷新右下角计数标签文本与位置
 * @details maxCount >= 0 显示 "n / max"，否则只显示 n
 */
void DAChipsEdit::updateCountLabel()
{
	DA_D(d);
	if (!d->countLabel)
		return;
	const int n = d->tags.size();
	d->countLabel->setText(d->maxCount >= 0
							   ? QStringLiteral("%1 / %2").arg(n).arg(d->maxCount)
							   : QString::number(n));
	positionCountLabel();
}

/**
 * @brief 把计数标签定位到右下角
 * @details 标签不进 DAFlowLayout，否则会抢走末尾输入框的伸展
 */
void DAChipsEdit::positionCountLabel()
{
	DA_D(d);
	if (!d->countLabel)
		return;
	const QSize hint = d->countLabel->sizeHint();
	d->countLabel->setGeometry(width() - 4 - hint.width(), height() - 4 - hint.height(),
							   hint.width(), hint.height());
}
}  // namespace DA

/*** End of inlined file: DAChipsEdit.cpp ***/


/*** Start of inlined file: DASelectComboBox.cpp ***/
#include <QApplication>
#include <QIcon>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPainter>
#include <QPointer>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QStyle>
#include <QStyleOptionComboBox>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

namespace DA
{
namespace {
constexpr int kGroupRole = Qt::UserRole + 1;

// 隐形占位 QLineEdit：QComboBox editable 模式需要一个 lineEdit，这里让它透明、
// 不绘制、不接收鼠标、不参与焦点，编辑区完全交给覆盖在上层的 DAChipsEdit。
class HiddenLineEdit : public QLineEdit
{
public:
	explicit HiddenLineEdit(QWidget *parent = nullptr) : QLineEdit(parent)
	{
		setFrame(false);
		setFocusPolicy(Qt::NoFocus);
		setAttribute(Qt::WA_TransparentForMouseEvents);
	}
	QSize sizeHint() const override { return QSize(0, 0); }
	QSize minimumSizeHint() const override { return QSize(0, 0); }
protected:
	void paintEvent(QPaintEvent *) override {}
};

// 空态面板：叠在弹层 viewport 上的覆盖控件（不插占位 item，popup->clear() 不会波及）
class EmptyPanel : public QWidget
{
public:
	explicit EmptyPanel(QWidget *parent = nullptr) : QWidget(parent)
	{
		setObjectName(QStringLiteral("DASelectEmptyPanel"));
		auto *lay = new QVBoxLayout(this);
		lay->setContentsMargins(8, 8, 8, 8);
		lay->setSpacing(8);
		lay->addStretch(1);

		m_text = new QLabel(this);
		m_text->setObjectName(QStringLiteral("DASelectEmptyText"));
		m_text->setAlignment(Qt::AlignCenter);
		m_text->setWordWrap(true);
		lay->addWidget(m_text);

		m_create = new QPushButton(this);
		m_create->setObjectName(QStringLiteral("DASelectCreateButton"));
		m_create->setCursor(Qt::PointingHandCursor);
		m_create->hide();
		lay->addWidget(m_create, 0, Qt::AlignHCenter);
		lay->addStretch(1);

		setStyleSheet(QStringLiteral(
			"QLabel#DASelectEmptyText { color:#909399; }"
			"QPushButton#DASelectCreateButton { color:#409eff; border:1px solid #d9ecff;"
			" border-radius:4px; padding:2px 10px; background:#ecf5ff; }"
			"QPushButton#DASelectCreateButton:hover { background:#d9ecff; }"));
	}

	void setText(const QString &text) { m_text->setText(text); }
	void setCreateText(const QString &text) { m_create->setText(text); }
	void setCreateVisible(bool on) { m_create->setVisible(on); }
	QPushButton *createButton() const { return m_create; }

private:
	QLabel *m_text { nullptr };
	QPushButton *m_create { nullptr };
};

QPixmap makeClearPixmap(const QColor &color)
{
	QPixmap pm(12, 12);
	pm.fill(Qt::transparent);
	QPainter p(&pm);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setPen(QPen(color, 1.6, Qt::SolidLine, Qt::RoundCap));
	p.drawLine(QPointF(3.5, 3.5), QPointF(8.5, 8.5));
	p.drawLine(QPointF(8.5, 3.5), QPointF(3.5, 8.5));
	p.end();
	return pm;
}

bool isGroupHeader(const QListWidgetItem *item)
{
	return item && item->data(kGroupRole).isValid();
}bool isActionable(const QListWidgetItem *item)
{
	return item && (item->flags() & Qt::ItemIsSelectable);
}

int nextActionableRow(QListWidget *popup, int from, int step)
{
	if (!popup || step == 0)
		return -1;
	int i = from;
	if (i < 0) {
		if (step < 0)
			return -1;
		i = -1;
	}
	for (i += step; i >= 0 && i < popup->count(); i += step) {
		if (isActionable(popup->item(i)))
			return i;
	}
	return -1;
}

int firstActionableRow(QListWidget *popup)
{
	if (!popup)
		return -1;
	for (int i = 0; i < popup->count(); ++i) {
		if (isActionable(popup->item(i)))
			return i;
	}
	return -1;
}
}  // namespace

class DASelectComboBox::PrivateData
{
public:
	PrivateData(DASelectComboBox *) {}
	DAChipsEdit *chips { nullptr };
	QWidget *container { nullptr };
	QScrollArea *scroll { nullptr };
	QListWidget *popup { nullptr };
	QStringList options;
	QVector< DASelectGroup > groups;
	bool allowCreate { true };
	bool filterable { true };
	HeightMode heightMode { HeightMode::GrowWithTags };
	int fixedRowCount { 1 };
	bool popupVisible { false };

	bool emptyStateEnabled { false };
	QString noMatchText;
	EmptyPanel *emptyPanel { nullptr };
	QPointer< QWidget > emptyWidget;

	bool clearable { false };
	bool collapseTags { false };
	int collapseTagCount { 2 };
	QToolButton *clearButton { nullptr };
	QLabel *collapseLabel { nullptr };
	bool hovered { false };
};

/**
 * @brief 构造多标签选择下拉框
 * @param parent 父控件
 * @details 创建 DAChipsEdit 编辑区、隐形占位 QLineEdit、QListWidget 候选弹出框，
 *          安装全局事件过滤器与默认 qss 样式。编辑区组件必须在 setEditable 之前
 *          就绪，否则 sizeHint 协商会访问空指针。
 */
DASelectComboBox::DASelectComboBox(QWidget *parent) : QComboBox(parent), DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	ensureResourcesInitialized();  // 静态库需显式注册 qrc 资源,否则被链接器丢弃
	setObjectName(QStringLiteral("DASelectComboBox"));

	// 先创建 chips 和 container,sizeHint() 会访问它们,必须在 setEditable 之前就绪
	d->chips = new DAChipsEdit;
	d->chips->setFrameShape(QFrame::NoFrame);
	d->chips->setAllowCreate(false);
	d->chips->setSeparators(QList<DAChipsEdit::Separator>{});

	d->container = new QWidget(this);
	auto *clay = new QVBoxLayout(d->container);
	clay->setContentsMargins(0, 0, 0, 0);
	clay->setSpacing(0);

	applyHeightMode();  // 初始 GrowWithTags：把 m_chips 放进 container

	// setEditable 会触发 adjustSize → sizeHint,此时 chips/container 必须已就绪
	setEditable(true);
	setLineEdit(new HiddenLineEdit(this));  // 占位，实际编辑由 DAChipsEdit 接管

	d->popup = new QListWidget;
	d->popup->setObjectName(QStringLiteral("DASelectPopup"));
	// 用 ToolTip 而非 Popup：Popup 在 QComboBox editable 模式下鼠标 grab 会失效，
	// 既不自动关闭也拦截外部事件。ToolTip 不抢抓取、不抢焦点，关闭完全由
	// eventFilter 手动管理，行为可控且可靠。
	d->popup->setWindowFlags(Qt::ToolTip);
	d->popup->setFocusPolicy(Qt::NoFocus);
	d->popup->setEditTriggers(QAbstractItemView::NoEditTriggers);
	d->popup->setSelectionMode(QAbstractItemView::SingleSelection);
	d->popup->setMouseTracking(true);
	d->popup->setMinimumHeight(40);
	d->popup->hide();

	// 空态面板是弹层 viewport 上的覆盖控件:popup->clear() 不会销毁它
	d->emptyPanel = new EmptyPanel(d->popup);
	d->emptyPanel->hide();
	d->noMatchText = tr("No matching items");  //cn:没有匹配项
	connect(d->emptyPanel->createButton(), &QPushButton::clicked, this, [this] { activateByInput(); });

	connect(d->chips, &DAChipsEdit::inputTextChanged, this, &DASelectComboBox::onInputTextChanged);
	connect(d->chips, &DAChipsEdit::inputFocusChanged, this, &DASelectComboBox::onInputFocusChanged);
	connect(d->chips, &DAChipsEdit::inputKeyEvent, this, &DASelectComboBox::onInputKey);
	connect(d->chips, &DAChipsEdit::tagsChanged, this, [this](const QStringList &) {
		DA_D(d);
		if (d->heightMode == HeightMode::GrowWithTags)
			updateGeometry();
	});
	connect(d->chips, &DAChipsEdit::tagsChanged, this, &DASelectComboBox::tagsChanged);
	connect(d->chips, &DAChipsEdit::tagsChanged, this, [this] { updateEditOverlays(); });
	connect(d->popup, &QListWidget::itemClicked, this, &DASelectComboBox::onPopupItemClicked);

	// 监听全局鼠标按下：popup 可见时点击外部（非 combobox、非 popup 自身）应关闭。
	// Qt::ToolTip 不抢 mouse grab，外部点击由本过滤器兜底关闭。
	qApp->installEventFilter(this);

	// 默认现代化样式：圆角边框 + 无边框 drop-down + qrc 内置 SVG 箭头。
	// 用户可随后自行 setStyleSheet 覆盖。用本对象名作用域，避免污染其它 QComboBox。
	applyDefaultStyle();
}

/**
 * @brief 析构
 * @details 弹层是无父顶层窗口，必须显式删除；PIMPL 指针由宏自动释放
 */
DASelectComboBox::~DASelectComboBox()
{
	DA_D(d);
	// 先断开子控件的信号与全局过滤器：~QWidget 删除子控件时会触发焦点变化，
	// 此时本对象的 d_ptr 已释放，槽里再访问私有数据就是 use-after-free
	qApp->removeEventFilter(this);
	if (d->chips)
		d->chips->disconnect(this);
	if (d->popup) {
		d->popup->hide();
		delete d->popup;
		d->popup = nullptr;
	}
}

/**
 * @brief 应用默认现代化样式
 * @details qss 直接引用 qrc 中的 SVG 箭头资源（:/DAWidgetsIcons/arrow-down.svg）。
 *          静态库通过 ensureResourcesInitialized() 显式注册资源,确保被链接器保留。
 */
void DASelectComboBox::applyDefaultStyle()
{
	static const char *kDefaultQss = R"(
DA--DASelectComboBox {
	background:#ffffff;
	border:1px solid #dcdfe6;
	border-radius:4px;
	padding:1px 24px 1px 6px;
	min-height:22px;
}
DA--DASelectComboBox:on { border:1px solid #409eff; }
DA--DASelectComboBox:focus { border:1px solid #409eff; }
DA--DASelectComboBox::drop-down {
	subcontrol-origin:padding;
	subcontrol-position:top right;
	width:20px;
	border:none;
	border-left:1px solid #dcdfe6;
	border-top-right-radius:4px;
	border-bottom-right-radius:4px;
	background:transparent;
}
DA--DASelectComboBox::drop-down:hover { background:#f5f7fa; }
DA--DASelectComboBox::down-arrow {
	image:url(:/DAWidgetsIcons/arrow-down.svg);
	width:10px;
	height:6px;
}
DA--DAChipsEdit { background:transparent; border:none; }
)";
	setStyleSheet(QString::fromLatin1(kDefaultQss));
}

/**
 * @brief 获取候选选项列表
 * @return 当前候选选项的字符串列表
 */
QStringList DASelectComboBox::options() const
{
	DA_DC(d);
	return d->options;
}

/**
 * @brief 设置候选选项列表
 * @param options 候选选项字符串列表
 * @details 替换当前所有候选选项，若弹出框可见则立即刷新过滤结果
 */
void DASelectComboBox::setOptions(const QStringList &options)
{
	DA_D(d);
	d->groups.clear();
	d->options = options;
	if (d->popupVisible)
		refreshPopup(d->chips->inputText());
}

/**
 * @brief 设置分组候选
 * @param groups 分组列表，title 为空串表示无标题组
 * @details 同时把组内候选按「组顺序 + 组内顺序」扁平化到 options()，弹层可见时立即刷新
 */
void DASelectComboBox::setGroupedOptions(const QVector< DASelectGroup > &groups)
{
	DA_D(d);
	d->groups = groups;
	d->options.clear();
	for (const DASelectGroup &g : groups)
		d->options.append(g.options);
	if (d->popupVisible)
		refreshPopup(d->chips->inputText());
}

/**
 * @brief 获取分组候选
 * @return 分组列表副本
 */
QVector< DASelectGroup > DASelectComboBox::groupedOptions() const
{
	DA_DC(d);
	return d->groups;
}

/**
 * @brief 获取已选标签列表
 * @return 当前已选标签的字符串列表
 */
QStringList DASelectComboBox::tags() const
{
	DA_DC(d);
	return d->chips->tags();
}

/**
 * @brief 设置已选标签列表
 * @param tags 标签字符串列表
 * @details 委托给 DAChipsEdit 替换当前所有标签
 */
void DASelectComboBox::setTags(const QStringList &tags)
{
	DA_D(d);
	d->chips->setTags(tags);
}

/**
 * @brief 清空所有已选标签与输入文本
 * @details 不修改候选选项；清空后关闭弹层（保持旧行为）
 */
void DASelectComboBox::clear()
{
	DA_D(d);
	d->chips->clear();
	d->chips->clearInput();
	hideCustomPopup();
	updateEditOverlays();
}

/**
 * @brief 是否允许在无匹配时创建新标签
 * @return 允许创建返回 true
 */
bool DASelectComboBox::allowCreate() const
{
	DA_DC(d);
	return d->allowCreate;
}

/**
 * @brief 设置是否允许创建新标签
 * @param on 是否允许
 */
void DASelectComboBox::setAllowCreate(bool on)
{
	DA_D(d);
	d->allowCreate = on;
}

/**
 * @brief 下拉列表是否支持输入过滤
 * @return 支持过滤返回 true
 */
bool DASelectComboBox::filterable() const
{
	DA_DC(d);
	return d->filterable;
}

/**
 * @brief 设置下拉列表是否支持输入过滤
 * @param on 是否支持过滤
 */
void DASelectComboBox::setFilterable(bool on)
{
	DA_D(d);
	d->filterable = on;
}

/**
 * @brief 获取输入框占位提示文本
 * @return 占位提示文本
 */
QString DASelectComboBox::placeholder() const
{
	DA_DC(d);
	return d->chips->placeholder();
}

/**
 * @brief 设置输入框占位提示文本
 * @param text 占位提示文本
 */
void DASelectComboBox::setPlaceholder(const QString &text)
{
	DA_D(d);
	d->chips->setPlaceholder(text);
}

/**
 * @brief 获取当前高度策略
 * @return 高度策略枚举值
 */
DASelectComboBox::HeightMode DASelectComboBox::heightMode() const
{
	DA_DC(d);
	return d->heightMode;
}

/**
 * @brief 设置高度策略
 * @param mode 高度策略
 * @details 若策略变化则重新组装编辑区容器布局（GrowWithTags 直接放入，
 *          FixedHeightScrollable 包入 QScrollArea）
 */
void DASelectComboBox::setHeightMode(HeightMode mode)
{
	DA_D(d);
	if (d->heightMode == mode)
		return;
	d->heightMode = mode;
	applyHeightMode();
}

/**
 * @brief 获取固定高度模式下的显示行数
 * @return 显示行数
 */
int DASelectComboBox::fixedRowCount() const
{
	DA_DC(d);
	return d->fixedRowCount;
}

/**
 * @brief 设置固定高度模式下的显示行数
 * @param rows 显示行数，最小为 1
 * @details 若当前处于固定高度模式则触发 updateGeometry 重新协商尺寸
 */
void DASelectComboBox::setFixedRowCount(int rows)
{
	DA_D(d);
	d->fixedRowCount = qMax(1, rows);
	if (d->heightMode == HeightMode::FixedHeightScrollable)
		updateGeometry();
}

/**
 * @brief 打开/关闭空态面板
 * @param on 是否启用（默认 false）
 * @details 关闭时保持旧行为：无匹配立即关闭弹层
 */
void DASelectComboBox::setEmptyStateEnabled(bool on)
{
	DA_D(d);
	if (d->emptyStateEnabled == on)
		return;
	d->emptyStateEnabled = on;
	if (d->popupVisible) {
		refreshPopup(d->chips->inputText());
		if (!on && firstActionableRow(d->popup) < 0)
			hideCustomPopup();
	}
	updateEmptyOverlay();
}

/** @brief 空态面板是否启用 */
bool DASelectComboBox::isEmptyStateEnabled() const
{
	DA_DC(d);
	return d->emptyStateEnabled;
}

/**
 * @brief 设置空态主文案
 * @param text 文案
 */
void DASelectComboBox::setNoMatchText(const QString &text)
{
	DA_D(d);
	d->noMatchText = text;
	updateEmptyOverlay();
}

/** @brief 获取空态主文案 */
QString DASelectComboBox::noMatchText() const
{
	DA_DC(d);
	return d->noMatchText;
}

/**
 * @brief 设置自定义空态控件
 * @param w 控件（接管所有权，随弹层析构）；nullptr 还原内置面板
 */
void DASelectComboBox::setEmptyWidget(QWidget *w)
{
	DA_D(d);
	if (d->emptyWidget == w)
		return;
	if (d->emptyWidget) {
		d->emptyWidget->hide();
		d->emptyWidget->deleteLater();
	}
	d->emptyWidget = w;
	if (w) {
		w->setParent(d->popup);
		w->hide();
	}
	updateEmptyOverlay();
}

/** @brief 获取自定义空态控件 */
QWidget *DASelectComboBox::emptyWidget() const
{
	DA_DC(d);
	return d->emptyWidget.data();
}

/**
 * @brief 打开/关闭编辑区清空按钮
 * @param on 是否启用（默认 false）
 * @details 关闭时销毁按钮子控件，保持默认视觉与旧行为一致
 */
void DASelectComboBox::setClearable(bool on)
{
	DA_D(d);
	if (d->clearable == on)
		return;
	d->clearable = on;
	if (on && !d->clearButton) {
		d->clearButton = new QToolButton(this);
		d->clearButton->setObjectName(QStringLiteral("DASelectClearButton"));
		// 用绘制的图标而不是文本 ×：不依赖平台字体，跨平台像素断言才稳定
		QIcon icon;
		icon.addPixmap(makeClearPixmap(QColor(0xC0, 0xC4, 0xCC)), QIcon::Normal);
		icon.addPixmap(makeClearPixmap(QColor(0x90, 0x93, 0x99)), QIcon::Active);
		d->clearButton->setIcon(icon);
		d->clearButton->setIconSize(QSize(10, 10));
		d->clearButton->setAccessibleName(tr("Clear all"));  //cn:清空全部
		d->clearButton->setAutoRaise(true);
		d->clearButton->setFocusPolicy(Qt::NoFocus);
		d->clearButton->setCursor(Qt::ArrowCursor);
		d->clearButton->setFixedSize(20, 20);
		d->clearButton->setStyleSheet(QStringLiteral(
			"QToolButton#DASelectClearButton { border:none; background:transparent; padding:0; }"));
		d->clearButton->hide();
		connect(d->clearButton, &QToolButton::clicked, this, [this] {
			DA_D(d);
			d->chips->clear();
			d->chips->clearInput();
			if (d->popupVisible)
				refreshPopup(QString());
			updateEditOverlays();
			Q_EMIT clearClicked();
		});
	}
	if (!on && d->clearButton) {
		d->clearButton->hide();
		d->clearButton->deleteLater();
		d->clearButton = nullptr;
	}
	updateEditOverlays();
}

/** @brief 是否启用清空按钮 */
bool DASelectComboBox::isClearable() const
{
	DA_DC(d);
	return d->clearable;
}

/**
 * @brief 打开/关闭已选标签折叠
 * @param on 是否启用（默认 false）
 */
void DASelectComboBox::setCollapseTags(bool on)
{
	DA_D(d);
	if (d->collapseTags == on)
		return;
	d->collapseTags = on;
	if (on && !d->collapseLabel) {
		d->collapseLabel = new QLabel(this);
		d->collapseLabel->setObjectName(QStringLiteral("DASelectTagSummary"));
		d->collapseLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
		d->collapseLabel->setStyleSheet(QStringLiteral(
			"QLabel#DASelectTagSummary { color:#909399; background:#f4f4f5; border-radius:8px;"
			" padding:1px 6px; }"));
		d->collapseLabel->hide();
	}
	if (!on && d->collapseLabel) {
		d->collapseLabel->hide();
		d->collapseLabel->deleteLater();
		d->collapseLabel = nullptr;
	}
	updateEditOverlays();
}

/** @brief 是否启用已选标签折叠 */
bool DASelectComboBox::collapseTags() const
{
	DA_DC(d);
	return d->collapseTags;
}

/**
 * @brief 设置折叠后保留的标签数
 * @param count 保留数，小于 1 时按 1 处理
 */
void DASelectComboBox::setCollapseTagCount(int count)
{
	DA_D(d);
	d->collapseTagCount = qMax(1, count);
	updateEditOverlays();
}

/** @brief 折叠后保留的标签数 */
int DASelectComboBox::collapseTagCount() const
{
	DA_DC(d);
	return d->collapseTagCount;
}

/**
 * @brief 计算推荐尺寸
 * @return 推荐的 QSize
 * @details GrowWithTags 模式下用 layout()->heightForWidth 计算换行高度，
 *          绕过 QWidget::heightForWidth 的 isVisible 检查以保证协商阶段正确；
 *          FixedHeightScrollable 模式下用 rowHeightHint * fixedRowCount。
 *          宽度取所有候选选项的最大文本宽度与 DAChipsEdit 宽度的较大值。
 */
QSize DASelectComboBox::sizeHint() const
{
	DA_DC(d);
	QStyleOptionComboBox opt;
	initStyleOption(&opt);
	const int fw = style()->pixelMetric(QStyle::PM_ComboBoxFrameWidth, &opt, this);
	const QRect arrow = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this);

	int h;
	if (d->heightMode == HeightMode::GrowWithTags) {
		// 用当前宽度算 DAChipsEdit 换行后的高度（FlowLayout 支持 heightForWidth）。
		// 直接调 layout()->heightForWidth 绕过 QWidget::heightForWidth 的 isVisible 检查，
		// 保证 sizeHint 协商阶段（widget 可能尚未可见）也能得到正确换行高度。
		int cw = width();
		if (cw <= 0)
			cw = 200;
		const int chipsW = qMax(cw - fw * 2 - arrow.width(), d->chips->minimumSizeHint().width());
		h = d->chips->layout() ? d->chips->layout()->heightForWidth(chipsW) : -1;
		if (h < 0)
			h = d->chips->sizeHint().height();
	} else {
		h = d->chips->rowHeightHint() * qMax(1, d->fixedRowCount);
	}
	h += fw * 2;

	int w = d->chips->sizeHint().width();
	const QFontMetrics fm = fontMetrics();
	for (const QString &o : d->options) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
		w = qMax(w, fm.horizontalAdvance(o));
#else
		w = qMax(w, fm.width(o));
#endif
	}
	w += fw * 2 + arrow.width();
	return QSize(w, h);
}

/**
 * @brief 计算最小推荐尺寸
 * @return 最小 QSize
 * @details 高度基于单行 rowHeightHint，FixedHeightScrollable 模式乘以行数；
 *          宽度保证箭头按钮可显示
 */
QSize DASelectComboBox::minimumSizeHint() const
{
	DA_DC(d);
	QStyleOptionComboBox opt;
	initStyleOption(&opt);
	const int fw = style()->pixelMetric(QStyle::PM_ComboBoxFrameWidth, &opt, this);
	int h = d->chips->rowHeightHint();
	if (d->heightMode == HeightMode::FixedHeightScrollable)
		h *= qMax(1, d->fixedRowCount);
	h += fw * 2;
	const QRect arrow = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this);
	return QSize(arrow.width() + 40, h);
}

/**
 * @brief 处理尺寸变化事件
 * @param event 尺寸变化事件
 * @details 将编辑区容器对齐到 QComboBox 的 EditField subcontrol 区域，
 *          保证 DAChipsEdit 覆盖原生编辑区
 */
void DASelectComboBox::resizeEvent(QResizeEvent *event)
{
	QComboBox::resizeEvent(event);
	updateOverlayGeometry();
}

/**
 * @brief 弹出/收起下拉候选列表
 * @details 不调用 QComboBox::showPopup，改用自己的 QListWidget 弹出以支持过滤
 */
void DASelectComboBox::showPopup()
{
	// 不调用 QComboBox::showPopup：用自己的 QListWidget 弹出，支持过滤
	DA_D(d);
	if (d->popupVisible)
		hideCustomPopup();
	else
		showCustomPopup();
}

/**
 * @brief 全局事件过滤器
 * @param watched 被监听的对象
 * @param event 事件
 * @return 是否拦截事件
 * @details popup 可见时，外部鼠标按下关闭下拉（点 popup 内部与自身覆盖层除外），
 *          Tab/Backtab 焦点切换也关闭下拉；Enter/Leave 维护清空按钮的悬停显隐
 */
bool DASelectComboBox::eventFilter(QObject *watched, QEvent *event)
{
	DA_D(d);
	if (event->type() == QEvent::Enter) {
		if (watched == this || (d->clearButton && watched == d->clearButton)) {
			d->hovered = true;
			updateEditOverlays();
		}
	} else if (event->type() == QEvent::Leave) {
		if (watched == this || (d->clearButton && watched == d->clearButton)) {
			d->hovered = false;
			updateEditOverlays();
		}
	}
	if (d->popupVisible) {
		switch (event->type()) {
		case QEvent::MouseButtonPress: {
			// 任何鼠标按下都关闭，除非点在 popup 内部（由 itemClicked 处理选中）。
			// × 与 +N 是本控件的覆盖层，点它们时不能把弹层关掉（否则“清空后保持打开”做不到）。
			auto *w = qobject_cast< QWidget * >(watched);
			const bool ownOverlay = (w && (w == d->clearButton || w == d->collapseLabel));
			if (w && w != d->popup && !d->popup->isAncestorOf(w) && !ownOverlay)
				hideCustomPopup();
			break;
		}
		case QEvent::KeyPress: {
			// popup 可见时，非输入框来源的 Escape/方向键/回车由 onInputKey 处理；
			// 此处兜底：任何 Tab/焦点切换键关闭下拉。
			auto *ke = static_cast< QKeyEvent * >(event);
			if (ke->key() == Qt::Key_Tab || ke->key() == Qt::Key_Backtab)
				hideCustomPopup();
			break;
		}
		default:
			break;
		}
	}
	return QComboBox::eventFilter(watched, event);
}

/**
 * @brief 输入框文本变化时刷新候选列表
 * @param text 当前输入文本
 * @details 可过滤时根据文本显示/隐藏弹出框并刷新过滤结果
 */
void DASelectComboBox::onInputTextChanged(const QString &text)
{
	DA_D(d);
	if (!d->filterable)
		return;
	if (text.isEmpty()) {
		if (d->popupVisible)
			refreshPopup(QString());
	} else {
		showCustomPopup();
		refreshPopup(text);
	}
}

/**
 * @brief 输入框焦点变化处理
 * @param focused 是否获得焦点
 * @details 获焦时若可过滤且有候选选项则弹出下拉；失焦时不主动关闭，
 *          避免与 popup itemClicked 的点击事件竞争导致误关
 */
void DASelectComboBox::onInputFocusChanged(bool focused)
{
	// 控件析构时子控件删除会引发焦点变化，此时 d_ptr 可能已释放
	if (!d_func())
		return;
	DA_D(d);
	if (focused) {
		if (d->filterable && !d->options.isEmpty())
			showCustomPopup();
	}
	// 失焦时不主动关闭：Qt::Popup 在外部点击时自动关闭，选中/Esc 会显式关闭，
	// 避免与 popup itemClicked 的点击事件竞争导致误关。
	updateEditOverlays();
}

/**
 * @brief 输入框按键事件拦截
 * @param event 按键事件
 * @param handled 输出参数，置 true 阻止 DAChipsEdit 默认处理
 * @details 回车/Enter 选中当前候选项或创建新标签；方向键导航候选列表；
 *          Escape 关闭下拉并清空输入
 */
void DASelectComboBox::onInputKey(QKeyEvent *event, bool *handled)
{
	DA_D(d);
	const int key = event->key();
	if (key == Qt::Key_Return || key == Qt::Key_Enter) {
		QListWidgetItem *current = d->popup->currentItem();
		if (d->popupVisible && current && isActionable(current)) {
			pickCurrent();
			*handled = true;
		} else if (d->allowCreate && !d->chips->inputText().trimmed().isEmpty()) {
			activateByInput();
			*handled = true;
		}
		return;
	}
	if (!d->popupVisible)
		return;
	if (key == Qt::Key_Down || key == Qt::Key_Up) {
		const int step = (key == Qt::Key_Down) ? 1 : -1;
		const int row  = nextActionableRow(d->popup, d->popup->currentRow(), step);
		if (row >= 0)
			d->popup->setCurrentRow(row);
		*handled = true;
	} else if (key == Qt::Key_Escape) {
		hideCustomPopup();
		d->chips->clearInput();
		*handled = true;
	}
}

/**
 * @brief 点击弹出列表项时选中该项
 * @param item 被点击的列表项
 * @details 组标题不可选；候选追加为标签并发射 optionSelected
 */
void DASelectComboBox::onPopupItemClicked(QListWidgetItem *item)
{
	DA_D(d);
	if (!isActionable(item))
		return;
	d->chips->appendTag(item->text());
	Q_EMIT optionSelected(item->text());
	d->chips->clearInput();
	if (d->popupVisible)
		refreshPopup(QString());
}

/**
 * @brief 应用高度策略，组装编辑区容器布局
 * @details GrowWithTags 模式将 DAChipsEdit 直接放入容器；
 *          FixedHeightScrollable 模式将其包入 QScrollArea 以支持滚动
 */
void DASelectComboBox::applyHeightMode()
{
	DA_D(d);
	auto *lay = static_cast< QVBoxLayout * >(d->container->layout());
	if (d->heightMode == HeightMode::GrowWithTags) {
		if (d->scroll) {
			d->scroll->takeWidget();  // 归还 m_chips 所有权
			lay->removeWidget(d->scroll);
			d->scroll->deleteLater();
			d->scroll = nullptr;
		}
		d->chips->setParent(d->container);
		lay->addWidget(d->chips);
	} else {
		lay->removeWidget(d->chips);
		d->scroll = new QScrollArea(d->container);
		d->scroll->setFrameShape(QFrame::NoFrame);
		d->scroll->setWidgetResizable(true);
		d->scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		d->scroll->setFocusPolicy(Qt::NoFocus);
		d->scroll->setWidget(d->chips);
		lay->addWidget(d->scroll);
	}
	updateGeometry();
}

/**
 * @brief 显示自定义候选弹出框
 * @details 刷新候选列表后定位到 combobox 正下方；default-first-option 取首个可选行；
 *          空态开启时即使没有候选也保持弹层并显示覆盖面板
 */
void DASelectComboBox::showCustomPopup()
{
	DA_D(d);
	if (d->popupVisible)
		return;
	refreshPopup(d->chips->inputText());
	if (d->popup->count() == 0 && !d->emptyStateEnabled)
		return;
	d->popup->setFixedWidth(width());
	int h = 0;
	if (d->popup->count() == 0) {
		// 空态：按面板/自定义控件的高度给一个不低于最小高度的弹层
		QWidget *w = d->emptyWidget ? d->emptyWidget.data() : d->emptyPanel;
		h = qMax(w ? w->sizeHint().height() + 6 : 0, 40);
	} else {
		int contentH = 0;
		for (int i = 0; i < d->popup->count(); ++i)
			contentH += d->popup->sizeHintForRow(i);
		h = qMin(contentH + 6, 220);
	}
	d->popup->setFixedHeight(qMax(h, 40));
	d->popup->move(mapToGlobal(QPoint(0, height())));
	d->popup->show();
	d->popup->raise();
	const int first = firstActionableRow(d->popup);
	if (first >= 0)
		d->popup->setCurrentRow(first);  // default-first-option
	d->popupVisible = true;
	updateEmptyOverlay();
	updateEditOverlays();
}

/** @brief 隐藏自定义候选弹出框并收起空态面板 */
void DASelectComboBox::hideCustomPopup()
{
	DA_D(d);
	if (!d->popupVisible)
		return;
	d->popup->hide();
	d->popupVisible = false;
	updateEmptyOverlay();
	updateEditOverlays();
}

/**
 * @brief 刷新弹出框候选列表
 * @param filter 过滤文本，空字符串表示显示全部
 * @details 无分组时保持原扁平路径；有分组时插入不可选组标题，
 *          组内无命中则整组隐藏。无匹配且未开空态时关闭弹层。
 */
void DASelectComboBox::refreshPopup(const QString &filter)
{
	DA_D(d);
	d->popup->clear();
	const QStringList selected = d->chips->tags();
	if (d->groups.isEmpty()) {
		// 默认路径：与加入分组前逐项一致
		for (const QString &opt : d->options) {
			if (selected.contains(opt))
				continue;
			if (filter.isEmpty() || opt.contains(filter, Qt::CaseInsensitive))
				d->popup->addItem(opt);
		}
	} else {
		for (int gi = 0; gi < d->groups.size(); ++gi) {
			const DASelectGroup &group = d->groups.at(gi);
			QStringList hits;
			for (const QString &opt : group.options) {
				if (selected.contains(opt))
					continue;
				if (filter.isEmpty() || opt.contains(filter, Qt::CaseInsensitive))
					hits << opt;
			}
			if (hits.isEmpty())
				continue;
			if (!group.title.isEmpty()) {
				auto *header = new QListWidgetItem(group.title, d->popup);
				header->setFlags(header->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEnabled);
				header->setData(kGroupRole, gi);
				header->setSizeHint(QSize(0, 22));
			}
			for (const QString &opt : hits)
				d->popup->addItem(opt);
		}
	}

	const int first = firstActionableRow(d->popup);
	if (first < 0) {
		if (d->popupVisible && !d->emptyStateEnabled) {
			hideCustomPopup();
			return;
		}
	} else {
		d->popup->setCurrentRow(first);
	}
	updateEmptyOverlay();
}

/**
 * @brief 选中当前高亮的候选项
 * @details 组标题不可选；候选追加为标签并发射 optionSelected
 */
void DASelectComboBox::pickCurrent()
{
	DA_D(d);
	QListWidgetItem *item = d->popup->currentItem();
	if (!isActionable(item))
		return;
	d->chips->appendTag(item->text());
	Q_EMIT optionSelected(item->text());
	d->chips->clearInput();
	if (d->popupVisible)
		refreshPopup(QString());
}

/** @brief 回车与空态「创建」按钮共用的创建入口 */
void DASelectComboBox::activateByInput()
{
	DA_D(d);
	const QString text = d->chips->inputText().trimmed();
	if (text.isEmpty())
		return;
	d->chips->appendTag(text);
	Q_EMIT created(text);
	d->chips->clearInput();
	if (d->popupVisible)
		refreshPopup(QString());
}

/**
 * @brief 刷新空态覆盖层
 * @details 只在「开关打开 + 弹层可见 + 没有任何可选候选行」时显示；
 *          面板不是 item，popup->clear() 不会销毁它
 */
void DASelectComboBox::updateEmptyOverlay()
{
	DA_D(d);
	if (!d->emptyPanel)
		return;
	const bool show = d->emptyStateEnabled && d->popupVisible && firstActionableRow(d->popup) < 0;
	if (!show) {
		d->emptyPanel->hide();
		if (d->emptyWidget)
			d->emptyWidget->hide();
		return;
	}
	QWidget *w = d->emptyWidget ? d->emptyWidget.data() : d->emptyPanel;
	if (w == d->emptyPanel) {
		d->emptyPanel->setText(d->noMatchText);
		const QString input  = d->chips->inputText().trimmed();
		const bool canCreate = d->allowCreate && !input.isEmpty();
		d->emptyPanel->setCreateVisible(canCreate);
		if (canCreate)
			d->emptyPanel->setCreateText(tr("Create \"%1\"").arg(input));  //cn:创建“%1”
	}
	const QRect viewportRect = d->popup->viewport()->geometry();
	w->setGeometry(viewportRect);
	w->show();
	w->raise();
	if (w != d->emptyPanel)
		d->emptyPanel->hide();
}

/**
 * @brief 刷新 × 与 +N 的几何
 * @details 唯一度量来源是 style()->subControlRect(SC_ComboBoxEditField)
 */
void DASelectComboBox::updateOverlayGeometry()
{
	if (!d_func())
		return;
	DA_D(d);
	if (!d->container)
		return;
	QStyleOptionComboBox opt;
	initStyleOption(&opt);
	const QRect field = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);

	QRect containerRect = field;
	if (d->clearable && !tags().isEmpty())
		containerRect.adjust(0, 0, -24, 0);
	d->container->setGeometry(containerRect);

	const int rowH = d->chips->rowHeightHint();
	if (d->clearButton) {
		const int b = 20;
		const int y = field.top() + qMax(0, (rowH - b) / 2);
		d->clearButton->setGeometry(field.right() - b + 1, y, b, b);
	}
	if (d->collapseLabel) {
		const QSize hint = d->collapseLabel->sizeHint();
		int right        = field.right();
		if (d->clearButton && d->clearButton->isVisible())
			right = d->clearButton->geometry().left() - 4;
		const int y = field.top() + qMax(0, (rowH - hint.height()) / 2);
		d->collapseLabel->setGeometry(right - hint.width(), y, hint.width(), hint.height());
	}
}

/**
 * @brief 按折叠状态隐藏/显示 chip 与 +N 摘要
 * @details 只切可见性，不改写 tags()；聚焦或弹层展开时恢复全部
 */
void DASelectComboBox::updateTagCollapse()
{
	DA_D(d);
	QList< DAChip * > chips;
	if (auto *lay = d->chips->layout()) {
		for (int i = 0; i < lay->count(); ++i) {
			if (auto *chip = qobject_cast< DAChip * >(lay->itemAt(i)->widget()))
				chips << chip;
		}
	}
	const int n     = d->chips->tags().size();
	const int keep  = qMax(1, d->collapseTagCount);
	const bool collapse = d->collapseTags && n > keep;
	QWidget *focus      = QApplication::focusWidget();
	const bool focused  = d->chips->hasFocus() || (focus && d->chips->isAncestorOf(focus));
	const bool showAll  = !collapse || focused || d->popupVisible;
	for (int i = 0; i < chips.size(); ++i)
		chips[i]->setVisible(showAll || i < keep);
	if (d->collapseLabel) {
		d->collapseLabel->setText(QStringLiteral("+%1").arg(n - keep));
		d->collapseLabel->setVisible(collapse && !showAll);
	}
	if (auto *lay = d->chips->layout())
		lay->invalidate();
	d->chips->updateGeometry();
	updateGeometry();
}

/** @brief 综合刷新悬停态、× 与折叠摘要 */
void DASelectComboBox::updateEditOverlays()
{
	if (!d_func())
		return;
	DA_D(d);
	if (d->clearButton) {
		const bool show = d->clearable && d->hovered && !tags().isEmpty() && isEnabled();
		d->clearButton->setVisible(show);
	}
	updateTagCollapse();
	updateOverlayGeometry();
}
}  // namespace DA

/*** End of inlined file: DASelectComboBox.cpp ***/

// ---- Collapse 折叠面板家族 ----

/*** Start of inlined file: DACollapseLayout.cpp ***/
#include <QWidget>
#include <QSpacerItem>
#include <QWidgetItem>

namespace DA
{
/**
 * @brief 构造布局器
 * @param parent 宿主控件,可为 nullptr
 */
DACollapseLayout::DACollapseLayout(QWidget* parent) : QLayout(parent), m_dirty(true)
{
}

/** @brief 析构,释放所有 layout item */
DACollapseLayout::~DACollapseLayout()
{
	while (auto* item = takeAt(0))
		delete item;
}

/**
 * @brief 添加 layout item(末尾追加)
 * @param item 待添加项
 */
void DACollapseLayout::addItem(QLayoutItem* item)
{
	m_items.append(item);
	invalidate();
}

/** @brief 返回子项数量 */
int DACollapseLayout::count() const
{
	return m_items.size();
}

/**
 * @brief 获取指定索引的 layout item
 * @param index 索引
 * @return 索引有效时返回 item,否则返回 nullptr
 */
QLayoutItem* DACollapseLayout::itemAt(int index) const
{
	return (index >= 0 && index < m_items.size()) ? m_items.at(index) : nullptr;
}

/**
 * @brief 移除并返回指定索引的 layout item
 * @param index 索引
 * @return 索引有效时返回 item(调用方负责删除),否则返回 nullptr
 */
QLayoutItem* DACollapseLayout::takeAt(int index)
{
	if (index < 0 || index >= m_items.size())
		return nullptr;
	auto* item = m_items.takeAt(index);
	invalidate();
	return item;
}

/**
 * @brief 重新计算缓存的 sizeHint 和 minimumSize
 * @details 遍历所有 item,高度累加,宽度取最大值。
 *          SpacerItem(末尾 stretch)的高度按 sizeHint 累加,通常为 0。
 */
void DACollapseLayout::recalc() const
{
	if (!m_dirty)
		return;
	const int spacing = this->spacing();
	int h   = 0;
	int w   = 0;
	int minH = 0;
	for (int i = 0; i < m_items.size(); ++i) {
		if (i > 0) {
			h += spacing;
			minH += spacing;
		}
		const auto* item = m_items.at(i);
		h += item->sizeHint().height();
		w = qMax(w, item->sizeHint().width());
		minH += item->minimumSize().height();
		w = qMax(w, item->minimumSize().width());
	}
	auto margins   = contentsMargins();
	h += margins.top() + margins.bottom();
	w += margins.left() + margins.right();
	minH += margins.top() + margins.bottom();
	w = qMax(w, margins.left() + margins.right());
	m_sizeHint = QSize(w, h);
	m_minSize  = QSize(w, minH);
	m_dirty    = false;
}

/**
 * @brief 布局核心:从上到下依次给每个 item 分配 sizeHint 高度
 * @param rect 可用矩形区域
 * @details 不调用 qGeomCalc(),不做比例分配或压缩。
 *          每个 item 永远获得 sizeHint 高度,空间不足时溢出底部。
 */
void DACollapseLayout::setGeometry(const QRect& rect)
{
	QLayout::setGeometry(rect);
	auto margins = contentsMargins();
	const int x  = rect.x() + margins.left();
	const int w  = rect.width() - margins.left() - margins.right();
	const int sp = spacing();
	int y        = rect.y() + margins.top();

	for (int i = 0; i < m_items.size(); ++i) {
		if (i > 0)
			y += sp;
		QLayoutItem* item = m_items[i];
		const int h = item->sizeHint().height();
		item->setGeometry(QRect(x, y, w, h));
		y += h;
	}
}

/** @brief 推荐尺寸:所有 item sizeHint 高度之和 */
QSize DACollapseLayout::sizeHint() const
{
	recalc();
	return m_sizeHint;
}

/** @brief 最小尺寸:所有 item minimumSize 高度之和 */
QSize DACollapseLayout::minimumSize() const
{
	recalc();
	return m_minSize;
}

/** @brief 最大尺寸:不限制 */
QSize DACollapseLayout::maximumSize() const
{
	return QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}

/** @brief 水平方向可扩展,垂直方向不可扩展 */
Qt::Orientations DACollapseLayout::expandingDirections() const
{
	return Qt::Horizontal;
}

/** @brief 不支持 heightForWidth 布局 */
bool DACollapseLayout::hasHeightForWidth() const
{
	return false;
}

/** @brief heightForWidth 返回 sizeHint 高度(不支持宽度依赖) */
int DACollapseLayout::heightForWidth(int /*width*/) const
{
	return sizeHint().height();
}

/** @brief 置 dirty 标志,下次查询时重新计算缓存 */
void DACollapseLayout::invalidate()
{
	m_dirty = true;
	QLayout::invalidate();
}

/**
 * @brief 在指定索引处插入控件
 * @param index 插入位置
 * @param widget 待插入控件
 * @details 创建 QWidgetItem 并插入到 m_items 的指定位置,
 *          同时调用 addChildWidget 完成 Qt 内部父子关系注册
 */
void DACollapseLayout::insertWidget(int index, QWidget* widget)
{
	addChildWidget(widget);
	const int clampedIndex = qBound(0, index, m_items.size());
	auto* item = new QWidgetItem(widget);
	m_items.insert(clampedIndex, item);
	invalidate();
}

/**
 * @brief 添加末尾弹簧
 * @param stretch 弹簧伸缩因子(0 时为扩展弹簧,占满剩余空间)
 */
void DACollapseLayout::addStretch(int stretch)
{
	auto* spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	if (stretch > 0)
		spacer->changeSize(0, stretch, QSizePolicy::Minimum, QSizePolicy::Fixed);
	addItem(spacer);
}
}  // namespace DA

/*** End of inlined file: DACollapseLayout.cpp ***/



/*** Start of inlined file: DACollapseItem.cpp ***/
#include <QAbstractButton>
#include <QColor>
#include <QEvent>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QStyle>
#include <QStyleOption>
#include <QSvgRenderer>
#include <QVBoxLayout>

namespace DA
{
namespace
{
/// 箭头逻辑尺寸(像素),与 m_arrow 的固定尺寸保持一致
constexpr int kArrowSize = 14;

/// 折叠态箭头(▶):惰性初始化的全局缓存,整个进程只渲染一次。
/// Disabled 模式交由 QIcon 自动从 Normal 生成灰度版本。
const QIcon& arrowRightIcon()
{
	static QIcon icon(QStringLiteral(":/DAWidgetsIcons/arrow-right.svg"));
	return icon;
}

/// 展开态箭头(▼)
const QIcon& arrowDownIcon()
{
	static const QIcon icon(QStringLiteral(":/DAWidgetsIcons/arrow-down.svg"));
	return icon;
}

/**
 * @brief 标题栏按钮(仅本文件内使用,不带 Q_OBJECT)
 * @details 用 QAbstractButton 提供 Button 无障碍角色与 Space 语义,同时保持
 *          "按下即切换"的旧交互。没有 Q_OBJECT 意味着:不能定义信号(直接复用
 *          clicked)、qobject_cast 到本类型不可靠、qss 类选择器只能写
 *          QAbstractButton#header 或 #header —— 容器反查条目依赖 parentWidget()。
 */
class DACollapseHeaderButton : public QAbstractButton
{
public:
	explicit DACollapseHeaderButton(QWidget* parent = nullptr) : QAbstractButton(parent)
	{
		setObjectName(QStringLiteral("header"));
		setCursor(Qt::PointingHandCursor);
		setFocusPolicy(Qt::NoFocus);
		setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		// 让调用方的 #header { background: ... } 仍然生效
		setAttribute(Qt::WA_StyledBackground, true);
	}

	// QAbstractButton 会按 text/icon 走样式算高度,会改标题高度;标题高度由内部布局决定
	QSize sizeHint() const override
	{
		return QWidget::sizeHint();
	}
	QSize minimumSizeHint() const override
	{
		return QWidget::minimumSizeHint();
	}

protected:
	void mousePressEvent(QMouseEvent* e) override
	{
		// 保持旧语义:按下即开合,而不是 QAbstractButton 默认的抬起触发
		if (e->button() == Qt::LeftButton) {
			click();
			e->accept();
			return;
		}
		QAbstractButton::mousePressEvent(e);
	}

	void mouseReleaseEvent(QMouseEvent* e) override
	{
		// click() 已经在按下时复位 down,这里吞掉左键抬起避免二次触发
		if (e->button() == Qt::LeftButton) {
			e->accept();
			return;
		}
		QAbstractButton::mouseReleaseEvent(e);
	}

	void keyPressEvent(QKeyEvent* e) override
	{
		switch (e->key()) {
		case Qt::Key_Return:
		case Qt::Key_Enter:
		case Qt::Key_Space:
			click();
			e->accept();
			return;
		case Qt::Key_Up:
		case Qt::Key_Down:
			// 方向键交给容器的事件过滤器做面板间导航
			e->ignore();
			return;
		default:
			break;
		}
		QAbstractButton::keyPressEvent(e);
	}

	void paintEvent(QPaintEvent*) override
	{
		QPainter p(this);
		QStyleOption opt;
		opt.initFrom(this);
		// 换成 QAbstractButton 后不再经过 QFrame 的绘制路径,qss 背景要自己接住
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
		if (hasFocus()) {
			p.setRenderHint(QPainter::Antialiasing, true);
			p.setPen(QPen(QColor(64, 158, 255, 160), 2));
			p.setBrush(Qt::NoBrush);
			p.drawRect(rect().adjusted(1, 1, -2, -2));
		}
	}

	void focusInEvent(QFocusEvent* e) override
	{
		QAbstractButton::focusInEvent(e);
		update();
	}

	void focusOutEvent(QFocusEvent* e) override
	{
		QAbstractButton::focusOutEvent(e);
		update();
	}
};
}  // namespace

class DACollapseItem::PrivateData
{
public:
	PrivateData(DACollapseItem*)
	{
	}
	QAbstractButton* header { nullptr };
	QLabel* arrow { nullptr };
	QLabel* titleLabel { nullptr };
	QWidget* titleWidget { nullptr };
	QFrame* content { nullptr };
	QVBoxLayout* contentLayout { nullptr };
	QPropertyAnimation* animation { nullptr };

	QString title;
	QString name;
	QColor titleNormalColor;  // 记录正常态标题文本色,用于禁用时还原
	QMargins headerMargins { 12, 10, 12, 10 };
	QMargins contentMargins { 12, 8, 12, 12 };
	bool disabled { false };
	bool expanded { false };
	bool animationEnabled { true };
	bool contentRequested { false };
};

/**
 * @brief 构造空标题的折叠条目
 * @param parent 父控件
 */
DACollapseItem::DACollapseItem(QWidget* parent) : QFrame(parent), DA_PIMPL_CONSTRUCT
{
	init();
}

/** @brief 析构,由 PIMPL 智能指针自动释放私有数据 */
DACollapseItem::~DACollapseItem() = default;

/**
 * @brief 构造指定标题的折叠条目
 * @param title 标题文本
 * @param parent 父控件
 */
DACollapseItem::DACollapseItem(const QString& title, QWidget* parent) : QFrame(parent), DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	d->title = title;
	init();
	d->titleLabel->setText(title);
}

/**
 * @brief 初始化标题栏、内容区与动画
 * @details 显式注册 qrc 资源避免静态库被链接器丢弃;标题栏垂直固定提供正确 sizeHint;
 *          内容区初始 maximumHeight=0 折叠;动画绑定 content 的 maximumHeight 属性
 */
void DACollapseItem::init()
{
	DA_D(d);
	ensureResourcesInitialized();  // 静态库需显式注册 qrc 资源,否则被链接器丢弃
	setObjectName(QStringLiteral("DACollapseItem"));
	setFrameShape(QFrame::NoFrame);
	installEventFilter(this);

	auto* outer = new DACollapseLayout(this);
	outer->setContentsMargins(0, 0, 0, 0);
	outer->setSpacing(0);

	// ---- 标题栏(私有按钮,保留 objectName "header") ----
	d->header = new DACollapseHeaderButton(this);
	connect(d->header, &QAbstractButton::clicked, this, &DACollapseItem::onHeaderClicked);

	auto* headerLay = new QHBoxLayout(d->header);
	headerLay->setContentsMargins(d->headerMargins);
	headerLay->setSpacing(8);

	d->arrow = new QLabel(d->header);
	d->arrow->setObjectName(QStringLiteral("DACollapseItemArrow"));
	d->arrow->setFixedSize(kArrowSize, kArrowSize);
	headerLay->addWidget(d->arrow);

	d->titleLabel = new QLabel(d->title, d->header);
	d->titleLabel->setObjectName(QStringLiteral("DACollapseItemTitle"));
	headerLay->addWidget(d->titleLabel, 1);

	outer->addWidget(d->header);

	// ---- 内容区 ----
	d->content = new QFrame(this);
	d->content->setObjectName(QStringLiteral("DACollapseItemContent"));
	d->content->setFrameShape(QFrame::NoFrame);
	d->content->setMaximumHeight(0);  // 初始折叠
	d->content->hide();
	d->content->installEventFilter(this);  // 监听 LayoutRequest 适配动态内容/宽度变化

	d->contentLayout = new QVBoxLayout(d->content);
	d->contentLayout->setContentsMargins(d->contentMargins);
	d->contentLayout->setSpacing(6);

	outer->addWidget(d->content);

	// ---- 动画 ----
	d->animation = new QPropertyAnimation(d->content, "maximumHeight", this);
	d->animation->setDuration(250);
	d->animation->setEasingCurve(QEasingCurve::OutCubic);
	connect(d->animation, &QPropertyAnimation::finished, this, &DACollapseItem::onAnimationFinished);

	updateArrow();
	d->titleNormalColor = d->titleLabel->palette().color(QPalette::WindowText);
	updateHeaderVisualState();
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
}

/** @brief 根据展开态与禁用态刷新箭头图标 */
void DACollapseItem::updateArrow()
{
	DA_D(d);
	const QIcon& icon = d->expanded ? arrowDownIcon() : arrowRightIcon();
	// Disabled 模式:QIcon 未显式 addPixmap(Disabled) 时,Qt 自动从 Normal 生成灰度版本
	const QIcon::Mode mode = d->disabled ? QIcon::Disabled : QIcon::Normal;
	d->arrow->setPixmap(icon.pixmap(QSize(kArrowSize, kArrowSize), mode));
}

/**
 * @brief 刷新标题栏禁用视觉
 * @details qss 的 color 作用在 QFrame 上不会传递给子 QLabel,故在此内置禁用
 *          置灰与光标切换,并维护 disabled 动态属性供 qss 进一步定制
 */
void DACollapseItem::updateHeaderVisualState()
{
	DA_D(d);
	// 内置禁用视觉,不依赖外部 qss(qss 的 color 作用在 QFrame 上不会传给子 QLabel)
	const QColor c = d->disabled ? QColor(QStringLiteral("#c0c4cc")) : d->titleNormalColor;
	QPalette pal   = d->titleLabel->palette();
	pal.setColor(QPalette::Active, QPalette::WindowText, c);
	pal.setColor(QPalette::Inactive, QPalette::WindowText, c);
	// 标题按钮禁用会连带子标签进 Disabled 组,这里同步设置,否则读到 Qt 默认灰
	pal.setColor(QPalette::Disabled, QPalette::WindowText, c);
	d->titleLabel->setPalette(pal);
	// 禁用时光标为"禁止"图标,比箭头更直观
	d->header->setCursor(d->disabled ? Qt::ForbiddenCursor : Qt::PointingHandCursor);
	// 复用 QAbstractButton 的 disabled 语义:禁用后既不能点也拿不到焦点
	d->header->setEnabled(!d->disabled);
	// 保留 disabled 动态属性供 qss 进一步定制(如 hover 背景抑制)
	setProperty("disabled", d->disabled);
	style()->unpolish(this);
	style()->polish(this);
}

/**
 * @brief 计算内容区在当前宽度下的自然高度
 * @return heightForWidth 结果,退化为 sizeHint
 * @details 用 item 当前宽度调用 heightForWidth,而非 layout->sizeHint().height()
 *          (后者基于 idealWidth,对 wordWrap 内容偏小)。
 *          widget 未显示时 width() 为 0,heightForWidth(0) 因边距为负返回 -1,
 *          此时退化为 sizeHint 作为初始估算,待 Resize 事件获得实际宽度后修正。
 *          outer layout margins=0 且 content 为 NoFrame,故 content 宽度 == item 宽度。
 */
int DACollapseItem::contentNaturalHeight() const
{
	DA_DC(d);
	if (auto* lay = d->content->layout()) {
		const int w = width();
		if (w > 0) {
			const int h = lay->heightForWidth(w);
			if (h > 0)
				return h;
		}
		return lay->sizeHint().height();
	}
	return d->content->sizeHint().height();
}

/**
 * @brief 执行展开/折叠的实际动画或即时切换
 * @param on true 展开, false 折叠
 * @details 通过动画驱动 content 的 maximumHeight 在 0 与自然高度间过渡。
 *          DACollapseLayout 不压缩子项,无需 setMinimumHeight 防护。
 *          折叠时以当前实际高度作为动画起点。
 */
void DACollapseItem::doExpand(bool on)
{
	DA_D(d);
	if (d->animationEnabled) {
		if (on) {
			d->content->setMaximumHeight(0);
			d->content->show();
			const int target = contentNaturalHeight();
			d->animation->stop();
			d->animation->setStartValue(0);
			d->animation->setEndValue(target);
			d->animation->start();
		} else {
			const int start = d->content->height();
			d->content->setMaximumHeight(start);
			d->animation->stop();
			d->animation->setStartValue(start);
			d->animation->setEndValue(0);
			d->animation->start();
		}
	} else {
		// 无动画:直接显示/隐藏
		if (on) {
			d->content->show();
			d->content->setMaximumHeight(contentNaturalHeight());
		} else {
			d->content->setMaximumHeight(0);
			d->content->hide();
		}
	}
	updateArrow();
}

/** @brief 标题栏点击处理,切换展开态(禁用面板忽略) */
void DACollapseItem::onHeaderClicked()
{
	DA_D(d);
	if (d->disabled)
		return;
	setExpanded(!d->expanded);
}

/**
 * @brief 动画结束后收尾
 * @details 展开态:动画已将 maximumHeight 设为目标值,无需额外操作。
 *          折叠态:隐藏 content,避免 maximumHeight=0 的可见残留。
 *          DACollapseLayout 不压缩子项,无需 setMinimumHeight 双向锁定。
 */
void DACollapseItem::onAnimationFinished()
{
	DA_D(d);
	if (!d->expanded) {
		d->content->hide();
	}
}

/**
 * @brief 重新计算内容区高度
 * @details 未展开直接返回;动画中更新终点,否则直接修正 maximumHeight。
 *          异步加载内容的宿主在 setContentWidget() 之后调用。
 */
void DACollapseItem::refreshContentHeight()
{
	DA_D(d);
	if (!d->expanded)
		return;
	const int h = contentNaturalHeight();
	if (d->animation->state() == QAbstractAnimation::Running) {
		d->animation->setEndValue(h);
	} else if (d->content->maximumHeight() != h) {
		d->content->setMaximumHeight(h);
		d->content->updateGeometry();
	}
}

/**
 * @brief 按 daVariant 动态属性重设标题/内容留白
 * @details 只改留白,不碰展开状态;Default 与加入变体前逐像素一致
 */
void DACollapseItem::applyVariantChrome()
{
	DA_D(d);
	const QString key = property("daVariant").toString();
	if (key == QStringLiteral("borderless")) {
		d->headerMargins  = QMargins(0, 8, 0, 8);
		d->contentMargins = QMargins(0, 4, 0, 8);
	} else {
		d->headerMargins  = QMargins(12, 10, 12, 10);
		d->contentMargins = QMargins(12, 8, 12, 12);
	}
	if (auto* lay = qobject_cast< QHBoxLayout* >(d->header->layout()))
		lay->setContentsMargins(d->headerMargins);
	d->contentLayout->setContentsMargins(d->contentMargins);
	style()->unpolish(this);
	style()->polish(this);
	updateGeometry();
}

/** @brief 获取标题栏控件(内部按钮) */
QWidget* DACollapseItem::headerWidget() const
{
	DA_DC(d);
	return d->header;
}

/**
 * @brief 事件过滤器:适配内容区布局变化与首次显示高度修正
 * @param watched 被监听对象(自身/内容区)
 * @param event 事件
 * @details 内容区 LayoutRequest 与自身 Resize 都走 refreshContentHeight(),
 *          修正 widget 首次显示时 width()==0 导致的初始高度估算偏差
 */
bool DACollapseItem::eventFilter(QObject* watched, QEvent* event)
{
	DA_D(d);
	if (watched == d->content && event->type() == QEvent::LayoutRequest) {
		// 展开态(含动画中)下内容变化时,更新高度或动画终点
		refreshContentHeight();
	} else if (watched == this && event->type() == QEvent::Resize) {
		refreshContentHeight();
	}
	return QFrame::eventFilter(watched, event);
}

/** @brief 获取标题文本 */
QString DACollapseItem::title() const
{
	DA_DC(d);
	return d->title;
}

/**
 * @brief 设置标题文本
 * @param title 新标题文本
 * @details 值未变化时跳过;更新标签后发射 titleChanged
 */
void DACollapseItem::setTitle(const QString& title)
{
	DA_D(d);
	if (d->title == title)
		return;
	d->title = title;
	d->titleLabel->setText(title);
	Q_EMIT titleChanged(title);
}

/** @brief 获取面板标识 */
QString DACollapseItem::name() const
{
	DA_DC(d);
	return d->name;
}

/**
 * @brief 设置面板标识
 * @param name 新面板标识
 * @details 值未变化时跳过;更新后发射 nameChanged
 */
void DACollapseItem::setName(const QString& name)
{
	DA_D(d);
	if (d->name == name)
		return;
	d->name = name;
	Q_EMIT nameChanged(name);
}

/** @brief 是否禁用 */
bool DACollapseItem::disabled() const
{
	DA_DC(d);
	return d->disabled;
}

/**
 * @brief 启用/禁用面板交互
 * @param on true 禁用
 * @details 更新标题置灰、箭头灰度与光标,并维护 qss disabled 属性后发射信号
 */
void DACollapseItem::setDisabled(bool on)
{
	DA_D(d);
	if (d->disabled == on)
		return;
	d->disabled = on;
	updateHeaderVisualState();  // 标题文本置灰 + 光标 + qss 属性
	updateArrow();              // 箭头随 QIcon::Disabled 自动变灰
	Q_EMIT disabledChanged(on);
}

/** @brief 是否处于展开态 */
bool DACollapseItem::expanded() const
{
	DA_DC(d);
	return d->expanded;
}

/**
 * @brief 展开/折叠面板
 * @param on true 展开, false 折叠
 * @details 值未变化时跳过;更新状态后调用 doExpand 执行动画或即时切换,
 *          随后发射 expandChanged 供容器协调互斥
 */
void DACollapseItem::setExpanded(bool on)
{
	DA_D(d);
	if (d->expanded == on)
		return;
	// 懒加载钩子必须在 doExpand() 之前:doExpand 立刻按当前内容算动画目标
	if (on && !d->contentRequested && d->contentLayout->count() == 0) {
		d->contentRequested = true;
		Q_EMIT contentRequested();
	}
	d->expanded = on;
	// 维护 expanded 动态属性,让 [expanded="true"] 的 qss 选择器成立
	setProperty("expanded", on);
	style()->unpolish(this);
	style()->polish(this);
	doExpand(on);
	Q_EMIT expandChanged(on);
}

/** @brief 是否启用展开/折叠动画 */
bool DACollapseItem::animationEnabled() const
{
	DA_DC(d);
	return d->animationEnabled;
}

/**
 * @brief 设置是否启用动画
 * @param on true 启用
 * @details 值未变化时跳过;更新后发射 animationEnabledChanged
 */
void DACollapseItem::setAnimationEnabled(bool on)
{
	DA_D(d);
	if (d->animationEnabled == on)
		return;
	d->animationEnabled = on;
	Q_EMIT animationEnabledChanged(on);
}

/**
 * @brief 设置自定义标题控件
 * @param widget 标题控件,传入 nullptr 还原默认文本标签
 * @details 取代默认标题文本,插入到箭头与标题之间;旧的自定义控件被销毁
 */
void DACollapseItem::setTitleWidget(QWidget* widget)
{
	DA_D(d);
	if (d->titleWidget == widget)
		return;
	if (d->titleWidget) {
		d->titleWidget->setParent(nullptr);
		delete d->titleWidget;
	}
	d->titleWidget = widget;
	if (d->titleWidget) {
		d->titleWidget->setParent(d->header);
		// 标题 widget 取代默认文本标签与箭头之间的位置
		auto* lay = qobject_cast< QHBoxLayout* >(d->header->layout());
		lay->insertWidget(lay->indexOf(d->arrow) + 1, d->titleWidget, 1);
		d->titleLabel->hide();
	} else {
		d->titleLabel->show();
	}
}

/** @brief 获取自定义标题控件,未设置时返回 nullptr */
QWidget* DACollapseItem::titleWidget() const
{
	DA_DC(d);
	return d->titleWidget;
}

/**
 * @brief 设置内容控件
 * @param widget 内容控件,可为 nullptr 仅清空
 * @details 清空旧内容布局项后追加新控件,取代默认内容
 */
void DACollapseItem::setContentWidget(QWidget* widget)
{
	DA_D(d);
	// 清空旧内容
	while (d->contentLayout->count() > 0) {
		QLayoutItem* it = d->contentLayout->takeAt(0);
		if (it->widget())
			delete it->widget();
		delete it;
	}
	if (widget) {
		d->contentLayout->addWidget(widget);
		// 内容区已可见时,新加入的子控件不会自动 show,隐藏控件 sizeHint 为 0,
		// 会让 refreshContentHeight() 算不出异步加载后的高度
		widget->show();
	}
}

/** @brief 获取内容控件,未设置时返回 nullptr */
QWidget* DACollapseItem::contentWidget() const
{
	DA_DC(d);
	if (d->contentLayout->count() > 0) {
		if (auto* it = d->contentLayout->itemAt(0))
			return it->widget();
	}
	return nullptr;
}

/** @brief 获取内容区布局,可直接 addWidget 追加多个控件 */
QVBoxLayout* DACollapseItem::contentLayout() const
{
	DA_DC(d);
	return d->contentLayout;
}

/** @brief 推荐尺寸,委托基类实现 */
QSize DACollapseItem::sizeHint() const
{
	return QFrame::sizeHint();
}
}  // namespace DA

/*** End of inlined file: DACollapseItem.cpp ***/


/*** Start of inlined file: DACollapseWidget.cpp ***/
#include <QKeyEvent>
#include <QSet>
#include <QVBoxLayout>

namespace DA
{
namespace
{
/// 变体的 qss 动态属性值
QString variantKey(DACollapseWidget::Variant v)
{
	switch (v) {
	case DACollapseWidget::Variant::Borderless:
		return QStringLiteral("borderless");
	case DACollapseWidget::Variant::Card:
		return QStringLiteral("card");
	case DACollapseWidget::Variant::Default:
		break;
	}
	return QStringLiteral("default");
}
}  // namespace

class DACollapseWidget::PrivateData
{
public:
	PrivateData(DACollapseWidget*)
	{
	}
	DACollapseLayout* layout { nullptr };
	QList< DACollapseItem* > items;
	bool accordion { false };
	bool keyboardNavigation { false };
	Variant variant { Variant::Default };
	// setActiveNames 批量同步期间抑制逐项 activeNamesChanged,但不阻断条目自身信号
	bool activating { false };
};

/**
 * @brief 构造折叠面板容器
 * @param parent 父控件
 * @details 初始化纵向布局并插入末尾弹簧,使条目顶部对齐
 */
DACollapseWidget::DACollapseWidget(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
	init();
}

/** @brief 析构,由 PIMPL 智能指针自动释放私有数据 */
DACollapseWidget::~DACollapseWidget() = default;

/** @brief 初始化无间距纵向布局与末尾弹簧 */
void DACollapseWidget::init()
{
	DA_D(d);
	setObjectName(QStringLiteral("DACollapse"));
	d->layout = new DACollapseLayout(this);
	d->layout->setContentsMargins(0, 0, 0, 0);
	d->layout->setSpacing(0);
	// 末尾弹簧,使条目顶部对齐
	d->layout->addStretch(1);
}

/**
 * @brief 查询手风琴模式
 * @return true 表示已启用手风琴模式
 */
bool DACollapseWidget::accordion() const
{
	DA_DC(d);
	return d->accordion;
}

/**
 * @brief 设置手风琴模式
 * @param on true 启用, false 禁用
 * @details 启用瞬间若当前多个面板展开,仅保留首个激活面板以维持互斥语义
 */
void DACollapseWidget::setAccordion(bool on)
{
	DA_D(d);
	if (d->accordion == on)
		return;
	d->accordion = on;
	// 切换到手风琴模式时,只保留首个激活面板
	if (on) {
		const QStringList act = activeNames();
		if (act.size() > 1) {
			// 仅保留第一个
			for (int i = 1; i < act.size(); ++i) {
				if (auto* it = item(act[ i ]))
					activate(it, false);
			}
		}
	}
	Q_EMIT accordionChanged(on);
}

/**
 * @brief 打开/关闭键盘导航
 * @param on 是否开启(默认 false)
 * @details 开启时每个标题变为 StrongFocus;关闭时收回焦点策略并释放焦点
 */
void DACollapseWidget::setKeyboardNavigation(bool on)
{
	DA_D(d);
	if (d->keyboardNavigation == on)
		return;
	d->keyboardNavigation = on;
	for (auto* it : std::as_const(d->items)) {
		QWidget* header = it->headerWidget();
		if (!header)
			continue;
		if (on) {
			header->setFocusPolicy(Qt::StrongFocus);
		} else {
			header->setFocusPolicy(Qt::NoFocus);
			if (header->hasFocus())
				header->clearFocus();
		}
	}
}

/** @brief 键盘导航是否开启 */
bool DACollapseWidget::isKeyboardNavigation() const
{
	DA_DC(d);
	return d->keyboardNavigation;
}

/**
 * @brief 设置外观变体
 * @param variant 变体枚举(默认 Default)
 * @details 只改间距与内留白,不碰 accordion / activeNames
 */
void DACollapseWidget::setVariant(Variant variant)
{
	DA_D(d);
	if (d->variant == variant)
		return;
	d->variant = variant;
	const QString key = variantKey(variant);
	setProperty("daVariant", key);
	d->layout->setSpacing(variant == Variant::Card ? 8 : 0);
	for (auto* it : std::as_const(d->items))
		applyVariantToItem(it);
	d->layout->invalidate();
}

/** @brief 当前外观变体 */
DACollapseWidget::Variant DACollapseWidget::variant() const
{
	DA_DC(d);
	return d->variant;
}

/**
 * @brief 获取当前激活面板的 name 列表
 * @return 遍历所有条目收集展开项的 name
 */
QStringList DACollapseWidget::activeNames() const
{
	DA_DC(d);
	QStringList names;
	for (auto* it : std::as_const(d->items)) {
		if (it->expanded())
			names << it->name();
	}
	return names;
}

/**
 * @brief 设置激活面板集合
 * @param names 需展开面板的 name 列表
 * @details 手风琴模式下仅展开 names 中首个实际存在的面板;非手风琴模式按集合
 *          匹配逐项展开/折叠。完成后发射 activeNamesChanged
 */
void DACollapseWidget::setActiveNames(const QStringList& names)
{
	DA_D(d);
	const QSet< QString > set(names.begin(), names.end());
	if (d->accordion) {
		// 手风琴:仅展开 names 中第一个实际存在的面板,其余折叠
		DACollapseItem* firstMatch = nullptr;
		for (const auto& n : names) {
			firstMatch = item(n);
			if (firstMatch)
				break;
		}
		for (auto* it : std::as_const(d->items)) {
			activate(it, it == firstMatch);
		}
	} else {
		for (auto* it : std::as_const(d->items)) {
			activate(it, set.contains(it->name()));
		}
	}
	Q_EMIT activeNamesChanged(activeNames());
}

/**
 * @brief 单向设置条目展开态
 * @param item 目标条目
 * @param on 是否展开
 * @details 展开禁用面板将被拒绝(对标 Element UI disabled);临时阻断信号避免
 *          setActiveNames 过程中逐个触发 onItemExpandChanged 造成抖动
 */
void DACollapseWidget::activate(DACollapseItem* item, bool on)
{
	DA_D(d);
	if (!item) {
		return;
	}
	// 禁用面板不可激活(对标 Element UI disabled),折叠则不受限
	if (on && item->disabled())
		return;
	// 用标志位抑制 onItemExpandChanged 的逐项发射,而不是 blockSignals():
	// 后者会连 contentRequested 一起吞掉,破坏 setActiveNames 触发的懒加载
	const bool prev = d->activating;
	d->activating  = true;
	item->setExpanded(on);
	d->activating = prev;
}

/**
 * @brief 添加已有条目并接管所有权
 * @param item 待添加条目
 * @details 在末尾弹簧之前插入以保持条目在上,并连接 expandChanged 用于互斥协调
 */
void DACollapseWidget::addItem(DACollapseItem* item)
{
	DA_D(d);
	if (!item)
		return;
	item->setParent(this);
	// 在弹簧之前插入,保持条目在上、弹簧在底
	const int idx = d->layout->count() - 1;  // 弹簧索引
	d->layout->insertWidget(idx, item);
	d->items.append(item);
	// 键盘导航:容器统一监听标题按键
	if (QWidget* header = item->headerWidget()) {
		header->installEventFilter(this);
		if (d->keyboardNavigation)
			header->setFocusPolicy(Qt::StrongFocus);
	}
	connect(item, &DACollapseItem::contentRequested, this,
			[ this, item ] { Q_EMIT expandRequested(item->name()); });
	connect(item, &DACollapseItem::expandChanged, this, [ this ](bool) { onItemExpandChanged(); });
	// 后加入的条目也要吃到当前变体
	applyVariantToItem(item);
}

/**
 * @brief 便捷创建并添加条目
 * @param title 标题文本
 * @param name 面板标识,为空时回退使用 title
 * @return 新建条目实例
 */
DACollapseItem* DACollapseWidget::addItem(const QString& title, const QString& name)
{
	auto* it = new DACollapseItem(title, this);
	it->setName(name.isEmpty() ? title : name);
	addItem(it);
	return it;
}

/**
 * @brief 按 name 查找条目
 * @param name 面板标识
 * @return 匹配条目,未找到返回 nullptr
 */
DACollapseItem* DACollapseWidget::item(const QString& name) const
{
	DA_DC(d);
	for (auto* it : d->items) {
		if (it->name() == name)
			return it;
	}
	return nullptr;
}

/** @brief 获取全部条目列表 */
QList< DACollapseItem* > DACollapseWidget::items() const
{
	DA_DC(d);
	return d->items;
}

/**
 * @brief 移除并销毁条目
 * @param item 待移除条目
 * @details 从布局与列表移除后 delete;移除后发射 activeNamesChanged 同步状态
 */
void DACollapseWidget::removeItem(DACollapseItem* item)
{
	DA_D(d);
	if (!item)
		return;
	const int i = d->items.indexOf(item);
	if (i < 0)
		return;
	if (QWidget* header = item->headerWidget())
		header->removeEventFilter(this);
	d->items.removeAt(i);
	d->layout->removeWidget(item);
	delete item;
	Q_EMIT activeNamesChanged(activeNames());
}

/**
 * @brief 清空并销毁所有条目
 * @details 移除布局中的每个条目并 delete,最后发射空 activeNamesChanged
 */
void DACollapseWidget::clear()
{
	DA_D(d);
	for (auto* it : std::as_const(d->items)) {
		if (QWidget* header = it->headerWidget())
			header->removeEventFilter(this);
		d->layout->removeWidget(it);
		delete it;
	}
	d->items.clear();
	Q_EMIT activeNamesChanged({});
}

/**
 * @brief 子条目展开态变化时的协调槽
 * @details 手风琴模式下新展开面板会折叠其余已展开面板(阻断信号避免递归),
 *          随后发射 activeNamesChanged
 */
void DACollapseWidget::onItemExpandChanged()
{
	DA_D(d);
	if (d->activating)
		return;
	auto* src = qobject_cast< DACollapseItem* >(sender());
	if (!src)
		return;

	if (src->expanded() && d->accordion) {
		// 手风琴:折叠其它所有已展开面板
		for (auto* it : std::as_const(d->items)) {
			if (it != src && it->expanded()) {
				// 阻断信号避免递归
				it->blockSignals(true);
				it->setExpanded(false);
				it->blockSignals(false);
			}
		}
	}
	Q_EMIT activeNamesChanged(activeNames());
}

/**
 * @brief 事件过滤器:标题上的 ↑/↓ 面板间导航
 * @param watched 被监听对象(标题控件)
 * @param event 事件
 * @details 跳过禁用项、到首尾不循环;方向键一律吞掉,避免焦点跑出容器
 */
bool DACollapseWidget::eventFilter(QObject* watched, QEvent* event)
{
	DA_D(d);
	if (!d->keyboardNavigation || event->type() != QEvent::KeyPress || d->items.isEmpty())
		return QWidget::eventFilter(watched, event);

	auto* ke           = static_cast< QKeyEvent* >(event);
	const int key      = ke->key();
	if (key != Qt::Key_Up && key != Qt::Key_Down)
		return QWidget::eventFilter(watched, event);

	DACollapseItem* current = nullptr;
	for (auto* it : std::as_const(d->items)) {
		if (it->headerWidget() == watched) {
			current = it;
			break;
		}
	}
	if (!current)
		return QWidget::eventFilter(watched, event);

	const int dir   = (key == Qt::Key_Down) ? 1 : -1;
	const int start = d->items.indexOf(current);
	for (int i = start + dir; i >= 0 && i < d->items.size(); i += dir) {
		DACollapseItem* candidate = d->items.at(i);
		if (candidate->disabled())
			continue;
		if (QWidget* header = candidate->headerWidget()) {
			header->setFocus(Qt::ShortcutFocusReason);
			break;
		}
	}
	return true;
}

/**
 * @brief 把当前变体下发给单个条目
 * @param item 目标条目
 */
void DACollapseWidget::applyVariantToItem(DACollapseItem* item)
{
	DA_D(d);
	if (!item)
		return;
	item->setProperty("daVariant", variantKey(d->variant));
	item->applyVariantChrome();
}

/** @brief 推荐尺寸,委托基类实现 */
QSize DACollapseWidget::sizeHint() const
{
	return QWidget::sizeHint();
}

/**
 * @brief 在指定索引处插入普通控件
 * @param index 插入位置(相对于布局中所有项,含 DACollapseItem 与弹簧)
 * @param widget 待插入控件,可为任意 QWidget(非 DACollapseItem)
 * @details 直接委托给内部 DACollapseLayout::insertWidget,
 *          控件不进入 items 列表,不参与 activeNames/accordion 协调。
 *          负索引或超出范围时钳制到 [0, count]。
 */
void DACollapseWidget::insertWidget(int index, QWidget* widget)
{
	DA_D(d);
	if (!widget)
		return;
	// 插入到弹簧之前:布局末尾是 stretch,新控件应位于其前
	const int stretchIdx = d->layout->count() - 1;
	const int clamped    = qBound(0, index, stretchIdx);
	d->layout->insertWidget(clamped, widget);
}

/**
 * @brief 末尾(弹簧前)添加普通控件
 * @param widget 待添加控件
 * @details 等价于 insertWidget(count-1, widget),确保控件在末尾弹簧之前。
 */
void DACollapseWidget::addWidget(QWidget* widget)
{
	DA_D(d);
	insertWidget(d->layout->count() - 1, widget);
}

/**
 * @brief 移除普通控件(不销毁)
 * @param widget 待移除控件
 * @details 遍历布局查找匹配的 QWidgetItem 并 takeAt,调用方负责后续所有权。
 *          若 widget 同时是 DACollapseItem,应用 removeItem 以同步 items 列表。
 */
void DACollapseWidget::removeWidget(QWidget* widget)
{
	DA_D(d);
	if (!widget)
		return;
	const int n = d->layout->count();
	for (int i = 0; i < n; ++i) {
		if (QLayoutItem* it = d->layout->itemAt(i)) {
			if (it->widget() == widget) {
				d->layout->takeAt(i);
				return;
			}
		}
	}
}

/**
 * @brief 返回普通控件的索引
 * @param widget 目标控件
 * @return 布局中的索引,不存在返回 -1
 * @details 索引相对于布局中所有项(含 DACollapseItem 与弹簧)。
 */
int DACollapseWidget::indexOf(QWidget* widget) const
{
	DA_DC(d);
	if (!widget)
		return -1;
	const int n = d->layout->count();
	for (int i = 0; i < n; ++i) {
		if (QLayoutItem* it = d->layout->itemAt(i)) {
			if (it->widget() == widget)
				return i;
		}
	}
	return -1;
}
}  // namespace DA

/*** End of inlined file: DACollapseWidget.cpp ***/

// ---- 光标 / 遮罩 / 撤销历史等工具组件 ----

/*** Start of inlined file: DAWidgetsResources.cpp ***/
#include <QtCore/QResource>

// 必须位于全局作用域：Q_INIT_RESOURCE 在这里声明的是全局函数
// qInitResources_dawidgetsicons()，与 rcc 生成的定义一致。
static void initDawidgetsIconsResource()
{
	Q_INIT_RESOURCE(dawidgetsicons);
}

namespace DA {

void ensureResourcesInitialized()
{
	// 函数内静态变量保证只注册一次（C++11 起线程安全）
	static const bool initialized = (initDawidgetsIconsResource(), true);
	Q_UNUSED(initialized)
}

}  // namespace DA

/*** End of inlined file: DAWidgetsResources.cpp ***/



/*** Start of inlined file: DACursorScoped.cpp ***/
#include <QApplication>
namespace DA
{

/**
 * @brief 传入图标并全局设置为当前图标
 * @param cur
 */
DACursorScoped::DACursorScoped(const QCursor& cur)
{
	setCursor(cur);
}

/**
 * @brief 析构函数
 * @details 通过调用 release() 恢复覆盖光标，利用 RAII 保证异常安全路径下光标也能恢复
 */
DACursorScoped::~DACursorScoped()
{
	release();
}

/**
 * @brief 设置覆盖光标
 * @param cur 要设置的光标
 * @details 调用 QApplication::setOverrideCursor 压入光标栈，原光标被覆盖
 */
void DACursorScoped::setCursor(const QCursor& cur)
{
	QApplication::setOverrideCursor(cur);
}

/**
 * @brief 提前恢复图标
 */
void DACursorScoped::release()
{
	QApplication::restoreOverrideCursor();
}
}  // end namespace DA

/*** End of inlined file: DACursorScoped.cpp ***/


/*** Start of inlined file: DAWaitCursorScoped.cpp ***/
#include <QApplication>

namespace DA
{

/**
 * @brief 构造等待光标守卫
 * @details 以 Qt::WaitCursor 作为参数委托给基类 DACursorScoped，构造时即显示等待光标
 */
DAWaitCursorScoped::DAWaitCursorScoped() : DACursorScoped(QCursor(Qt::WaitCursor))
{
}

/**
 * @brief 析构函数
 * @details 恢复覆盖光标，用于在离开作用域时还原鼠标指针
 */
DAWaitCursorScoped::~DAWaitCursorScoped()
{
	QApplication::restoreOverrideCursor();
}
}  // end namespace DA

/*** End of inlined file: DAWaitCursorScoped.cpp ***/


/*** Start of inlined file: DAAbstractWidgetOverlay.cpp ***/
#include <QPainter>
#include <QPaintEngine>
#include <QPainterPath>
#include <QImage>
#include <QEvent>
#include <QPaintEvent>
#include <QDebug>
#include <cstdlib>
namespace DA
{

class DAAbstractWidgetOverlay::PrivateData
{
	DA_DECLARE_PUBLIC(DAAbstractWidgetOverlay)
public:
	PrivateData(DAAbstractWidgetOverlay* p);
	static bool isX11GraphicsSystem();
	static QImage::Format maskImageFormat();
	static QRegion alphaMask(const QImage& image, const QRegion& region);
	void resetRgbaBuffer();
	MaskMode maskMode { DAAbstractWidgetOverlay::MaskHint };
	RenderMode renderMode { DAAbstractWidgetOverlay::AutoRenderMode };
	uchar* rgbaBuffer { nullptr };
};

DAAbstractWidgetOverlay::PrivateData::PrivateData(DAAbstractWidgetOverlay* p) : q_ptr(p)
{
}

bool DAAbstractWidgetOverlay::PrivateData::isX11GraphicsSystem()
{
	static int onX11 = -1;
	if (onX11 < 0) {
		QPixmap pm(1, 1);
		QPainter painter(&pm);

		onX11 = (painter.paintEngine()->type() == QPaintEngine::X11) ? 1 : 0;
	}

	return onX11 == 1;
}

QImage::Format DAAbstractWidgetOverlay::PrivateData::maskImageFormat()
{
	if (isX11GraphicsSystem())
		return QImage::Format_ARGB32;

	return QImage::Format_ARGB32_Premultiplied;
}

QRegion DAAbstractWidgetOverlay::PrivateData::alphaMask(const QImage& image, const QRegion& region)
{
	const int w = image.width();
	const int h = image.height();

	QRegion mask;
	QRect rect;

#if QT_VERSION >= 0x050800
	for (QRegion::const_iterator it = region.cbegin(); it != region.cend(); ++it) {
		const QRect& r = *it;
#else
	const QVector< QRect > rects = region.rects();
	for (int i = 0; i < rects.size(); i++) {
		const QRect& r = rects[ i ];
#endif
		int x1, x2, y1, y2;
		r.getCoords(&x1, &y1, &x2, &y2);

		x1 = qMax(x1, 0);
		x2 = qMin(x2, w - 1);
		y1 = qMax(y1, 0);
		y2 = qMin(y2, h - 1);

		for (int y = y1; y <= y2; ++y) {
			bool inRect = false;
			int rx0     = -1;

			const uint* line = reinterpret_cast< const uint* >(image.scanLine(y)) + x1;
			for (int x = x1; x <= x2; x++) {
				const bool on = ((*line++ >> 24) != 0);
				if (on != inRect) {
					if (inRect) {
						rect.setCoords(rx0, y, x - 1, y);
						mask += rect;
					} else {
						rx0 = x;
					}

					inRect = on;
				}
			}

			if (inRect) {
				rect.setCoords(rx0, y, x2, y);
				mask = mask.united(rect);
			}
		}
	}

	return mask;
}

void DAAbstractWidgetOverlay::PrivateData::resetRgbaBuffer()
{
	if (rgbaBuffer) {
		std::free(rgbaBuffer);
		rgbaBuffer = NULL;
	}
}

//===============================================================
// DAWidgetOverlay
//===============================================================

DAAbstractWidgetOverlay::DAAbstractWidgetOverlay(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_NoSystemBackground);
	setFocusPolicy(Qt::NoFocus);

	if (parent) {
		resize(parent->size());
		parent->installEventFilter(this);
	}
}

DAAbstractWidgetOverlay::~DAAbstractWidgetOverlay()
{
}

void DAAbstractWidgetOverlay::setMaskMode(MaskMode mode)
{
	if (mode != d_ptr->maskMode) {
		d_ptr->maskMode = mode;
		d_ptr->resetRgbaBuffer();
	}
}

DAAbstractWidgetOverlay::MaskMode DAAbstractWidgetOverlay::getMaskMode() const
{
	return d_ptr->maskMode;
}

void DAAbstractWidgetOverlay::setRenderMode(RenderMode mode)
{
	d_ptr->renderMode = mode;
}

DAAbstractWidgetOverlay::RenderMode DAAbstractWidgetOverlay::getRenderMode() const
{
	return d_ptr->renderMode;
}

bool DAAbstractWidgetOverlay::eventFilter(QObject* object, QEvent* event)
{
	if (object && object == parent()) {
		switch (event->type()) {
		case QEvent::Resize: {
			QResizeEvent* resizeEvent = static_cast< QResizeEvent* >(event);
			resize(resizeEvent->size());
		} break;
		default:
			break;
		}
	}
	return QWidget::eventFilter(object, event);
}

QRegion DAAbstractWidgetOverlay::maskRegion(const QRect& r, int penWidth)
{
	const int pw  = qMax(penWidth, 1);
	const int pw2 = penWidth / 2;

	int x1 = r.left() - pw2;
	int x2 = r.right() + 1 + pw2 + (pw % 2);

	int y1 = r.top() - pw2;
	int y2 = r.bottom() + 1 + pw2 + (pw % 2);

	QRegion region;

	region += QRect(x1, y1, x2 - x1, pw);
	region += QRect(x1, y1, pw, y2 - y1);
	region += QRect(x1, y2 - pw, x2 - x1, pw);
	region += QRect(x2 - pw, y1, pw, y2 - y1);

	return region;
}

QRegion DAAbstractWidgetOverlay::maskRegionVOrHLine(const QLine& VOrHLine, int penWidth)
{
	const int pw  = qMax(penWidth, 1);
	const int pw2 = penWidth / 2;

	QRegion region;

	if (VOrHLine.x1() == VOrHLine.x2()) {
		// 竖直线：宽度=pw，高度=|y2-y1|
		int y1 = qMin(VOrHLine.y1(), VOrHLine.y2());
		int y2 = qMax(VOrHLine.y1(), VOrHLine.y2());
		region += QRect(VOrHLine.x1() - pw2, y1, pw, y2 - y1 + 1);
	} else if (VOrHLine.y1() == VOrHLine.y2()) {
		// 水平线：高度=pw，宽度=|x2-x1|
		int x1 = qMin(VOrHLine.x1(), VOrHLine.x2());
		int x2 = qMax(VOrHLine.x1(), VOrHLine.x2());
		region += QRect(x1, VOrHLine.y1() - pw2, x2 - x1 + 1, pw);
	}

	return region;
}

void DAAbstractWidgetOverlay::paintEvent(QPaintEvent* event)
{
	const QRegion& clipRegion = event->region();

	QPainter painter(this);

	bool useRgbaBuffer = false;
	if (d_ptr->renderMode == DAAbstractWidgetOverlay::CopyAlphaMask) {
		useRgbaBuffer = true;
	} else if (d_ptr->renderMode == DAAbstractWidgetOverlay::AutoRenderMode) {
		if (painter.paintEngine()->type() == QPaintEngine::Raster)
			useRgbaBuffer = true;
	}

	if (d_ptr->rgbaBuffer && useRgbaBuffer) {
		const QImage image(d_ptr->rgbaBuffer, width(), height(), DAAbstractWidgetOverlay::PrivateData::maskImageFormat());

		const int rectCount = clipRegion.rectCount();

		if (rectCount > 2000) {
			// the region is to complex
			painter.setClipRegion(clipRegion);

			const QRect r = clipRegion.boundingRect();
			painter.drawImage(r.topLeft(), image, r);
		} else {
#if QT_VERSION >= 0x050800
			for (QRegion::const_iterator it = clipRegion.cbegin(); it != clipRegion.cend(); ++it) {
				const QRect& r = *it;
				painter.drawImage(r.topLeft(), image, r);
			}
#else
			const QVector< QRect > rects = clipRegion.rects();
			for (int i = 0; i < rects.size(); i++) {
				const QRect& r = rects[ i ];
				painter.drawImage(r.topLeft(), image, r);
			}
#endif
		}
	} else {
		painter.setClipRegion(clipRegion);
		draw(&painter);
	}
}

void DAAbstractWidgetOverlay::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event);

	d_ptr->resetRgbaBuffer();
}

QRegion DAAbstractWidgetOverlay::maskHint() const
{
	return QRegion();
}

void DAAbstractWidgetOverlay::updateOverlay()
{
	updateMask();
	update();
}

void DAAbstractWidgetOverlay::updateMask()
{
	d_ptr->resetRgbaBuffer();

	QRegion mask;

	if (d_ptr->maskMode == DAAbstractWidgetOverlay::MaskHint) {
		mask = maskHint();
	} else if (d_ptr->maskMode == DAAbstractWidgetOverlay::AlphaMask) {
		// TODO: the image doesn't need to be larger than
		//       the bounding rectangle of the hint !!

		QRegion hint = maskHint();
		if (hint.isEmpty())
			hint += QRect(0, 0, width(), height());

		// A fresh buffer from calloc() is usually faster
		// than reinitializing an existing one with
		// QImage::fill( 0 ) or memset()

		d_ptr->rgbaBuffer = (uchar*)::calloc(width() * height(), 4);

		QImage image(d_ptr->rgbaBuffer, width(), height(), DAAbstractWidgetOverlay::PrivateData::maskImageFormat());

		QPainter painter(&image);
		draw(&painter);
		painter.end();

		mask = DAAbstractWidgetOverlay::PrivateData::alphaMask(image, hint);

		if (d_ptr->renderMode == DAAbstractWidgetOverlay::DrawOverlay) {
			// we don't need the buffer later
			d_ptr->resetRgbaBuffer();
		}
	}

	// A bug in Qt initiates a full repaint of the widget
	// when we change the mask, while we are visible !

	setVisible(false);

	if (mask.isEmpty())
		clearMask();
	else
		setMask(mask);

	setVisible(true);
}

void DAAbstractWidgetOverlay::draw(QPainter* painter) const
{
	if (QWidget* widget = parentWidget()) {
		painter->setClipRect(widget->contentsRect());
	}
	drawOverlay(painter);
}

}  // end ns da

/*** End of inlined file: DAAbstractWidgetOverlay.cpp ***/


/*** Start of inlined file: DAUndoHistoryWidget.cpp ***/
#include <QAbstractItemModel>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QSpinBox>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <QUndoCommand>
#include <QUndoGroup>
#include <QUndoStack>
#include <QUndoView>
#include <QVBoxLayout>
#include <functional>

namespace DA
{
namespace
{
// 私有 delegate：左侧状态点 + 右侧时间；两个开关都关时完全交给基类
class UndoRowDelegate : public QStyledItemDelegate
{
public:
	using StampFn   = std::function< QDateTime(int row) >;
	using AppliedFn = std::function< bool(int row) >;
	using FlagFn    = std::function< bool() >;

	UndoRowDelegate(FlagFn timestamps, FlagFn icons, StampFn stamp, AppliedFn applied, QObject* parent)
		: QStyledItemDelegate(parent)
		, mTimestampsOn(std::move(timestamps))
		, mIconsOn(std::move(icons))
		, mStamp(std::move(stamp))
		, mApplied(std::move(applied))
	{
	}

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
	{
		const bool timestamps = mTimestampsOn && mTimestampsOn();
		const bool icons      = mIconsOn && mIconsOn();
		if (!timestamps && !icons) {
			QStyledItemDelegate::paint(painter, option, index);
			return;
		}
		QStyleOptionViewItem opt(option);
		initStyleOption(&opt, index);
		const QRect full = opt.rect;
		const QFontMetrics fm(opt.font);
		QString timeText;
		QRect timeRect;
		if (timestamps) {
			const QDateTime dt = mStamp ? mStamp(index.row()) : QDateTime();
			if (dt.isValid()) {
				timeText       = dt.toString(QStringLiteral("HH:mm:ss"));
				const int tw   = Qt5Qt6Compat_fontMetrics_width(fm, timeText);
				timeRect       = QRect(full.right() - tw - 2, full.top(), tw, full.height());
				opt.rect.setRight(full.right() - tw - 8);
			}
		}
		if (icons)
			opt.rect.setLeft(full.left() + 14);
		QStyledItemDelegate::paint(painter, opt, index);

		painter->save();
		painter->setRenderHint(QPainter::Antialiasing);
		if (icons && mApplied) {
			const QRect dot(full.left() + 6, full.center().y() - 3, 7, 7);
			if (mApplied(index.row())) {
				painter->setPen(Qt::NoPen);
				painter->setBrush(QColor(0x67, 0xC2, 0x3A));
			} else {
				painter->setPen(QPen(QColor(0xC0, 0xC4, 0xCC), 1.2));
				painter->setBrush(Qt::NoBrush);
			}
			painter->drawEllipse(dot);
		}
		if (!timeText.isEmpty()) {
			painter->setPen(QColor(0x90, 0x93, 0x99));
			painter->drawText(timeRect, Qt::AlignRight | Qt::AlignVCenter, timeText);
		}
		painter->restore();
	}

private:
	FlagFn mTimestampsOn;
	FlagFn mIconsOn;
	StampFn mStamp;
	AppliedFn mApplied;
};
}  // namespace

/**
 * @brief 构造：撤销深度调节行 + QUndoView
 * @param parent 父对象
 */
DAUndoHistoryWidget::DAUndoHistoryWidget(QWidget* parent) : QWidget(parent)
{
	QVBoxLayout* main = new QVBoxLayout(this);
	main->setContentsMargins(4, 4, 4, 4);
	// 深度调节行（R19：默认 20，面板可调大；配置持久化在 APP 侧）
	QHBoxLayout* limitRow = new QHBoxLayout();
	QLabel* label         = new QLabel(tr("Undo depth:"), this);  // cn:撤销深度：
	mSpinLimit            = new QSpinBox(this);
	mSpinLimit->setObjectName(QStringLiteral("undoLimitSpin"));
	mSpinLimit->setRange(1, 10000);
	mSpinLimit->setValue(20);
	mSpinLimit->setToolTip(tr("Maximum number of undo steps kept per table page (larger uses more memory for swap-type commands)"));  // cn:每个表格页保留的最大撤销步数（调大对转置类命令会增加内存占用）
	limitRow->addWidget(label);
	limitRow->addWidget(mSpinLimit);
	limitRow->addStretch();
	main->addLayout(limitRow);
	// 历史列表（QUndoView：跟随激活栈、点击条目回退/前进、显示命令描述）
	mView = new QUndoView(this);
	mView->setObjectName(QStringLiteral("undoHistoryView"));
	mView->setItemDelegate(new UndoRowDelegate(
		[ this ]() { return mTimestampsVisible; }, [ this ]() { return mIconsVisible; },
		[ this ](int row) {
			const int idx = rowToCommandIndex(row);
			return (idx >= 0 && idx < mStamps.size()) ? mStamps.at(idx) : QDateTime();
		},
		[ this ](int row) { return isRowApplied(row); }, mView));
	main->addWidget(mView, 1);
	connect(mSpinLimit, QOverload< int >::of(&QSpinBox::valueChanged), this, [ this ](int v) {
		if (mUpdatingLimit) {
			return;
		}
		Q_EMIT undoLimitChangeRequested(v);
	});
	if (QAbstractItemModel* model = mView->model()) {
		connect(model, &QAbstractItemModel::modelReset, this, [ this ]() { refilterRows(); });
		connect(model, &QAbstractItemModel::rowsInserted, this, [ this ]() { refilterRows(); });
		connect(model, &QAbstractItemModel::rowsRemoved, this, [ this ]() { refilterRows(); });
	}
}

DAUndoHistoryWidget::~DAUndoHistoryWidget()
{
}

/**
 * @brief 绑定全局撤销组（视图自动跟随组内激活栈）
 * @param group QUndoGroup（非拥有，可空=显示空列表）
 */
void DAUndoHistoryWidget::setUndoGroup(QUndoGroup* group)
{
	if (!mView) {
		return;
	}
	if (QUndoGroup* old = mView->group())
		disconnect(old, &QUndoGroup::activeStackChanged, this, &DAUndoHistoryWidget::attachStack);
	mView->setGroup(group);
	if (group) {
		connect(group, &QUndoGroup::activeStackChanged, this, &DAUndoHistoryWidget::attachStack,
				Qt::UniqueConnection);
		attachStack(group->activeStack());
	} else {
		attachStack(nullptr);
	}
}

QUndoGroup* DAUndoHistoryWidget::undoGroup() const
{
	return mView ? mView->group() : nullptr;
}

int DAUndoHistoryWidget::undoLimit() const
{
	return mSpinLimit ? mSpinLimit->value() : 20;
}

/**
 * @brief 设置深度显示值（APP 读配置注入；屏蔽信号避免回环写配置）
 * @param limit 撤销深度
 */
void DAUndoHistoryWidget::setUndoLimit(int limit)
{
	if (!mSpinLimit) {
		return;
	}
	mUpdatingLimit = true;
	mSpinLimit->setValue(qBound(mSpinLimit->minimum(), limit, mSpinLimit->maximum()));
	mUpdatingLimit = false;
}

/**
 * @brief 打开/关闭每行右侧时间
 * @param on 是否显示（默认 false）
 */
void DAUndoHistoryWidget::setTimestampsVisible(bool on)
{
	if (mTimestampsVisible == on)
		return;
	mTimestampsVisible = on;
	if (mView)
		mView->viewport()->update();
}

/** @brief 时间显示是否启用 */
bool DAUndoHistoryWidget::timestampsVisible() const
{
	return mTimestampsVisible;
}

/**
 * @brief 打开/关闭每行左侧状态点
 * @param on 是否显示（默认 false）
 */
void DAUndoHistoryWidget::setIconsVisible(bool on)
{
	if (mIconsVisible == on)
		return;
	mIconsVisible = on;
	if (mView)
		mView->viewport()->update();
}

/** @brief 状态点是否启用 */
bool DAUndoHistoryWidget::iconsVisible() const
{
	return mIconsVisible;
}

/**
 * @brief 打开/关闭空命令隐藏
 * @param on 是否隐藏文本为空的命令行（默认 false）
 */
void DAUndoHistoryWidget::setEmptyCommandFilterEnabled(bool on)
{
	if (mEmptyFilterEnabled == on)
		return;
	mEmptyFilterEnabled = on;
	refilterRows();
}

/** @brief 空命令隐藏是否启用 */
bool DAUndoHistoryWidget::isEmptyCommandFilterEnabled() const
{
	return mEmptyFilterEnabled;
}

/**
 * @brief 查询某条命令的入栈时刻
 * @param index 与 QUndoStack 下标一致
 * @return 本地时间；未记录或越界返回无效值
 */
QDateTime DAUndoHistoryWidget::commandTimestamp(int index) const
{
	if (index < 0 || index >= mStamps.size())
		return QDateTime();
	return mStamps.at(index);
}

int DAUndoHistoryWidget::rowToCommandIndex(int row) const
{
	// QUndoModel: row 0 = 初始状态，row r = 命令 r-1
	if (!mWatchedStack || row <= 0)
		return -1;
	const int idx = row - 1;
	if (idx >= mWatchedStack->count())
		return -1;
	return idx;
}

bool DAUndoHistoryWidget::isRowApplied(int row) const
{
	const int idx = rowToCommandIndex(row);
	return idx >= 0 && mWatchedStack && idx < mWatchedStack->index();
}

void DAUndoHistoryWidget::attachStack(QUndoStack* stack)
{
	if (mWatchedStack)
		disconnect(mWatchedStack, nullptr, this, nullptr);
	mWatchedStack = stack;
	mStamps.clear();
	if (!stack) {
		refilterRows();
		return;
	}
	// 已有历史补一份当前时间（用户中途打开显示也能看到时间）
	const QDateTime now = QDateTime::currentDateTime();
	for (int i = 0; i < stack->count(); ++i)
		mStamps.append(now);
	connect(stack, &QUndoStack::indexChanged, this, [ this ](int) {
		noteStackChange(mWatchedStack ? mWatchedStack->count() : 0);
		refilterRows();
	});
	refilterRows();
}

void DAUndoHistoryWidget::noteStackChange(int count)
{
	if (count > mStamps.size()) {
		while (mStamps.size() < count)
			mStamps.append(QDateTime::currentDateTime());
	} else if (count < mStamps.size()) {
		// redo 分支被截断：旧时间作废
		mStamps.resize(count);
	}
}

void DAUndoHistoryWidget::refilterRows()
{
	if (!mView || !mView->model())
		return;
	const int rows = mView->model()->rowCount();
	for (int row = 0; row < rows; ++row) {
		bool hide = false;
		const int idx = rowToCommandIndex(row);
		if (mEmptyFilterEnabled && idx >= 0 && mWatchedStack)
			hide = mWatchedStack->text(idx).trimmed().isEmpty();
		mView->setRowHidden(row, hide);
	}
}

}  // end of namespace DA

/*** End of inlined file: DAUndoHistoryWidget.cpp ***/

// ---- Property Editing 属性编辑家族 ----

/*** Start of inlined file: DAPropertyItemWidget.cpp ***/
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolTip>
#include <QEvent>

namespace DA
{
class DAPropertyItemWidget::PrivateData
{
	DA_DECLARE_PUBLIC(DAPropertyItemWidget)
public:
	PrivateData(DAPropertyItemWidget* p);

	int mPropertyId = -1;
	QString mPropertyName;
	QString mPropertyDescription;
	QWidget* mEditorWidget                       = nullptr;
	DAPropertyItemWidget::LayoutMode mLayoutMode = DAPropertyItemWidget::InlineLayout;
	int mNameLabelWidth                          = 100;
	bool mFrameVisible                           = false;

	QLabel* mLabelName      = nullptr;
	QBoxLayout* mMainLayout = nullptr;

	void setupInlineLayout();
	void setupBelowLayout();
	void clearLayout();
	void updateTooltip();
};

//===================================================
// DAPropertyItemWidget::PrivateData
//===================================================

DAPropertyItemWidget::PrivateData::PrivateData(DAPropertyItemWidget* p) : q_ptr(p)
{
	if (p->objectName().isEmpty()) {
		p->setObjectName(QStringLiteral("DAPropertyItemWidget"));
	}
	// 默认Frame不可见
	p->setFrameShape(QFrame::NoFrame);

	// 创建属性名标签
	mLabelName = new QLabel(p);
	mLabelName->setWordWrap(true);
	mLabelName->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	mLabelName->setFixedWidth(mNameLabelWidth);
	mLabelName->setObjectName(QStringLiteral("labelPropertyName"));

	// 默认使用Inline布局
	setupInlineLayout();
}

void DAPropertyItemWidget::PrivateData::setupInlineLayout()
{
	clearLayout();

	mMainLayout = new QHBoxLayout(q_ptr);
	mMainLayout->setContentsMargins(2, 2, 2, 2);
	mMainLayout->setSpacing(4);
	mMainLayout->setObjectName(QStringLiteral("inlineLayout"));

	mLabelName->setFixedWidth(mNameLabelWidth);
	mLabelName->setWordWrap(true);
	mLabelName->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	mMainLayout->addWidget(mLabelName);

	if (mEditorWidget) {
		if (mEditorWidget->sizePolicy().horizontalPolicy() & QSizePolicy::ExpandFlag) {
			// 水平可扩展的编辑器（如 QSpinBox/QDoubleSpinBox）：stretch=1 让其填满剩余空间
			mMainLayout->addWidget(mEditorWidget, 1, Qt::AlignTop);
		} else {
			// 水平不可扩展的编辑器（如 DAColorPickerButton/QToolButton）：保持 sizeHint 宽度并左对齐，
			// 末尾加 stretch 吸收剩余空间，避免 editor 在 stretch=1 的大 cell 中水平居中
			mMainLayout->addWidget(mEditorWidget, 0, Qt::AlignTop);
			mMainLayout->addStretch(1);
		}
	}

	q_ptr->setLayout(mMainLayout);
}

void DAPropertyItemWidget::PrivateData::setupBelowLayout()
{
	clearLayout();

	mMainLayout = new QVBoxLayout(q_ptr);
	mMainLayout->setContentsMargins(2, 2, 2, 2);
	mMainLayout->setSpacing(2);
	mMainLayout->setObjectName(QStringLiteral("belowLayout"));

	// Below模式下标签不需要固定宽度，自适应宽度
	mLabelName->setFixedWidth(QWIDGETSIZE_MAX);
	mLabelName->setWordWrap(false);
	mLabelName->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	mMainLayout->addWidget(mLabelName);

	if (mEditorWidget) {
		mMainLayout->addWidget(mEditorWidget, 1);
	}

	q_ptr->setLayout(mMainLayout);
}

void DAPropertyItemWidget::PrivateData::clearLayout()
{
	if (!mMainLayout) {
		return;
	}
	// 移除所有子项但保留mLabelName和mEditorWidget
	while (mMainLayout->count() > 0) {
		QLayoutItem* item = mMainLayout->takeAt(0);
		if (item->widget() != mLabelName && item->widget() != mEditorWidget) {
			delete item;
		} else {
			// 不删除mLabelName和mEditorWidget对应的layout item
			// 但需要从布局移除
			delete item;
		}
	}
	// 删除旧布局（QLayout删除时会清理）
	QLayout* oldLayout = q_ptr->layout();
	if (oldLayout) {
		// 先移除控件避免被layout删除
		if (mLabelName) {
			oldLayout->removeWidget(mLabelName);
		}
		if (mEditorWidget) {
			oldLayout->removeWidget(mEditorWidget);
		}
		delete oldLayout;
	}
	mMainLayout = nullptr;
}

void DAPropertyItemWidget::PrivateData::updateTooltip()
{
	if (mLabelName) {
		if (!mPropertyDescription.isEmpty()) {
			mLabelName->setToolTip(mPropertyDescription);
		} else {
			mLabelName->setToolTip(QString());
		}
	}
}

//===================================================
// DAPropertyItemWidget
//===================================================

/**
 * @brief 构造属性项控件
 * @param parent 父控件
 * @details 初始化 PIMPL 私有数据，默认创建 Inline 布局和属性名标签
 */
DAPropertyItemWidget::DAPropertyItemWidget(QWidget* parent) : QFrame(parent), DA_PIMPL_CONSTRUCT
{
}

/**
 * @brief 析构函数
 * @details 通过 PIMPL 自动释放私有数据
 */
DAPropertyItemWidget::~DAPropertyItemWidget()
{
}

/**
 * @brief 获取属性ID
 *
 * 返回当前属性项的唯一标识符。如果未通过setPropertyId()设置，
 * 默认值为-1表示未分配ID。
 *
 * @return 属性ID
 * @see setPropertyId
 */
int DAPropertyItemWidget::propertyId() const
{
	DA_DC(d);
	return d->mPropertyId;
}

/**
 * @brief 设置属性ID
 *
 * @param[in] id 属性ID，-1表示未分配
 * @see propertyId
 */
void DAPropertyItemWidget::setPropertyId(int id)
{
	DA_D(d);
	d->mPropertyId = id;
}

/**
 * @brief 获取属性名称
 *
 * @return 属性名称文本
 * @see setPropertyName
 */
QString DAPropertyItemWidget::propertyName() const
{
	DA_DC(d);
	return d->mPropertyName;
}

/**
 * @brief 设置属性名称
 *
 * 设置属性名标签的文本内容。在Inline模式下，文本会在固定宽度内换行显示；
 * 在Below模式下，文本自适应宽度显示。
 *
 * @param[in] name 属性名称
 * @see propertyName
 */
void DAPropertyItemWidget::setPropertyName(const QString& name)
{
	DA_D(d);
	d->mPropertyName = name;
	if (d->mLabelName) {
		d->mLabelName->setText(name);
	}
}

/**
 * @brief 获取属性描述
 *
 * 属性描述作为属性名标签的tooltip显示。
 *
 * @return 属性描述文本
 * @see setPropertyDescription
 */
QString DAPropertyItemWidget::propertyDescription() const
{
	DA_DC(d);
	return d->mPropertyDescription;
}

/**
 * @brief 设置属性描述
 *
 * 设置属性名标签的tooltip文本。鼠标悬停在属性名标签上时显示此描述。
 * Inline和Below模式统一使用tooltip显示描述信息。
 *
 * @param[in] desc 属性描述文本
 * @see propertyDescription
 */
void DAPropertyItemWidget::setPropertyDescription(const QString& desc)
{
	DA_D(d);
	d->mPropertyDescription = desc;
	d->updateTooltip();
}

/**
 * @brief 获取编辑Widget
 *
 * @return 编辑Widget指针，未设置则返回nullptr
 * @see setEditorWidget
 */
QWidget* DAPropertyItemWidget::editorWidget() const
{
	DA_DC(d);
	return d->mEditorWidget;
}

/**
 * @brief 设置编辑Widget
 *
 * 设置属性项的编辑Widget。此方法会接管widget的父子关系，
 * 将其设为DAPropertyItemWidget的子控件，并添加到当前布局中。
 *
 * 如果之前已设置编辑Widget，旧Widget会被移除但不会删除，
 * 调用者需自行管理旧Widget的生命周期。
 *
 * @param[in] widget 编辑Widget指针
 * @see editorWidget
 */
void DAPropertyItemWidget::setEditorWidget(QWidget* widget)
{
	DA_D(d);
	// 移除旧编辑Widget
	if (d->mEditorWidget) {
		d->mEditorWidget->setParent(nullptr);
	}
	d->mEditorWidget = widget;
	if (widget) {
		widget->setParent(this);
		widget->setObjectName(QStringLiteral("editorWidget"));
	}
	// 重建布局：setupInlineLayout 会根据 editor 的水平 sizePolicy 决定是否在末尾追加 stretch，
	// 保证非 Expanding 编辑器（如 DAColorPickerButton）左对齐，而非在大 cell 中水平居中
	if (d->mMainLayout) {
		updateLayout();
	}
}

/**
 * @brief 获取布局模式
 *
 * @return 当前布局模式
 * @see setLayoutMode
 */
DAPropertyItemWidget::LayoutMode DAPropertyItemWidget::layoutMode() const
{
	DA_DC(d);
	return d->mLayoutMode;
}

/**
 * @brief 设置布局模式
 *
 * 支持动态切换Inline和Below布局模式。切换时会重建布局结构，
 * 保留属性名标签和编辑Widget。
 *
 * @param[in] mode 布局模式
 * @see layoutMode
 */
void DAPropertyItemWidget::setLayoutMode(LayoutMode mode)
{
	DA_D(d);
	if (d->mLayoutMode == mode) {
		return;
	}
	d->mLayoutMode = mode;
	if (mode == InlineLayout) {
		d->setupInlineLayout();
	} else {
		d->setupBelowLayout();
	}
}

/**
 * @brief 获取Frame边界可见性
 *
 * @return true表示Frame可见，false表示不可见（默认）
 * @see setFrameVisible
 */
bool DAPropertyItemWidget::isFrameVisible() const
{
	DA_DC(d);
	return d->mFrameVisible;
}

/**
 * @brief 设置Frame边界可见性
 *
 * 默认Frame不可见（NoFrame）。设置为可见时使用StyledPanel样式。
 *
 * @param[in] visible 是否可见
 * @see isFrameVisible
 */
void DAPropertyItemWidget::setFrameVisible(bool visible)
{
	DA_D(d);
	d->mFrameVisible = visible;
	if (visible) {
		setFrameShape(QFrame::StyledPanel);
	} else {
		setFrameShape(QFrame::NoFrame);
	}
}

/**
 * @brief 设置属性名标签宽度
 *
 * 仅在Inline模式下有效。设置属性名标签的固定宽度，
 * 超出此宽度的文本会自动换行显示。
 *
 * @param[in] width 标签宽度（像素）
 */
void DAPropertyItemWidget::setNameLabelWidth(int width)
{
	DA_D(d);
	d->mNameLabelWidth = width;
	if (d->mLayoutMode == InlineLayout && d->mLabelName) {
		d->mLabelName->setFixedWidth(width);
	}
}

void DAPropertyItemWidget::updateLayout()
{
	DA_D(d);
	if (d->mLayoutMode == InlineLayout) {
		d->setupInlineLayout();
	} else {
		d->setupBelowLayout();
	}
}

}  // namespace DA

/*** End of inlined file: DAPropertyItemWidget.cpp ***/



/*** Start of inlined file: DATimeFormatEditWidget.cpp ***/

/*** Start of inlined file: ui_DATimeFormatEditWidget.h ***/
#ifndef UI_DATIMEFORMATEDITWIDGET_H
#define UI_DATIMEFORMATEDITWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DATimeFormatEditWidget
{
public:
	QVBoxLayout *verticalLayout;
	QHBoxLayout *horizontalLayout;
	QLabel *label1;
	QComboBox *comboBox;
	QLabel *label2;
	QLabel *labelExample;

	void setupUi(QWidget *DATimeFormatEditWidget)
	{
		if (DATimeFormatEditWidget->objectName().isEmpty())
			DATimeFormatEditWidget->setObjectName("DATimeFormatEditWidget");
		DATimeFormatEditWidget->resize(289, 108);
		QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Maximum);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(DATimeFormatEditWidget->sizePolicy().hasHeightForWidth());
		DATimeFormatEditWidget->setSizePolicy(sizePolicy);
		DATimeFormatEditWidget->setMinimumSize(QSize(0, 108));
		verticalLayout = new QVBoxLayout(DATimeFormatEditWidget);
		verticalLayout->setObjectName("verticalLayout");
		horizontalLayout = new QHBoxLayout();
		horizontalLayout->setObjectName("horizontalLayout");
		label1 = new QLabel(DATimeFormatEditWidget);
		label1->setObjectName("label1");

		horizontalLayout->addWidget(label1);

		comboBox = new QComboBox(DATimeFormatEditWidget);
		comboBox->setObjectName("comboBox");
		comboBox->setEditable(true);

		horizontalLayout->addWidget(comboBox);

		verticalLayout->addLayout(horizontalLayout);

		label2 = new QLabel(DATimeFormatEditWidget);
		label2->setObjectName("label2");

		verticalLayout->addWidget(label2);

		labelExample = new QLabel(DATimeFormatEditWidget);
		labelExample->setObjectName("labelExample");
		QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
		sizePolicy1.setHorizontalStretch(0);
		sizePolicy1.setVerticalStretch(0);
		sizePolicy1.setHeightForWidth(labelExample->sizePolicy().hasHeightForWidth());
		labelExample->setSizePolicy(sizePolicy1);
		labelExample->setMinimumSize(QSize(0, 40));
		labelExample->setFrameShape(QFrame::StyledPanel);
		labelExample->setFrameShadow(QFrame::Plain);
		labelExample->setMidLineWidth(0);
		labelExample->setAlignment(Qt::AlignCenter);

		verticalLayout->addWidget(labelExample);

		retranslateUi(DATimeFormatEditWidget);

		QMetaObject::connectSlotsByName(DATimeFormatEditWidget);
	} // setupUi

	void retranslateUi(QWidget *DATimeFormatEditWidget)
	{
		DATimeFormatEditWidget->setWindowTitle(QCoreApplication::translate("DATimeFormatEditWidget", "Time Format Set", nullptr));
		label1->setText(QCoreApplication::translate("DATimeFormatEditWidget", "select time format\357\274\232", nullptr));
		label2->setText(QCoreApplication::translate("DATimeFormatEditWidget", "example:", nullptr));
		labelExample->setText(QCoreApplication::translate("DATimeFormatEditWidget", "TextLabel", nullptr));
	} // retranslateUi

};

namespace Ui {
	class DATimeFormatEditWidget: public Ui_DATimeFormatEditWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DATIMEFORMATEDITWIDGET_H

/*** End of inlined file: ui_DATimeFormatEditWidget.h ***/

#include <QComboBox>
namespace DA
{

/**
 * @brief 构造时间格式编辑窗口
 * @param parent 父控件
 * @details 加载 ui，填充常用时间格式选项到下拉框并连接文本变化信号，
 *          默认选中最后一个（最完整的日期时间）格式
 */
DATimeFormatEditWidget::DATimeFormatEditWidget(QWidget* parent)
	: QWidget(parent), ui(new Ui::DATimeFormatEditWidget), m_currentDateTime(QDateTime::currentDateTime())
{
	ui->setupUi(this);
	setComboxTimeFormat(ui->comboBox);
	connect(ui->comboBox, &QComboBox::currentTextChanged, this, &DATimeFormatEditWidget::onCurrentTextChanged);
	ui->comboBox->setCurrentIndex(ui->comboBox->count() - 1);
}

/**
 * @brief 析构函数
 * @details 释放 ui 指针
 */
DATimeFormatEditWidget::~DATimeFormatEditWidget()
{
	delete ui;
}

/**
 * @brief 获取当前时间格式
 * @return 下拉框当前选中的时间格式字符串
 */
QString DATimeFormatEditWidget::getTimeFormat() const
{
	return ui->comboBox->currentText();
}

/**
 * @brief 设置时间格式文本
 * @param str 时间格式字符串
 * @details 设置下拉框的当前文本，若匹配已有项则选中对应项
 */
void DATimeFormatEditWidget::setTimeFormatText(const QString& str)
{
	ui->comboBox->setCurrentText(str);
}

void DATimeFormatEditWidget::onCurrentTextChanged(const QString& text)
{
	ui->labelExample->setText(m_currentDateTime.toString(text));
}

void DATimeFormatEditWidget::setComboxTimeFormat(QComboBox* comb)
{
	comb->clear();
	comb->addItem(QString("h:m"), QVariant(QString("h:m")));
	comb->setItemData(comb->count() - 1, tr("example: 8:5"), Qt::ToolTipRole);

	comb->addItem(QString("hh:mm"), QVariant(QString("hh:mm")));
	comb->setItemData(comb->count() - 1, tr("example: 08:05"), Qt::ToolTipRole);

	comb->addItem(QString("h:m:s"), QVariant(QString("h:m:s")));
	comb->setItemData(comb->count() - 1, tr("example: 8:5:12"), Qt::ToolTipRole);

	comb->addItem(QString("hh:mm:ss"), QVariant(QString("hh:mm:ss")));
	comb->setItemData(comb->count() - 1, tr("example: 08:05:12"), Qt::ToolTipRole);

	comb->addItem(QString("yyyy-M-d"), QVariant(QString("yyyy-M-d")));
	comb->setItemData(comb->count() - 1, tr("example: 2017-8-1"), Qt::ToolTipRole);

	comb->addItem(QString("yyyy-M-d h:m"), QVariant(QString("yyyy-M-d h:m")));
	comb->setItemData(comb->count() - 1, tr("example: 2017-8-1 8:5"), Qt::ToolTipRole);

	comb->addItem(QString("yyyy-M-d h:m:s"), QVariant(QString("yyyy-M-d h:m:s")));
	comb->setItemData(comb->count() - 1, tr("example: 2017-8-1 8:5:12"), Qt::ToolTipRole);

	comb->addItem(QString("yyyy-MM-dd"), QVariant(QString("yyyy-MM-dd")));
	comb->setItemData(comb->count() - 1, tr("example: 2017-08-01"), Qt::ToolTipRole);

	comb->addItem(QString("yyyy-MM-dd hh:mm"), QVariant(QString("yyyy-MM-dd hh:mm")));
	comb->setItemData(comb->count() - 1, tr("example: 2017-08-01 08:05"), Qt::ToolTipRole);

	comb->addItem(QString("yyyy-MM-dd hh:mm:ss"), QVariant(QString("yyyy-MM-dd hh:mm:ss")));
	comb->setItemData(comb->count() - 1, tr("example: 2017-08-01 08:05:12"), Qt::ToolTipRole);
}

}  // end DA

/*** End of inlined file: DATimeFormatEditWidget.cpp ***/


/*** Start of inlined file: DAAligmentEditWidget.cpp ***/

/*** Start of inlined file: ui_DAAligmentEditWidget.h ***/
#ifndef UI_DAALIGMENTEDITWIDGET_H
#define UI_DAALIGMENTEDITWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DAAligmentEditWidget
{
public:
	QHBoxLayout *horizontalLayout;
	QToolButton *toolButtonAligmentLeft;
	QToolButton *toolButtonAligmentRight;
	QToolButton *toolButtonAligmentTop;
	QToolButton *toolButtonAligmentBottom;
	QToolButton *toolButtonAligmentCenter;
	QSpacerItem *horizontalSpacer;
	QButtonGroup *buttonGroup;

	void setupUi(QWidget *DAAligmentEditWidget)
	{
		if (DAAligmentEditWidget->objectName().isEmpty())
			DAAligmentEditWidget->setObjectName("DAAligmentEditWidget");
		DAAligmentEditWidget->resize(122, 24);
		DAAligmentEditWidget->setMinimumSize(QSize(100, 20));
		horizontalLayout = new QHBoxLayout(DAAligmentEditWidget);
		horizontalLayout->setSpacing(1);
		horizontalLayout->setObjectName("horizontalLayout");
		horizontalLayout->setContentsMargins(1, 1, 1, 1);
		toolButtonAligmentLeft = new QToolButton(DAAligmentEditWidget);
		buttonGroup = new QButtonGroup(DAAligmentEditWidget);
		buttonGroup->setObjectName("buttonGroup");
		buttonGroup->addButton(toolButtonAligmentLeft);
		toolButtonAligmentLeft->setObjectName("toolButtonAligmentLeft");
		QIcon icon;
		icon.addFile(QString::fromUtf8(":/DAWidgetsIcons/AligmentLeft.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonAligmentLeft->setIcon(icon);
		toolButtonAligmentLeft->setCheckable(true);

		horizontalLayout->addWidget(toolButtonAligmentLeft);

		toolButtonAligmentRight = new QToolButton(DAAligmentEditWidget);
		buttonGroup->addButton(toolButtonAligmentRight);
		toolButtonAligmentRight->setObjectName("toolButtonAligmentRight");
		QIcon icon1;
		icon1.addFile(QString::fromUtf8(":/DAWidgetsIcons/AligmentRight.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonAligmentRight->setIcon(icon1);
		toolButtonAligmentRight->setCheckable(true);

		horizontalLayout->addWidget(toolButtonAligmentRight);

		toolButtonAligmentTop = new QToolButton(DAAligmentEditWidget);
		buttonGroup->addButton(toolButtonAligmentTop);
		toolButtonAligmentTop->setObjectName("toolButtonAligmentTop");
		QIcon icon2;
		icon2.addFile(QString::fromUtf8(":/DAWidgetsIcons/AligmentTop.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonAligmentTop->setIcon(icon2);
		toolButtonAligmentTop->setCheckable(true);

		horizontalLayout->addWidget(toolButtonAligmentTop);

		toolButtonAligmentBottom = new QToolButton(DAAligmentEditWidget);
		buttonGroup->addButton(toolButtonAligmentBottom);
		toolButtonAligmentBottom->setObjectName("toolButtonAligmentBottom");
		QIcon icon3;
		icon3.addFile(QString::fromUtf8(":/DAWidgetsIcons/AligmentBottom.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonAligmentBottom->setIcon(icon3);
		toolButtonAligmentBottom->setCheckable(true);

		horizontalLayout->addWidget(toolButtonAligmentBottom);

		toolButtonAligmentCenter = new QToolButton(DAAligmentEditWidget);
		buttonGroup->addButton(toolButtonAligmentCenter);
		toolButtonAligmentCenter->setObjectName("toolButtonAligmentCenter");
		QIcon icon4;
		icon4.addFile(QString::fromUtf8(":/DAWidgetsIcons/AligmentCenter.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonAligmentCenter->setIcon(icon4);
		toolButtonAligmentCenter->setCheckable(true);

		horizontalLayout->addWidget(toolButtonAligmentCenter);

		horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

		horizontalLayout->addItem(horizontalSpacer);

		retranslateUi(DAAligmentEditWidget);

		QMetaObject::connectSlotsByName(DAAligmentEditWidget);
	} // setupUi

	void retranslateUi(QWidget *DAAligmentEditWidget)
	{
		DAAligmentEditWidget->setWindowTitle(QCoreApplication::translate("DAAligmentEditWidget", "Aligment Edit Widget", nullptr));
		toolButtonAligmentLeft->setText(QCoreApplication::translate("DAAligmentEditWidget", "Aligment Left", nullptr));
		toolButtonAligmentRight->setText(QCoreApplication::translate("DAAligmentEditWidget", "Aligment Right", nullptr));
		toolButtonAligmentTop->setText(QCoreApplication::translate("DAAligmentEditWidget", "Aligment Top", nullptr));
		toolButtonAligmentBottom->setText(QCoreApplication::translate("DAAligmentEditWidget", "Aligment Bottom", nullptr));
		toolButtonAligmentCenter->setText(QCoreApplication::translate("DAAligmentEditWidget", "Aligment Center", nullptr));
	} // retranslateUi

};

namespace Ui {
	class DAAligmentEditWidget: public Ui_DAAligmentEditWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DAALIGMENTEDITWIDGET_H

/*** End of inlined file: ui_DAAligmentEditWidget.h ***/

namespace DA
{

/**
 * @brief 构造对齐编辑窗口
 * @param parent 父控件
 * @details 加载 ui 并将各对齐按钮的 buttonGroup id 绑定到对应的 Qt::Alignment 枚举值
 */
DAAligmentEditWidget::DAAligmentEditWidget(QWidget* parent) : QWidget(parent), ui(new Ui::DAAligmentEditWidget)
{
	ui->setupUi(this);
	Qt5Qt6Compat_Connect_ButtonGroupClicked_int(ui->buttonGroup, DAAligmentEditWidget::onButtonGroupClicked);
	ui->buttonGroup->setId(ui->toolButtonAligmentBottom, static_cast< int >(Qt::AlignBottom));
	ui->buttonGroup->setId(ui->toolButtonAligmentTop, static_cast< int >(Qt::AlignTop));
	ui->buttonGroup->setId(ui->toolButtonAligmentLeft, static_cast< int >(Qt::AlignLeft));
	ui->buttonGroup->setId(ui->toolButtonAligmentRight, static_cast< int >(Qt::AlignRight));
	ui->buttonGroup->setId(ui->toolButtonAligmentCenter, static_cast< int >(Qt::AlignCenter));
}

/**
 * @brief 析构函数
 * @details 释放 ui 指针
 */
DAAligmentEditWidget::~DAAligmentEditWidget()
{
	delete ui;
}

/**
 * @brief 设置对其
 * @param al
 */
void DAAligmentEditWidget::setCurrentAlignment(Qt::Alignment al)
{
	auto cur = getCurrentAlignment();
	if (cur == al) {
		return;
	}
	if (al.testFlag(Qt::AlignLeft)) {
		ui->toolButtonAligmentLeft->setChecked(true);
	} else if (al.testFlag(Qt::AlignRight)) {
		ui->toolButtonAligmentRight->setChecked(true);
	} else if (al.testFlag(Qt::AlignTop)) {
		ui->toolButtonAligmentTop->setChecked(true);
	} else if (al.testFlag(Qt::AlignBottom)) {
		ui->toolButtonAligmentBottom->setChecked(true);
	} else if (al.testFlag(Qt::AlignCenter) || al.testFlag(Qt::AlignHCenter) || al.testFlag(Qt::AlignVCenter)) {
		if (cur.testFlag(Qt::AlignCenter) || cur.testFlag(Qt::AlignHCenter) || cur.testFlag(Qt::AlignVCenter)) {
			// 这个相当于cur == al
			return;
		}
		ui->toolButtonAligmentCenter->setChecked(true);
	}
	emit alignmentChanged(al);
}

/**
 * @brief DAAligmentEditWidget::getCurrentAlignment
 * @return
 */
Qt::Alignment DAAligmentEditWidget::getCurrentAlignment() const
{
	if (ui->toolButtonAligmentLeft->isChecked()) {
		return Qt::AlignLeft;
	} else if (ui->toolButtonAligmentRight->isChecked()) {
		return Qt::AlignRight;
	} else if (ui->toolButtonAligmentTop->isChecked()) {
		return Qt::AlignTop;
	} else if (ui->toolButtonAligmentBottom->isChecked()) {
		return Qt::AlignBottom;
	} else if (ui->toolButtonAligmentCenter->isChecked()) {
		return Qt::AlignCenter;
	}
	return Qt::AlignCenter;
}

void DAAligmentEditWidget::changeEvent(QEvent* e)
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

void DAAligmentEditWidget::onButtonGroupClicked(int id)
{
	Qt::Alignment al = static_cast< Qt::Alignment >(id);
	emit alignmentChanged(al);
}
}  // end da

/*** End of inlined file: DAAligmentEditWidget.cpp ***/


/*** Start of inlined file: DAAligmentPositionEditWidget.cpp ***/

/*** Start of inlined file: ui_DAAligmentPositionEditWidget.h ***/
#ifndef UI_DAALIGMENTPOSITIONEDITWIDGET_H
#define UI_DAALIGMENTPOSITIONEDITWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DAAligmentPositionEditWidget
{
public:
	QGridLayout *gridLayout;
	QSpacerItem *verticalSpacer_2;
	QToolButton *toolButtonTopLeft;
	QToolButton *toolButtonTop;
	QToolButton *toolButtonTopRight;
	QSpacerItem *horizontalSpacer;
	QToolButton *toolButtonLeft;
	QToolButton *toolButtonCenter;
	QToolButton *toolButtonRight;
	QSpacerItem *horizontalSpacer_2;
	QToolButton *toolButtonBottomLeft;
	QToolButton *toolButtonBottom;
	QToolButton *toolButtonBottomRight;
	QSpacerItem *verticalSpacer;
	QButtonGroup *buttonGroup;

	void setupUi(QWidget *DAAligmentPositionEditWidget)
	{
		if (DAAligmentPositionEditWidget->objectName().isEmpty())
			DAAligmentPositionEditWidget->setObjectName("DAAligmentPositionEditWidget");
		DAAligmentPositionEditWidget->resize(92, 82);
		DAAligmentPositionEditWidget->setMinimumSize(QSize(80, 70));
		gridLayout = new QGridLayout(DAAligmentPositionEditWidget);
		gridLayout->setSpacing(3);
		gridLayout->setObjectName("gridLayout");
		gridLayout->setContentsMargins(3, 3, 3, 3);
		verticalSpacer_2 = new QSpacerItem(20, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

		gridLayout->addItem(verticalSpacer_2, 0, 2, 1, 1);

		toolButtonTopLeft = new QToolButton(DAAligmentPositionEditWidget);
		buttonGroup = new QButtonGroup(DAAligmentPositionEditWidget);
		buttonGroup->setObjectName("buttonGroup");
		buttonGroup->addButton(toolButtonTopLeft);
		toolButtonTopLeft->setObjectName("toolButtonTopLeft");
		QIcon icon;
		icon.addFile(QString::fromUtf8(":/DAWidgetsIcons/topleft.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonTopLeft->setIcon(icon);
		toolButtonTopLeft->setIconSize(QSize(18, 18));
		toolButtonTopLeft->setCheckable(true);
		toolButtonTopLeft->setAutoRaise(true);

		gridLayout->addWidget(toolButtonTopLeft, 1, 1, 1, 1);

		toolButtonTop = new QToolButton(DAAligmentPositionEditWidget);
		buttonGroup->addButton(toolButtonTop);
		toolButtonTop->setObjectName("toolButtonTop");
		QIcon icon1;
		icon1.addFile(QString::fromUtf8(":/DAWidgetsIcons/top.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonTop->setIcon(icon1);
		toolButtonTop->setIconSize(QSize(18, 18));
		toolButtonTop->setCheckable(true);
		toolButtonTop->setAutoRaise(true);

		gridLayout->addWidget(toolButtonTop, 1, 2, 1, 1);

		toolButtonTopRight = new QToolButton(DAAligmentPositionEditWidget);
		buttonGroup->addButton(toolButtonTopRight);
		toolButtonTopRight->setObjectName("toolButtonTopRight");
		QIcon icon2;
		icon2.addFile(QString::fromUtf8(":/DAWidgetsIcons/topRight.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonTopRight->setIcon(icon2);
		toolButtonTopRight->setIconSize(QSize(18, 18));
		toolButtonTopRight->setCheckable(true);
		toolButtonTopRight->setAutoRaise(true);

		gridLayout->addWidget(toolButtonTopRight, 1, 3, 1, 1);

		horizontalSpacer = new QSpacerItem(0, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

		gridLayout->addItem(horizontalSpacer, 2, 0, 1, 1);

		toolButtonLeft = new QToolButton(DAAligmentPositionEditWidget);
		buttonGroup->addButton(toolButtonLeft);
		toolButtonLeft->setObjectName("toolButtonLeft");
		QIcon icon3;
		icon3.addFile(QString::fromUtf8(":/DAWidgetsIcons/left.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonLeft->setIcon(icon3);
		toolButtonLeft->setIconSize(QSize(18, 18));
		toolButtonLeft->setCheckable(true);
		toolButtonLeft->setAutoRaise(true);

		gridLayout->addWidget(toolButtonLeft, 2, 1, 1, 1);

		toolButtonCenter = new QToolButton(DAAligmentPositionEditWidget);
		buttonGroup->addButton(toolButtonCenter);
		toolButtonCenter->setObjectName("toolButtonCenter");
		QIcon icon4;
		icon4.addFile(QString::fromUtf8(":/DAWidgetsIcons/center.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonCenter->setIcon(icon4);
		toolButtonCenter->setIconSize(QSize(18, 18));
		toolButtonCenter->setCheckable(true);
		toolButtonCenter->setAutoRaise(true);

		gridLayout->addWidget(toolButtonCenter, 2, 2, 1, 1);

		toolButtonRight = new QToolButton(DAAligmentPositionEditWidget);
		buttonGroup->addButton(toolButtonRight);
		toolButtonRight->setObjectName("toolButtonRight");
		QIcon icon5;
		icon5.addFile(QString::fromUtf8(":/DAWidgetsIcons/right.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonRight->setIcon(icon5);
		toolButtonRight->setIconSize(QSize(18, 18));
		toolButtonRight->setCheckable(true);
		toolButtonRight->setAutoRaise(true);

		gridLayout->addWidget(toolButtonRight, 2, 3, 1, 1);

		horizontalSpacer_2 = new QSpacerItem(0, 19, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

		gridLayout->addItem(horizontalSpacer_2, 2, 4, 1, 1);

		toolButtonBottomLeft = new QToolButton(DAAligmentPositionEditWidget);
		buttonGroup->addButton(toolButtonBottomLeft);
		toolButtonBottomLeft->setObjectName("toolButtonBottomLeft");
		QIcon icon6;
		icon6.addFile(QString::fromUtf8(":/DAWidgetsIcons/bottomLeft.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonBottomLeft->setIcon(icon6);
		toolButtonBottomLeft->setIconSize(QSize(18, 18));
		toolButtonBottomLeft->setCheckable(true);
		toolButtonBottomLeft->setAutoRaise(true);

		gridLayout->addWidget(toolButtonBottomLeft, 3, 1, 1, 1);

		toolButtonBottom = new QToolButton(DAAligmentPositionEditWidget);
		buttonGroup->addButton(toolButtonBottom);
		toolButtonBottom->setObjectName("toolButtonBottom");
		QIcon icon7;
		icon7.addFile(QString::fromUtf8(":/DAWidgetsIcons/bottom.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonBottom->setIcon(icon7);
		toolButtonBottom->setIconSize(QSize(18, 18));
		toolButtonBottom->setCheckable(true);
		toolButtonBottom->setAutoRaise(true);

		gridLayout->addWidget(toolButtonBottom, 3, 2, 1, 1);

		toolButtonBottomRight = new QToolButton(DAAligmentPositionEditWidget);
		buttonGroup->addButton(toolButtonBottomRight);
		toolButtonBottomRight->setObjectName("toolButtonBottomRight");
		QIcon icon8;
		icon8.addFile(QString::fromUtf8(":/DAWidgetsIcons/bottomRight.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonBottomRight->setIcon(icon8);
		toolButtonBottomRight->setIconSize(QSize(18, 18));
		toolButtonBottomRight->setCheckable(true);
		toolButtonBottomRight->setAutoRaise(true);

		gridLayout->addWidget(toolButtonBottomRight, 3, 3, 1, 1);

		verticalSpacer = new QSpacerItem(20, 1, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

		gridLayout->addItem(verticalSpacer, 4, 2, 1, 1);

		retranslateUi(DAAligmentPositionEditWidget);

		QMetaObject::connectSlotsByName(DAAligmentPositionEditWidget);
	} // setupUi

	void retranslateUi(QWidget *DAAligmentPositionEditWidget)
	{
		DAAligmentPositionEditWidget->setWindowTitle(QCoreApplication::translate("DAAligmentPositionEditWidget", "Aligment Position", nullptr));
		toolButtonTopLeft->setText(QString());
		toolButtonTop->setText(QString());
		toolButtonTopRight->setText(QString());
		toolButtonLeft->setText(QString());
		toolButtonCenter->setText(QString());
		toolButtonRight->setText(QString());
		toolButtonBottomLeft->setText(QString());
		toolButtonBottom->setText(QString());
		toolButtonBottomRight->setText(QString());
	} // retranslateUi

};

namespace Ui {
	class DAAligmentPositionEditWidget: public Ui_DAAligmentPositionEditWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DAALIGMENTPOSITIONEDITWIDGET_H

/*** End of inlined file: ui_DAAligmentPositionEditWidget.h ***/

namespace DA
{
/**
 * @brief 构造对齐位置编辑窗口
 * @param parent 父控件
 * @details 加载 ui 并将九宫格按钮的 buttonGroup id 绑定到对应的方向组合对齐值，
 *          同时按 Qt 版本连接 buttonClicked/idClicked 信号
 */
DAAligmentPositionEditWidget::DAAligmentPositionEditWidget(QWidget* parent)
	: QWidget(parent), ui(new Ui::DAAligmentPositionEditWidget)
{
	ui->setupUi(this);
	ui->buttonGroup->setId(ui->toolButtonTopLeft, static_cast< int >(Qt::AlignLeft | Qt::AlignTop));
	ui->buttonGroup->setId(ui->toolButtonTop, static_cast< int >(Qt::AlignTop | Qt::AlignHCenter));
	ui->buttonGroup->setId(ui->toolButtonTopRight, static_cast< int >(Qt::AlignTop | Qt::AlignRight));
	ui->buttonGroup->setId(ui->toolButtonLeft, static_cast< int >(Qt::AlignLeft | Qt::AlignVCenter));
	ui->buttonGroup->setId(ui->toolButtonCenter, static_cast< int >(Qt::AlignCenter));
	ui->buttonGroup->setId(ui->toolButtonRight, static_cast< int >(Qt::AlignRight | Qt::AlignVCenter));
	ui->buttonGroup->setId(ui->toolButtonBottomLeft, static_cast< int >(Qt::AlignLeft | Qt::AlignBottom));
	ui->buttonGroup->setId(ui->toolButtonBottom, static_cast< int >(Qt::AlignBottom | Qt::AlignHCenter));
	ui->buttonGroup->setId(ui->toolButtonBottomRight, static_cast< int >(Qt::AlignBottom | Qt::AlignRight));
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
	connect(ui->buttonGroup,
			QOverload< int >::of(&QButtonGroup::buttonClicked),
			this,
			&DAAligmentPositionEditWidget::onButtonClicked);
#else
	connect(ui->buttonGroup, &QButtonGroup::idClicked, this, &DAAligmentPositionEditWidget::onButtonClicked);
#endif
}

/**
 * @brief 析构函数
 * @details 释放 ui 指针
 */
DAAligmentPositionEditWidget::~DAAligmentPositionEditWidget()
{
	delete ui;
}

/**
 * @brief 设置对齐位置
 * @param al 要设置的对齐方式
 * @details 先通过 acceptAligment 过滤为本控件支持的方向组合，再选中对应按钮并发射信号
 */
void DAAligmentPositionEditWidget::setAligmentPosition(Qt::Alignment al)
{
	Qt::Alignment realal = acceptAligment(al);
	if (realal == getAligmentPosition()) {
		return;
	}
	auto button = ui->buttonGroup->button(static_cast< int >(realal));
	if (button) {
		if (!button->isChecked()) {
			button->setChecked(true);
			emit aligmentPositionChanged(realal);
		}
	}
}

/**
 * @brief 获取当前对齐位置
 * @return 当前选中的对齐方式
 * @details 直接返回按钮组当前选中项的 id 转换而来的对齐值
 */
Qt::Alignment DAAligmentPositionEditWidget::getAligmentPosition() const
{
	return static_cast< Qt::Alignment >(ui->buttonGroup->checkedId());
}

/**
 * @brief 过滤对齐方式为本控件支持的方向组合
 * @param al 输入的任意对齐方式
 * @return 映射到九宫格按钮支持的组合对齐值，无匹配时返回默认构造的空对齐
 * @details 九宫格仅支持 9 种固定方向组合，此函数把任意对齐值归一化到这些组合之一
 */
Qt::Alignment DAAligmentPositionEditWidget::acceptAligment(Qt::Alignment al)
{
	Qt::Alignment realal;
	if (al.testFlag(Qt::AlignLeft) && al.testFlag(Qt::AlignTop)) {
		realal = Qt::AlignLeft | Qt::AlignTop;
	} else if (al.testFlag(Qt::AlignTop) && al.testFlag(Qt::AlignHCenter)) {
		realal = Qt::AlignTop | Qt::AlignHCenter;
	} else if (al.testFlag(Qt::AlignTop) && al.testFlag(Qt::AlignRight)) {
		realal = Qt::AlignTop | Qt::AlignRight;
	} else if (al.testFlag(Qt::AlignLeft) && al.testFlag(Qt::AlignVCenter)) {
		realal = Qt::AlignLeft | Qt::AlignVCenter;
	} else if (al.testFlag(Qt::AlignCenter)) {
		realal = Qt::AlignCenter;
	} else if (al.testFlag(Qt::AlignRight) && al.testFlag(Qt::AlignVCenter)) {
		realal = Qt::AlignRight | Qt::AlignVCenter;
	} else if (al.testFlag(Qt::AlignLeft) && al.testFlag(Qt::AlignBottom)) {
		realal = Qt::AlignLeft | Qt::AlignBottom;
	} else if (al.testFlag(Qt::AlignBottom) && al.testFlag(Qt::AlignHCenter)) {
		realal = Qt::AlignBottom | Qt::AlignHCenter;
	} else if (al.testFlag(Qt::AlignRight) && al.testFlag(Qt::AlignBottom)) {
		realal = Qt::AlignRight | Qt::AlignBottom;
	}
	return realal;
}

void DAAligmentPositionEditWidget::changeEvent(QEvent* e)
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

void DAAligmentPositionEditWidget::onButtonClicked(int id)
{
	emit aligmentPositionChanged(static_cast< Qt::Alignment >(id));
}
}

/*** End of inlined file: DAAligmentPositionEditWidget.cpp ***/


/*** Start of inlined file: DATextAlignmentEditWidget.cpp ***/
#include <QButtonGroup>
#include <QEvent>
#include <QHBoxLayout>
#include <QIcon>
#include <QToolButton>
#include <QVBoxLayout>

namespace DA
{

// 图标资源（本库 QRC，前缀 /DAWidgetsIcons）。
// 注意：AligmentCenter.svg 实际是「垂直居中」语义（横线在中间、文本块上下居中），
// 水平居中用本控件新增的 AligmentHCenter.svg。
namespace
{
constexpr int c_buttonSize   = 26;  ///< 单按钮边长（16px 图标 + 边距，两行合计约 53px）
constexpr int c_iconSize     = 16;  ///< 图标边长
constexpr int c_layoutSpacing = 1;  ///< 行内/行间间距
}  // namespace

/**
 * @brief 文本对齐编辑控件私有数据
 */
class DATextAlignmentEditWidget::PrivateData
{
	DA_DECLARE_PUBLIC(DATextAlignmentEditWidget)
public:
	explicit PrivateData(DATextAlignmentEditWidget* p) : q_ptr(p) {}

	// 组装一行按钮（三个）
	void setupButton(QToolButton* btn, const QString& objName, const QString& iconPath);
	// 按当前值刷新两行按钮的选中态
	void syncButtonStates();
	// 取水平/垂直位（缺位返回空）
	static Qt::Alignment horizontalOf(Qt::Alignment al);
	static Qt::Alignment verticalOf(Qt::Alignment al);
	// 用户点击某个按钮：更新对应轴，值有变化才发信号
	void onButtonClicked(Qt::Alignment axisValue, bool horizontal);
	// 切换语言时刷新提示文本
	void retranslateUi();

	// 当前值（默认与表格默认一致：左对齐 + 垂直居中）
	Qt::Alignment mAlignment { Qt::AlignLeft | Qt::AlignVCenter };

	QToolButton* mBtnLeft { nullptr };
	QToolButton* mBtnHCenter { nullptr };
	QToolButton* mBtnRight { nullptr };
	QToolButton* mBtnTop { nullptr };
	QToolButton* mBtnVCenter { nullptr };
	QToolButton* mBtnBottom { nullptr };
	QButtonGroup* mHGroup { nullptr };  ///< 水平行互斥组
	QButtonGroup* mVGroup { nullptr };  ///< 垂直行互斥组
};

/**
 * @brief 组装单个按钮（图标、可选中、自适应扁平样式）
 */
void DATextAlignmentEditWidget::PrivateData::setupButton(QToolButton* btn, const QString& objName, const QString& iconPath)
{
	btn->setObjectName(objName);
	btn->setAutoRaise(true);
	btn->setCheckable(true);
	btn->setIcon(QIcon(iconPath));
	btn->setIconSize(QSize(c_iconSize, c_iconSize));
	btn->setFixedSize(c_buttonSize, c_buttonSize);
}

/**
 * @brief 按当前值刷新两行按钮的选中态
 */
void DATextAlignmentEditWidget::PrivateData::syncButtonStates()
{
	const Qt::Alignment h = horizontalOf(mAlignment);
	const Qt::Alignment v = verticalOf(mAlignment);
	mBtnLeft->setChecked(h == Qt::AlignLeft);
	mBtnHCenter->setChecked(h == Qt::AlignHCenter);
	mBtnRight->setChecked(h == Qt::AlignRight);
	mBtnTop->setChecked(v == Qt::AlignTop);
	mBtnVCenter->setChecked(v == Qt::AlignVCenter);
	mBtnBottom->setChecked(v == Qt::AlignBottom);
}

/**
 * @brief 取对齐值中的水平位（AlignLeft/AlignHCenter/AlignRight；都没有返回空）
 */
Qt::Alignment DATextAlignmentEditWidget::PrivateData::horizontalOf(Qt::Alignment al)
{
	if (al.testFlag(Qt::AlignLeft)) {
		return Qt::AlignLeft;
	}
	if (al.testFlag(Qt::AlignRight)) {
		return Qt::AlignRight;
	}
	if (al.testFlag(Qt::AlignHCenter)) {
		return Qt::AlignHCenter;
	}
	return Qt::Alignment();
}

/**
 * @brief 取对齐值中的垂直位（AlignTop/AlignVCenter/AlignBottom；都没有返回空）
 */
Qt::Alignment DATextAlignmentEditWidget::PrivateData::verticalOf(Qt::Alignment al)
{
	if (al.testFlag(Qt::AlignTop)) {
		return Qt::AlignTop;
	}
	if (al.testFlag(Qt::AlignBottom)) {
		return Qt::AlignBottom;
	}
	if (al.testFlag(Qt::AlignVCenter)) {
		return Qt::AlignVCenter;
	}
	return Qt::Alignment();
}

/**
 * @brief 用户点击按钮：替换对应轴后同步选中态，值有变化才发 alignmentChanged
 * @param axisValue AlignLeft/AlignHCenter/AlignRight 或 AlignTop/AlignVCenter/AlignBottom
 * @param horizontal true=水平行，false=垂直行
 */
void DATextAlignmentEditWidget::PrivateData::onButtonClicked(Qt::Alignment axisValue, bool horizontal)
{
	const Qt::Alignment old = mAlignment;
	if (horizontal) {
		mAlignment = (mAlignment & ~Qt::AlignHorizontal_Mask) | axisValue;
	} else {
		mAlignment = (mAlignment & ~Qt::AlignVertical_Mask) | axisValue;
	}
	syncButtonStates();
	if (mAlignment != old) {
		Q_EMIT q_func()->alignmentChanged(mAlignment);
	}
}

/**
 * @brief 切换语言时刷新按钮提示文本
 */
void DATextAlignmentEditWidget::PrivateData::retranslateUi()
{
	mBtnLeft->setToolTip(DATextAlignmentEditWidget::tr("Align Left"));  // cn:左对齐
	mBtnHCenter->setToolTip(DATextAlignmentEditWidget::tr("Align Center"));  // cn:居中
	mBtnRight->setToolTip(DATextAlignmentEditWidget::tr("Align Right"));  // cn:右对齐
	mBtnTop->setToolTip(DATextAlignmentEditWidget::tr("Align Top"));  // cn:顶端对齐
	mBtnVCenter->setToolTip(DATextAlignmentEditWidget::tr("Align Middle"));  // cn:垂直居中
	mBtnBottom->setToolTip(DATextAlignmentEditWidget::tr("Align Bottom"));  // cn:底端对齐
}

/**
 * @brief 构造函数：两行互斥按钮组（水平 → 垂直）
 */
DATextAlignmentEditWidget::DATextAlignmentEditWidget(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	// 水平行：左 / 居中 / 右
	QHBoxLayout* hRow = new QHBoxLayout;
	hRow->setContentsMargins(0, 0, 0, 0);
	hRow->setSpacing(c_layoutSpacing);
	d->mBtnLeft    = new QToolButton(this);
	d->mBtnHCenter = new QToolButton(this);
	d->mBtnRight   = new QToolButton(this);
	d->setupButton(d->mBtnLeft, QStringLiteral("tbAlignLeft"), QStringLiteral(":/DAWidgetsIcons/AligmentLeft.svg"));
	d->setupButton(d->mBtnHCenter, QStringLiteral("tbAlignHCenter"), QStringLiteral(":/DAWidgetsIcons/AligmentHCenter.svg"));
	d->setupButton(d->mBtnRight, QStringLiteral("tbAlignRight"), QStringLiteral(":/DAWidgetsIcons/AligmentRight.svg"));
	hRow->addWidget(d->mBtnLeft);
	hRow->addWidget(d->mBtnHCenter);
	hRow->addWidget(d->mBtnRight);
	hRow->addStretch(1);
	// 垂直行：顶端 / 居中 / 底端
	QHBoxLayout* vRow = new QHBoxLayout;
	vRow->setContentsMargins(0, 0, 0, 0);
	vRow->setSpacing(c_layoutSpacing);
	d->mBtnTop     = new QToolButton(this);
	d->mBtnVCenter = new QToolButton(this);
	d->mBtnBottom  = new QToolButton(this);
	d->setupButton(d->mBtnTop, QStringLiteral("tbAlignTop"), QStringLiteral(":/DAWidgetsIcons/AligmentTop.svg"));
	d->setupButton(d->mBtnVCenter, QStringLiteral("tbAlignVCenter"), QStringLiteral(":/DAWidgetsIcons/AligmentCenter.svg"));
	d->setupButton(d->mBtnBottom, QStringLiteral("tbAlignBottom"), QStringLiteral(":/DAWidgetsIcons/AligmentBottom.svg"));
	vRow->addWidget(d->mBtnTop);
	vRow->addWidget(d->mBtnVCenter);
	vRow->addWidget(d->mBtnBottom);
	vRow->addStretch(1);

	QVBoxLayout* root = new QVBoxLayout(this);
	root->setContentsMargins(0, 0, 0, 0);
	root->setSpacing(c_layoutSpacing);
	root->addLayout(hRow);
	root->addLayout(vRow);

	// 两行各自互斥（QButtonGroup 保证同一行只能选中一个）
	d->mHGroup = new QButtonGroup(this);
	d->mHGroup->setExclusive(true);
	d->mHGroup->addButton(d->mBtnLeft);
	d->mHGroup->addButton(d->mBtnHCenter);
	d->mHGroup->addButton(d->mBtnRight);
	d->mVGroup = new QButtonGroup(this);
	d->mVGroup->setExclusive(true);
	d->mVGroup->addButton(d->mBtnTop);
	d->mVGroup->addButton(d->mBtnVCenter);
	d->mVGroup->addButton(d->mBtnBottom);

	// 点击即应用（逐按钮连接，避免 Qt5/Qt6 按钮组信号差异）；重复点击已选中的按钮不改变值、不发信号
	QObject::connect(d->mBtnLeft, &QToolButton::clicked, this, [ d ]() { d->onButtonClicked(Qt::AlignLeft, true); });
	QObject::connect(d->mBtnHCenter, &QToolButton::clicked, this, [ d ]() { d->onButtonClicked(Qt::AlignHCenter, true); });
	QObject::connect(d->mBtnRight, &QToolButton::clicked, this, [ d ]() { d->onButtonClicked(Qt::AlignRight, true); });
	QObject::connect(d->mBtnTop, &QToolButton::clicked, this, [ d ]() { d->onButtonClicked(Qt::AlignTop, false); });
	QObject::connect(d->mBtnVCenter, &QToolButton::clicked, this, [ d ]() { d->onButtonClicked(Qt::AlignVCenter, false); });
	QObject::connect(d->mBtnBottom, &QToolButton::clicked, this, [ d ]() { d->onButtonClicked(Qt::AlignBottom, false); });

	d->retranslateUi();
	d->syncButtonStates();
}

/**
 * @brief 析构函数
 */
DATextAlignmentEditWidget::~DATextAlignmentEditWidget()
{
}

/**
 * @brief 当前对齐（水平位 + 垂直位）
 */
Qt::Alignment DATextAlignmentEditWidget::alignment() const
{
	DA_DC(d);
	return d->mAlignment;
}

/**
 * @brief 当前水平对齐位
 */
Qt::Alignment DATextAlignmentEditWidget::horizontalAlignment() const
{
	DA_DC(d);
	return PrivateData::horizontalOf(d->mAlignment);
}

/**
 * @brief 当前垂直对齐位
 */
Qt::Alignment DATextAlignmentEditWidget::verticalAlignment() const
{
	DA_DC(d);
	return PrivateData::verticalOf(d->mAlignment);
}

/**
 * @brief 设置对齐：缺水平位/垂直位的那一半保持当前值；与当前值相同不发信号
 */
void DATextAlignmentEditWidget::setAlignment(Qt::Alignment al)
{
	DA_D(d);
	Qt::Alignment h = PrivateData::horizontalOf(al);
	Qt::Alignment v = PrivateData::verticalOf(al);
	const Qt::Alignment old = d->mAlignment;
	if (h != Qt::Alignment()) {
		d->mAlignment = (d->mAlignment & ~Qt::AlignHorizontal_Mask) | h;
	}
	if (v != Qt::Alignment()) {
		d->mAlignment = (d->mAlignment & ~Qt::AlignVertical_Mask) | v;
	}
	d->syncButtonStates();
	if (d->mAlignment != old) {
		Q_EMIT alignmentChanged(d->mAlignment);
	}
}

/**
 * @brief 切换语言时刷新提示文本
 */
void DATextAlignmentEditWidget::changeEvent(QEvent* e)
{
	QWidget::changeEvent(e);
	if (e->type() == QEvent::LanguageChange) {
		DA_D(d);
		d->retranslateUi();
	}
}

}  // namespace DA

/*** End of inlined file: DATextAlignmentEditWidget.cpp ***/


/*** Start of inlined file: DAFilePathEditWidget.cpp ***/

/*** Start of inlined file: ui_DAFilePathEditWidget.h ***/
#ifndef UI_DAFILEPATHEDITWIDGET_H
#define UI_DAFILEPATHEDITWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DAFilePathEditWidget
{
public:
	QVBoxLayout *verticalLayout;
	QHBoxLayout *horizontalLayout;
	QLineEdit *lineEdit;
	QToolButton *toolButtonRecent;
	QToolButton *toolButtonOpen;
	QWidget *widgetStatus;
	QHBoxLayout *statusLayout;
	QLabel *labelStatusIcon;
	QLabel *labelStatusText;
	QLabel *labelDropHint;

	void setupUi(QWidget *DAFilePathEditWidget)
	{
		if (DAFilePathEditWidget->objectName().isEmpty())
			DAFilePathEditWidget->setObjectName("DAFilePathEditWidget");
		DAFilePathEditWidget->resize(274, 22);
		verticalLayout = new QVBoxLayout(DAFilePathEditWidget);
		verticalLayout->setSpacing(2);
		verticalLayout->setObjectName("verticalLayout");
		verticalLayout->setContentsMargins(0, 0, 0, 0);
		horizontalLayout = new QHBoxLayout();
		horizontalLayout->setSpacing(1);
		horizontalLayout->setObjectName("horizontalLayout");
		horizontalLayout->setContentsMargins(0, 0, 0, 0);
		lineEdit = new QLineEdit(DAFilePathEditWidget);
		lineEdit->setObjectName("lineEdit");

		horizontalLayout->addWidget(lineEdit);

		toolButtonRecent = new QToolButton(DAFilePathEditWidget);
		toolButtonRecent->setObjectName("toolButtonRecent");
		QIcon icon;
		icon.addFile(QString::fromUtf8(":/DAWidgetsIcons/arrow-down.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonRecent->setIcon(icon);
		toolButtonRecent->setIconSize(QSize(16, 16));
		toolButtonRecent->setPopupMode(QToolButton::InstantPopup);
		toolButtonRecent->setAutoRaise(true);

		horizontalLayout->addWidget(toolButtonRecent);

		toolButtonOpen = new QToolButton(DAFilePathEditWidget);
		toolButtonOpen->setObjectName("toolButtonOpen");
		QIcon icon1;
		icon1.addFile(QString::fromUtf8(":/DAWidgetsIcons/open.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonOpen->setIcon(icon1);
		toolButtonOpen->setIconSize(QSize(16, 16));
		toolButtonOpen->setAutoRaise(true);

		horizontalLayout->addWidget(toolButtonOpen);

		verticalLayout->addLayout(horizontalLayout);

		widgetStatus = new QWidget(DAFilePathEditWidget);
		widgetStatus->setObjectName("widgetStatus");
		statusLayout = new QHBoxLayout(widgetStatus);
		statusLayout->setSpacing(4);
		statusLayout->setObjectName("statusLayout");
		statusLayout->setContentsMargins(0, 0, 0, 0);
		labelStatusIcon = new QLabel(widgetStatus);
		labelStatusIcon->setObjectName("labelStatusIcon");
		labelStatusIcon->setMinimumSize(QSize(13, 13));
		labelStatusIcon->setMaximumSize(QSize(13, 13));

		statusLayout->addWidget(labelStatusIcon);

		labelStatusText = new QLabel(widgetStatus);
		labelStatusText->setObjectName("labelStatusText");

		statusLayout->addWidget(labelStatusText);

		verticalLayout->addWidget(widgetStatus);

		labelDropHint = new QLabel(DAFilePathEditWidget);
		labelDropHint->setObjectName("labelDropHint");
		labelDropHint->setFrameShape(QFrame::StyledPanel);
		labelDropHint->setAlignment(Qt::AlignCenter);
		labelDropHint->setMinimumHeight(54);
		labelDropHint->setWordWrap(true);

		verticalLayout->addWidget(labelDropHint);

		retranslateUi(DAFilePathEditWidget);

		QMetaObject::connectSlotsByName(DAFilePathEditWidget);
	} // setupUi

	void retranslateUi(QWidget *DAFilePathEditWidget)
	{
		DAFilePathEditWidget->setWindowTitle(QCoreApplication::translate("DAFilePathEditWidget", "File Path Edit", nullptr));
#if QT_CONFIG(tooltip)
		toolButtonRecent->setToolTip(QCoreApplication::translate("DAFilePathEditWidget", "Recent paths", nullptr));
#endif // QT_CONFIG(tooltip)
		toolButtonOpen->setText(QCoreApplication::translate("DAFilePathEditWidget", "Open", nullptr));
#if QT_CONFIG(shortcut)
		toolButtonOpen->setShortcut(QCoreApplication::translate("DAFilePathEditWidget", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
		labelStatusIcon->setText(QString());
		labelStatusText->setText(QString());
		labelDropHint->setText(QCoreApplication::translate("DAFilePathEditWidget", "Drop a file or folder here", nullptr));
	} // retranslateUi

};

namespace Ui {
	class DAFilePathEditWidget: public Ui_DAFilePathEditWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DAFILEPATHEDITWIDGET_H

/*** End of inlined file: ui_DAFilePathEditWidget.h ***/

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

/*** End of inlined file: DAFilePathEditWidget.cpp ***/


/*** Start of inlined file: DATextEditWidget.cpp ***/
#include <QAction>
#include <QCheckBox>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPlainTextEdit>
#include <QShortcut>
#include <QSignalBlocker>
#include <QStyle>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

namespace DA
{
namespace
{
class DAInternalTextEdit;

// 行号区：只负责把绘制转回编辑器
class DAInternalLineNumberArea : public QWidget
{
public:
	explicit DAInternalLineNumberArea(DAInternalTextEdit* editor);
	QSize sizeHint() const override;
	void paintEvent(QPaintEvent* e) override;

private:
	DAInternalTextEdit* mEditor { nullptr };
};

// 编辑器子类：用 viewportMargins 在左侧挤出宽度并自绘行号
class DAInternalTextEdit : public QPlainTextEdit
{
public:
	explicit DAInternalTextEdit(QWidget* parent = nullptr);

	int lineNumberAreaWidth() const;
	void setLineNumberAreaVisible(bool on);
	bool lineNumberAreaVisible() const
	{
		return mLineNumbers;
	}
	void paintLineNumbers(QPaintEvent* e);
	void updateLineNumberAreaWidth(int newBlockCount);
	void updateLineNumberArea(const QRect& rect, int dy);

protected:
	void resizeEvent(QResizeEvent* e) override;

private:
	QWidget* mLineNumberArea { nullptr };
	bool mLineNumbers { false };
};

DAInternalLineNumberArea::DAInternalLineNumberArea(DAInternalTextEdit* editor)
	: QWidget(editor), mEditor(editor)
{
	setObjectName(QStringLiteral("lineNumberArea"));
}

QSize DAInternalLineNumberArea::sizeHint() const
{
	return QSize(mEditor ? mEditor->lineNumberAreaWidth() : 0, 0);
}

void DAInternalLineNumberArea::paintEvent(QPaintEvent* e)
{
	if (mEditor)
		mEditor->paintLineNumbers(e);
}

DAInternalTextEdit::DAInternalTextEdit(QWidget* parent) : QPlainTextEdit(parent)
{
	mLineNumberArea = new DAInternalLineNumberArea(this);
	mLineNumberArea->hide();
	connect(this, &QPlainTextEdit::blockCountChanged, this, &DAInternalTextEdit::updateLineNumberAreaWidth);
	connect(this, &QPlainTextEdit::updateRequest, this, &DAInternalTextEdit::updateLineNumberArea);
	updateLineNumberAreaWidth(0);
}

int DAInternalTextEdit::lineNumberAreaWidth() const
{
	int digits = 1;
	int max    = qMax(1, blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}
	return 8 + Qt5Qt6Compat_fontMetrics_width(fontMetrics(), QString(digits, QLatin1Char('9')));
}

void DAInternalTextEdit::setLineNumberAreaVisible(bool on)
{
	if (mLineNumbers == on)
		return;
	mLineNumbers = on;
	if (on) {
		mLineNumberArea->show();
		updateLineNumberAreaWidth(0);
	} else {
		mLineNumberArea->hide();
		setViewportMargins(0, 0, 0, 0);
	}
}

void DAInternalTextEdit::updateLineNumberAreaWidth(int)
{
	const int width = mLineNumbers ? lineNumberAreaWidth() : 0;
	setViewportMargins(width, 0, 0, 0);
	if (mLineNumbers && mLineNumberArea) {
		const QRect cr = contentsRect();
		mLineNumberArea->setGeometry(QRect(cr.left(), cr.top(), width, cr.height()));
	}
}

void DAInternalTextEdit::updateLineNumberArea(const QRect& rect, int dy)
{
	if (!mLineNumbers || !mLineNumberArea)
		return;
	if (dy)
		mLineNumberArea->scroll(0, dy);
	else
		mLineNumberArea->update(0, rect.y(), mLineNumberArea->width(), rect.height());
	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

void DAInternalTextEdit::resizeEvent(QResizeEvent* e)
{
	QPlainTextEdit::resizeEvent(e);
	if (!mLineNumbers || !mLineNumberArea)
		return;
	const QRect cr = contentsRect();
	mLineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void DAInternalTextEdit::paintLineNumbers(QPaintEvent* e)
{
	QPainter painter(mLineNumberArea);
	painter.fillRect(e->rect(), QColor(0xF5, 0xF7, 0xFA));
	painter.setPen(QColor(0x90, 0x93, 0x99));

	QTextBlock block = firstVisibleBlock();
	int blockNumber  = block.blockNumber();
	int top          = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
	int bottom       = top + qRound(blockBoundingRect(block).height());
	while (block.isValid() && top <= e->rect().bottom()) {
		if (block.isVisible() && bottom >= e->rect().top()) {
			painter.drawText(0, top, mLineNumberArea->width() - 4, fontMetrics().height(),
							 Qt::AlignRight, QString::number(blockNumber + 1));
		}
		block = block.next();
		top    = bottom;
		bottom = top + qRound(blockBoundingRect(block).height());
		++blockNumber;
	}
}
}  // namespace

class DATextEditWidget::PrivateData
{
	DA_DECLARE_PUBLIC(DATextEditWidget)
public:
	PrivateData(DATextEditWidget* p);
	void init();
	void buildFindBar();
	int collectMatches(const QString& key, QVector< QPair< int, int > >& ranges) const;
	void applyFindHighlight();
	void updateFindCount();
	void clearFindState();
	void refreshFind(bool jumpToFirst);
	void moveToMatch(bool forward);

	QToolBar* mToolbar { nullptr };
	DAInternalTextEdit* mEdit { nullptr };
	QFont mDefaultFont;
	bool mMonospace { false };

	QWidget* mFindBar { nullptr };
	QLineEdit* mFindEdit { nullptr };
	QLabel* mFindCount { nullptr };
	QCheckBox* mFindCase { nullptr };
	QToolButton* mFindPrev { nullptr };
	QToolButton* mFindNext { nullptr };
	QToolButton* mFindClose { nullptr };
	QAction* mFindAction { nullptr };
	bool mFindEnabled { false };
	bool mFindCaseSensitive { false };
	QVector< QPair< int, int > > mFindRanges;
	int mFindIndex { -1 };
};

DATextEditWidget::PrivateData::PrivateData(DATextEditWidget* p) : q_ptr(p)
{
}

void DATextEditWidget::PrivateData::init()
{
	DA_Q(DATextEditWidget, q);
	QVBoxLayout* root = new QVBoxLayout(q);
	root->setContentsMargins(0, 0, 0, 0);
	root->setSpacing(0);

	mToolbar = new QToolBar(q);
	mToolbar->setObjectName(QStringLiteral("textToolbar"));
	mToolbar->setIconSize(QSize(16, 16));
	// 注入的动作可能没有图标，用 TextBesideIcon 保证文字可见
	mToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	mToolbar->setVisible(false);
	root->addWidget(mToolbar);

	buildFindBar();
	mFindBar->setVisible(false);
	root->addWidget(mFindBar);

	mEdit = new DAInternalTextEdit(q);
	mEdit->setObjectName(QStringLiteral("textEdit"));
	mDefaultFont = mEdit->font();
	root->addWidget(mEdit, 1);

	// 查找动作：context 限定在控件子树，只读时同样可用
	mFindAction = new QAction(q);
	mFindAction->setShortcut(QKeySequence::Find);
	mFindAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	mFindAction->setEnabled(false);
	QObject::connect(mFindAction, &QAction::triggered, q, [ q ]() { q->showFindBar(); });
	q->addAction(mFindAction);

	QObject::connect(mEdit, &QPlainTextEdit::textChanged, q, [ q ]() { Q_EMIT q->textChanged(); });
}

void DATextEditWidget::PrivateData::buildFindBar()
{
	DA_Q(DATextEditWidget, q);
	mFindBar = new QWidget(q);
	mFindBar->setObjectName(QStringLiteral("findBar"));
	auto* lay = new QHBoxLayout(mFindBar);
	lay->setContentsMargins(4, 2, 4, 2);
	lay->setSpacing(4);

	mFindEdit = new QLineEdit(mFindBar);
	mFindEdit->setObjectName(QStringLiteral("findEdit"));
	mFindEdit->setPlaceholderText(DATextEditWidget::tr("Find in text"));  //cn:在文本中查找

	mFindPrev = new QToolButton(mFindBar);
	mFindPrev->setObjectName(QStringLiteral("findPrev"));
	mFindPrev->setAutoRaise(true);
	mFindPrev->setIcon(q->style()->standardIcon(QStyle::SP_ArrowUp));
	mFindPrev->setToolTip(DATextEditWidget::tr("Previous"));  //cn:上一个

	mFindNext = new QToolButton(mFindBar);
	mFindNext->setObjectName(QStringLiteral("findNext"));
	mFindNext->setAutoRaise(true);
	mFindNext->setIcon(q->style()->standardIcon(QStyle::SP_ArrowDown));
	mFindNext->setToolTip(DATextEditWidget::tr("Next"));  //cn:下一个

	mFindCount = new QLabel(QStringLiteral("0 / 0"), mFindBar);
	mFindCount->setObjectName(QStringLiteral("findCount"));

	mFindCase = new QCheckBox(DATextEditWidget::tr("Match case"), mFindBar);  //cn:区分大小写
	mFindCase->setObjectName(QStringLiteral("findCase"));

	mFindClose = new QToolButton(mFindBar);
	mFindClose->setObjectName(QStringLiteral("findClose"));
	mFindClose->setAutoRaise(true);
	mFindClose->setIcon(q->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	mFindClose->setToolTip(DATextEditWidget::tr("Close"));  //cn:关闭

	lay->addWidget(mFindEdit, 1);
	lay->addWidget(mFindPrev);
	lay->addWidget(mFindNext);
	lay->addWidget(mFindCount);
	lay->addWidget(mFindCase);
	lay->addWidget(mFindClose);

	QObject::connect(mFindEdit, &QLineEdit::textChanged, q, [ this ](const QString&) { refreshFind(true); });
	QObject::connect(mFindPrev, &QToolButton::clicked, q, [ this ]() { moveToMatch(false); });
	QObject::connect(mFindNext, &QToolButton::clicked, q, [ this ]() { moveToMatch(true); });
	QObject::connect(mFindClose, &QToolButton::clicked, q, [ q ]() { q->hideFindBar(); });
	QObject::connect(mFindCase, &QCheckBox::toggled, q, [ q ](bool on) { q->setFindCaseSensitive(on); });

	auto* esc = new QShortcut(QKeySequence(Qt::Key_Escape), mFindEdit);
	QObject::connect(esc, &QShortcut::activated, q, [ q ]() { q->hideFindBar(); });
}

int DATextEditWidget::PrivateData::collectMatches(const QString& key,
												  QVector< QPair< int, int > >& ranges) const
{
	ranges.clear();
	if (key.isEmpty() || !mEdit)
		return 0;
	QTextDocument::FindFlags flags;
	if (mFindCaseSensitive)
		flags |= QTextDocument::FindCaseSensitively;
	QTextCursor cursor(mEdit->document());
	while (true) {
		cursor = mEdit->document()->find(key, cursor, flags);
		if (cursor.isNull())
			break;
		const int start = cursor.selectionStart();
		ranges.append(qMakePair(start, cursor.selectionEnd() - start));
	}
	return ranges.size();
}

void DATextEditWidget::PrivateData::applyFindHighlight()
{
	QList< QTextEdit::ExtraSelection > selections;
	for (int i = 0; i < mFindRanges.size(); ++i) {
		const auto& r = mFindRanges.at(i);
		QTextEdit::ExtraSelection sel;
		sel.cursor = QTextCursor(mEdit->document());
		sel.cursor.setPosition(r.first);
		sel.cursor.setPosition(r.first + r.second, QTextCursor::KeepAnchor);
		sel.format.setBackground(i == mFindIndex ? QColor(0xFF, 0xD6, 0x66, 180)
												 : QColor(0xFF, 0xF1, 0xB8, 140));
		selections.append(sel);
	}
	mEdit->setExtraSelections(selections);
}

void DATextEditWidget::PrivateData::updateFindCount()
{
	if (!mFindCount)
		return;
	const int total = mFindRanges.size();
	const int shown = (total > 0 && mFindIndex >= 0) ? (mFindIndex + 1) : 0;
	mFindCount->setText(QStringLiteral("%1 / %2").arg(shown).arg(total));
	const bool invalid = total == 0 && mFindEdit && !mFindEdit->text().isEmpty();
	mFindEdit->setProperty("invalid", invalid);
	mFindEdit->style()->unpolish(mFindEdit);
	mFindEdit->style()->polish(mFindEdit);
	mFindEdit->update();
}

void DATextEditWidget::PrivateData::clearFindState()
{
	mFindRanges.clear();
	mFindIndex = -1;
	if (mEdit)
		mEdit->setExtraSelections({});
	if (mFindCount)
		mFindCount->setText(QStringLiteral("0 / 0"));
}

void DATextEditWidget::PrivateData::refreshFind(bool jumpToFirst)
{
	if (!mFindEdit)
		return;
	const QString key = mFindEdit->text();
	collectMatches(key, mFindRanges);
	if (mFindRanges.isEmpty()) {
		mFindIndex = -1;
		mEdit->setExtraSelections({});
		updateFindCount();
		return;
	}
	mFindIndex = jumpToFirst ? 0 : qBound(0, mFindIndex, mFindRanges.size() - 1);
	const auto& r = mFindRanges.at(mFindIndex);
	QTextCursor c(mEdit->document());
	c.setPosition(r.first);
	c.setPosition(r.first + r.second, QTextCursor::KeepAnchor);
	mEdit->setTextCursor(c);
	mEdit->centerCursor();
	applyFindHighlight();
	updateFindCount();
}

void DATextEditWidget::PrivateData::moveToMatch(bool forward)
{
	if (mFindRanges.isEmpty()) {
		refreshFind(true);
		return;
	}
	const int total = mFindRanges.size();
	mFindIndex      = forward ? (mFindIndex + 1) % total : (mFindIndex - 1 + total) % total;
	const auto& r   = mFindRanges.at(mFindIndex);
	QTextCursor c(mEdit->document());
	c.setPosition(r.first);
	c.setPosition(r.first + r.second, QTextCursor::KeepAnchor);
	mEdit->setTextCursor(c);
	mEdit->centerCursor();
	applyFindHighlight();
	updateFindCount();
}

//===============================================================
// DATextEditWidget
//===============================================================

/**
 * @brief 构造文本编辑控件
 * @param parent 父控件
 */
DATextEditWidget::DATextEditWidget(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	d->init();
}

/**
 * @brief 析构
 */
DATextEditWidget::~DATextEditWidget()
{
}

/**
 * @brief 设置只读
 * @param on true = 只读
 */
void DATextEditWidget::setReadOnly(bool on)
{
	DA_D(d);
	d->mEdit->setReadOnly(on);
}

/**
 * @brief 是否只读
 * @return true = 只读
 */
bool DATextEditWidget::isReadOnly() const
{
	DA_DC(d);
	return d->mEdit->isReadOnly();
}

/**
 * @brief 设置等宽字体
 * @param on true = 使用系统等宽字体
 */
void DATextEditWidget::setMonospace(bool on)
{
	DA_D(d);
	if (on == d->mMonospace) {
		return;
	}
	d->mMonospace = on;
	if (on) {
		QFont fixed = QFontDatabase::systemFont(QFontDatabase::FixedFont);
		fixed.setPointSize(d->mDefaultFont.pointSize());
		d->mEdit->setFont(fixed);
	} else {
		d->mEdit->setFont(d->mDefaultFont);
	}
	d->mEdit->updateLineNumberAreaWidth(0);
}

/**
 * @brief 是否等宽字体
 * @return true = 等宽
 */
bool DATextEditWidget::isMonospace() const
{
	DA_DC(d);
	return d->mMonospace;
}

/**
 * @brief 设置自动换行
 * @param on true = 按控件宽度换行
 */
void DATextEditWidget::setLineWrapEnabled(bool on)
{
	DA_D(d);
	d->mEdit->setLineWrapMode(on ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
}

/**
 * @brief 是否自动换行
 * @return true = 自动换行
 */
bool DATextEditWidget::isLineWrapEnabled() const
{
	DA_DC(d);
	return d->mEdit->lineWrapMode() != QPlainTextEdit::NoWrap;
}

/**
 * @brief 设置文本（不发 textChanged）
 * @param text 文本
 */
void DATextEditWidget::setText(const QString& text)
{
	DA_D(d);
	QSignalBlocker blocker(d->mEdit);
	d->mEdit->setPlainText(text);
	// 信号被屏蔽，blockCountChanged 不会来，行号宽度需手动重算
	d->mEdit->updateLineNumberAreaWidth(0);
	// 旧高亮指向旧文档，必须清掉
	d->clearFindState();
	d->updateFindCount();
}

/**
 * @brief 获取文本
 * @return 纯文本
 */
QString DATextEditWidget::text() const
{
	DA_DC(d);
	return d->mEdit->toPlainText();
}

/**
 * @brief 设置占位提示
 * @param t 提示文本
 */
void DATextEditWidget::setPlaceholderText(const QString& t)
{
	DA_D(d);
	d->mEdit->setPlaceholderText(t);
}

/**
 * @brief 工具条显隐
 * @param on 是否显示
 */
void DATextEditWidget::setToolbarVisible(bool on)
{
	DA_D(d);
	d->mToolbar->setVisible(on);
}

/**
 * @brief 工具条是否显示
 * @return true = 显示
 */
bool DATextEditWidget::isToolbarVisible() const
{
	DA_DC(d);
	return d->mToolbar->isVisibleTo(const_cast< DATextEditWidget* >(this));
}

/**
 * @brief 注入工具动作
 * @param action 动作（由调用方负责生命周期）
 */
void DATextEditWidget::addToolAction(QAction* action)
{
	DA_D(d);
	if (!action) {
		return;
	}
	d->mToolbar->addAction(action);
}

/**
 * @brief 打开/关闭左侧行号栏
 * @param on 是否显示（默认 false）
 */
void DATextEditWidget::setLineNumbersVisible(bool on)
{
	DA_D(d);
	d->mEdit->setLineNumberAreaVisible(on);
}

/** @brief 行号栏是否启用 */
bool DATextEditWidget::lineNumbersVisible() const
{
	DA_DC(d);
	return d->mEdit->lineNumberAreaVisible();
}

/**
 * @brief 启用/禁用 Ctrl+F
 * @param on 是否启用（默认 false）
 */
void DATextEditWidget::setFindEnabled(bool on)
{
	DA_D(d);
	if (d->mFindEnabled == on)
		return;
	d->mFindEnabled = on;
	if (d->mFindAction)
		d->mFindAction->setEnabled(on);
	if (!on)
		hideFindBar();
}

/** @brief Ctrl+F 是否启用 */
bool DATextEditWidget::findEnabled() const
{
	DA_DC(d);
	return d->mFindEnabled;
}

/**
 * @brief 直接显隐查找条
 * @param on 是否显示（不检查 findEnabled）
 */
void DATextEditWidget::setFindBarVisible(bool on)
{
	DA_D(d);
	if (on)
		showFindBar();
	else
		hideFindBar();
}

/** @brief 查找条是否可见 */
bool DATextEditWidget::findBarVisible() const
{
	DA_DC(d);
	return d->mFindBar && d->mFindBar->isVisibleTo(const_cast< DATextEditWidget* >(this));
}

/**
 * @brief 打开查找条并可预填关键词
 * @param initialText 初始关键词；空串沿用上次
 */
void DATextEditWidget::showFindBar(const QString& initialText)
{
	DA_D(d);
	if (!d->mFindBar)
		return;
	d->mFindBar->setVisible(true);
	if (!initialText.isEmpty())
		d->mFindEdit->setText(initialText);
	d->mFindEdit->setFocus();
	d->mFindEdit->selectAll();
	d->refreshFind(true);
}

/**
 * @brief 隐藏查找条、清空高亮并把焦点还给编辑器
 */
void DATextEditWidget::hideFindBar()
{
	DA_D(d);
	if (!d->mFindBar)
		return;
	d->mFindBar->setVisible(false);
	d->clearFindState();
	d->mEdit->setFocus();
}

/**
 * @brief 设置查找是否区分大小写
 * @param on 是否区分（默认 false）
 */
void DATextEditWidget::setFindCaseSensitive(bool on)
{
	DA_D(d);
	if (d->mFindCaseSensitive == on)
		return;
	d->mFindCaseSensitive = on;
	if (d->mFindCase && d->mFindCase->isChecked() != on) {
		QSignalBlocker blocker(d->mFindCase);
		d->mFindCase->setChecked(on);
	}
	if (d->mFindBar && d->mFindBar->isVisible())
		d->refreshFind(true);
}

/** @brief 查找是否区分大小写 */
bool DATextEditWidget::findCaseSensitive() const
{
	DA_DC(d);
	return d->mFindCaseSensitive;
}

}  // namespace DA

/*** End of inlined file: DATextEditWidget.cpp ***/


/*** Start of inlined file: DABrushStyleComboBox.cpp ***/
#include <memory>
#include <QPainter>
namespace DA
{

class DABrushStyleComboBox::PrivateData
{
	DA_DECLARE_PUBLIC(DABrushStyleComboBox)
public:
	PrivateData(DABrushStyleComboBox* p);
	bool mIsShowNoBrush;
	std::unique_ptr< QColor > mColor;
	std::unique_ptr< QGradient > mGradient;
	std::unique_ptr< QPixmap > mPixmap;
	Qt::BrushStyle mCurrentBrushStyle;  ///< 记录当前的style
	bool mStyleTextVisible { false };
};

DABrushStyleComboBox::PrivateData::PrivateData(DABrushStyleComboBox* p)
	: q_ptr(p), mIsShowNoBrush(true), mCurrentBrushStyle(Qt::NoBrush)
{
}

//===================================================
// DABrushStyleComboBox
//===================================================

/**
 * @brief 构造画刷样式下拉框
 * @param parent 父控件
 * @details 默认以纯色模式初始化，颜色为黑色，并连接索引变更信号
 */
DABrushStyleComboBox::DABrushStyleComboBox(QWidget* parent) : QComboBox(parent), DA_PIMPL_CONSTRUCT
{
	setBrushColor(Qt::black);
	connect(this, QOverload< int >::of(&QComboBox::currentIndexChanged), this, &DABrushStyleComboBox::onCurrentIndexChanged);
}

/**
 * @brief 析构函数
 */
DABrushStyleComboBox::~DABrushStyleComboBox()
{
}

/**
 * @brief 将画刷样式枚举转换为可读的本地化字符串
 * @param s 画刷样式
 * @return 样式对应的本地化名称，未知样式返回空字符串
 */
QString DABrushStyleComboBox::brushStyleToString(Qt::BrushStyle s)
{
	switch (s) {
	case Qt::NoBrush:
		return QObject::tr("No Brush");  // No brush pattern
	case Qt::SolidPattern:
		return QObject::tr("Solid Pattern");  // Uniform color
	case Qt::Dense1Pattern:
		return QObject::tr("Dense1 Pattern");  // Extremely dense brush pattern
	case Qt::Dense2Pattern:
		return QObject::tr("Dense2 Pattern");  // Very dense brush pattern
	case Qt::Dense3Pattern:
		return QObject::tr("Dense3 Pattern");  // Somewhat dense brush pattern
	case Qt::Dense4Pattern:
		return QObject::tr("Dense4 Pattern");  // Half dense brush pattern
	case Qt::Dense5Pattern:
		return QObject::tr("Dense5 Pattern");  // Somewhat sparse brush pattern
	case Qt::Dense6Pattern:
		return QObject::tr("Dense6 Pattern");  // Very sparse brush pattern
	case Qt::Dense7Pattern:
		return QObject::tr("Dense7 Pattern");  // Extremely sparse brush pattern
	case Qt::HorPattern:
		return QObject::tr("Hor Pattern");  // Horizontal lines
	case Qt::VerPattern:
		return QObject::tr("Ver Pattern");  // Vertical lines
	case Qt::CrossPattern:
		return QObject::tr("Cross Pattern");  // Crossing horizontal and vertical lines
	case Qt::BDiagPattern:
		return QObject::tr("Backward Diagonal Pattern");  // Backward diagonal lines
	case Qt::FDiagPattern:
		return QObject::tr("Forward Diagonal Pattern");  // Forward diagonal lines
	case Qt::DiagCrossPattern:
		return QObject::tr("Crossing Diagonal Pattern");  // Crossing diagonal lines
	case Qt::LinearGradientPattern:
		return QObject::tr("Linear Gradient Pattern");  // Linear gradient (set using a dedicated QBrush constructor).
	case Qt::ConicalGradientPattern:
		return QObject::tr("Conical Gradient Pattern");  // Conical gradient (set using a dedicated QBrush constructor).
	case Qt::RadialGradientPattern:
		return QObject::tr("Radial Gradient Pattern");  // Radial gradient (set using a dedicated QBrush constructor).
	case Qt::TexturePattern:
		return QObject::tr("Texture Pattern");  // Custom pattern (see QBrush::setTexture()).
	default:
		break;
	}
	return QString();
}

/**
 * @brief 设置显示无画刷
 * @param v
 */
void DABrushStyleComboBox::setShowNoBrushStyle(bool v)
{
	if (d_ptr->mIsShowNoBrush != v) {
		d_ptr->mIsShowNoBrush = v;
		rebuildItems();
	}
}

/**
 * @brief 显示无画刷
 * @return
 */
bool DABrushStyleComboBox::isShowNoBrushStyle() const
{
	return d_ptr->mIsShowNoBrush;
}

/**
 * @brief 判断是否是纯色填充
 * @return
 */
bool DABrushStyleComboBox::isColorBrush() const
{
	return d_ptr->mColor != nullptr;
}

/**
 * @brief ShowPaintStyle显示，ShowGradientStyle和ShowTextureStyle被去除
 * @param v
 */
void DABrushStyleComboBox::setBrushColor(const QColor& v)
{
	if (d_ptr->mColor) {
		*(d_ptr->mColor) = v;
	} else {
		d_ptr->mColor.reset(new QColor(v));
		d_ptr->mGradient.reset();
		d_ptr->mPixmap.reset();
		rebuildItems();
	}
}

/**
 * @brief DABrushStyleComboBox::getBrushColor
 * @return
 */
QColor DABrushStyleComboBox::getBrushColor() const
{
	if (d_ptr->mColor) {
		return *(d_ptr->mColor);
	}
	return QColor();
}

/**
 * @brief 重建所有item
 */
void DABrushStyleComboBox::rebuildItems()
{
	clear();
	if (isColorBrush()) {
		addItem(Qt::NoBrush);
		addItem(Qt::SolidPattern);
		addItem(Qt::Dense1Pattern);
		addItem(Qt::Dense2Pattern);
		addItem(Qt::Dense3Pattern);
		addItem(Qt::Dense4Pattern);
		addItem(Qt::Dense5Pattern);
		addItem(Qt::Dense6Pattern);
		addItem(Qt::Dense7Pattern);
		addItem(Qt::HorPattern);
		addItem(Qt::VerPattern);
		addItem(Qt::CrossPattern);
		addItem(Qt::BDiagPattern);
		addItem(Qt::FDiagPattern);
		addItem(Qt::DiagCrossPattern);
		// 渐变填充
		addItem(Qt::LinearGradientPattern);
		addItem(Qt::RadialGradientPattern);
		addItem(Qt::ConicalGradientPattern);
		// 纹理填充
		addItem(Qt::TexturePattern);
	}
}

/**
 * @brief 生成指定画刷样式的图标
 * @param s 画刷样式
 * @return 绘制好的 QPixmap 图标
 * @details 根据样式类型用 QPainter 绘制对应图案：NoBrush绘制禁止符号，纯色/图案用fillRect，渐变用对应QGradient，纹理用棋盘格
 */
QIcon DABrushStyleComboBox::generateBrushStyleIcon(Qt::BrushStyle s)
{
	QSize iss = iconSize();
	if (!iss.isValid()) {
		return QIcon();
	}
	switch (s) {
	case Qt::NoBrush: {
		// 程序绘制禁止符号（圆环+对角斜杠），替代原 no-style.svg
		QPixmap pixmap(iss);
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		painter.setRenderHint(QPainter::Antialiasing);
		QPen pen(QColor(0x72, 0x72, 0x72), 1.5);
		painter.setPen(pen);
		painter.setBrush(Qt::NoBrush);
		const int margin = 2;
		const QRectF r(margin, margin, iss.width() - 2 * margin, iss.height() - 2 * margin);
		painter.drawEllipse(r);
		painter.drawLine(r.topLeft() + QPointF(3, 3), r.bottomRight() - QPointF(3, 3));
		return QIcon(pixmap);
	}
	case Qt::SolidPattern:
	case Qt::Dense1Pattern:
	case Qt::Dense2Pattern:
	case Qt::Dense3Pattern:
	case Qt::Dense4Pattern:
	case Qt::Dense5Pattern:
	case Qt::Dense6Pattern:
	case Qt::Dense7Pattern:
	case Qt::HorPattern:
	case Qt::VerPattern:
	case Qt::CrossPattern:
	case Qt::BDiagPattern:
	case Qt::FDiagPattern:
	case Qt::DiagCrossPattern: {
		QPixmap pixmap(iss);
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		QBrush b(getBrushColor(), s);
		painter.fillRect(QRect(QPoint(0, 0), iss), b);
		return QIcon(pixmap);
	}
	case Qt::LinearGradientPattern: {
		QPixmap pixmap(iss);
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		QColor c = getBrushColor();
		QLinearGradient g(0, 0, iss.width(), 0);
		g.setColorAt(0.0, c);
		g.setColorAt(1.0, c.lighter(180));
		painter.fillRect(QRect(QPoint(0, 0), iss), QBrush(g));
		return QIcon(pixmap);
	}
	case Qt::RadialGradientPattern: {
		QPixmap pixmap(iss);
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		QColor c = getBrushColor();
		QRadialGradient g(iss.width() / 2.0, iss.height() / 2.0, qMax(iss.width(), iss.height()) / 2.0);
		g.setColorAt(0.0, c.lighter(180));
		g.setColorAt(1.0, c);
		painter.fillRect(QRect(QPoint(0, 0), iss), QBrush(g));
		return QIcon(pixmap);
	}
	case Qt::ConicalGradientPattern: {
		QPixmap pixmap(iss);
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		QColor c = getBrushColor();
		QConicalGradient g(iss.width() / 2.0, iss.height() / 2.0, 0);
		g.setColorAt(0.0, c);
		g.setColorAt(0.5, c.lighter(180));
		g.setColorAt(1.0, c);
		painter.fillRect(QRect(QPoint(0, 0), iss), QBrush(g));
		return QIcon(pixmap);
	}
	case Qt::TexturePattern: {
		// 棋盘格图案表示纹理
		QPixmap pixmap(iss);
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		const int cellW = qMax(2, iss.width() / 4);
		const int cellH = qMax(2, iss.height() / 4);
		for (int y = 0; y < iss.height(); y += cellH) {
			for (int x = 0; x < iss.width(); x += cellW) {
				if (((x / cellW) + (y / cellH)) % 2 == 0)
					painter.fillRect(QRect(x, y, cellW, cellH), QColor(180, 180, 180));
				else
					painter.fillRect(QRect(x, y, cellW, cellH), QColor(220, 220, 220));
			}
		}
		return QIcon(pixmap);
	}
	default:
		break;
	}
	return QIcon();
}

/**
 * @brief 更新icon
 */
void DABrushStyleComboBox::updateIcons()
{
	int c = count();

	for (int i = 0; i < c; ++i) {
		Qt::BrushStyle s = static_cast< Qt::BrushStyle >(itemData(i).toInt());
		if (Qt::NoBrush == s) {
			continue;
		}
		setItemIcon(i, generateBrushStyleIcon(s));
	}
}

/**
 * @brief 设置是否在下拉项上显示样式文字
 * @param on 是否显示
 * @details 切换后会重新构建所有item以应用文字显示状态
 */
void DABrushStyleComboBox::setStyleTextVisible(bool on)
{
	if (d_ptr->mStyleTextVisible != on) {
		// 要重新生成
		// 先获取已经设置的
		QList< Qt::BrushStyle > ps;
		int c = count();
		for (int i = 0; i < c; ++i) {
			ps.append(static_cast< Qt::BrushStyle >(itemData(i).toInt()));
		}
		clear();
		for (Qt::BrushStyle s : std::as_const(ps)) {
			addItem(s);
		}
	}
	d_ptr->mStyleTextVisible = on;
}

/**
 * @brief 返回是否在样式上显示文字
 * @return 当前是否显示文字
 */
bool DABrushStyleComboBox::isStyleTextVisible() const
{
	return d_ptr->mStyleTextVisible;
}

/**
   @brief 获取当前的画刷样式
   @return
 */
Qt::BrushStyle DABrushStyleComboBox::getCurrentBrushStyle() const
{
	QVariant v = currentData();
	if (v.isValid()) {
		return static_cast< Qt::BrushStyle >(v.toInt());
	}
	return Qt::NoBrush;
}

/**
   @brief 获取画刷样式
   @param index
   @return
 */
Qt::BrushStyle DABrushStyleComboBox::getBrushStyle(int index) const
{
	QVariant v = itemData(index);
	if (v.isValid()) {
		return static_cast< Qt::BrushStyle >(v.toInt());
	}
	return Qt::NoBrush;
}

/**
 * @brief 设置当前选中的样式
 *
 * @note 此函数会发射信号@sa currentBrushStyleChanged
 * @note 如果此时index和style一致，不做任何处理
 * @param s
 */
void DABrushStyleComboBox::setCurrentBrushStyle(Qt::BrushStyle s)
{
	const int c = count();
	for (int i = 0; i < c; ++i) {
		Qt::BrushStyle ds = static_cast< Qt::BrushStyle >(itemData(i).toInt());
		if (ds == s) {
			if (currentIndex() != i) {
				setCurrentIndex(i);
			}
		}
	}
}

/**
 * @brief 下拉框索引变化时发射画刷样式变更信号
 * @param index 当前索引
 */
void DABrushStyleComboBox::onCurrentIndexChanged(int index)
{
	emit currentBrushStyleChanged(getBrushStyle(index));
}

/**
 * @brief 添加一个画刷样式项到下拉框
 * @param s 要添加的画刷样式
 * @details 根据样式生成图标和可选文字，将样式枚举值存入itemData
 */
void DABrushStyleComboBox::addItem(Qt::BrushStyle s)
{
	QComboBox::addItem(generateBrushStyleIcon(s), isStyleTextVisible() ? brushStyleToString(s) : "", static_cast< int >(s));
}

}

/*** End of inlined file: DABrushStyleComboBox.cpp ***/


/*** Start of inlined file: DAPenStyleComboBox.cpp ***/
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QStyleOptionFocusRect>
#include <QStyleOption>
#include <QStyle>
#include <QApplication>
//===================================================
// using DA namespace -- 禁止在头文件using！！
//===================================================
namespace DA
{

class DAPenStyleComboBox::PrivateData
{
	DA_DECLARE_PUBLIC(DAPenStyleComboBox)
public:
	PrivateData(DAPenStyleComboBox* p);
	QPen mDrawPen { Qt::black };
	bool mStyleTextVisible { false };
};

DAPenStyleComboBox::PrivateData::PrivateData(DAPenStyleComboBox* p) : q_ptr(p)
{
	mDrawPen.setWidth(1);
}

//===================================================
// DAPenStyleComboBox
//===================================================

/**
 * @brief 构造画笔样式下拉框
 * @param parent 父控件
 * @details 设置不可编辑模式，初始化图标尺寸并重建所有画笔样式item
 */
DAPenStyleComboBox::DAPenStyleComboBox(QWidget* parent) : QComboBox(parent), DA_PIMPL_CONSTRUCT
{
	setEditable(false);
	setIconSize(QSize(40, 15));
	rebuildItems();
	connect(this, QOverload< int >::of(&QComboBox::currentIndexChanged), this, &DAPenStyleComboBox::onCurrentIndexChanged);
}

/**
 * @brief 析构函数
 */
DAPenStyleComboBox::~DAPenStyleComboBox()
{
}

/**
 * @brief PenStyle转换为文本
 * @param s
 * @return 如果没有对应的penstyle，返回QString()
 */
QString DAPenStyleComboBox::penStyleToString(Qt::PenStyle s)
{
	switch (s) {
	case Qt::NoPen:
		return QObject::tr("No Pen");
	case Qt::SolidLine:
		return QObject::tr("Solid Line");
	case Qt::DashLine:
		return QObject::tr("Dash Line");
	case Qt::DotLine:
		return QObject::tr("Dot Line");
	case Qt::DashDotLine:
		return QObject::tr("Dash Dot Line");
	case Qt::DashDotDotLine:
		return QObject::tr("Dash Dot Dot Line");
	case Qt::CustomDashLine:
		return QObject::tr("Custom Dash Line");
	default:
		break;
	}
	return QString();
}

/**
 * @brief 设置颜色
 * @param c
 */
void DAPenStyleComboBox::setPenColor(const QColor& c)
{
	d_ptr->mDrawPen.setColor(c);
	updateItems();
}

/**
 * @brief 设置绘制画笔的线条宽度
 * @param w 线宽
 * @details 更新内部画笔宽度后刷新所有item图标
 */
void DAPenStyleComboBox::setPenLineWidth(int w)
{
	d_ptr->mDrawPen.setWidth(w);
	updateItems();
}

/**
 * @brief 设置当前的画笔样式
 * @param s
 */
void DAPenStyleComboBox::setCurrentPenStyle(Qt::PenStyle s)
{
	int i = findData(static_cast< int >(s));
	if (i >= 0 && i < count() && (i != currentIndex())) {
		setCurrentIndex(i);
	}
}

/**
 * @brief 生成icon
 * @param s
 * @return
 */
QIcon DAPenStyleComboBox::generatePenIcon(Qt::PenStyle s) const
{
#if DAPENSTYLECOMBOBOX_USE_DELEGATE
	QSize iss = iconSize();
	if (!iss.isValid()) {
		return QIcon();
	}
	QPixmap pixmap(iss);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	DAPenStyleComboBoxItemDelegate* d = qobject_cast< DAPenStyleComboBoxItemDelegate* >(itemDelegate());
	// 获取颜色
	QColor c = (d == nullptr) ? Qt::black : d->getPenColor();
	int w    = (d == nullptr) ? 2 : d->getPenLineWidth();
	QPen p(c);
	p.setWidth(w);
	p.setStyle(s);
	drawPenStyle(&painter, QRect(QPoint(0, 0), iss), p);
	return QIcon(pixmap);
#else
	if (Qt::NoPen == s) {
		// 程序绘制禁止符号（圆环+对角斜杠），替代原 no-style.svg
		QSize iss = iconSize();
		QPixmap pixmap(iss);
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		painter.setRenderHint(QPainter::Antialiasing);
		QPen pen(QColor(0x72, 0x72, 0x72), 1.5);
		painter.setPen(pen);
		painter.setBrush(Qt::NoBrush);
		const int margin = 1;
		const int d = iss.height() - 2 * margin;  // 圆的直径取高度
		const QRectF r(margin, margin, d, d);
		painter.drawEllipse(r);
		painter.drawLine(r.topLeft() + QPointF(2, 2), r.bottomRight() - QPointF(2, 2));
		return QIcon(pixmap);
	}
	QSize iss = iconSize();
	QPixmap pixmap(iss);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	d_ptr->mDrawPen.setStyle(s);
	drawPenStyle(&painter, QRect(QPoint(0, 0), iss), d_ptr->mDrawPen);
	return QIcon(pixmap);
#endif
}

/**
 * @brief 在指定矩形区域内绘制画笔样式线条
 * @param painter 画笔
 * @param rect 绘制区域
 * @param pen 画笔配置（颜色、宽度、样式）
 * @details 在矩形垂直中心绘制一条水平线，用于展示画笔样式
 */
void DAPenStyleComboBox::drawPenStyle(QPainter* painter, const QRect& rect, const QPen& pen)
{
	painter->save();
	int y = rect.top() + rect.height() / 2;
	painter->setPen(pen);
	painter->drawLine(rect.left() + 3, y, rect.right() - 3, y);
	painter->restore();
}

/**
 * @brief 刷新所有item的图标
 * @details 遍历所有item，根据其存储的PenStyle重新生成图标
 */
void DAPenStyleComboBox::updateItems()
{
	for (int i = 0; i < count(); ++i) {
		setItemIcon(i, generatePenIcon(Qt::PenStyle(itemData(i).toInt())));
	}
}

/**
   @brief 是否在样式上显示文字

   @default 默认为否
   @param on
 */
void DAPenStyleComboBox::setStyleTextVisible(bool on)
{
	if (d_ptr->mStyleTextVisible != on) {
		// 要重新生成
		// 先获取已经设置的
		QList< Qt::PenStyle > ps;
		int c = count();
		for (int i = 0; i < c; ++i) {
			ps.append(static_cast< Qt::PenStyle >(itemData(i).toInt()));
		}
		clear();
		for (Qt::PenStyle s : std::as_const(ps)) {
			addItem(s);
		}
	}
	d_ptr->mStyleTextVisible = on;
}

/**
 * @brief 返回是否在样式上显示文字
 * @return 当前是否显示文字
 */
bool DAPenStyleComboBox::isStyleTextVisible() const
{
	return d_ptr->mStyleTextVisible;
}

/**
   @brief 重建所有items
 */
void DAPenStyleComboBox::rebuildItems()
{
	clear();
	addItem(Qt::NoPen);
	addItem(Qt::SolidLine);
	addItem(Qt::DashLine);
	addItem(Qt::DotLine);
	addItem(Qt::DashDotLine);
	addItem(Qt::DashDotDotLine);
}

/**
 * @brief 获取当前的画笔类型
 * @return
 */
Qt::PenStyle DAPenStyleComboBox::getCurrentPenStyle() const
{
	return static_cast< Qt::PenStyle >(currentData().toInt());
}

/**
 * @brief 设置画笔
 * @param p
 */
void DAPenStyleComboBox::setPen(const QPen& p)
{
	d_ptr->mDrawPen = p;
	updateItems();
	setCurrentPenStyle(p.style());
}

/**
 * @brief 添加item
 * @param s
 */
void DAPenStyleComboBox::addItem(Qt::PenStyle s)
{
	QComboBox::addItem(generatePenIcon(s), isStyleTextVisible() ? penStyleToString(s) : "", static_cast< int >(s));
}

/**
 * @brief 下拉框索引变化时发射画笔样式变更信号
 * @param index 当前索引
 */
void DAPenStyleComboBox::onCurrentIndexChanged(int index)
{
	emit currentPenStyleChanged(static_cast< Qt::PenStyle >(itemData(index).toInt()));
}

}  // DA

/*** End of inlined file: DAPenStyleComboBox.cpp ***/


/*** Start of inlined file: DAColorGridWidget.cpp ***/
#include <QBrush>
#include <QEvent>
#include <QGridLayout>
#include <QButtonGroup>
#include <QKeyEvent>
#include <QToolButton>
#include <QSpacerItem>
#include <QPainter>
#include <QPixmap>
#include <QVariant>

namespace DA
{
namespace
{
// 16×16、8px 见方的棋盘格（#E6E9EE / #FFFFFF）
QPixmap makeCheckerPixmap(int cell = 8)
{
	QPixmap pm(cell * 2, cell * 2);
	QPainter p(&pm);
	p.fillRect(QRect(0, 0, cell, cell), QColor(0xE6, 0xE9, 0xEE));
	p.fillRect(QRect(cell, 0, cell, cell), Qt::white);
	p.fillRect(QRect(0, cell, cell, cell), Qt::white);
	p.fillRect(QRect(cell, cell, cell, cell), QColor(0xE6, 0xE9, 0xEE));
	p.end();
	return pm;
}
}  // namespace

class DAColorGridWidget::PrivateData
{
	DA_DECLARE_PUBLIC(DAColorGridWidget)
public:
	PrivateData(DAColorGridWidget* p);
	QList<QColor> mColors;
	QGridLayout* mGridLayout { nullptr };
	QButtonGroup* mButtonGroup { nullptr };
	QSize mIconSize { 16, 16 };
	int mColumnCount { 8 };
	bool mColorCheckable { true };
	bool mHorizontalSpacerToRight { false };
	bool mKeyboardNavigation { false };
	bool mAlphaBackground { false };
	QPixmap mCheckerPixmap;
};

DAColorGridWidget::PrivateData::PrivateData(DAColorGridWidget* p)
	: q_ptr(p)
{
	mGridLayout = new QGridLayout(p);
	mGridLayout->setSpacing(0);
	mGridLayout->setContentsMargins(1, 1, 1, 1);
	mButtonGroup = new QButtonGroup(p);
	mButtonGroup->setExclusive(true);
	mCheckerPixmap = makeCheckerPixmap();
}

//===================================================
// DAColorGridWidget
//===================================================

/**
 * @brief 构造颜色网格控件
 * @param par 父控件
 * @details 创建QGridLayout+QButtonGroup(exclusive)，连接按钮组的点击/按下/释放信号
 */
DAColorGridWidget::DAColorGridWidget(QWidget* par) : QWidget(par), DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	connect(d->mButtonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
			this, [this](QAbstractButton* btn) {
		auto* tb = qobject_cast<QToolButton*>(btn);
		if (tb) {
			QColor c = tb->property("color").value<QColor>();
			Q_EMIT colorClicked(c);
		}
	});
	connect(d->mButtonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonPressed),
			this, [this](QAbstractButton* btn) {
		auto* tb = qobject_cast<QToolButton*>(btn);
		if (tb) {
			QColor c = tb->property("color").value<QColor>();
			Q_EMIT colorPressed(c);
		}
	});
	connect(d->mButtonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonReleased),
			this, [this](QAbstractButton* btn) {
		auto* tb = qobject_cast<QToolButton*>(btn);
		if (tb) {
			QColor c = tb->property("color").value<QColor>();
			Q_EMIT colorReleased(c);
		}
	});
}

/**
 * @brief 析构函数
 */
DAColorGridWidget::~DAColorGridWidget()
{
}

/**
 * @brief 设置网格列数
 * @param c 列数
 * @details 颜色按钮按行优先排列，改变列数会触发网格重建
 */
void DAColorGridWidget::setColumnCount(int c)
{
	DA_D(d);
	d->mColumnCount = c;
	updateGridColor();
}

/**
 * @brief 获取网格列数
 * @return 当前列数
 */
int DAColorGridWidget::columnCount() const
{
	DA_DC(d);
	return d->mColumnCount;
}

/**
 * @brief 设置颜色列表
 * @param cls 颜色列表
 * @details 会触发网格重建，原有按钮会被回收或复用
 */
void DAColorGridWidget::setColorList(const QList<QColor>& cls)
{
	DA_D(d);
	d->mColors = cls;
	updateGridColor();
}

/**
 * @brief 获取当前颜色列表
 * @return 颜色列表
 */
QList<QColor> DAColorGridWidget::getColorList() const
{
	DA_DC(d);
	return d->mColors;
}

/**
 * @brief 获取颜色间距
 * @return 间距像素值
 */
int DAColorGridWidget::spacing() const
{
	DA_DC(d);
	return d->mGridLayout->spacing();
}

/**
 * @brief 设置颜色间距（水平和垂直统一）
 * @param v 间距像素值
 */
void DAColorGridWidget::setSpacing(int v)
{
	DA_D(d);
	d->mGridLayout->setSpacing(v);
}

/**
 * @brief 获取颜色数量
 * @return 颜色列表的大小
 */
int DAColorGridWidget::colorCount() const
{
	DA_DC(d);
	return d->mColors.size();
}

/**
 * @brief 设置色块图标尺寸
 * @param s 图标尺寸
 * @details 遍历所有按钮更新图标大小，并设置第一行的最小高度
 */
void DAColorGridWidget::setColorIconSize(const QSize& s)
{
	DA_D(d);
	d->mIconSize = s;
	iterationColorBtns([s](QToolButton* btn) { btn->setIconSize(s); });
	d->mGridLayout->setRowMinimumHeight(0, s.height());
}

/**
 * @brief 获取色块图标尺寸
 * @return 图标尺寸
 */
QSize DAColorGridWidget::colorIconSize() const
{
	DA_DC(d);
	return d->mIconSize;
}

/**
 * @brief 设置色块是否可选中
 * @param on 是否可选中
 * @details 遍历所有按钮设置checkable属性
 */
void DAColorGridWidget::setColorCheckable(bool on)
{
	DA_D(d);
	d->mColorCheckable = on;
	iterationColorBtns([on](QToolButton* btn) { btn->setCheckable(on); });
}

/**
 * @brief 返回色块是否可选中
 * @return 是否可选中
 */
bool DAColorGridWidget::isColorCheckable() const
{
	DA_DC(d);
	return d->mColorCheckable;
}

/**
 * @brief 获取当前被选中的颜色
 * @return 被选中的颜色，无选中项返回无效颜色
 * @details 基于QButtonGroup的checkedButton查询
 */
QColor DAColorGridWidget::currentCheckedColor() const
{
	DA_DC(d);
	auto* btn = qobject_cast<QToolButton*>(d->mButtonGroup->checkedButton());
	if (btn)
		return btn->property("color").value<QColor>();
	return QColor();
}

/**
 * @brief 根据索引获取色块按钮
 * @param index 按钮索引（按布局顺序）
 * @return 对应的QToolButton，索引越界返回nullptr
 */
QToolButton* DAColorGridWidget::colorButton(int index) const
{
	DA_DC(d);
	// 遍历布局找到第 index 个 button
	int count = 0;
	for (int i = 0; i < d->mGridLayout->count(); ++i) {
		auto* item = d->mGridLayout->itemAt(i);
		if (item && item->widget()) {
			auto* btn = qobject_cast<QToolButton*>(item->widget());
			if (btn) {
				if (count == index)
					return btn;
				++count;
			}
		}
	}
	return nullptr;
}

/**
 * @brief 设置垂直间距
 * @param v 垂直间距像素值
 */
void DAColorGridWidget::setVerticalSpacing(int v)
{
	DA_D(d);
	d->mGridLayout->setVerticalSpacing(v);
}

/**
 * @brief 获取垂直间距
 * @return 垂直间距像素值
 */
int DAColorGridWidget::verticalSpacing() const
{
	DA_DC(d);
	return d->mGridLayout->verticalSpacing();
}

/**
 * @brief 设置水平间距
 * @param v 水平间距像素值
 */
void DAColorGridWidget::setHorizontalSpacing(int v)
{
	DA_D(d);
	d->mGridLayout->setHorizontalSpacing(v);
}

/**
 * @brief 获取水平间距
 * @return 水平间距像素值
 */
int DAColorGridWidget::horizontalSpacing() const
{
	DA_DC(d);
	return d->mGridLayout->horizontalSpacing();
}

/**
 * @brief 清除所有色块的选中状态
 * @details 临时取消互斥模式以清除选中，然后恢复互斥模式
 */
void DAColorGridWidget::clearCheckedState()
{
	DA_D(d);
	if (d->mButtonGroup->exclusive()) {
		d->mButtonGroup->setExclusive(false);
		iterationColorBtns([](QToolButton* btn) { btn->setChecked(false); });
		d->mButtonGroup->setExclusive(true);
	} else {
		iterationColorBtns([](QToolButton* btn) {
			if (btn->isChecked())
				btn->setChecked(false);
		});
	}
}

/**
 * @brief 遍历所有色块按钮并执行回调
 * @param fn 对每个按钮执行的回调函数
 */
void DAColorGridWidget::iterationColorBtns(FunColorBtn fn)
{
	DA_D(d);
	for (int i = 0; i < d->mGridLayout->count(); ++i) {
		auto* item = d->mGridLayout->itemAt(i);
		if (item && item->widget()) {
			auto* btn = qobject_cast<QToolButton*>(item->widget());
			if (btn)
				fn(btn);
		}
	}
}

/**
 * @brief 设置指定行的最小高度
 * @param row 行号
 * @param minSize 最小高度
 */
void DAColorGridWidget::setRowMinimumHeight(int row, int minSize)
{
	DA_D(d);
	d->mGridLayout->setRowMinimumHeight(row, minSize);
}

/**
 * @brief 在右侧添加水平弹簧，使色块左对齐
 * @param on 是否添加弹簧
 * @details true时在第0行末列添加Expanding弹簧，false时移除已有弹簧
 */
void DAColorGridWidget::setHorizontalSpacerToRight(bool on)
{
	DA_D(d);
	if (on) {
		if (!d->mHorizontalSpacerToRight) {
			auto* spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
			d->mGridLayout->addItem(spacer, 0, d->mColumnCount, 1, 1);
			d->mHorizontalSpacerToRight = true;
		}
	} else {
		if (d->mHorizontalSpacerToRight) {
			d->mGridLayout->removeItem(d->mGridLayout->itemAtPosition(0, d->mColumnCount));
			d->mHorizontalSpacerToRight = false;
		}
	}
}

/**
 * @brief 推荐尺寸
 * @return 基于网格布局的推荐尺寸
 */
QSize DAColorGridWidget::sizeHint() const
{
	DA_DC(d);
	return d->mGridLayout->sizeHint();
}

void DAColorGridWidget::updateGridColor(bool isRemoveSpacer)
{
	DA_D(d);
	int col = d->mColumnCount;
	if (col <= 0)
		col = d->mColors.size();
	if (col <= 0)
		col = 1;

	const int n = d->mColors.size();
	for (int i = 0; i < n; ++i) {
		int r = i / col;
		int c = i % col;
		setColorAt(d->mColors[i], r, c);
	}
	// 移除多余的 button
	int extraStart = n;
	while (true) {
		int r = extraStart / col;
		int c = extraStart % col;
		auto* item = d->mGridLayout->itemAtPosition(r, c);
		if (!item)
			break;
		if (item->widget()) {
			item->widget()->deleteLater();
		}
		d->mGridLayout->removeItem(item);
		delete item;
		++extraStart;
	}
}

void DAColorGridWidget::setColorAt(const QColor& clr, int r, int c)
{
	DA_D(d);
	auto* item = d->mGridLayout->itemAtPosition(r, c);
	QToolButton* btn = nullptr;
	if (item && item->widget()) {
		btn = qobject_cast<QToolButton*>(item->widget());
	}
	if (!btn) {
		btn = new QToolButton(this);
		btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
		btn->setIconSize(d->mIconSize);
		btn->setAutoRaise(true);
		btn->setCheckable(d->mColorCheckable);
		d->mButtonGroup->addButton(btn, r * 1000 + c);  // 唯一 id
		d->mGridLayout->addWidget(btn, r, c);
		btn->installEventFilter(this);
	}
	btn->setProperty("color", QVariant::fromValue(clr));
	btn->setIcon(d->mAlphaBackground ? makeColorIconWithAlpha(clr, d->mIconSize)
									 : makeColorIcon(clr, d->mIconSize));
	if (d->mKeyboardNavigation)
		applyFocusPolicies();
}

QIcon DAColorGridWidget::makeColorIcon(const QColor& clr, const QSize& size)
{
	QPixmap pixmap(size);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	if (clr.isValid()) {
		painter.fillRect(QRect(QPoint(0, 0), size), clr);
	} else {
		// 无效颜色:绘制禁止符号
		QPen pen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap);
		painter.setPen(pen);
		int ss = size.width() / 3;
		QRect r(QPoint(0, 0), size);
		painter.drawLine(QPoint(r.x() + ss, r.bottom()), QPoint(r.right() - ss, r.top()));
		pen.setColor(Qt::black);
		painter.setPen(pen);
		painter.drawRect(r.adjusted(0, 0, -1, -1));
	}
	return QIcon(pixmap);
}

/**
 * @brief 带棋盘格底的色块图标（仅半透明色需要）
 * @param clr 颜色
 * @param size 图标尺寸
 * @return 图标；不透明色与 makeColorIcon() 逐像素一致
 */
QIcon DAColorGridWidget::makeColorIconWithAlpha(const QColor& clr, const QSize& size) const
{
	DA_DC(d);
	if (!clr.isValid() || clr.alpha() == 255)
		return makeColorIcon(clr, size);
	QPixmap pixmap(size);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	painter.fillRect(QRect(QPoint(0, 0), size), QBrush(d->mCheckerPixmap));
	painter.fillRect(QRect(QPoint(0, 0), size), clr);
	painter.end();
	return QIcon(pixmap);
}

/** @brief 开启/关闭色块键盘导航（默认关闭） */
void DAColorGridWidget::setKeyboardNavigationEnabled(bool on)
{
	DA_D(d);
	if (d->mKeyboardNavigation == on)
		return;
	d->mKeyboardNavigation = on;
	applyFocusPolicies();
}

/** @brief 开启/关闭棋盘格底（默认关闭） */
void DAColorGridWidget::setAlphaBackgroundEnabled(bool on)
{
	DA_D(d);
	if (d->mAlphaBackground == on)
		return;
	d->mAlphaBackground = on;
	updateGridColor();
}

/** @brief columnCount()==0 时的单行规则 */
int DAColorGridWidget::effectiveColumns() const
{
	DA_DC(d);
	int col = d->mColumnCount;
	if (col <= 0)
		col = d->mColors.size();
	if (col <= 0)
		col = 1;
	return col;
}

/**
 * @brief 应用焦点策略
 * @details 开启时只让首块进 Tab 链（其余 ClickFocus），关闭时恢复 QToolButton 默认 StrongFocus
 */
void DAColorGridWidget::applyFocusPolicies()
{
	DA_D(d);
	const int n = d->mColors.size();
	for (int i = 0; i < n; ++i) {
		QToolButton* btn = colorButton(i);
		if (!btn)
			continue;
		btn->setFocusPolicy(d->mKeyboardNavigation ? (i == 0 ? Qt::StrongFocus : Qt::ClickFocus)
												   : Qt::StrongFocus);
	}
}

/**
 * @brief 色块键盘导航：方向键移动、Enter 取色
 * @details 行列数从 effectiveColumns() 读取，不写死；越界时停在原块并吞掉按键
 */
bool DAColorGridWidget::eventFilter(QObject* watched, QEvent* event)
{
	DA_D(d);
	if (!d->mKeyboardNavigation || event->type() != QEvent::KeyPress)
		return QWidget::eventFilter(watched, event);
	auto* btn = qobject_cast< QToolButton* >(watched);
	if (!btn)
		return QWidget::eventFilter(watched, event);
	const int n = d->mColors.size();
	int idx = -1;
	for (int i = 0; i < n; ++i) {
		if (colorButton(i) == btn) {
			idx = i;
			break;
		}
	}
	if (idx < 0)
		return QWidget::eventFilter(watched, event);

	auto* ke = static_cast< QKeyEvent* >(event);
	const int key = ke->key();
	if (key == Qt::Key_Return || key == Qt::Key_Enter) {
		btn->click();
		return true;
	}
	const int cols = effectiveColumns();
	int next = idx;
	switch (key) {
	case Qt::Key_Left:
		if (idx % cols != 0)
			next = idx - 1;
		break;
	case Qt::Key_Right:
		if ((idx % cols) != (cols - 1) && idx + 1 < n)
			next = idx + 1;
		break;
	case Qt::Key_Up:
		if (idx - cols >= 0)
			next = idx - cols;
		break;
	case Qt::Key_Down:
		if (idx + cols < n)
			next = idx + cols;
		break;
	default:
		return QWidget::eventFilter(watched, event);
	}
	if (next != idx) {
		if (QToolButton* nb = colorButton(next))
			nb->setFocus(Qt::ShortcutFocusReason);
	}
	return true;
}
}  // namespace DA

/*** End of inlined file: DAColorGridWidget.cpp ***/


/*** Start of inlined file: DAColorPaletteGridWidget.cpp ***/
#include <QVBoxLayout>

namespace DA
{
/// Office 风格默认 10 色主色列表
static QList<QColor> getStandardColorList()
{
	return {
		QColor(192, 0, 0),    // 深红
		QColor(255, 0, 0),    // 红
		QColor(255, 192, 0),  // 橙
		QColor(255, 255, 0),  // 黄
		QColor(146, 208, 80), // 黄绿
		QColor(0, 176, 80),   // 绿
		QColor(0, 176, 240),  // 浅蓝
		QColor(0, 112, 192),  // 蓝
		QColor(0, 32, 96),    // 深蓝
		QColor(112, 48, 160), // 紫
	};
}

class DAColorPaletteGridWidget::PrivateData
{
	DA_DECLARE_PUBLIC(DAColorPaletteGridWidget)
public:
	PrivateData(DAColorPaletteGridWidget* p);
	QList<int> mFactor { 180, 160, 140, 75, 50 };  // 3 行浅色 + 2 行深色
	QVBoxLayout* mLayout { nullptr };
	DAColorGridWidget* mMainColorList { nullptr };
	DAColorGridWidget* mPaletteColorGrid { nullptr };
	QList<QColor> mColors;
};

DAColorPaletteGridWidget::PrivateData::PrivateData(DAColorPaletteGridWidget* p)
	: q_ptr(p)
{
	mLayout = new QVBoxLayout(p);
	mLayout->setContentsMargins(1, 1, 1, 1);
	mLayout->setSpacing(8);

	mMainColorList = new DAColorGridWidget(p);
	mMainColorList->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	mMainColorList->setColumnCount(0);  // 单行
	mLayout->addWidget(mMainColorList);

	mPaletteColorGrid = new DAColorGridWidget(p);
	mPaletteColorGrid->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	mPaletteColorGrid->setVerticalSpacing(0);
	mLayout->addWidget(mPaletteColorGrid);
}

//===================================================
// DAColorPaletteGridWidget
//===================================================

/**
 * @brief 构造默认Office风格色板
 * @param par 父控件
 * @details 使用内置10色Office主色列表初始化
 */
DAColorPaletteGridWidget::DAColorPaletteGridWidget(QWidget* par) : QWidget(par), DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	d->mColors = getStandardColorList();
	init();
}

/**
 * @brief 构造自定义主色色板
 * @param cls 主色列表
 * @param par 父控件
 */
DAColorPaletteGridWidget::DAColorPaletteGridWidget(const QList<QColor>& cls, QWidget* par)
	: QWidget(par), DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	d->mColors = cls;
	init();
}

/**
 * @brief 析构函数
 */
DAColorPaletteGridWidget::~DAColorPaletteGridWidget()
{
}

void DAColorPaletteGridWidget::init()
{
	DA_D(d);
	setColorIconSize(QSize(10, 10));
	setColorList(d->mColors);

	connect(d->mMainColorList, &DAColorGridWidget::colorClicked,
			this, &DAColorPaletteGridWidget::onMainColorClicked);
	connect(d->mPaletteColorGrid, &DAColorGridWidget::colorClicked,
			this, &DAColorPaletteGridWidget::onPaletteColorClicked);
}

/**
 * @brief 设置主色列表
 * @param cls 主色列表
 * @details 同时重建主色行和调色板行
 */
void DAColorPaletteGridWidget::setColorList(const QList<QColor>& cls)
{
	DA_D(d);
	d->mColors = cls;
	d->mMainColorList->setColumnCount(0);  // 单行
	d->mMainColorList->setColorList(cls);
	d->mPaletteColorGrid->setColumnCount(cls.size());
	d->mPaletteColorGrid->setColorList(makeColorPalette(cls));
}

/**
 * @brief 获取主色列表
 * @return 主色列表
 */
QList<QColor> DAColorPaletteGridWidget::colorList() const
{
	DA_DC(d);
	return d->mColors;
}

/**
 * @brief 设置浅色/深色变体因子列表
 * @param factor lighter因子列表，前几个为浅色(>100)，后几个为深色(<100)
 * @details 会根据新因子重新生成调色板
 */
void DAColorPaletteGridWidget::setFactor(const QList<int>& factor)
{
	DA_D(d);
	d->mFactor = factor;
	d->mPaletteColorGrid->setColorList(makeColorPalette(d->mColors));
}

/**
 * @brief 获取浅色/深色变体因子列表
 * @return 因子列表
 */
QList<int> DAColorPaletteGridWidget::factor() const
{
	DA_DC(d);
	return d->mFactor;
}

/**
 * @brief 设置色块图标尺寸
 * @param s 图标尺寸
 * @details 同时设置主色行和调色板行的图标尺寸
 */
void DAColorPaletteGridWidget::setColorIconSize(const QSize& s)
{
	DA_D(d);
	d->mMainColorList->setColorIconSize(s);
	d->mPaletteColorGrid->setColorIconSize(s);
}

/**
 * @brief 获取色块图标尺寸
 * @return 图标尺寸
 */
QSize DAColorPaletteGridWidget::colorIconSize() const
{
	DA_DC(d);
	return d->mMainColorList->colorIconSize();
}

/**
 * @brief 设置色块是否可选中
 * @param on 是否可选中
 * @details 同时设置主色行和调色板行的checkable
 */
void DAColorPaletteGridWidget::setColorCheckable(bool on)
{
	DA_D(d);
	d->mMainColorList->setColorCheckable(on);
	d->mPaletteColorGrid->setColorCheckable(on);
}

/**
 * @brief 返回色块是否可选中
 * @return 是否可选中
 */
bool DAColorPaletteGridWidget::isColorCheckable() const
{
	DA_DC(d);
	return d->mMainColorList->isColorCheckable();
}

/**
 * @brief 转发键盘导航开关到内部两个网格
 * @param on 是否启用（默认关闭）
 */
void DAColorPaletteGridWidget::setKeyboardNavigationEnabled(bool on)
{
	DA_D(d);
	d->mMainColorList->setKeyboardNavigationEnabled(on);
	d->mPaletteColorGrid->setKeyboardNavigationEnabled(on);
}

QList<QColor> DAColorPaletteGridWidget::makeColorPalette(const QList<QColor>& cls) const
{
	DA_DC(d);
	QList<QColor> res;
	for (int f : d->mFactor) {
		for (const QColor& c : cls) {
			res.append(c.lighter(f));
		}
	}
	return res;
}

void DAColorPaletteGridWidget::onMainColorClicked(const QColor& c)
{
	DA_D(d);
	d->mPaletteColorGrid->clearCheckedState();
	Q_EMIT colorClicked(c);
}

void DAColorPaletteGridWidget::onPaletteColorClicked(const QColor& c)
{
	DA_D(d);
	d->mMainColorList->clearCheckedState();
	Q_EMIT colorClicked(c);
}
}  // namespace DA

/*** End of inlined file: DAColorPaletteGridWidget.cpp ***/


/*** Start of inlined file: DAColorMenu.cpp ***/
#include <QAction>
#include <QLabel>
#include <QWidgetAction>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QPointer>
#include <QSignalBlocker>
#include <QSlider>
#include <utility>

namespace DA
{
class DAColorMenu::PrivateData
{
	DA_DECLARE_PUBLIC(DAColorMenu)
public:
	PrivateData(DAColorMenu* p);
	QLabel* mTitleLabel { nullptr };
	QWidgetAction* mTitleLabelAction { nullptr };
	DAColorPaletteGridWidget* mThemeColorsWidget { nullptr };
	QWidgetAction* mThemeColorPaletteAction { nullptr };
	QAction* mCustomColorAction { nullptr };
	DAColorGridWidget* mCustomColorsWidget { nullptr };
	QWidgetAction* mCustomColorsWidgetAction { nullptr };
	QAction* mNoneColorAction { nullptr };
	QList<QColor> mCustomColors;
	int mMaxCustomColorSize { 10 };
	QPointer<QColorDialog> mColorDlg;

	// alpha
	bool mAlphaEnabled { false };
	int mAlpha { 255 };
	QWidget* mAlphaRow { nullptr };
	QSlider* mAlphaSlider { nullptr };
	QLabel* mAlphaValueLabel { nullptr };
	QLabel* mAlphaPreview { nullptr };
	QWidgetAction* mAlphaAction { nullptr };
	QColor mLastColor;

	// 最近色 / 恢复默认
	bool mRecentColorsEnabled { false };
	QList<QColor> mRecentColors;
	int mMaxRecentColors { 5 };
	QAction* mDefaultColorAction { nullptr };
	QColor mDefaultColor;
};

DAColorMenu::PrivateData::PrivateData(DAColorMenu* p) : q_ptr(p)
{
}

//===================================================
// DAColorMenu
//===================================================

/**
 * @brief 构造默认主题色的颜色菜单
 * @param parent 父控件
 * @details 使用DAColorPaletteGridWidget的默认Office主题色初始化
 */
DAColorMenu::DAColorMenu(QWidget* parent) : QMenu(parent), DA_PIMPL_CONSTRUCT
{
	// 默认主题色由 DAColorPaletteGridWidget 构造函数提供
	init({});
}

/**
 * @brief 构造带标题的颜色菜单
 * @param title 菜单标题
 * @param parent 父控件
 */
DAColorMenu::DAColorMenu(const QString& title, QWidget* parent) : QMenu(title, parent), DA_PIMPL_CONSTRUCT
{
	init({});
}

/**
 * @brief 析构函数
 */
DAColorMenu::~DAColorMenu()
{
}

void DAColorMenu::init(const QList<QColor>& themeCls)
{
	DA_D(d);
	// 标题
	d->mTitleLabel = new QLabel(QStringLiteral("Theme Colors"), this);
	d->mTitleLabelAction = new QWidgetAction(this);
	d->mTitleLabelAction->setDefaultWidget(d->mTitleLabel);
	addAction(d->mTitleLabelAction);

	// 主题色板
	if (themeCls.isEmpty())
		d->mThemeColorsWidget = new DAColorPaletteGridWidget(this);
	else
		d->mThemeColorsWidget = new DAColorPaletteGridWidget(themeCls, this);
	d->mThemeColorsWidget->setColorCheckable(false);
	d->mThemeColorPaletteAction = new QWidgetAction(this);
	d->mThemeColorPaletteAction->setDefaultWidget(d->mThemeColorsWidget);
	addAction(d->mThemeColorPaletteAction);

	// Custom Color action
	d->mCustomColorAction = new QAction(QStringLiteral("Custom Color"), this);
	addAction(d->mCustomColorAction);

	// 自定义颜色记录网格
	d->mCustomColorsWidget = new DAColorGridWidget(this);
	d->mCustomColorsWidget->setRowMinimumHeight(0, d->mThemeColorsWidget->colorIconSize().height());
	d->mCustomColorsWidget->setHorizontalSpacerToRight(true);
	d->mCustomColorsWidget->setColorIconSize(d->mThemeColorsWidget->colorIconSize());
	d->mCustomColorsWidget->setColumnCount(d->mMaxCustomColorSize);
	d->mCustomColorsWidget->setColorCheckable(false);
	d->mCustomColorsWidgetAction = new QWidgetAction(this);
	d->mCustomColorsWidgetAction->setDefaultWidget(d->mCustomColorsWidget);
	addAction(d->mCustomColorsWidgetAction);

	// 连线
	connect(d->mCustomColorAction, &QAction::triggered, this, &DAColorMenu::onCustomColorActionTriggered);
	connect(d->mThemeColorsWidget, &DAColorPaletteGridWidget::colorClicked, this, &DAColorMenu::emitSelectedColor);
	connect(d->mCustomColorsWidget, &DAColorGridWidget::colorClicked, this, &DAColorMenu::emitSelectedColor);
	// 最近色重排/预览更新推迟到菜单显示时，避免在按钮点击回调里重建网格
	connect(this, &QMenu::aboutToShow, this, [ this ]() {
		DA_D(d);
		refreshRecordGrid();
		updateAlphaPreview();
	});
}

/**
 * @brief 获取主题色板的 QWidgetAction
 * @return 主题色板对应的QWidgetAction
 */
QWidgetAction* DAColorMenu::themeColorsPaletteAction() const
{
	DA_DC(d);
	return d->mThemeColorPaletteAction;
}

/**
 * @brief 获取自定义颜色记录网格的 QWidgetAction
 * @return 自定义颜色网格对应的QWidgetAction
 */
QWidgetAction* DAColorMenu::getCustomColorsWidgetAction() const
{
	DA_DC(d);
	return d->mCustomColorsWidgetAction;
}

/**
 * @brief 获取"自定义颜色"Action
 * @return 自定义颜色Action，点击后弹出QColorDialog
 */
QAction* DAColorMenu::customColorAction() const
{
	DA_DC(d);
	return d->mCustomColorAction;
}

/**
 * @brief 获取主题色板网格控件
 * @return 主题色板控件指针
 */
DAColorPaletteGridWidget* DAColorMenu::colorPaletteGridWidget() const
{
	DA_DC(d);
	return d->mThemeColorsWidget;
}

/**
 * @brief 获取自定义颜色记录网格控件
 * @return 自定义颜色网格控件指针
 */
DAColorGridWidget* DAColorMenu::customColorsWidget() const
{
	DA_DC(d);
	return d->mCustomColorsWidget;
}

/**
 * @brief 启用或禁用"无颜色"Action
 * @param on true启用，false禁用
 * @details 启用时在Custom Color之前插入None Action，禁用时移除
 */
void DAColorMenu::enableNoneColorAction(bool on)
{
	DA_D(d);
	if (on) {
		if (!d->mNoneColorAction) {
			d->mNoneColorAction = new QAction(createNoneColorIcon(), QStringLiteral("None"), this);
			connect(d->mNoneColorAction, &QAction::triggered, this, &DAColorMenu::onNoneColorActionTriggered);
			// 插入到 Custom Color action 之前
			insertAction(d->mCustomColorAction, d->mNoneColorAction);
		}
	} else {
		if (d->mNoneColorAction) {
			removeAction(d->mNoneColorAction);
		}
	}
}

/**
 * @brief 获取"无颜色"Action
 * @return 无颜色Action，未启用时返回nullptr
 */
QAction* DAColorMenu::noneColorAction() const
{
	DA_DC(d);
	return d->mNoneColorAction;
}

/**
 * @brief 发射 selectedColor 信号并隐藏菜单
 * @param c 选中的颜色，无效颜色表示"无颜色"
 */
void DAColorMenu::emitSelectedColor(const QColor& c)
{
	selectColor(c, true);
}

void DAColorMenu::onCustomColorActionTriggered(bool)
{
	const QColor c = getColorByDialog();
	if (c.isValid())
		emitSelectedColor(c);
}

void DAColorMenu::onNoneColorActionTriggered(bool)
{
	emitSelectedColor(QColor());  // 无效颜色表示"无颜色"
}

QColor DAColorMenu::getColorByDialog()
{
	DA_D(d);
	if (!d->mColorDlg) {
		d->mColorDlg = new QColorDialog(this);
		d->mColorDlg->setOption(QColorDialog::ShowAlphaChannel, true);
	}
	if (d->mAlphaEnabled && d->mLastColor.isValid()) {
		d->mColorDlg->setCurrentColor(
			QColor(d->mLastColor.red(), d->mLastColor.green(), d->mLastColor.blue(), d->mAlpha));
	}
	if (d->mColorDlg->exec() == QDialog::Accepted) {
		const QColor c = d->mColorDlg->currentColor();
		// 对话框返回的 alpha 回写滑杆，保持"滑杆是唯一真源"
		if (d->mAlphaEnabled)
			setAlpha(c.alpha());
		return c;
	}
	return QColor();
}

void DAColorMenu::recordCustomColor(const QColor& c)
{
	DA_D(d);
	if (d->mCustomColors.size() < d->mMaxCustomColorSize) {
		d->mCustomColors.append(c);
	} else {
		// FIFO 淘汰最旧
		for (int i = 0; i < d->mCustomColors.size() - 1; ++i) {
			d->mCustomColors[i] = d->mCustomColors[i + 1];
		}
		d->mCustomColors.back() = c;
	}
}

QIcon DAColorMenu::createNoneColorIcon()
{
	QPixmap pixmap(32, 32);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	QRect r = pixmap.rect().adjusted(1, 1, -1, -1);
	// 红色对角线
	QPen pen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap);
	painter.setPen(pen);
	int ss = r.width() / 3;
	painter.drawLine(QPoint(r.x() + ss, r.bottom()), QPoint(r.right() - ss, r.top()));
	// 黑色边框
	pen.setColor(Qt::black);
	painter.setPen(pen);
	painter.drawRect(r);
	return QIcon(pixmap);
}

/**
 * @brief 打开/关闭透明度行
 * @param on 是否启用（默认 false）
 * @details 打开时把行追加到菜单末尾；关闭时只移除 action，行对象保留，滑杆位置不丢
 */
void DAColorMenu::setAlphaEnabled(bool on)
{
	DA_D(d);
	if (d->mAlphaEnabled == on)
		return;
	d->mAlphaEnabled = on;
	if (on) {
		if (!d->mAlphaAction)
			buildAlphaRow();
		addAction(d->mAlphaAction);
		updateAlphaPreview();
	} else if (d->mAlphaAction) {
		removeAction(d->mAlphaAction);
	}
}

/** @brief 透明度行是否启用 */
bool DAColorMenu::isAlphaEnabled() const
{
	DA_DC(d);
	return d->mAlphaEnabled;
}

/** @brief 当前 alpha（0–255） */
int DAColorMenu::alpha() const
{
	DA_DC(d);
	return d->mAlpha;
}

/**
 * @brief 设置当前 alpha
 * @param a 0–255，越界限幅；值不变时不发 alphaChanged
 */
void DAColorMenu::setAlpha(int a)
{
	DA_D(d);
	const int clamped = qBound(0, a, 255);
	if (clamped == d->mAlpha)
		return;
	d->mAlpha = clamped;
	if (d->mAlphaSlider && d->mAlphaSlider->value() != clamped) {
		QSignalBlocker blocker(d->mAlphaSlider);
		d->mAlphaSlider->setValue(clamped);
	}
	updateAlphaPreview();
	Q_EMIT alphaChanged(clamped);
}

void DAColorMenu::buildAlphaRow()
{
	DA_D(d);
	d->mAlphaRow = new QWidget(this);
	auto* lay = new QHBoxLayout(d->mAlphaRow);
	lay->setContentsMargins(6, 2, 6, 2);
	lay->setSpacing(6);

	auto* label = new QLabel(tr("Opacity"), d->mAlphaRow);  //cn:不透明度
	d->mAlphaSlider = new QSlider(Qt::Horizontal, d->mAlphaRow);
	d->mAlphaSlider->setObjectName(QStringLiteral("DAAlphaSlider"));
	d->mAlphaSlider->setRange(0, 255);
	d->mAlphaSlider->setValue(d->mAlpha);

	d->mAlphaValueLabel = new QLabel(d->mAlphaRow);
	d->mAlphaValueLabel->setObjectName(QStringLiteral("DAAlphaValueLabel"));
	// 定宽，避免 9%→100% 时行宽抖动
	const int w = Qt5Qt6Compat_fontMetrics_width(d->mAlphaValueLabel->fontMetrics(), QStringLiteral("100%")) + 6;
	d->mAlphaValueLabel->setFixedWidth(w);

	d->mAlphaPreview = new QLabel(d->mAlphaRow);
	d->mAlphaPreview->setObjectName(QStringLiteral("DAAlphaPreview"));
	d->mAlphaPreview->setFixedSize(24, 18);

	lay->addWidget(label);
	lay->addWidget(d->mAlphaSlider, 1);
	lay->addWidget(d->mAlphaValueLabel);
	lay->addWidget(d->mAlphaPreview);

	d->mAlphaAction = new QWidgetAction(this);
	d->mAlphaAction->setDefaultWidget(d->mAlphaRow);
	connect(d->mAlphaSlider, &QSlider::valueChanged, this, &DAColorMenu::setAlpha);
	updateAlphaPreview();
}

void DAColorMenu::updateAlphaPreview()
{
	DA_D(d);
	if (!d->mAlphaSlider || !d->mAlphaValueLabel || !d->mAlphaPreview)
		return;
	d->mAlphaValueLabel->setText(QStringLiteral("%1%").arg(qRound(d->mAlpha * 100.0 / 255.0)));
	QPixmap pm(24, 18);
	pm.fill(Qt::transparent);
	QPainter p(&pm);
	for (int y = 0; y < 18; y += 6) {
		for (int x = 0; x < 24; x += 6) {
			const bool odd = ((x / 6) + (y / 6)) % 2 != 0;
			p.fillRect(QRect(x, y, 6, 6), odd ? QColor(0xE6, 0xE9, 0xEE) : QColor(Qt::white));
		}
	}
	QColor c = d->mLastColor.isValid() ? d->mLastColor : QColor(64, 158, 255);
	c.setAlpha(d->mAlpha);
	p.fillRect(pm.rect(), c);
	p.end();
	d->mAlphaPreview->setPixmap(pm);
}

/**
 * @brief 启用最近色策略
 * @param colors 初始最近色（最近优先），去重后截断到 5
 */
void DAColorMenu::setRecentColors(const QList<QColor>& colors)
{
	DA_D(d);
	d->mRecentColorsEnabled = true;
	d->mRecentColors.clear();
	for (const QColor& c : colors) {
		if (!c.isValid())
			continue;
		const QRgb key = c.toRgb().rgba();
		bool seen = false;
		for (const QColor& e : std::as_const(d->mRecentColors)) {
			if (e.toRgb().rgba() == key) {
				seen = true;
				break;
			}
		}
		if (seen)
			continue;
		d->mRecentColors << c;
		if (d->mRecentColors.size() >= d->mMaxRecentColors)
			break;
	}
	d->mCustomColorsWidget->setAlphaBackgroundEnabled(true);
	const QSize icon = d->mCustomColorsWidget->colorIconSize();
	d->mCustomColorsWidget->setMinimumWidth(icon.width() * d->mMaxRecentColors
											+ d->mCustomColorsWidget->horizontalSpacing()
												  * (d->mMaxRecentColors - 1));
	refreshRecordGrid();
}

/** @brief 获取最近色（最近优先）；未启用时为空 */
QList<QColor> DAColorMenu::recentColors() const
{
	DA_DC(d);
	return d->mRecentColors;
}

/** @brief 把当前生效的记录列表刷进网格 */
void DAColorMenu::refreshRecordGrid()
{
	DA_D(d);
	if (!d->mCustomColorsWidget)
		return;
	d->mCustomColorsWidget->setColorList(d->mRecentColorsEnabled ? d->mRecentColors : d->mCustomColors);
	d->mCustomColorsWidget->updateGeometry();
}

void DAColorMenu::recordRecentColor(const QColor& c)
{
	DA_D(d);
	const QRgb key = c.toRgb().rgba();
	for (int i = d->mRecentColors.size() - 1; i >= 0; --i) {
		if (d->mRecentColors.at(i).toRgb().rgba() == key)
			d->mRecentColors.removeAt(i);
	}
	d->mRecentColors.prepend(c);
	while (d->mRecentColors.size() > d->mMaxRecentColors)
		d->mRecentColors.removeLast();
}

/**
 * @brief 设置/移除「恢复默认」入口
 * @param color 有效色插入 action；无效色移除入口
 */
void DAColorMenu::setDefaultColor(const QColor& color)
{
	DA_D(d);
	d->mDefaultColor = color;
	if (color.isValid()) {
		if (!d->mDefaultColorAction) {
			d->mDefaultColorAction = new QAction(tr("Restore Default"), this);  //cn:恢复默认
			connect(d->mDefaultColorAction, &QAction::triggered, this, [ this ]() {
				DA_D(d);
				selectColor(d->mDefaultColor, false);
			});
		}
		insertAction(d->mCustomColorAction, d->mDefaultColorAction);
	} else if (d->mDefaultColorAction) {
		removeAction(d->mDefaultColorAction);
		d->mDefaultColorAction->deleteLater();
		d->mDefaultColorAction = nullptr;
	}
}

/** @brief 当前默认色（未设置时无效） */
QColor DAColorMenu::defaultColor() const
{
	DA_DC(d);
	return d->mDefaultColor;
}

/**
 * @brief 统一选色出口
 * @param c 选中的颜色
 * @param recordToRecent true = 常规选色：应用 alpha 并记录；false = 恢复默认：原样发出
 */
void DAColorMenu::selectColor(const QColor& c, bool recordToRecent)
{
	DA_D(d);
	QColor out = c;
	if (d->mAlphaEnabled && recordToRecent && out.isValid())
		out.setAlpha(d->mAlpha);
	if (out.isValid())
		d->mLastColor = out;
	if (out.isValid() && recordToRecent) {
		if (d->mRecentColorsEnabled)
			recordRecentColor(out);
		else
			recordCustomColor(out);
	}
	Q_EMIT selectedColor(out);
	hide();
}
}  // namespace DA

/*** End of inlined file: DAColorMenu.cpp ***/


/*** Start of inlined file: DAColorPickerButton.cpp ***/
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QStyle>
#include <QResizeEvent>
#include <QPaintEvent>

namespace DA
{
class DAColorPickerButton::PrivateData
{
	DA_DECLARE_PUBLIC(DAColorPickerButton)
public:
	PrivateData(DAColorPickerButton* p);
	QColor mColor { Qt::white };
	int mSpacing { 2 };                              // icon 与 color 之间的间隔
	QMargins mMargins { 3, 3, 3, 3 };                // 内容边距
	ColorToolButtonStyle mColorButtonStyle { WithColorMenu };
};

DAColorPickerButton::PrivateData::PrivateData(DAColorPickerButton* p) : q_ptr(p)
{
}

//===================================================
// DAColorPickerButton
//===================================================

/**
 * @brief 构造默认WithColorMenu模式的颜色拾取按钮
 * @param parent 父控件
 */
DAColorPickerButton::DAColorPickerButton(QWidget* parent)
	: QToolButton(parent), DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	connect(this, &QToolButton::clicked, this, &DAColorPickerButton::onButtonClicked);
	setColorToolButtonStyle(WithColorMenu);
}

/**
 * @brief 构造指定样式的颜色拾取按钮
 * @param style 按钮样式模式
 * @param parent 父控件
 */
DAColorPickerButton::DAColorPickerButton(ColorToolButtonStyle style, QWidget* parent)
	: QToolButton(parent), DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	connect(this, &QToolButton::clicked, this, &DAColorPickerButton::onButtonClicked);
	setColorToolButtonStyle(style);
}

/**
 * @brief 析构函数
 */
DAColorPickerButton::~DAColorPickerButton()
{
}

/**
 * @brief 获取当前颜色
 * @return 当前颜色
 */
QColor DAColorPickerButton::color() const
{
	DA_DC(d);
	return d->mColor;
}

/**
 * @brief 设置当前颜色
 * @param c 新颜色
 * @details 颜色变化时更新显示并发射colorChanged信号
 */
void DAColorPickerButton::setColor(const QColor& c)
{
	DA_D(d);
	if (d->mColor == c)
		return;
	d->mColor = c;
	update();
	Q_EMIT colorChanged(c);
}

/**
 * @brief 获取绘制边距
 * @return 内容边距
 */
QMargins DAColorPickerButton::margins() const
{
	DA_DC(d);
	return d->mMargins;
}

/**
 * @brief 设置绘制边距
 * @param mg 新边距
 */
void DAColorPickerButton::setMargins(const QMargins& mg)
{
	DA_D(d);
	d->mMargins = mg;
	update();
}

/**
 * @brief 获取按钮样式模式
 * @return 当前样式模式
 */
DAColorPickerButton::ColorToolButtonStyle DAColorPickerButton::colorToolButtonStyle() const
{
	DA_DC(d);
	return d->mColorButtonStyle;
}

/**
 * @brief 设置按钮样式模式
 * @param s 样式模式
 * @details WithColorMenu模式自动创建DAColorMenu，NoColorMenu模式移除已有菜单
 */
void DAColorPickerButton::setColorToolButtonStyle(ColorToolButtonStyle s)
{
	DA_D(d);
	d->mColorButtonStyle = s;
	if (s == WithColorMenu) {
		createColorMenu();
	} else {
		QMenu* m = menu();
		if (m) {
			setMenu(nullptr);
			m->deleteLater();
		}
		setPopupMode(QToolButton::DelayedPopup);
	}
}

/**
 * @brief 获取关联的颜色菜单
 * @return 颜色菜单指针，不存在返回nullptr
 */
DAColorMenu* DAColorPickerButton::colorMenu() const
{
	return qobject_cast<DAColorMenu*>(menu());
}

/**
 * @brief 创建并关联颜色菜单
 * @return 创建的DAColorMenu指针
 * @details 设置InstantPopup模式，启用NoneColorAction，连接选色信号
 */
DAColorMenu* DAColorPickerButton::createColorMenu()
{
	DA_D(d);
	setPopupMode(QToolButton::InstantPopup);
	DAColorMenu* m = new DAColorMenu(this);
	m->enableNoneColorAction(true);
	connect(m, &DAColorMenu::selectedColor, this, &DAColorPickerButton::setColor);
	setMenu(m);
	return m;
}

/**
 * @brief 绘制"无颜色"标记
 * @param p 画笔
 * @param colorRect 绘制区域
 * @details 绘制红色对角线+黑色边框表示无颜色状态
 */
void DAColorPickerButton::paintNoneColor(QPainter* p, const QRect& colorRect)
{
	p->save();
	QPen pen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap);
	p->setPen(pen);
	p->setRenderHint(QPainter::SmoothPixmapTransform, true);
	p->setRenderHint(QPainter::Antialiasing, true);
	int ss = colorRect.width() / 3;
	p->drawLine(QPoint(colorRect.x() + ss, colorRect.bottom()),
				QPoint(colorRect.right() - ss, colorRect.top()));
	pen.setColor(Qt::black);
	p->setPen(pen);
	p->drawRect(colorRect);
	p->restore();
}

/**
 * @brief 按钮点击时发射colorClicked信号
 * @param checked 按钮是否处于选中状态
 */
void DAColorPickerButton::onButtonClicked(bool checked)
{
	DA_D(d);
	Q_EMIT colorClicked(d->mColor, checked);
}

/**
 * @brief 重绘事件，自绘按钮全部内容
 * @details 按顺序绘制：按钮面板→图标→文字→颜色块
 */
void DAColorPickerButton::paintEvent(QPaintEvent*)
{
	QStylePainter p(this);
	QStyleOptionToolButton opt;
	initStyleOption(&opt);

	QRect iconRect, textRect, colorRect;
	calcRect(opt, iconRect, textRect, colorRect);

	paintButton(&p, opt);
	paintIcon(&p, iconRect, opt);
	paintText(&p, textRect, opt);
	DA_D(d);
	paintColor(&p, colorRect, d->mColor, opt);
}

/**
 * @brief 尺寸变化事件，触发重绘
 * @param event 尺寸变化事件
 */
void DAColorPickerButton::resizeEvent(QResizeEvent* event)
{
	QToolButton::resizeEvent(event);
	update();
}

/**
 * @brief 推荐尺寸
 * @return 根据toolButtonStyle、图标、文字、边距计算的推荐尺寸
 * @details 不同ToolButtonStyle下图标/文字/颜色块的排列方式不同
 */
QSize DAColorPickerButton::sizeHint() const
{
	DA_DC(d);
	QStyleOptionToolButton opt;
	initStyleOption(&opt);

	int w = 0, h = 0;
	const QSize is = iconSize();
	const QSize ts = opt.fontMetrics.size(Qt::TextShowMnemonic, text());

	switch (toolButtonStyle()) {
	case Qt::ToolButtonIconOnly:
		w = is.width() + d->mMargins.left() + d->mMargins.right();
		h = is.height() + d->mMargins.top() + d->mMargins.bottom();
		break;
	case Qt::ToolButtonTextOnly:
		w = ts.width() + d->mSpacing + is.width() + d->mMargins.left() + d->mMargins.right();
		h = qMax(ts.height(), is.height()) + d->mMargins.top() + d->mMargins.bottom();
		break;
	case Qt::ToolButtonTextBesideIcon: {
		int extraH = 0;
		if (!opt.icon.isNull())
			extraH = is.height() / 4 + d->mSpacing;
		w = ts.width() + d->mSpacing + is.width() + d->mMargins.left() + d->mMargins.right();
		h = qMax(ts.height(), is.height() + extraH) + d->mMargins.top() + d->mMargins.bottom();
		break;
	}
	case Qt::ToolButtonTextUnderIcon:
		w = qMax(ts.width(), is.width()) + d->mMargins.left() + d->mMargins.right();
		h = ts.height() + is.height() + d->mSpacing + d->mMargins.top() + d->mMargins.bottom();
		break;
	default:
		w = is.width() + d->mMargins.left() + d->mMargins.right();
		h = is.height() + d->mMargins.top() + d->mMargins.bottom();
		break;
	}

	if (opt.features & QStyleOptionToolButton::MenuButtonPopup ||
		opt.features & QStyleOptionToolButton::HasMenu) {
		w += style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);
	}

	return style()->sizeFromContents(QStyle::CT_ToolButton, &opt, QSize(w, h), this).expandedTo(QSize(2, 2));
}

/**
 * @brief 计算图标、文字、颜色块的布局矩形
 * @param opt 样式选项
 * @param iconRect [out] 图标矩形
 * @param textRect [out] 文字矩形
 * @param colorRect [out] 颜色块矩形
 * @details 根据ToolButtonStyle和是否有图标计算各元素的位置和大小
 */
void DAColorPickerButton::calcRect(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect)
{
	DA_D(d);
	QRect buttonRect = style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButton, this);
	buttonRect = buttonRect.marginsRemoved(d->mMargins);

	const QSize is = iconSize();
	const QSize ts = opt.fontMetrics.size(Qt::TextShowMnemonic, text());

	switch (opt.toolButtonStyle) {
	case Qt::ToolButtonIconOnly: {
		if (opt.icon.isNull()) {
			// 无 icon:颜色占满
			colorRect = buttonRect;
			iconRect = QRect();
			textRect = QRect();
		} else {
			// icon 在上,color 在下(高度为 icon 的 1/4)
			int colorHeight = is.height() / 4;
			int totalHeight = colorHeight + is.height() + d->mSpacing;
			if (totalHeight > buttonRect.height()) {
				// 空间不足:反推
				colorHeight = (buttonRect.height() - d->mSpacing) / 5;
				int iconHeight = colorHeight * 4;
				int iconWidth = is.width() * (float)iconHeight / is.height();
				int yOffset = (buttonRect.height() - totalHeight) / 2;
				if (yOffset < 0)
					yOffset = 0;
				iconRect = QRect(buttonRect.x() + (buttonRect.width() - iconWidth) / 2,
								 buttonRect.y() + yOffset, iconWidth, iconHeight);
				colorRect = QRect(iconRect.x(), iconRect.bottom() + d->mSpacing,
								  iconRect.width(), colorHeight);
			} else {
				int yOffset = (buttonRect.height() - totalHeight) / 2;
				iconRect = QRect(buttonRect.x() + (buttonRect.width() - is.width()) / 2,
								 buttonRect.y() + yOffset, is.width(), is.height());
				colorRect = QRect(iconRect.x(), iconRect.bottom() + d->mSpacing,
								  iconRect.width(), colorHeight);
			}
			textRect = QRect();
		}
		break;
	}
	case Qt::ToolButtonTextOnly: {
		iconRect = QRect();
		QSize colorSize = is.isValid() ? is : QSize(16, 16);
		colorRect = QRect(buttonRect.x(),
						  buttonRect.y() + (buttonRect.height() - colorSize.height()) / 2,
						  colorSize.width(), colorSize.height());
		textRect = buttonRect.adjusted(colorRect.right() + d->mSpacing, 0, 0, 0);
		break;
	}
	case Qt::ToolButtonTextBesideIcon: {
		if (opt.icon.isNull()) {
			// 无 icon:同 TextOnly 的左色右文
			iconRect = QRect();
			QSize colorSize = is.isValid() ? is : QSize(16, 16);
			colorRect = QRect(buttonRect.x(),
							  buttonRect.y() + (buttonRect.height() - colorSize.height()) / 2,
							  colorSize.width(), colorSize.height());
			textRect = buttonRect.adjusted(colorRect.right() + d->mSpacing, 0, 0, 0);
		} else {
			// icon+color 在左(垂直叠放),text 在右
			int colorHeight = is.height() / 4;
			int totalHeight = is.height() + colorHeight + d->mSpacing;
			int yOffset = (buttonRect.height() - totalHeight) / 2;
			if (yOffset < 0)
				yOffset = 0;
			iconRect = QRect(buttonRect.x(), buttonRect.y() + yOffset, is.width(), is.height());
			colorRect = QRect(iconRect.x(), iconRect.bottom() + d->mSpacing, iconRect.width(), colorHeight);
			textRect = buttonRect.adjusted(iconRect.right() + d->mSpacing, 0, 0, 0);
		}
		break;
	}
	case Qt::ToolButtonTextUnderIcon: {
		if (opt.icon.isNull()) {
			// 无 icon:color 在上,text 在下
			int colorHeight = is.isValid() ? is.height() : 16;
			int totalHeight = colorHeight + ts.height() + d->mSpacing;
			int yOffset = (buttonRect.height() - totalHeight) / 2;
			if (yOffset < 0)
				yOffset = 0;
			int colorWidth = is.isValid() ? is.width() : 16;
			colorRect = QRect(buttonRect.x() + (buttonRect.width() - colorWidth) / 2,
							  buttonRect.y() + yOffset, colorWidth, colorHeight);
			textRect = QRect(buttonRect.x(), colorRect.bottom() + d->mSpacing,
							 buttonRect.width(), ts.height());
			iconRect = QRect();
		} else {
			// 从上到下:icon → color → text
			int colorHeight = is.height() / 4;
			int totalHeight = ts.height() + is.height() + colorHeight + 2 * d->mSpacing;
			int yOffset = (buttonRect.height() - totalHeight) / 2;
			if (yOffset < 0)
				yOffset = 0;
			iconRect = QRect(buttonRect.x() + (buttonRect.width() - is.width()) / 2,
							 buttonRect.y() + yOffset, is.width(), is.height());
			colorRect = QRect(iconRect.x(), iconRect.bottom() + d->mSpacing,
							  iconRect.width(), colorHeight);
			textRect = QRect(buttonRect.x(), colorRect.bottom() + d->mSpacing,
							 buttonRect.width(), ts.height());
		}
		break;
	}
	default:
		colorRect = buttonRect;
		iconRect = QRect();
		textRect = QRect();
		break;
	}
}

/**
 * @brief 绘制按钮面板（背景、边框、下拉箭头、焦点框）
 * @param p 画笔
 * @param opt 样式选项
 */
void DAColorPickerButton::paintButton(QStylePainter* p, const QStyleOptionToolButton& opt)
{
	if (opt.state & QStyle::State_AutoRaise) {
		p->drawPrimitive(QStyle::PE_PanelButtonTool, opt);
	} else {
		p->drawPrimitive(QStyle::PE_PanelButtonBevel, opt);
	}
	// 下拉箭头
	if (opt.features & QStyleOptionToolButton::MenuButtonPopup) {
		QStyleOptionToolButton arrowOpt = opt;
		QRect mr = style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButtonMenu, this);
		arrowOpt.rect = mr;
		p->drawPrimitive(QStyle::PE_IndicatorArrowDown, arrowOpt);
	}
	// 焦点框
	if (opt.state & QStyle::State_HasFocus) {
		QStyleOptionFocusRect fr;
		fr.QStyleOption::operator=(opt);
		fr.rect.adjust(3, 3, -3, -3);
		p->drawPrimitive(QStyle::PE_FrameFocusRect, fr);
	}
}

/**
 * @brief 绘制图标
 * @param p 画笔
 * @param iconRect 图标区域
 * @param opt 样式选项
 */
void DAColorPickerButton::paintIcon(QStylePainter* p, const QRect& iconRect, const QStyleOptionToolButton& opt)
{
	if (iconRect.isNull() || iconRect.isEmpty())
		return;
	if (!opt.icon.isNull()) {
		opt.icon.paint(p, iconRect);
	}
}

/**
 * @brief 绘制文字
 * @param p 画笔
 * @param textRect 文字区域
 * @param opt 样式选项
 */
void DAColorPickerButton::paintText(QStylePainter* p, const QRect& textRect, const QStyleOptionToolButton& opt)
{
	if (textRect.isNull() || textRect.isEmpty())
		return;
	if (!opt.text.isEmpty()) {
		p->drawText(textRect, Qt::AlignCenter, opt.text);
	}
}

/**
 * @brief 绘制颜色块
 * @param p 画笔
 * @param colorRect 颜色块区域
 * @param color 颜色，无效颜色时绘制"无颜色"标记
 * @param opt 样式选项
 */
void DAColorPickerButton::paintColor(QStylePainter* p, const QRect& colorRect, const QColor& color, const QStyleOptionToolButton& opt)
{
	if (colorRect.isNull() || colorRect.isEmpty())
		return;
	if (color.isValid()) {
		p->fillRect(colorRect, color);
	} else {
		paintNoneColor(p, colorRect);
	}
}
}  // namespace DA

/*** End of inlined file: DAColorPickerButton.cpp ***/


/*** Start of inlined file: DAPenEditWidget.cpp ***/
#include <QVariant>
#include <QApplication>
#include <QSignalBlocker>
#include <QHBoxLayout>
#include <QDoubleSpinBox>

namespace DA
{
class DAPenEditWidget::PrivateData
{
	DA_DECLARE_PUBLIC(DAPenEditWidget)
public:
	PrivateData(DAPenEditWidget* p);

public:
	QPen mPen;
	QHBoxLayout* horizontalLayout;
	DAColorPickerButton* colorButton;
	DAPenStyleComboBox* comboBox;
	QDoubleSpinBox* spinBoxWidth;
};

//===================================================
// DAPenEditWidgetPrivate
//===================================================

DAPenEditWidget::PrivateData::PrivateData(DAPenEditWidget* p) : q_ptr(p)
{
	if (p->objectName().isEmpty())
		p->setObjectName(QStringLiteral("DAPenEditWidget"));
	p->resize(166, 25);
	horizontalLayout = new QHBoxLayout(p);
	horizontalLayout->setSpacing(1);
	horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
	horizontalLayout->setContentsMargins(1, 1, 1, 1);
	colorButton = new DAColorPickerButton(p);
	colorButton->setAutoRaise(true);
	colorButton->setColor(QColor());
	colorButton->setObjectName(QStringLiteral("pushButtonColor"));
	horizontalLayout->addWidget(colorButton);

	comboBox = new DAPenStyleComboBox(p);
	comboBox->setObjectName(QStringLiteral("comboBox"));
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(comboBox->sizePolicy().hasHeightForWidth());
	comboBox->setSizePolicy(sizePolicy);

	horizontalLayout->addWidget(comboBox);

	spinBoxWidth = new QDoubleSpinBox(p);
	spinBoxWidth->setObjectName(QStringLiteral("spinBoxWidth"));
	spinBoxWidth->setMinimum(0.0);
	spinBoxWidth->setMaximum(99.0);
	spinBoxWidth->setDecimals(1);
	spinBoxWidth->setSingleStep(0.5);
	horizontalLayout->addWidget(spinBoxWidth);

	p->setMinimumHeight(22);
}

//===================================================
// DAPenEditWidget
//===================================================

/**
 * @brief 构造画笔编辑窗口
 * @param parent 父控件
 * @details 以默认画笔(SolidLine,无色,1px)初始化并连接内部信号
 */
DAPenEditWidget::DAPenEditWidget(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
	QPen p;
	p.setStyle(Qt::SolidLine);
	p.setColor(QColor());
	p.setWidth(1.0);
	setCurrentPen(p);
	initConnect();
}

/**
 * @brief 构造并设置初始画笔
 * @param p 初始画笔
 * @param parent 父控件
 */
DAPenEditWidget::DAPenEditWidget(const QPen& p, QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
	setCurrentPen(p);
	initConnect();
}

/**
 * @brief 析构函数
 */
DAPenEditWidget::~DAPenEditWidget()
{
}

/**
 * @brief 设置画笔,设置画笔会触发penChanged信号
 * @param p
 */
void DAPenEditWidget::setCurrentPen(const QPen& p)
{
	if (p == d_ptr->mPen) {
		return;
	}
	QSignalBlocker bl(d_ptr->colorButton);
	QSignalBlocker bl2(d_ptr->comboBox);
	QSignalBlocker bl3(d_ptr->spinBoxWidth);
	Q_UNUSED(bl);
	Q_UNUSED(bl2);
	Q_UNUSED(bl3);
	d_ptr->mPen = p;
	d_ptr->colorButton->setColor(d_ptr->mPen.color());
	if (p.style() == Qt::NoPen) {
		d_ptr->colorButton->setColor(QColor());
	}
	d_ptr->comboBox->setCurrentPenStyle(p.style());
	d_ptr->comboBox->setPenColor(d_ptr->mPen.color());
	d_ptr->comboBox->setPenLineWidth(d_ptr->mPen.width());
	d_ptr->comboBox->updateItems();
	d_ptr->spinBoxWidth->setValue(d_ptr->mPen.width());
	d_ptr->colorButton->setEnabled(d_ptr->mPen.style() != Qt::NoPen);
	emit penChanged(d_ptr->mPen);
}

/**
 * @brief 重新翻译UI文本
 * @details 支持多语言，更新窗口标题和线宽后缀
 */
void DAPenEditWidget::retranslateUi()
{
	setWindowTitle(QCoreApplication::translate("DAPenEditWidget", "Pen Editor", nullptr));
	d_ptr->spinBoxWidth->setSuffix(QCoreApplication::translate("DAPenEditWidget", " px", nullptr));
}

/**
 * @brief 获取当前画笔
 * @return 由颜色按钮、样式下拉框、线宽spinbox组合而成的画笔
 */
QPen DAPenEditWidget::getCurrentPen() const
{
	QPen p(d_ptr->colorButton->color());
	p.setStyle(d_ptr->comboBox->getCurrentPenStyle());
	p.setWidthF(d_ptr->spinBoxWidth->value());
	return p;
}

/**
 * @brief 设置是否在线型样式上显示文字
 * @param on 是否显示
 */
void DAPenEditWidget::setLineStyleTextVisible(bool on)
{
	d_ptr->comboBox->setStyleTextVisible(on);
}

/**
 * @brief 返回是否在线型样式上显示文字
 * @return 是否显示
 */
bool DAPenEditWidget::isLineStyleTextVisible() const
{
	return d_ptr->comboBox->isStyleTextVisible();
}

void DAPenEditWidget::onColorChanged(const QColor& c)
{
	d_ptr->mPen.setColor(c);
	QSignalBlocker b(d_ptr->comboBox);
	if (!c.isValid()) {
		// 设置了QColor(),也就是设置NoPen
		d_ptr->comboBox->setCurrentPenStyle(Qt::NoPen);
	} else {
		if (d_ptr->mPen.style() == Qt::NoPen) {
			// 原来是no pen设置颜色，要不style设置为solid
			d_ptr->comboBox->setCurrentPenStyle(Qt::SolidLine);
			d_ptr->mPen.setStyle(Qt::SolidLine);
		}
		d_ptr->comboBox->setPenColor(c);
		d_ptr->comboBox->updateItems();
	}
	emit penChanged(d_ptr->mPen);
}

void DAPenEditWidget::onPenWidthValueChanged(double w)
{
	d_ptr->mPen.setWidthF(w);
	d_ptr->comboBox->setPenLineWidth(w);
	d_ptr->comboBox->updateItems();
	emit penChanged(d_ptr->mPen);
}

void DAPenEditWidget::onPenStyleChanged(Qt::PenStyle s)
{
	QSignalBlocker b(d_ptr->colorButton);
	if (d_ptr->mPen.style() == Qt::NoPen) {
		// 原来是no pen,设置为非no pen，则要把颜色设置回来
		if (Qt::NoPen != s) {
			if (d_ptr->mPen.color().isValid()) {
				d_ptr->colorButton->setColor(d_ptr->mPen.color());
			} else {
				d_ptr->colorButton->setColor(Qt::black);
				d_ptr->mPen.setColor(Qt::black);
			}
		}
	} else {
		// 原来不是no pen，现在变为no pen
		if (Qt::NoPen == s) {
			d_ptr->colorButton->setColor(QColor());
		}
	}
	d_ptr->mPen.setStyle(s);
	emit penChanged(d_ptr->mPen);
}

void DAPenEditWidget::initConnect()
{
	connect(d_ptr->colorButton, &DAColorPickerButton::colorChanged, this, &DAPenEditWidget::onColorChanged);
	connect(d_ptr->spinBoxWidth,
			QOverload< double >::of(&QDoubleSpinBox::valueChanged),
			this,
			&DAPenEditWidget::onPenWidthValueChanged);
	connect(d_ptr->comboBox, &DAPenStyleComboBox::currentPenStyleChanged, this, &DAPenEditWidget::onPenStyleChanged);
}
}  // namespace DA

/*** End of inlined file: DAPenEditWidget.cpp ***/


/*** Start of inlined file: DABrushEditWidget.cpp ***/

/*** Start of inlined file: ui_DABrushEditWidget.h ***/
#ifndef UI_DABRUSHEDITWIDGET_H
#define UI_DABRUSHEDITWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DABrushEditWidget
{
public:
	QHBoxLayout *horizontalLayout;
	DA::DAColorPickerButton *colorButton;
	DA::DABrushStyleComboBox *comboBox;

	void setupUi(QWidget *DABrushEditWidget)
	{
		if (DABrushEditWidget->objectName().isEmpty())
			DABrushEditWidget->setObjectName("DABrushEditWidget");
		DABrushEditWidget->resize(113, 24);
		DABrushEditWidget->setMinimumSize(QSize(0, 22));
		horizontalLayout = new QHBoxLayout(DABrushEditWidget);
		horizontalLayout->setSpacing(1);
		horizontalLayout->setObjectName("horizontalLayout");
		horizontalLayout->setContentsMargins(1, 1, 1, 1);
		colorButton = new DA::DAColorPickerButton(DABrushEditWidget);
		colorButton->setObjectName("colorButton");
		QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(colorButton->sizePolicy().hasHeightForWidth());
		colorButton->setSizePolicy(sizePolicy);
		colorButton->setAutoRaise(true);

		horizontalLayout->addWidget(colorButton);

		comboBox = new DA::DABrushStyleComboBox(DABrushEditWidget);
		comboBox->setObjectName("comboBox");
		QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
		sizePolicy1.setHorizontalStretch(0);
		sizePolicy1.setVerticalStretch(0);
		sizePolicy1.setHeightForWidth(comboBox->sizePolicy().hasHeightForWidth());
		comboBox->setSizePolicy(sizePolicy1);

		horizontalLayout->addWidget(comboBox);

		retranslateUi(DABrushEditWidget);

		QMetaObject::connectSlotsByName(DABrushEditWidget);
	} // setupUi

	void retranslateUi(QWidget *DABrushEditWidget)
	{
		DABrushEditWidget->setWindowTitle(QCoreApplication::translate("DABrushEditWidget", "Brush Edit", nullptr));
		colorButton->setText(QString());
	} // retranslateUi

};

namespace Ui {
	class DABrushEditWidget: public Ui_DABrushEditWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DABRUSHEDITWIDGET_H

/*** End of inlined file: ui_DABrushEditWidget.h ***/

#include <QColorDialog>
#include <QFileDialog>
#include <QGradient>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QPainter>
#include <QPixmap>
#include <QSignalBlocker>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QSpinBox>
#include <QTimer>
#include <QtMath>

namespace DA
{
//===================================================
// 渐变编辑对话框(内部使用)
//===================================================
class DABrushEditWidget::PrivateData
{
public:
	Ui::DABrushEditWidget ui;
	QBrush mBrush;
};

/**
 * @brief 构造画刷编辑窗口
 * @param parent 父控件
 * @details 初始化UI，颜色按钮初始为无效色，连接颜色和样式变更信号
 */
DABrushEditWidget::DABrushEditWidget(QWidget* parent) : QWidget(parent), d(new PrivateData)
{
	d->ui.setupUi(this);
	d->ui.colorButton->setColor(QColor());
	connect(d->ui.colorButton, &DAColorPickerButton::colorChanged, this, &DABrushEditWidget::onColorChanged);
	connect(d->ui.comboBox, &DABrushStyleComboBox::currentBrushStyleChanged, this, &DABrushEditWidget::onBrushStyleChanged);
}

/**
 * @brief 析构函数
 */
DABrushEditWidget::~DABrushEditWidget()
{
	delete d;
}

/**
 * @brief 获取当前画刷
 * @return 当前画刷
 */
QBrush DABrushEditWidget::getCurrentBrush() const
{
	return d->mBrush;
}

/**
 * @brief 设置是否在样式上显示文字
 * @param on 是否显示
 */
void DABrushEditWidget::setStyleTextVisible(bool on)
{
	d->ui.comboBox->setStyleTextVisible(on);
}

/**
 * @brief 返回是否在样式上显示文字
 * @return 是否显示
 */
bool DABrushEditWidget::isStyleTextVisible() const
{
	return d->ui.comboBox->isStyleTextVisible();
}

/**
 * @brief 获取当前画刷样式
 * @return 当前画刷样式
 */
Qt::BrushStyle DABrushEditWidget::getCurrentBrushStyle() const
{
	return d->ui.comboBox->getCurrentBrushStyle();
}

/**
 * @brief 获取当前画刷颜色
 * @return 当前画刷颜色（纯色/图案模式下有效）
 */
QColor DABrushEditWidget::getCurrentBrushColor() const
{
	return d->ui.colorButton->color();
}

/**
 * @brief 设置当前画刷
 * @param b 新画刷
 * @details 支持纯色/图案/渐变/纹理四种模式。渐变模式下颜色按钮显示起始色，纹理模式下显示无效色
 */
void DABrushEditWidget::setCurrentBrush(const QBrush& b)
{
	QSignalBlocker b1(d->ui.colorButton), b2(d->ui.comboBox);
	d->mBrush = b;
	if (b.style() >= Qt::LinearGradientPattern && b.style() <= Qt::ConicalGradientPattern) {
		// 渐变:颜色按钮显示起始色
		if (b.gradient() && !b.gradient()->stops().isEmpty()) {
			d->ui.colorButton->setColor(b.gradient()->stops().first().second);
		} else {
			d->ui.colorButton->setColor(b.color());
		}
	} else if (b.style() == Qt::TexturePattern) {
		// 纹理:颜色按钮显示无效色
		d->ui.colorButton->setColor(QColor());
	} else {
		d->ui.colorButton->setColor(b.color());
	}
	d->ui.comboBox->setCurrentBrushStyle(b.style());
	Q_EMIT brushChanged(b);
}

/**
 * @brief 设置当前画刷样式
 * @param s 画刷样式
 */
void DABrushEditWidget::setCurrentBrushStyle(Qt::BrushStyle s)
{
	d->ui.comboBox->setCurrentBrushStyle(s);
}

void DABrushEditWidget::onColorChanged(const QColor& c)
{
	Qt::BrushStyle s = getCurrentBrushStyle();
	if (c.isValid()) {
		if (s == Qt::NoBrush) {
			QSignalBlocker b1(d->ui.comboBox);
			d->ui.comboBox->setCurrentBrushStyle(Qt::SolidPattern);
			d->mBrush.setStyle(Qt::SolidPattern);
		}
	} else {
		if (s != Qt::NoBrush && s < Qt::LinearGradientPattern) {
			QSignalBlocker b1(d->ui.comboBox);
			d->ui.comboBox->setCurrentBrushStyle(Qt::NoBrush);
			d->mBrush.setStyle(Qt::NoBrush);
		}
	}
	d->mBrush.setColor(c);
	// 同步更新 combobox 的颜色(修复原版 onBrushStyleChanged 未回写颜色的 bug)
	d->ui.comboBox->setBrushColor(c);
	Q_EMIT brushChanged(d->mBrush);
}

void DABrushEditWidget::onBrushStyleChanged(Qt::BrushStyle s)
{
	// 渐变/纹理样式:弹出编辑器
	if (s >= Qt::LinearGradientPattern && s <= Qt::ConicalGradientPattern) {
		editGradient(s);
		return;
	}
	if (s == Qt::TexturePattern) {
		editTexture();
		return;
	}

	// 普通样式
	if (d->mBrush.style() == s) {
		return;
	}
	d->mBrush.setStyle(s);
	// 修复联动 bug:样式改变时同步颜色到 combobox
	if (d->mBrush.color().isValid()) {
		d->ui.comboBox->setBrushColor(d->mBrush.color());
	}
	Q_EMIT brushChanged(d->mBrush);
}

void DABrushEditWidget::editGradient(Qt::BrushStyle style)
{
	// 弹出渐变编辑对话框
	QDialog dlg(this);
	dlg.setWindowTitle(QStringLiteral("Gradient Editor"));

	auto* layout = new QVBoxLayout(&dlg);

	// 渐变类型选择
	auto* typeGroup = new QButtonGroup(&dlg);
	auto* typeLayout = new QHBoxLayout();
	auto* rbLinear = new QRadioButton(QStringLiteral("Linear"), &dlg);
	auto* rbRadial = new QRadioButton(QStringLiteral("Radial"), &dlg);
	auto* rbConical = new QRadioButton(QStringLiteral("Conical"), &dlg);
	typeGroup->addButton(rbLinear, Qt::LinearGradientPattern);
	typeGroup->addButton(rbRadial, Qt::RadialGradientPattern);
	typeGroup->addButton(rbConical, Qt::ConicalGradientPattern);
	// 选中当前类型
	if (style == Qt::RadialGradientPattern) rbRadial->setChecked(true);
	else if (style == Qt::ConicalGradientPattern) rbConical->setChecked(true);
	else rbLinear->setChecked(true);
	typeLayout->addWidget(rbLinear);
	typeLayout->addWidget(rbRadial);
	typeLayout->addWidget(rbConical);
	typeLayout->addStretch();
	layout->addLayout(typeLayout);

	// 起始色 / 结束色
	auto* colorLayout = new QHBoxLayout();
	colorLayout->addWidget(new QLabel(QStringLiteral("Start:"), &dlg));
	auto* btnStart = new DAColorPickerButton(&dlg);
	colorLayout->addWidget(btnStart);
	colorLayout->addWidget(new QLabel(QStringLiteral("End:"), &dlg));
	auto* btnEnd = new DAColorPickerButton(&dlg);
	colorLayout->addWidget(btnEnd);
	colorLayout->addStretch();
	layout->addLayout(colorLayout);

	// 从现有 brush 提取颜色
	QColor startColor = Qt::white, endColor = Qt::black;
	if (d->mBrush.gradient() && !d->mBrush.gradient()->stops().isEmpty()) {
		const auto& stops = d->mBrush.gradient()->stops();
		startColor = stops.first().second;
		endColor = stops.last().second;
	} else if (d->mBrush.color().isValid()) {
		startColor = d->mBrush.color();
		endColor = d->mBrush.color().darker(200);
	}
	btnStart->setColor(startColor);
	btnEnd->setColor(endColor);

	// 预览
	auto* previewLabel = new QLabel(&dlg);
	previewLabel->setMinimumHeight(40);
	previewLabel->setFrameShape(QFrame::Box);
	layout->addWidget(previewLabel);

	// 角度(仅 Linear/Conical 用)
	auto* angleLayout = new QHBoxLayout();
	angleLayout->addWidget(new QLabel(QStringLiteral("Angle:"), &dlg));
	auto* spinAngle = new QSpinBox(&dlg);
	spinAngle->setRange(0, 360);
	spinAngle->setSuffix(QStringLiteral("°"));
	angleLayout->addWidget(spinAngle);
	angleLayout->addStretch();
	layout->addLayout(angleLayout);

	// 按钮
	auto* btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
	layout->addWidget(btnBox);
	connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
	connect(btnBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

	// 更新预览
	auto updatePreview = [&]() {
		int w = previewLabel->width();
		int h = previewLabel->height();
		if (w <= 0) w = 200;
		if (h <= 0) h = 40;
		QPixmap pm(w, h);
		QPainter painter(&pm);
		int type = typeGroup->checkedId();
		int angle = spinAngle->value();
		if (type == Qt::LinearGradientPattern) {
			QLinearGradient g(0, 0, w, 0);
			// 按角度旋转
			qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
			g.setStart(0, 0);
			g.setFinalStop(w * qCos(rad), h * qSin(rad));
			g.setColorAt(0.0, btnStart->color());
			g.setColorAt(1.0, btnEnd->color());
			painter.fillRect(pm.rect(), QBrush(g));
		} else if (type == Qt::RadialGradientPattern) {
			QRadialGradient g(w / 2.0, h / 2.0, qMax(w, h) / 2.0);
			g.setColorAt(0.0, btnStart->color());
			g.setColorAt(1.0, btnEnd->color());
			painter.fillRect(pm.rect(), QBrush(g));
		} else {
			QConicalGradient g(w / 2.0, h / 2.0, angle);
			g.setColorAt(0.0, btnStart->color());
			g.setColorAt(1.0, btnEnd->color());
			painter.fillRect(pm.rect(), QBrush(g));
		}
		previewLabel->setPixmap(pm);
	};

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
	connect(typeGroup, &QButtonGroup::idClicked, [&](int) { updatePreview(); });
#else
	connect(typeGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [&](int) { updatePreview(); });
#endif
	connect(btnStart, &DAColorPickerButton::colorChanged, [&](const QColor&) { updatePreview(); });
	connect(btnEnd, &DAColorPickerButton::colorChanged, [&](const QColor&) { updatePreview(); });
	connect(spinAngle, QOverload<int>::of(&QSpinBox::valueChanged), [&](int) { updatePreview(); });

	// 延迟更新预览(等 layout 完成后)
	QTimer::singleShot(0, [&]() { updatePreview(); });

	if (dlg.exec() == QDialog::Accepted) {
		int type = typeGroup->checkedId();
		int angle = spinAngle->value();
		QGradient* gradient = nullptr;
		if (type == Qt::LinearGradientPattern) {
			qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
			auto* g = new QLinearGradient(0, 0, 100 * qCos(rad), 100 * qSin(rad));
			g->setColorAt(0.0, btnStart->color());
			g->setColorAt(1.0, btnEnd->color());
			gradient = g;
		} else if (type == Qt::RadialGradientPattern) {
			auto* g = new QRadialGradient(0.5, 0.5, 0.5);
			g->setCoordinateMode(QGradient::ObjectBoundingMode);
			g->setColorAt(0.0, btnStart->color());
			g->setColorAt(1.0, btnEnd->color());
			gradient = g;
		} else {
			auto* g = new QConicalGradient(0.5, 0.5, angle);
			g->setCoordinateMode(QGradient::ObjectBoundingMode);
			g->setColorAt(0.0, btnStart->color());
			g->setColorAt(1.0, btnEnd->color());
			gradient = g;
		}

		QBrush newBrush(*gradient);
		delete gradient;
		d->mBrush = newBrush;
		// 颜色按钮显示起始色
		QSignalBlocker b(d->ui.colorButton);
		d->ui.colorButton->setColor(btnStart->color());
		Q_EMIT brushChanged(d->mBrush);
	} else {
		// 取消:恢复原来的样式
		QSignalBlocker b(d->ui.comboBox);
		d->ui.comboBox->setCurrentBrushStyle(d->mBrush.style());
	}
}

void DABrushEditWidget::editTexture()
{
	QString path = QFileDialog::getOpenFileName(
		this, QStringLiteral("Select Texture Image"), QString(),
		QStringLiteral("Images (*.png *.jpg *.jpeg *.bmp *.gif *.svg);;All Files (*)"));
	if (path.isEmpty()) {
		// 取消:恢复原来的样式
		QSignalBlocker b(d->ui.comboBox);
		d->ui.comboBox->setCurrentBrushStyle(d->mBrush.style());
		return;
	}
	QPixmap pm(path);
	if (pm.isNull()) {
		QSignalBlocker b(d->ui.comboBox);
		d->ui.comboBox->setCurrentBrushStyle(d->mBrush.style());
		return;
	}
	d->mBrush.setTexture(pm);
	QSignalBlocker b(d->ui.colorButton);
	d->ui.colorButton->setColor(QColor());  // 纹理模式下颜色无效
	Q_EMIT brushChanged(d->mBrush);
}
}  // namespace DA

/*** End of inlined file: DABrushEditWidget.cpp ***/


/*** Start of inlined file: DAFontEditPannelWidget.cpp ***/

/*** Start of inlined file: ui_DAFontEditPannelWidget.h ***/
#ifndef UI_DAFONTEDITPANNELWIDGET_H
#define UI_DAFONTEDITPANNELWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DAFontEditPannelWidget
{
public:
	QVBoxLayout *verticalLayout;
	QFontComboBox *fontComboBox;
	QHBoxLayout *horizontalLayout;
	QToolButton *toolButtonBold;
	QToolButton *toolButtonItalic;
	QToolButton *toolButtonUnderline;
	DA::DAColorPickerButton *colorButton;
	QComboBox *comboBoxFontSize;
	QToolButton *toolButtonFontSizeAdd;
	QToolButton *toolButtonFontSizeSub;

	void setupUi(QWidget *DAFontEditPannelWidget)
	{
		if (DAFontEditPannelWidget->objectName().isEmpty())
			DAFontEditPannelWidget->setObjectName("DAFontEditPannelWidget");
		DAFontEditPannelWidget->resize(229, 54);
		QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(DAFontEditPannelWidget->sizePolicy().hasHeightForWidth());
		DAFontEditPannelWidget->setSizePolicy(sizePolicy);
		DAFontEditPannelWidget->setMinimumSize(QSize(0, 54));
		verticalLayout = new QVBoxLayout(DAFontEditPannelWidget);
		verticalLayout->setSpacing(5);
		verticalLayout->setObjectName("verticalLayout");
		verticalLayout->setContentsMargins(2, 2, 2, 2);
		fontComboBox = new QFontComboBox(DAFontEditPannelWidget);
		fontComboBox->setObjectName("fontComboBox");
		QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
		sizePolicy1.setHorizontalStretch(0);
		sizePolicy1.setVerticalStretch(0);
		sizePolicy1.setHeightForWidth(fontComboBox->sizePolicy().hasHeightForWidth());
		fontComboBox->setSizePolicy(sizePolicy1);

		verticalLayout->addWidget(fontComboBox);

		horizontalLayout = new QHBoxLayout();
		horizontalLayout->setSpacing(0);
		horizontalLayout->setObjectName("horizontalLayout");
		toolButtonBold = new QToolButton(DAFontEditPannelWidget);
		toolButtonBold->setObjectName("toolButtonBold");
		QSizePolicy sizePolicy2(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
		sizePolicy2.setHorizontalStretch(0);
		sizePolicy2.setVerticalStretch(0);
		sizePolicy2.setHeightForWidth(toolButtonBold->sizePolicy().hasHeightForWidth());
		toolButtonBold->setSizePolicy(sizePolicy2);
		toolButtonBold->setMinimumSize(QSize(16, 16));
		QIcon icon;
		icon.addFile(QString::fromUtf8(":/DAWidgetsIcons/text-bold.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonBold->setIcon(icon);
		toolButtonBold->setCheckable(true);
		toolButtonBold->setAutoRaise(true);

		horizontalLayout->addWidget(toolButtonBold);

		toolButtonItalic = new QToolButton(DAFontEditPannelWidget);
		toolButtonItalic->setObjectName("toolButtonItalic");
		sizePolicy2.setHeightForWidth(toolButtonItalic->sizePolicy().hasHeightForWidth());
		toolButtonItalic->setSizePolicy(sizePolicy2);
		toolButtonItalic->setMinimumSize(QSize(16, 16));
		QIcon icon1;
		icon1.addFile(QString::fromUtf8(":/DAWidgetsIcons/text-italic.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonItalic->setIcon(icon1);
		toolButtonItalic->setCheckable(true);
		toolButtonItalic->setAutoRaise(true);

		horizontalLayout->addWidget(toolButtonItalic);

		toolButtonUnderline = new QToolButton(DAFontEditPannelWidget);
		toolButtonUnderline->setObjectName("toolButtonUnderline");
		sizePolicy2.setHeightForWidth(toolButtonUnderline->sizePolicy().hasHeightForWidth());
		toolButtonUnderline->setSizePolicy(sizePolicy2);
		toolButtonUnderline->setMinimumSize(QSize(16, 16));
		QIcon icon2;
		icon2.addFile(QString::fromUtf8(":/DAWidgetsIcons/text-underline.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonUnderline->setIcon(icon2);
		toolButtonUnderline->setCheckable(true);
		toolButtonUnderline->setAutoRaise(true);

		horizontalLayout->addWidget(toolButtonUnderline);

		colorButton = new DA::DAColorPickerButton(DAFontEditPannelWidget);
		colorButton->setObjectName("colorButton");
		sizePolicy2.setHeightForWidth(colorButton->sizePolicy().hasHeightForWidth());
		colorButton->setSizePolicy(sizePolicy2);
		colorButton->setMinimumSize(QSize(16, 16));
		colorButton->setAutoRaise(true);

		horizontalLayout->addWidget(colorButton);

		comboBoxFontSize = new QComboBox(DAFontEditPannelWidget);
		comboBoxFontSize->setObjectName("comboBoxFontSize");
		QSizePolicy sizePolicy3(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
		sizePolicy3.setHorizontalStretch(0);
		sizePolicy3.setVerticalStretch(0);
		sizePolicy3.setHeightForWidth(comboBoxFontSize->sizePolicy().hasHeightForWidth());
		comboBoxFontSize->setSizePolicy(sizePolicy3);
		comboBoxFontSize->setEditable(true);

		horizontalLayout->addWidget(comboBoxFontSize);

		toolButtonFontSizeAdd = new QToolButton(DAFontEditPannelWidget);
		toolButtonFontSizeAdd->setObjectName("toolButtonFontSizeAdd");
		toolButtonFontSizeAdd->setMinimumSize(QSize(16, 16));
		QIcon icon3;
		icon3.addFile(QString::fromUtf8(":/DAWidgetsIcons/font-size-add.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonFontSizeAdd->setIcon(icon3);
		toolButtonFontSizeAdd->setAutoRaise(true);

		horizontalLayout->addWidget(toolButtonFontSizeAdd);

		toolButtonFontSizeSub = new QToolButton(DAFontEditPannelWidget);
		toolButtonFontSizeSub->setObjectName("toolButtonFontSizeSub");
		toolButtonFontSizeSub->setMinimumSize(QSize(16, 16));
		QIcon icon4;
		icon4.addFile(QString::fromUtf8(":/DAWidgetsIcons/font-size-sub.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		toolButtonFontSizeSub->setIcon(icon4);
		toolButtonFontSizeSub->setAutoRaise(true);

		horizontalLayout->addWidget(toolButtonFontSizeSub);

		verticalLayout->addLayout(horizontalLayout);

		retranslateUi(DAFontEditPannelWidget);

		QMetaObject::connectSlotsByName(DAFontEditPannelWidget);
	} // setupUi

	void retranslateUi(QWidget *DAFontEditPannelWidget)
	{
		DAFontEditPannelWidget->setWindowTitle(QCoreApplication::translate("DAFontEditPannelWidget", "Font Edit", nullptr));
		toolButtonBold->setText(QString());
		toolButtonItalic->setText(QString());
		toolButtonUnderline->setText(QString());
		colorButton->setText(QString());
		toolButtonFontSizeAdd->setText(QString());
		toolButtonFontSizeSub->setText(QString());
	} // retranslateUi

};

namespace Ui {
	class DAFontEditPannelWidget: public Ui_DAFontEditPannelWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DAFONTEDITPANNELWIDGET_H

/*** End of inlined file: ui_DAFontEditPannelWidget.h ***/

#include <QAbstractItemView>
#include <QAction>
#include <QFontComboBox>
#include <QComboBox>
#include <QFontInfo>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QSignalBlocker>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QToolButton>
#include <QVBoxLayout>

namespace DA
{
namespace
{
/**
 * @brief 字体下拉的预览委托：每项用「自身 family + 面板当前样式」渲染
 * @details 只改 option->font，选中高亮/省略/禁用等仍由基类处理
 */
class FontPreviewDelegate : public QStyledItemDelegate
{
public:
	explicit FontPreviewDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent)
	{
	}

	void setPreviewFont(const QFont& f)
	{
		mPreviewFont = f;
	}

	void setMaxRowHeight(int h)
	{
		mMaxRowHeight = h;
	}

protected:
	void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override
	{
		QStyledItemDelegate::initStyleOption(option, index);
		const QString family = index.data(Qt::DisplayRole).toString();
		if (family.isEmpty())
			return;
		QFont f = mPreviewFont;
		f.setFamily(family);
		// 未安装的字体族退回面板字体，避免整行豆腐块
		if (QFontInfo(f).family() != family) {
			f = option->font;
		}
		option->font = f;
	}

	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override
	{
		QSize s = QStyledItemDelegate::sizeHint(option, index);
		if (s.height() > mMaxRowHeight)
			s.setHeight(mMaxRowHeight);
		return s;
	}

private:
	QFont mPreviewFont;
	int mMaxRowHeight { 40 };
};

// 时钟图标（不新增 qrc 资源）
QIcon makeClockIcon()
{
	QPixmap pm(16, 16);
	pm.fill(Qt::transparent);
	QPainter p(&pm);
	p.setRenderHint(QPainter::Antialiasing);
	p.setPen(QPen(QColor(0x60, 0x66, 0x6E), 1.4));
	p.drawEllipse(QRectF(1.5, 1.5, 13, 13));
	p.drawLine(QPointF(8, 4.5), QPointF(8, 8));
	p.drawLine(QPointF(8, 8), QPointF(11, 9.5));
	p.end();
	return QIcon(pm);
}
}  // namespace

class DAFontEditPannelWidget::PrivateData
{
	DA_DECLARE_PUBLIC(DAFontEditPannelWidget)
public:
	PrivateData(DAFontEditPannelWidget* p);
	void rebuildLayout();
	void syncPreviewFont();
	void initRecentUi();
	void syncRecentMenu();
	void applyRecentFamily(const QString& family);

public:
	Ui::DAFontEditPannelWidget ui;
	DAFontEditPannelWidget::LayoutMode mLayoutMode { DAFontEditPannelWidget::TwoRowsLayout };
	int mFontPointSize { 10 };
	bool mFontPreviewEnabled { false };
	FontPreviewDelegate* mPreviewDelegate { nullptr };
	QAbstractItemDelegate* mQtFontDelegate { nullptr };
	int mMaxRecentFontCount { 5 };
	QStringList mRecentFontFamilies;
	QToolButton* mRecentButton { nullptr };
	QMenu* mRecentMenu { nullptr };
};

DAFontEditPannelWidget::PrivateData::PrivateData(DAFontEditPannelWidget* p) : q_ptr(p)
{
}

void DAFontEditPannelWidget::PrivateData::initRecentUi()
{
	if (mRecentButton)
		return;
	mRecentButton = new QToolButton(q_ptr);
	mRecentButton->setObjectName(QStringLiteral("recentFontButton"));
	mRecentButton->setAutoRaise(true);
	mRecentButton->setMinimumSize(16, 16);
	mRecentButton->setPopupMode(QToolButton::InstantPopup);
	mRecentButton->setToolTip(DAFontEditPannelWidget::tr("Recent Fonts"));  //cn:最近字体
	mRecentButton->setIcon(makeClockIcon());
	mRecentButton->setVisible(false);
	mRecentMenu = new QMenu(mRecentButton);
	mRecentMenu->setObjectName(QStringLiteral("recentFontMenu"));
	mRecentButton->setMenu(mRecentMenu);
}

void DAFontEditPannelWidget::PrivateData::syncRecentMenu()
{
	if (!mRecentButton || !mRecentMenu)
		return;
	mRecentMenu->clear();
	for (const QString& family : mRecentFontFamilies) {
		QAction* action = mRecentMenu->addAction(family);
		action->setFont(QFont(family));
		action->setData(family);
		QObject::connect(action, &QAction::triggered, q_ptr,
						 [ this, family ]() { applyRecentFamily(family); });
	}
	mRecentButton->setVisible(!mRecentFontFamilies.isEmpty());
	// 两排布局只有在列表非空时才把下拉和按钮包进同一行
	rebuildLayout();
}

void DAFontEditPannelWidget::PrivateData::applyRecentFamily(const QString& family)
{
	// 只改字体族：字号仍由 mFontPointSize 决定（不能用本类 setCurrentFont）
	QFont f(family);
	f.setPointSize(mFontPointSize);
	ui.fontComboBox->setCurrentFont(f);
}

void DAFontEditPannelWidget::PrivateData::syncPreviewFont()
{
	if (!mFontPreviewEnabled || !mPreviewDelegate)
		return;
	QFont f = q_ptr->getCurrentFont();
	f.setPointSize(qBound(9, f.pointSize(), 20));
	f.setFamily(QString());  // family 由委托逐项替换
	const int base = QFontMetrics(ui.fontComboBox->font()).height() + 6;
	mPreviewDelegate->setPreviewFont(f);
	mPreviewDelegate->setMaxRowHeight(base * 2);
	if (ui.fontComboBox->view())
		ui.fontComboBox->view()->viewport()->update();
}

/**
 * @brief 根据当前布局模式重建布局
 * @details 控件由父widget持有，删除布局不会删除控件，仅重新排列
 */
void DAFontEditPannelWidget::PrivateData::rebuildLayout()
{
	// 移除旧布局的所有子项后删除（QLayoutItem析构不删除widget，仅解除布局管理）
	if (QLayout* oldLay = q_ptr->layout()) {
		while (oldLay->count() > 0) {
			QLayoutItem* item = oldLay->takeAt(0);
			delete item;
		}
		delete oldLay;
	}
	if (mLayoutMode == DAFontEditPannelWidget::SingleRowLayout) {
		// 单排：字体族(拉伸) + 样式按钮 + 颜色 + 字号 + 增减按钮
		q_ptr->setMinimumSize(0, 0);
		QHBoxLayout* lay = new QHBoxLayout(q_ptr);
		lay->setContentsMargins(2, 2, 2, 2);
		lay->setSpacing(2);
		lay->addWidget(ui.fontComboBox, 1);
		if (mRecentButton)
			lay->addWidget(mRecentButton);
		lay->addWidget(ui.toolButtonBold);
		lay->addWidget(ui.toolButtonItalic);
		lay->addWidget(ui.toolButtonUnderline);
		lay->addWidget(ui.colorButton);
		lay->addWidget(ui.comboBoxFontSize);
		lay->addWidget(ui.toolButtonFontSizeAdd);
		lay->addWidget(ui.toolButtonFontSizeSub);
	} else {
		// 两排：第一排字体族，第二排样式按钮+颜色+字号+增减按钮（与.ui默认一致）
		q_ptr->setMinimumSize(0, 54);
		QVBoxLayout* vlay = new QVBoxLayout(q_ptr);
		vlay->setContentsMargins(2, 2, 2, 2);
		vlay->setSpacing(5);
		if (mRecentButton && !mRecentFontFamilies.isEmpty()) {
			auto* fontRow = new QHBoxLayout();
			fontRow->setSpacing(2);
			fontRow->addWidget(ui.fontComboBox, 1);
			fontRow->addWidget(mRecentButton);
			vlay->addLayout(fontRow);
		} else {
			vlay->addWidget(ui.fontComboBox);
		}
		QHBoxLayout* hlay = new QHBoxLayout();
		hlay->setSpacing(0);
		hlay->addWidget(ui.toolButtonBold);
		hlay->addWidget(ui.toolButtonItalic);
		hlay->addWidget(ui.toolButtonUnderline);
		hlay->addWidget(ui.colorButton);
		hlay->addWidget(ui.comboBoxFontSize);
		hlay->addWidget(ui.toolButtonFontSizeAdd);
		hlay->addWidget(ui.toolButtonFontSizeSub);
		vlay->addLayout(hlay);
	}
}

/**
 * @brief 构造字体编辑面板
 * @param parent 父控件
 * @details 初始化字号下拉列表、颜色按钮，连接字体/粗体/斜体/下划线/字号变更信号
 */
DAFontEditPannelWidget::DAFontEditPannelWidget(QWidget* parent)
	: QWidget(parent), d(new PrivateData(this))
{
	d->ui.setupUi(this);
	d->initRecentUi();
	d->rebuildLayout();
	d->ui.comboBoxFontSize->addItems(
		{ "6", "8", "9", "10", "11", "12", "14", "16", "18", "20", "22", "24", "26", "28", "36", "48", "72" });
	d->ui.comboBoxFontSize->setCurrentText("10");
	d->ui.colorButton->setColor(Qt::black);

	connect(d->ui.fontComboBox, &QFontComboBox::currentFontChanged, this, &DAFontEditPannelWidget::signalEmitFontChanged);
	connect(d->ui.colorButton, &DAColorPickerButton::colorChanged, this, &DAFontEditPannelWidget::currentFontColorChanged);
	connect(d->ui.toolButtonBold, &QToolButton::clicked, this, &DAFontEditPannelWidget::signalEmitFontChanged);
	connect(d->ui.toolButtonItalic, &QToolButton::clicked, this, &DAFontEditPannelWidget::signalEmitFontChanged);
	connect(d->ui.toolButtonUnderline, &QToolButton::clicked, this, &DAFontEditPannelWidget::signalEmitFontChanged);
	connect(d->ui.comboBoxFontSize, &QComboBox::currentTextChanged, this, &DAFontEditPannelWidget::onComboBoxFontSizeTextChanged);
	connect(d->ui.toolButtonFontSizeAdd, &QToolButton::clicked, this, &DAFontEditPannelWidget::onFontSizeAdd);
	connect(d->ui.toolButtonFontSizeSub, &QToolButton::clicked, this, &DAFontEditPannelWidget::onFontSizeSub);
}

/**
 * @brief 析构函数
 */
DAFontEditPannelWidget::~DAFontEditPannelWidget()
{
	delete d;
}

/**
 * @brief 设置布局模式
 *
 * 默认TwoRowsLayout（两排布局）。SingleRowLayout将所有控件排为一行，
 * 适合嵌入工具栏等高度受限的场景。切换时重建布局，控件与状态保留。
 * @param m 布局模式
 * @see layoutMode
 */
void DAFontEditPannelWidget::setLayoutMode(LayoutMode m)
{
	if (d->mLayoutMode == m) {
		return;
	}
	d->mLayoutMode = m;
	d->rebuildLayout();
	updateGeometry();
}

/**
 * @brief 获取布局模式
 * @return 当前布局模式
 * @see setLayoutMode
 */
DAFontEditPannelWidget::LayoutMode DAFontEditPannelWidget::layoutMode() const
{
	return d->mLayoutMode;
}

/**
 * @brief 获取当前字体颜色
 * @return 当前字体颜色
 */
QColor DAFontEditPannelWidget::getCurrentFontColor() const
{
	return d->ui.colorButton->color();
}

/**
 * @brief 设置当前字体颜色
 * @param c 新颜色
 */
void DAFontEditPannelWidget::setCurrentFontColor(const QColor& c)
{
	if (d->ui.colorButton->color() == c) {
		return;
	}
	d->ui.colorButton->setColor(c);
}

/**
 * @brief 设置当前字体
 * @param f 新字体
 * @details 同步更新字体下拉框、字号、粗体/斜体/下划线状态
 */
void DAFontEditPannelWidget::setCurrentFont(const QFont& f)
{
	if (f == getCurrentFont()) {
		return;
	}
	QSignalBlocker b1(d->ui.fontComboBox), b2(d->ui.comboBoxFontSize);
	d->ui.toolButtonBold->setChecked(f.bold());
	d->ui.toolButtonItalic->setChecked(f.italic());
	d->ui.toolButtonUnderline->setChecked(f.underline());
	d->ui.fontComboBox->setCurrentFont(f);
	d->ui.comboBoxFontSize->setCurrentText(QString::number((f.pointSize() < 0) ? f.pixelSize() : f.pointSize()));
	signalEmitFontChanged();
}

/**
 * @brief 获取当前字体
 * @return 由字体下拉框、粗体/斜体/下划线按钮、字号组合而成的字体
 */
QFont DAFontEditPannelWidget::getCurrentFont()
{
	QFont f = d->ui.fontComboBox->currentFont();
	f.setBold(d->ui.toolButtonBold->isChecked());
	f.setItalic(d->ui.toolButtonItalic->isChecked());
	f.setUnderline(d->ui.toolButtonUnderline->isChecked());
	f.setPointSize(d->mFontPointSize);
	return f;
}

/**
 * @brief 设置字体粗细（字号）
 * @param w 字号值
 * @details Qt5中字重为0-99整数值，Qt6中为QFont::Weight枚举，此处用int兼容
 */
void DAFontEditPannelWidget::setFontWeight(int w)
{
	d->ui.comboBoxFontSize->setCurrentText(QString::number(w));
	d->mFontPointSize = w;
	signalEmitFontChanged();
}

/**
 * @brief 获取字体粗细（字号）
 * @return 当前字号
 */
int DAFontEditPannelWidget::getFontWeight() const
{
	return d->ui.comboBoxFontSize->currentText().toInt();
}

/**
 * @brief 打开/关闭字体下拉的每项预览
 * @param on 是否启用（默认 false）
 * @details 关闭时把 Qt 自己的委托原样还回去，像素与原生下拉一致
 */
void DAFontEditPannelWidget::setFontPreviewEnabled(bool on)
{
	if (d->mFontPreviewEnabled == on)
		return;
	QAbstractItemView* view = d->ui.fontComboBox->view();
	if (!view)
		return;
	d->mFontPreviewEnabled = on;
	if (on) {
		if (!d->mQtFontDelegate)
			d->mQtFontDelegate = view->itemDelegate();
		if (!d->mPreviewDelegate)
			d->mPreviewDelegate = new FontPreviewDelegate(view);
		// 直接设到 view：Qt5 的 QComboBox::setItemDelegate 会 delete 旧委托，
		// 那样存下来的原生委托就成了悬空指针，关闭时无法恢复
		view->setItemDelegate(d->mPreviewDelegate);
		d->syncPreviewFont();
	} else {
		if (d->mQtFontDelegate)
			view->setItemDelegate(d->mQtFontDelegate);
		if (d->mPreviewDelegate) {
			d->mPreviewDelegate->deleteLater();
			d->mPreviewDelegate = nullptr;
		}
	}
}

/** @brief 字体下拉每项预览是否启用 */
bool DAFontEditPannelWidget::isFontPreviewEnabled() const
{
	return d->mFontPreviewEnabled;
}

/**
 * @brief 设置最近字体条数上限
 * @param n 1–20，越界会限幅；变小时立即截断当前列表
 */
void DAFontEditPannelWidget::setMaxRecentFontCount(int n)
{
	const int clamped = qBound(1, n, 20);
	if (d->mMaxRecentFontCount == clamped)
		return;
	d->mMaxRecentFontCount = clamped;
	if (d->mRecentFontFamilies.size() > clamped) {
		d->mRecentFontFamilies = d->mRecentFontFamilies.mid(0, clamped);
	}
	d->syncRecentMenu();
}

/** @brief 最近字体条数上限 */
int DAFontEditPannelWidget::maxRecentFontCount() const
{
	return d->mMaxRecentFontCount;
}

/**
 * @brief 注入最近字体列表
 * @param families 最近 → 最旧；空串丢弃、保序去重、按上限截断
 */
void DAFontEditPannelWidget::setRecentFontFamilies(const QStringList& families)
{
	d->mRecentFontFamilies.clear();
	for (const QString& raw : families) {
		const QString family = raw.trimmed();
		if (family.isEmpty() || d->mRecentFontFamilies.contains(family))
			continue;
		d->mRecentFontFamilies << family;
		if (d->mRecentFontFamilies.size() >= d->mMaxRecentFontCount)
			break;
	}
	d->syncRecentMenu();
}

/** @brief 当前最近字体列表（最近优先） */
QStringList DAFontEditPannelWidget::recentFontFamilies() const
{
	return d->mRecentFontFamilies;
}

void DAFontEditPannelWidget::onComboBoxFontSizeTextChanged(const QString& t)
{
	bool isok;
	int v = t.toInt(&isok);
	if (!isok) {
		QSignalBlocker bl(d->ui.comboBoxFontSize);
		d->ui.comboBoxFontSize->setCurrentText(QString::number(d->mFontPointSize));
		return;
	}
	d->mFontPointSize = v;
	signalEmitFontChanged();
}

void DAFontEditPannelWidget::onFontSizeAdd()
{
	d->mFontPointSize += 1;
	QSignalBlocker bl(d->ui.comboBoxFontSize);
	d->ui.comboBoxFontSize->setCurrentText(QString::number(d->mFontPointSize));
	signalEmitFontChanged();
}

void DAFontEditPannelWidget::onFontSizeSub()
{
	d->mFontPointSize -= 1;
	if (d->mFontPointSize <= 0) {
		d->mFontPointSize = 1;
	}
	QSignalBlocker bl(d->ui.comboBoxFontSize);
	d->ui.comboBoxFontSize->setCurrentText(QString::number(d->mFontPointSize));
	signalEmitFontChanged();
}

void DAFontEditPannelWidget::signalEmitFontChanged()
{
	QFont f = getCurrentFont();
	Q_EMIT currentFontChanged(f);
	if (d->mFontPreviewEnabled)
		d->syncPreviewFont();
}
}  // namespace DA

/*** End of inlined file: DAFontEditPannelWidget.cpp ***/


/*** Start of inlined file: DAShapeEditPannelWidget.cpp ***/

/*** Start of inlined file: ui_DAShapeEditPannelWidget.h ***/
#ifndef UI_DASHAPEEDITPANNELWIDGET_H
#define UI_DASHAPEEDITPANNELWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DAShapeEditPannelWidget
{
public:
	QGridLayout *gridLayout;
	QLabel *label_2;
	DA::DAPenEditWidget *widgetPenEdit;
	DA::DABrushEditWidget *widgetBrushEdit;
	QLabel *label;

	void setupUi(QWidget *DAShapeEditPannelWidget)
	{
		if (DAShapeEditPannelWidget->objectName().isEmpty())
			DAShapeEditPannelWidget->setObjectName("DAShapeEditPannelWidget");
		DAShapeEditPannelWidget->resize(101, 47);
		gridLayout = new QGridLayout(DAShapeEditPannelWidget);
		gridLayout->setSpacing(1);
		gridLayout->setObjectName("gridLayout");
		gridLayout->setContentsMargins(1, 1, 1, 1);
		label_2 = new QLabel(DAShapeEditPannelWidget);
		label_2->setObjectName("label_2");

		gridLayout->addWidget(label_2, 1, 0, 1, 1);

		widgetPenEdit = new DA::DAPenEditWidget(DAShapeEditPannelWidget);
		widgetPenEdit->setObjectName("widgetPenEdit");
		QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(widgetPenEdit->sizePolicy().hasHeightForWidth());
		widgetPenEdit->setSizePolicy(sizePolicy);

		gridLayout->addWidget(widgetPenEdit, 1, 1, 1, 1);

		widgetBrushEdit = new DA::DABrushEditWidget(DAShapeEditPannelWidget);
		widgetBrushEdit->setObjectName("widgetBrushEdit");
		sizePolicy.setHeightForWidth(widgetBrushEdit->sizePolicy().hasHeightForWidth());
		widgetBrushEdit->setSizePolicy(sizePolicy);

		gridLayout->addWidget(widgetBrushEdit, 0, 1, 1, 1);

		label = new QLabel(DAShapeEditPannelWidget);
		label->setObjectName("label");

		gridLayout->addWidget(label, 0, 0, 1, 1);

		retranslateUi(DAShapeEditPannelWidget);

		QMetaObject::connectSlotsByName(DAShapeEditPannelWidget);
	} // setupUi

	void retranslateUi(QWidget *DAShapeEditPannelWidget)
	{
		DAShapeEditPannelWidget->setWindowTitle(QCoreApplication::translate("DAShapeEditPannelWidget", "Shape Edit", nullptr));
		label_2->setText(QCoreApplication::translate("DAShapeEditPannelWidget", "Border", nullptr));
		label->setText(QCoreApplication::translate("DAShapeEditPannelWidget", "Fill", nullptr));
	} // retranslateUi

};

namespace Ui {
	class DAShapeEditPannelWidget: public Ui_DAShapeEditPannelWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DASHAPEEDITPANNELWIDGET_H

/*** End of inlined file: ui_DAShapeEditPannelWidget.h ***/

namespace DA
{
class DAShapeEditPannelWidget::PrivateData
{
public:
	Ui::DAShapeEditPannelWidget ui;
};

/**
 * @brief 构造形状编辑面板
 * @param parent 父控件
 * @details 组合DABrushEditWidget和DAPenEditWidget，连接各自的变更信号
 */
DAShapeEditPannelWidget::DAShapeEditPannelWidget(QWidget* parent)
	: QWidget(parent), d(new PrivateData)
{
	d->ui.setupUi(this);
	connect(d->ui.widgetPenEdit, &DAPenEditWidget::penChanged, this, &DAShapeEditPannelWidget::borderPenChanged);
	connect(d->ui.widgetBrushEdit, &DABrushEditWidget::brushChanged, this, &DAShapeEditPannelWidget::backgroundBrushChanged);
}

/**
 * @brief 析构函数
 */
DAShapeEditPannelWidget::~DAShapeEditPannelWidget()
{
	delete d;
}

/**
 * @brief 获取当前背景画刷
 * @return 背景画刷
 */
QBrush DAShapeEditPannelWidget::getBackgroundBrush() const
{
	return d->ui.widgetBrushEdit->getCurrentBrush();
}

/**
 * @brief 获取当前边框画笔
 * @return 边框画笔
 */
QPen DAShapeEditPannelWidget::getBorderPen() const
{
	return d->ui.widgetPenEdit->getCurrentPen();
}

/**
 * @brief 设置背景画刷
 * @param b 新画刷
 */
void DAShapeEditPannelWidget::setBackgroundBrush(const QBrush& b)
{
	if (b == getBackgroundBrush()) {
		return;
	}
	d->ui.widgetBrushEdit->setCurrentBrush(b);
}

/**
 * @brief 设置边框画笔
 * @param v 新画笔
 */
void DAShapeEditPannelWidget::setBorderPen(const QPen& v)
{
	d->ui.widgetPenEdit->setCurrentPen(v);
}
}  // namespace DA

/*** End of inlined file: DAShapeEditPannelWidget.cpp ***/

// ---- Item Selection & Preview 条目选择与预览家族 ----

/*** Start of inlined file: DAItemListEditWidget.cpp ***/
#include <QApplication>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QStyle>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <functional>
#include <utility>

namespace DA
{
bool DAItemEntry::operator==(const DAItemEntry& o) const
{
	if (data.isValid() || o.data.isValid()) {
		return data == o.data;
	}
	return text == o.text;
}

bool DAItemEntry::operator!=(const DAItemEntry& o) const
{
	return !(*this == o);
}

namespace
{
/// 拖拽 MIME 类型：只传源行号，不序列化条目（行控件与 data 都不能走 Qt 的 InternalMove）
const char kRowMime[] = "application/x-dawidgets-itemlist-row";

QString rowMimeType()
{
	return QString::fromLatin1(kRowMime);
}

/**
 * @brief 行首 6 点拖拽手柄（文件内私有，不带 Q_OBJECT）
 * @details 行控件会吃掉鼠标事件，不能依赖 QListWidget::startDrag；
 *          手柄自己发起 QDrag，列表只当落点（DropOnly）。
 */
class RowHandle : public QWidget
{
public:
	RowHandle(QListWidget* list, QListWidgetItem* item, QWidget* parent)
		: QWidget(parent), mList(list), mItem(item)
	{
		setObjectName(QStringLiteral("rowHandle"));
		setFixedSize(14, 26);
		setCursor(Qt::OpenHandCursor);
		setToolTip(QObject::tr("Drag to reorder"));  //cn:拖动排序
	}

protected:
	void paintEvent(QPaintEvent*) override
	{
		QPainter p(this);
		p.setRenderHint(QPainter::Antialiasing, true);
		p.setPen(Qt::NoPen);
		p.setBrush(underMouse() ? QColor(0x90, 0x93, 0x99) : QColor(0xC0, 0xC4, 0xCC));
		for (int col = 0; col < 2; ++col) {
			for (int row = 0; row < 3; ++row)
				p.drawEllipse(QPointF(4.5 + col * 5.0, 7.0 + row * 6.0), 1.3, 1.3);
		}
	}

	bool event(QEvent* e) override
	{
		if (e->type() == QEvent::Enter || e->type() == QEvent::Leave)
			update();
		return QWidget::event(e);
	}

	void mousePressEvent(QMouseEvent* e) override
	{
		if (e->button() == Qt::LeftButton) {
			mPress   = e->pos();
			mPressed = true;
			e->accept();
			return;
		}
		QWidget::mousePressEvent(e);
	}

	void mouseMoveEvent(QMouseEvent* e) override
	{
		if (!mPressed || !(e->buttons() & Qt::LeftButton))
			return;
		if ((e->pos() - mPress).manhattanLength() < QApplication::startDragDistance())
			return;
		mPressed = false;
		QWidget* row = parentWidget();
		const int src = mList->row(mItem);
		if (!row || src < 0)
			return;
		auto* mime = new QMimeData;
		mime->setData(rowMimeType(), QByteArray::number(src));
		auto* drag = new QDrag(this);
		drag->setMimeData(mime);
		// 拖动影像是行控件当前外观，必须在任何半透明 effect 之前抓
		drag->setPixmap(row->grab());
		drag->exec(Qt::MoveAction);
	}

	void mouseReleaseEvent(QMouseEvent* e) override
	{
		mPressed = false;
		QWidget::mouseReleaseEvent(e);
	}

private:
	QListWidget* mList { nullptr };
	QListWidgetItem* mItem { nullptr };
	QPoint mPress;
	bool mPressed { false };
};

/**
 * @brief 只当落点的列表（文件内私有，不带 Q_OBJECT）
 * @details 接受自定义 MIME 时显示 2px 插入线，落位回调宿主 PrivateData::moveRow，
 *          因此 data 与行控件都不会丢。
 */
class ItemListWidget : public QListWidget
{
public:
	ItemListWidget(QWidget* parent, std::function< void(int, int) > moveRow)
		: QListWidget(parent), mMoveRow(std::move(moveRow))
	{
		mIndicator = new QFrame(viewport());
		mIndicator->setObjectName(QStringLiteral("dropIndicator"));
		mIndicator->setStyleSheet(QStringLiteral("background:#409eff;"));
		mIndicator->hide();
	}

protected:
	void dragEnterEvent(QDragEnterEvent* e) override
	{
		if (e->mimeData()->hasFormat(rowMimeType())) {
			e->acceptProposedAction();
			return;
		}
		// 外部拖入（文件 URL 等）一律放行给父控件
		e->ignore();
	}

	void dragMoveEvent(QDragMoveEvent* e) override
	{
		if (!e->mimeData()->hasFormat(rowMimeType())) {
			e->ignore();
			return;
		}
		updateIndicator(Qt5Qt6Compat_QXXEvent_Pos(e));
		e->acceptProposedAction();
	}

	void dragLeaveEvent(QDragLeaveEvent* e) override
	{
		mIndicator->hide();
		QListWidget::dragLeaveEvent(e);
	}

	void dropEvent(QDropEvent* e) override
	{
		mIndicator->hide();
		if (!e->mimeData()->hasFormat(rowMimeType())) {
			e->ignore();
			return;
		}
		bool ok = false;
		const int src = e->mimeData()->data(rowMimeType()).toInt(&ok);
		if (!ok) {
			e->ignore();
			return;
		}
		const QPoint vp = Qt5Qt6Compat_QXXEvent_Pos(e);
		const QModelIndex idx = indexAt(vp);
		int insertAt = count();
		if (idx.isValid()) {
			const QRect r = visualRect(idx);
			insertAt = idx.row() + ((vp.y() > r.center().y()) ? 1 : 0);
		}
		if (src < insertAt)
			--insertAt;
		if (insertAt == src || insertAt < 0 || insertAt >= count()) {
			e->ignore();
			return;
		}
		mMoveRow(src, insertAt);
		e->acceptProposedAction();
	}

private:
	void updateIndicator(const QPoint& vp)
	{
		const QModelIndex idx = indexAt(vp);
		int y = viewport()->height() - 2;
		if (idx.isValid()) {
			const QRect r = visualRect(idx);
			y = (vp.y() > r.center().y()) ? r.bottom() : r.top() - 1;
		}
		mIndicator->setGeometry(0, y, viewport()->width(), 2);
		mIndicator->show();
		mIndicator->raise();
	}

	QFrame* mIndicator { nullptr };
	std::function< void(int, int) > mMoveRow;
};
}  // namespace

class DAItemListEditWidget::PrivateData
{
	DA_DECLARE_PUBLIC(DAItemListEditWidget)
public:
	PrivateData(DAItemListEditWidget* p);
	void init();
	// 追加一行（列表项 + 行内删除按钮），不发射信号
	void appendRow(const DAItemEntry& entry);
	// 删除行并释放行控件，不发射信号
	void removeRow(int row);
	// 条目是否已选中
	bool contains(const DAItemEntry& entry) const;
	// 重建添加菜单（aboutToShow：候选中尚未选中者）
	void rebuildAddMenu();
	// 添加按钮主体点击：搜索多选对话框一次性添加
	void onAddButtonClicked();
	// 移除所有（二次确认）
	void onRemoveAllClicked();
	// 当前行上移/下移（按钮点击）
	void moveCurrent(int offset);
	// 把 from 行移动到 to（最终索引），只发 orderChanged
	void moveRow(int from, int to);
	// 刷新 Up/Down 按钮的显隐与可用状态
	void updateReorderButtons();
	// 过滤：只改行的隐藏状态
	void applyFilter(const QString& key);
	// 空态与列表互斥显隐
	void updateEmptyState();
	// 拖拽手柄增删
	void updateRowDecorations(int row);
	// 还原「再次点击清空」的按钮文案与武装状态
	void disarmClear();

	ItemListWidget* mList { nullptr };
	QToolButton* mAddButton { nullptr };
	QMenu* mAddMenu { nullptr };
	QPushButton* mUpButton { nullptr };
	QPushButton* mDownButton { nullptr };
	QPushButton* mRemoveAllButton { nullptr };
	QLineEdit* mFilter { nullptr };
	QLabel* mEmpty { nullptr };
	bool mReorderEnabled { false };      ///< 是否显示 Up/Down 按钮
	bool mDragReorderEnabled { false };  ///< 是否启用手柄拖拽
	bool mFilterVisible { false };       ///< 是否显示过滤框
	QVector< DAItemEntry > mAvailable;  ///< 候选条目
	QString mAddText;                  ///< 添加按钮文案
	QString mRemoveAllText;            ///< 移除所有按钮文案
	QString mConfirmTitle;             ///< 二次确认标题
	QString mConfirmText;              ///< 二次确认正文
	QString mEmptyText;                ///< 空态文案
	bool mConfirmClearEnabled { false };
	int mConfirmClearInterval { 2500 };
	QString mConfirmClearText;
	QTimer mArmTimer;
	bool mArmed { false };
};

DAItemListEditWidget::PrivateData::PrivateData(DAItemListEditWidget* p) : q_ptr(p)
{
}

void DAItemListEditWidget::PrivateData::init()
{
	DA_Q(DAItemListEditWidget, q);
	mAddText          = q->tr("Add");                           // cn:添加
	mRemoveAllText    = q->tr("Remove All");                    // cn:移除所有
	mConfirmTitle     = q->tr("Remove All");                    // cn:移除所有
	mConfirmText      = q->tr("Remove all selected items?");    // cn:确定移除所有已选条目吗？
	mEmptyText        = q->tr("No matching items");             // cn:没有匹配的条目
	mConfirmClearText = q->tr("Click again to clear");          // cn:再次点击清空

	mArmTimer.setSingleShot(true);
	QObject::connect(&mArmTimer, &QTimer::timeout, q, [ this ]() { disarmClear(); });

	QVBoxLayout* root = new QVBoxLayout(q);
	root->setContentsMargins(0, 0, 0, 0);

	// 过滤框（默认隐藏，setFilterVisible 打开）
	mFilter = new QLineEdit(q);
	mFilter->setObjectName(QStringLiteral("filterEdit"));
	mFilter->setPlaceholderText(q->tr("Filter..."));  // cn:过滤...
	mFilter->setClearButtonEnabled(true);
	mFilter->setVisible(false);
	root->insertWidget(0, mFilter);
	QObject::connect(mFilter, &QLineEdit::textChanged, q, [ this ](const QString& text) { applyFilter(text); });

	// 空态标签与列表互斥显隐（不插占位 item）
	mEmpty = new QLabel(mEmptyText, q);
	mEmpty->setObjectName(QStringLiteral("emptyLabel"));
	mEmpty->setAlignment(Qt::AlignCenter);
	mEmpty->setWordWrap(true);
	mEmpty->setVisible(false);
	root->insertWidget(1, mEmpty, 1);

	mList = new ItemListWidget(q, [ this ](int from, int to) { moveRow(from, to); });
	mList->setObjectName(QStringLiteral("itemList"));
	mList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	// 行文本由 setItemWidget 的行控件显示；delegate 再画一遍会在手柄把 label 挤右后透出重影，
	// 这里把 item 文字色设为透明（item->text() 仍保留，selectedItems()/过滤继续读它）
	QPalette listPalette = mList->palette();
	listPalette.setColor(QPalette::Text, Qt::transparent);
	listPalette.setColor(QPalette::HighlightedText, Qt::transparent);
	mList->setPalette(listPalette);
	root->addWidget(mList, 1);

	QHBoxLayout* bar = new QHBoxLayout();
	mAddButton = new QToolButton(q);
	mAddButton->setText(mAddText);
	mAddButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	mAddButton->setPopupMode(QToolButton::MenuButtonPopup);  // 箭头=候选菜单，主体=勾选对话框
	mAddMenu = new QMenu(mAddButton);
	mAddButton->setMenu(mAddMenu);
	QObject::connect(mAddMenu, &QMenu::aboutToShow, q, [ this ]() { rebuildAddMenu(); });
	QObject::connect(mAddButton, &QToolButton::clicked, q, [ this ]() { onAddButtonClicked(); });
	bar->addWidget(mAddButton);
	mUpButton = new QPushButton(q->tr("Up"), q);  // cn:上移
	mUpButton->setObjectName(QStringLiteral("upButton"));
	mUpButton->setVisible(false);
	QObject::connect(mUpButton, &QPushButton::clicked, q, [ this ]() { moveCurrent(-1); });
	bar->addWidget(mUpButton);
	mDownButton = new QPushButton(q->tr("Down"), q);  // cn:下移
	mDownButton->setObjectName(QStringLiteral("downButton"));
	mDownButton->setVisible(false);
	QObject::connect(mDownButton, &QPushButton::clicked, q, [ this ]() { moveCurrent(1); });
	bar->addWidget(mDownButton);
	mRemoveAllButton = new QPushButton(mRemoveAllText, q);
	mRemoveAllButton->setObjectName(QStringLiteral("removeAllButton"));
	QObject::connect(mRemoveAllButton, &QPushButton::clicked, q, [ this ]() { onRemoveAllClicked(); });
	bar->addWidget(mRemoveAllButton);
	bar->addStretch(1);
	root->addLayout(bar);
	QObject::connect(mList, &QListWidget::currentRowChanged, q, [ this ](int) { updateReorderButtons(); });
	updateReorderButtons();
	updateEmptyState();
}

void DAItemListEditWidget::PrivateData::appendRow(const DAItemEntry& entry)
{
	DA_Q(DAItemListEditWidget, q);
	QListWidgetItem* item = new QListWidgetItem(mList);
	item->setSizeHint(QSize(0, 26));
	// 展示文本与标识 data 存列表项（行控件只做展示与删除按钮）
	item->setText(entry.text);
	item->setData(Qt::UserRole, entry.data);
	QWidget* row = new QWidget(mList);
	QHBoxLayout* rl = new QHBoxLayout(row);
	rl->setContentsMargins(4, 0, 2, 0);
	rl->setSpacing(2);
	QLabel* label = new QLabel(entry.text, row);
	label->setToolTip(entry.text);
	rl->addWidget(label, 1);
	QToolButton* close = new QToolButton(row);
	close->setAutoRaise(true);
	close->setFixedSize(20, 20);
	close->setIcon(row->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	close->setToolTip(q->tr("Remove"));  // cn:移除
	// 行内删除：按 item 定位行号（item 在被删除前始终有效）
	QObject::connect(close, &QToolButton::clicked, q, [ this, item, q ]() {
		const int row = mList->row(item);
		if (row >= 0) {
			removeRow(row);
			Q_EMIT q->selectedItemsChanged();
		}
	});
	rl->addWidget(close);
	mList->addItem(item);
	mList->setItemWidget(item, row);
	updateRowDecorations(mList->row(item));
	applyFilter(mFilter->text());
	updateReorderButtons();
}

void DAItemListEditWidget::PrivateData::removeRow(int row)
{
	QListWidgetItem* item = mList->takeItem(row);
	if (!item) {
		return;
	}
	QWidget* w = mList->itemWidget(item);
	mList->removeItemWidget(item);
	delete item;
	if (w) {
		w->deleteLater();
	}
	applyFilter(mFilter->text());
	updateReorderButtons();
}

void DAItemListEditWidget::PrivateData::moveCurrent(int offset)
{
	DA_Q(DAItemListEditWidget, q);
	const int row = mList->currentRow();
	const int dst = row + offset;
	if (row < 0 || dst < 0 || dst >= mList->count()) {
		return;
	}
	moveRow(row, dst);
	mList->setCurrentRow(dst);
	updateReorderButtons();
	Q_EMIT q->selectedItemsChanged();
}

/**
 * @brief 把 from 行的 item（连带行控件）移动到 to
 * @param from 源行
 * @param to 目标最终索引
 * @details 顺序照抄旧 moveCurrent：takeItem 之后仍能取到 itemWidget；重插后挂回。
 *          不发 selectedItemsChanged，只发 orderChanged。
 */
void DAItemListEditWidget::PrivateData::moveRow(int from, int to)
{
	DA_Q(DAItemListEditWidget, q);
	if (from < 0 || from >= mList->count() || to < 0 || to >= mList->count() || from == to)
		return;
	QListWidgetItem* item = mList->takeItem(from);
	if (!item)
		return;
	// takeItem 会解除 itemWidget 关联，重插后需重新挂回行控件
	QWidget* w = mList->itemWidget(item);
	if (w) {
		mList->removeItemWidget(item);
	}
	mList->insertItem(to, item);
	if (w) {
		mList->setItemWidget(item, w);
	}
	// 行号已变化，隐藏状态必须按当前过滤词重算
	applyFilter(mFilter->text());
	Q_EMIT q->orderChanged(q->selectedItems());
}

/**
 * @brief 过滤：只设置行的隐藏状态，不改已选集合
 * @param key 过滤文本（去首尾空白，大小写不敏感，子串匹配）
 */
void DAItemListEditWidget::PrivateData::applyFilter(const QString& key)
{
	const QString k = key.trimmed();
	for (int i = 0; i < mList->count(); ++i) {
		QListWidgetItem* item = mList->item(i);
		const bool hit = k.isEmpty()
						 || DAStringFilterProxyModel::matchesText(item->text(), k, Qt::CaseInsensitive);
		mList->setRowHidden(i, !hit);
	}
	updateEmptyState();
}

/** @brief 空态与列表互斥显隐（0 条已选或过滤无命中） */
void DAItemListEditWidget::PrivateData::updateEmptyState()
{
	int visible = 0;
	for (int i = 0; i < mList->count(); ++i) {
		if (!mList->isRowHidden(i) && !mList->item(i)->isHidden())
			++visible;
	}
	const bool empty = visible == 0;
	if (mEmpty) {
		mEmpty->setText(mEmptyText);
		mEmpty->setVisible(empty);
	}
	mList->setVisible(!empty);
}

/**
 * @brief 按 mDragReorderEnabled 给行增删手柄
 * @param row 行号
 */
void DAItemListEditWidget::PrivateData::updateRowDecorations(int row)
{
	QListWidgetItem* item = mList->item(row);
	if (!item)
		return;
	QWidget* rowWidget = mList->itemWidget(item);
	if (!rowWidget)
		return;
	// RowHandle 没有 Q_OBJECT，不能用 findChild<RowHandle*>（会被当成 QWidget 匹配）；
	// 用 objectName + 直接子控件过滤
	QWidget* handle = rowWidget->findChild< QWidget* >(QStringLiteral("rowHandle"), Qt::FindDirectChildrenOnly);
	if (mDragReorderEnabled && !handle) {
		if (auto* lay = qobject_cast< QHBoxLayout* >(rowWidget->layout())) {
			auto* h = new RowHandle(mList, item, rowWidget);
			lay->insertWidget(0, h);
		}
	} else if (!mDragReorderEnabled && handle) {
		handle->hide();
		delete handle;
	}
}

/** @brief 还原「再次点击清空」的按钮文案与武装状态 */
void DAItemListEditWidget::PrivateData::disarmClear()
{
	if (mArmTimer.isActive())
		mArmTimer.stop();
	if (mArmed) {
		mArmed = false;
		mRemoveAllButton->setText(mRemoveAllText);
	}
}

void DAItemListEditWidget::PrivateData::updateReorderButtons()
{
	if (!mUpButton || !mDownButton) {
		return;
	}
	mUpButton->setVisible(mReorderEnabled);
	mDownButton->setVisible(mReorderEnabled);
	const int row = mList->currentRow();
	mUpButton->setEnabled(mReorderEnabled && row > 0);
	mDownButton->setEnabled(mReorderEnabled && row >= 0 && row < mList->count() - 1);
}

bool DAItemListEditWidget::PrivateData::contains(const DAItemEntry& entry) const
{
	const QVector< DAItemEntry > sel = q_func()->selectedItems();
	for (const DAItemEntry& e : sel) {
		if (e == entry) {
			return true;
		}
	}
	return false;
}

void DAItemListEditWidget::PrivateData::rebuildAddMenu()
{
	mAddMenu->clear();
	bool any = false;
	for (const DAItemEntry& e : std::as_const(mAvailable)) {
		if (contains(e)) {
			continue;
		}
		any = true;
		QAction* act = mAddMenu->addAction(e.text);
		QObject::connect(act, &QAction::triggered, q_func(), [ this, e ]() {
			if (contains(e)) {
				return;
			}
			appendRow(e);
			Q_EMIT q_func()->selectedItemsChanged();
		});
	}
	if (!any) {
		QAction* act = mAddMenu->addAction(q_func()->tr("No available items"));  // cn:无可用条目
		act->setEnabled(false);
	}
}

void DAItemListEditWidget::PrivateData::onAddButtonClicked()
{
	DA_Q(DAItemListEditWidget, q);
	QVector< DAItemEntry > candidates;
	for (const DAItemEntry& e : std::as_const(mAvailable)) {
		if (!contains(e)) {
			candidates.append(e);
		}
	}
	if (candidates.isEmpty()) {
		return;
	}
	DAItemListPickDialog dlg(q);
	dlg.setItems(candidates);
	if (QDialog::Accepted != dlg.exec()) {
		return;
	}
	const QVector< DAItemEntry > picked = dlg.checkedEntries();
	if (picked.isEmpty()) {
		return;
	}
	for (const DAItemEntry& e : picked) {
		appendRow(e);
	}
	Q_EMIT q->selectedItemsChanged();
}

void DAItemListEditWidget::PrivateData::onRemoveAllClicked()
{
	DA_Q(DAItemListEditWidget, q);
	if (mList->count() == 0) {
		return;
	}
	if (!mConfirmClearEnabled) {
		// 原有模态确认路径，一字不改
		if (QMessageBox::Yes
			!= QMessageBox::question(q, mConfirmTitle, mConfirmText, QMessageBox::Yes | QMessageBox::No,
									 QMessageBox::No)) {
			return;
		}
		while (mList->count() > 0) {
			removeRow(0);
		}
		Q_EMIT q->selectedItemsChanged();
		return;
	}
	if (!mArmed) {
		// 首次点击只武装：换文案 + 起定时器，不发信号
		mArmed = true;
		mRemoveAllButton->setText(mConfirmClearText);
		mArmTimer.start(mConfirmClearInterval);
		return;
	}
	mArmTimer.stop();
	mArmed = false;
	mRemoveAllButton->setText(mRemoveAllText);
	while (mList->count() > 0) {
		removeRow(0);
	}
	Q_EMIT q->selectedItemsChanged();
}

DAItemListEditWidget::DAItemListEditWidget(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
	// 信号参数是自定义类型：注册一次，QSignalSpy / 队列连接才能解析。
	// Qt5 的 moc 在命名空间内把签名记成 "QVector<DAItemEntry>"，Qt6 记全限定名，
	// 两个名字都注册到同一 id，跨版本才都能按签名查到。
	static const int entryVectorTypeId = qRegisterMetaType< QVector< DAItemEntry > >("QVector<DA::DAItemEntry>");
	qRegisterMetaType< QVector< DAItemEntry > >("QVector<DAItemEntry>");
	Q_UNUSED(entryVectorTypeId);
	DA_D(d);
	d->init();
}

DAItemListEditWidget::~DAItemListEditWidget()
{
	DA_D(d);
	d->disarmClear();
}

void DAItemListEditWidget::setAvailableItems(const QVector< DAItemEntry >& items)
{
	DA_D(d);
	d->mAvailable = items;
}

QVector< DAItemEntry > DAItemListEditWidget::availableItems() const
{
	DA_DC(d);
	return d->mAvailable;
}

void DAItemListEditWidget::setSelectedItems(const QVector< DAItemEntry >& items)
{
	DA_D(d);
	d->disarmClear();
	while (d->mList->count() > 0) {
		d->removeRow(0);
	}
	for (const DAItemEntry& e : items) {
		d->appendRow(e);
	}
	d->applyFilter(d->mFilter->text());
}

QVector< DAItemEntry > DAItemListEditWidget::selectedItems() const
{
	DA_DC(d);
	QVector< DAItemEntry > res;
	for (int i = 0; i < d->mList->count(); ++i) {
		const QListWidgetItem* item = d->mList->item(i);
		res.append(DAItemEntry { item->text(), item->data(Qt::UserRole) });
	}
	return res;
}

/**
 * @brief 显示 Up/Down 按钮（按钮移动当前行；与手柄拖拽互不依赖）
 * @param on 是否启用排序
 */
void DAItemListEditWidget::setReorderEnabled(bool on)
{
	DA_D(d);
	d->mReorderEnabled = on;
	d->updateReorderButtons();
}

/**
 * @brief 是否启用排序
 * @return true = 显示 Up/Down 按钮
 */
bool DAItemListEditWidget::isReorderEnabled() const
{
	DA_DC(d);
	return d->mReorderEnabled;
}

void DAItemListEditWidget::setAddButtonText(const QString& t)
{
	DA_D(d);
	d->mAddText = t;
	d->mAddButton->setText(t);
}

void DAItemListEditWidget::setRemoveAllButtonText(const QString& t)
{
	DA_D(d);
	d->mRemoveAllText = t;
	// 武装态显示的是「再次点击清空」，基础文案等 disarm 后再生效
	if (!d->mArmed)
		d->mRemoveAllButton->setText(t);
}

void DAItemListEditWidget::setRemoveAllConfirmText(const QString& title, const QString& text)
{
	DA_D(d);
	d->mConfirmTitle = title;
	d->mConfirmText  = text;
}

/**
 * @brief 打开/关闭行首手柄拖拽排序
 * @param on 是否启用（默认 false）
 * @details 只把列表设为 DropOnly（当落点），拖源是手柄自己发起的 QDrag
 */
void DAItemListEditWidget::setDragReorderEnabled(bool on)
{
	DA_D(d);
	if (d->mDragReorderEnabled == on)
		return;
	d->mDragReorderEnabled = on;
	d->mList->setDragDropMode(on ? QAbstractItemView::DropOnly : QAbstractItemView::NoDragDrop);
	for (int i = 0; i < d->mList->count(); ++i)
		d->updateRowDecorations(i);
}

/** @brief 是否启用行首手柄拖拽 */
bool DAItemListEditWidget::isDragReorderEnabled() const
{
	DA_DC(d);
	return d->mDragReorderEnabled;
}

/**
 * @brief 显示/隐藏顶部过滤框
 * @param on 是否显示（默认 false）
 * @details 关闭时清空过滤文本并恢复全部行
 */
void DAItemListEditWidget::setFilterVisible(bool on)
{
	DA_D(d);
	if (d->mFilterVisible == on)
		return;
	d->mFilterVisible = on;
	if (!on)
		d->mFilter->clear();  // 触发 applyFilter("")
	d->mFilter->setVisible(on);
	d->applyFilter(on ? d->mFilter->text() : QString());
}

/** @brief 过滤框是否显示 */
bool DAItemListEditWidget::isFilterVisible() const
{
	DA_DC(d);
	return d->mFilterVisible;
}

/**
 * @brief 设置空态文案
 * @param text 文案
 */
void DAItemListEditWidget::setEmptyText(const QString& text)
{
	DA_D(d);
	d->mEmptyText = text;
	d->updateEmptyState();
}

/** @brief 获取空态文案 */
QString DAItemListEditWidget::emptyText() const
{
	DA_DC(d);
	return d->mEmptyText;
}

/**
 * @brief 打开/关闭「再次点击清空」
 * @param on 是否启用（默认 false = 保持原有 QMessageBox）
 */
void DAItemListEditWidget::setConfirmClearEnabled(bool on)
{
	DA_D(d);
	if (d->mConfirmClearEnabled == on)
		return;
	d->mConfirmClearEnabled = on;
	if (!on)
		d->disarmClear();
}

/** @brief 是否启用「再次点击清空」 */
bool DAItemListEditWidget::isConfirmClearEnabled() const
{
	DA_DC(d);
	return d->mConfirmClearEnabled;
}

/**
 * @brief 设置武装态自动还原时间
 * @param ms 毫秒，最小 200
 */
void DAItemListEditWidget::setConfirmClearInterval(int ms)
{
	DA_D(d);
	d->mConfirmClearInterval = qMax(200, ms);
	if (d->mArmed)
		d->mArmTimer.start(d->mConfirmClearInterval);
}

/** @brief 获取武装态自动还原时间（毫秒） */
int DAItemListEditWidget::confirmClearInterval() const
{
	DA_DC(d);
	return d->mConfirmClearInterval;
}

/**
 * @brief 设置武装态按钮文案
 * @param text 文案（i18n 覆盖用）
 */
void DAItemListEditWidget::setConfirmClearText(const QString& text)
{
	DA_D(d);
	d->mConfirmClearText = text;
	if (d->mArmed)
		d->mRemoveAllButton->setText(text);
}

}  // namespace DA

/*** End of inlined file: DAItemListEditWidget.cpp ***/



/*** Start of inlined file: DAItemListPickDialog.cpp ***/
#include <QDialogButtonBox>
#include <QVBoxLayout>

namespace DA
{

class DAItemListPickDialog::PrivateData
{
	DA_DECLARE_PUBLIC(DAItemListPickDialog)
public:
	PrivateData(DAItemListPickDialog* p);
	void init();

	DACheckListWidget* mCheckList { nullptr };
};

DAItemListPickDialog::PrivateData::PrivateData(DAItemListPickDialog* p) : q_ptr(p)
{
}

void DAItemListPickDialog::PrivateData::init()
{
	DA_Q(DAItemListPickDialog, q);
	q->setWindowTitle(q->tr("Select Items"));  // cn:选择条目
	q->resize(320, 420);
	QVBoxLayout* root = new QVBoxLayout(q);
	// 勾选/搜索/forced 语义都在 DACheckListWidget 内实现，对话框只是壳
	mCheckList = new DACheckListWidget(q);
	mCheckList->setSearchVisible(true);
	root->addWidget(mCheckList, 1);
	QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, q);
	QObject::connect(box, &QDialogButtonBox::accepted, q, &QDialog::accept);
	QObject::connect(box, &QDialogButtonBox::rejected, q, &QDialog::reject);
	root->addWidget(box);
}

/**
 * @brief 构造条目勾选对话框
 * @param parent 父窗口
 */
DAItemListPickDialog::DAItemListPickDialog(QWidget* parent) : QDialog(parent), DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	d->init();
}

/**
 * @brief 析构
 */
DAItemListPickDialog::~DAItemListPickDialog()
{
}

/**
 * @brief 设置候选条目（全部未勾选）
 * @param items 候选条目
 */
void DAItemListPickDialog::setItems(const QVector< DAItemEntry >& items)
{
	DA_D(d);
	d->mCheckList->setItems(items);
}

/**
 * @brief 预勾选若干条目
 * @param entries 要勾选的条目
 */
void DAItemListPickDialog::setCheckedEntries(const QVector< DAItemEntry >& entries)
{
	DA_D(d);
	d->mCheckList->setCheckedEntries(entries);
}

/**
 * @brief 取回勾中的条目
 * @return 勾选条目（列表顺序）
 */
QVector< DAItemEntry > DAItemListPickDialog::checkedEntries() const
{
	DA_DC(d);
	return d->mCheckList->checkedEntries();
}

/**
 * @brief 覆盖搜索框占位文本
 * @param t 文本
 */
void DAItemListPickDialog::setSearchPlaceholderText(const QString& t)
{
	DA_D(d);
	d->mCheckList->setSearchPlaceholderText(t);
}

}  // namespace DA

/*** End of inlined file: DAItemListPickDialog.cpp ***/


/*** Start of inlined file: DACheckListWidget.cpp ***/
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListView>
#include <QMouseEvent>
#include <QPushButton>
#include <QShortcut>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVBoxLayout>

namespace DA
{

namespace
{
// 列表项上的附加数据（Qt::UserRole 已被 DAItemEntry::data 占用）
constexpr int kForcedRole    = Qt::UserRole + 1;
constexpr int kCheckableRole = Qt::UserRole + 2;
}  // namespace

class DACheckListWidget::PrivateData
{
	DA_DECLARE_PUBLIC(DACheckListWidget)
public:
	PrivateData(DACheckListWidget* p);
	void init();
	// 搜索过滤：由通用代理模型完成（只影响可见行，不改勾选状态）
	void applyFilter(const QString& key);
	// 按条目查找列表项（DAItemEntry 相等语义）
	QStandardItem* findItem(const DAItemEntry& e) const;
	DAItemEntry entryOf(const QStandardItem* item) const;
	bool itemForced(const QStandardItem* item) const;
	bool itemCheckable(const QStandardItem* item) const;
	bool itemUsable(const QStandardItem* item) const;
	void setCheckedSilently(QStandardItem* item, bool on);
	// 全选/反选：只作用于代理当前可见 + 可勾选 + 非 forced + 可用的条目
	void selectAllVisible();
	void invertVisible();
	void onItemChanged(QStandardItem* item);
	// 键盘连选
	int proxyRowOf(const QStandardItem* item) const;
	void applyRangeTo(int fromProxyRow, int toProxyRow, bool targetChecked);

	QLineEdit* mSearch { nullptr };
	QListView* mList { nullptr };
	QStandardItemModel* mModel { nullptr };
	DAStringFilterProxyModel* mProxy { nullptr };
	QPushButton* mSelectAll { nullptr };
	QPushButton* mInvert { nullptr };
	bool mUpdating { false };  ///< 程序化改动期间屏蔽 itemChanged 回环
	bool mKeyboardSelection { false };
	int mAnchorProxyRow { -1 };
	QShortcut* mSelectAllShortcut { nullptr };
};

DACheckListWidget::PrivateData::PrivateData(DACheckListWidget* p) : q_ptr(p)
{
}

void DACheckListWidget::PrivateData::init()
{
	DA_Q(DACheckListWidget, q);
	QVBoxLayout* root = new QVBoxLayout(q);
	root->setContentsMargins(0, 0, 0, 0);
	root->setSpacing(4);

	mSearch = new QLineEdit(q);
	mSearch->setObjectName(QStringLiteral("searchEdit"));
	mSearch->setPlaceholderText(q->tr("Search..."));  // cn:搜索...
	mSearch->setClearButtonEnabled(true);
	root->addWidget(mSearch);

	mModel = new QStandardItemModel(q);
	mProxy = new DAStringFilterProxyModel(q);
	mProxy->setSourceModel(mModel);
	mList = new QListView(q);
	mList->setObjectName(QStringLiteral("checkList"));
	mList->setModel(mProxy);
	// 勾选即选择，避免选中态干扰
	mList->setSelectionMode(QAbstractItemView::NoSelection);
	mList->viewport()->installEventFilter(q);
	root->addWidget(mList, 1);

	QHBoxLayout* btnRow = new QHBoxLayout();
	btnRow->setContentsMargins(0, 0, 0, 0);
	mSelectAll = new QPushButton(q->tr("Select All"), q);  // cn:全选
	mSelectAll->setObjectName(QStringLiteral("selectAllButton"));
	mInvert = new QPushButton(q->tr("Invert Selection"), q);  // cn:反选
	mInvert->setObjectName(QStringLiteral("invertButton"));
	btnRow->addWidget(mSelectAll);
	btnRow->addWidget(mInvert);
	btnRow->addStretch(1);
	root->addLayout(btnRow);

	QObject::connect(mSearch, &QLineEdit::textChanged, q, [ this ](const QString& t) { applyFilter(t); });
	QObject::connect(mSelectAll, &QPushButton::clicked, q, [ this ]() { selectAllVisible(); });
	QObject::connect(mInvert, &QPushButton::clicked, q, [ this ]() { invertVisible(); });
	QObject::connect(mModel, &QStandardItemModel::itemChanged, q, [ this ](QStandardItem* item) { onItemChanged(item); });
}

void DACheckListWidget::PrivateData::applyFilter(const QString& key)
{
	mProxy->setFilterText(key.trimmed());
	// 代理行号会重排，旧锚点不再指向同一条目
	mAnchorProxyRow = -1;
}

QStandardItem* DACheckListWidget::PrivateData::findItem(const DAItemEntry& e) const
{
	for (int i = 0; i < mModel->rowCount(); ++i) {
		QStandardItem* item = mModel->item(i);
		if (item && entryOf(item) == e) {
			return item;
		}
	}
	return nullptr;
}

DAItemEntry DACheckListWidget::PrivateData::entryOf(const QStandardItem* item) const
{
	return DAItemEntry { item->text(), item->data(Qt::UserRole) };
}

bool DACheckListWidget::PrivateData::itemForced(const QStandardItem* item) const
{
	return item->data(kForcedRole).toBool();
}

bool DACheckListWidget::PrivateData::itemCheckable(const QStandardItem* item) const
{
	return item->data(kCheckableRole).toBool();
}

bool DACheckListWidget::PrivateData::itemUsable(const QStandardItem* item) const
{
	return item->flags().testFlag(Qt::ItemIsEnabled);
}

void DACheckListWidget::PrivateData::setCheckedSilently(QStandardItem* item, bool on)
{
	const Qt::CheckState target = on ? Qt::Checked : Qt::Unchecked;
	if (item->checkState() == target) {
		return;
	}
	const bool prev = mUpdating;
	mUpdating       = true;
	item->setCheckState(target);
	mUpdating = prev;
}

void DACheckListWidget::PrivateData::selectAllVisible()
{
	DA_Q(DACheckListWidget, q);
	bool changed = false;
	for (int i = 0; i < mProxy->rowCount(); ++i) {
		QStandardItem* item = mModel->itemFromIndex(mProxy->mapToSource(mProxy->index(i, 0)));
		if (!item) {
			continue;
		}
		if (itemForced(item) || !itemCheckable(item) || !itemUsable(item)) {
			continue;
		}
		if (item->checkState() != Qt::Checked) {
			setCheckedSilently(item, true);
			changed = true;
		}
	}
	if (changed) {
		Q_EMIT q->checkedEntriesChanged();
	}
}

void DACheckListWidget::PrivateData::invertVisible()
{
	DA_Q(DACheckListWidget, q);
	bool changed = false;
	for (int i = 0; i < mProxy->rowCount(); ++i) {
		QStandardItem* item = mModel->itemFromIndex(mProxy->mapToSource(mProxy->index(i, 0)));
		if (!item) {
			continue;
		}
		if (itemForced(item) || !itemCheckable(item) || !itemUsable(item)) {
			continue;
		}
		setCheckedSilently(item, item->checkState() != Qt::Checked);
		changed = true;
	}
	if (changed) {
		Q_EMIT q->checkedEntriesChanged();
	}
}

void DACheckListWidget::PrivateData::onItemChanged(QStandardItem* item)
{
	DA_Q(DACheckListWidget, q);
	if (mUpdating || !item) {
		return;
	}
	// forced 条目永远勾选：用户取消后立即恢复，不发信号
	if (itemForced(item)) {
		setCheckedSilently(item, true);
		return;
	}
	// 没有复选框的条目不应出现勾选态
	if (!itemCheckable(item)) {
		setCheckedSilently(item, false);
		return;
	}
	// 锚点只能记录在 forced/不可勾选守卫之后：否则会把强制行当成"取消"意图
	if (mKeyboardSelection)
		mAnchorProxyRow = proxyRowOf(item);
	Q_EMIT q->checkedEntriesChanged();
}

int DACheckListWidget::PrivateData::proxyRowOf(const QStandardItem* item) const
{
	if (!item)
		return -1;
	const QModelIndex src = mModel->indexFromItem(item);
	if (!src.isValid())
		return -1;
	return mProxy->mapFromSource(src).row();
}

/**
 * @brief 把代理行区间内可操作的条目套用同一勾选状态
 * @details 跳过 forced / 无复选框 / 不可用条目；整段只发一次信号
 */
void DACheckListWidget::PrivateData::applyRangeTo(int fromProxyRow, int toProxyRow, bool targetChecked)
{
	DA_Q(DACheckListWidget, q);
	if (fromProxyRow > toProxyRow) {
		const int tmp = fromProxyRow;
		fromProxyRow = toProxyRow;
		toProxyRow   = tmp;
	}
	bool changed = false;
	for (int i = fromProxyRow; i <= toProxyRow; ++i) {
		QStandardItem* item = mModel->itemFromIndex(mProxy->mapToSource(mProxy->index(i, 0)));
		if (!item || itemForced(item) || !itemCheckable(item) || !itemUsable(item))
			continue;
		const Qt::CheckState target = targetChecked ? Qt::Checked : Qt::Unchecked;
		if (item->checkState() != target) {
			setCheckedSilently(item, targetChecked);
			changed = true;
		}
	}
	if (changed)
		Q_EMIT q->checkedEntriesChanged();
}

//===============================================================
// DACheckListWidget
//===============================================================

/**
 * @brief 构造勾选列表
 * @param parent 父控件
 */
DACheckListWidget::DACheckListWidget(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	d->init();
}

/**
 * @brief 析构
 */
DACheckListWidget::~DACheckListWidget()
{
}

/**
 * @brief 设置候选条目（全部未勾选、可用、可勾选、非 forced）
 * @param items 候选条目
 * @note 整体替换，不发 checkedEntriesChanged
 */
void DACheckListWidget::setItems(const QVector< DAItemEntry >& items)
{
	DA_D(d);
	d->mUpdating = true;
	d->mModel->clear();
	for (const DAItemEntry& e : items) {
		QStandardItem* item = new QStandardItem(e.text);
		item->setData(e.data, Qt::UserRole);
		item->setData(false, kForcedRole);
		item->setData(true, kCheckableRole);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
		item->setCheckState(Qt::Unchecked);
		d->mModel->appendRow(item);
	}
	d->mUpdating = false;
	// 新条目也要遵守当前搜索条件
	d->applyFilter(d->mSearch->text());
}

/**
 * @brief 获取全部候选条目
 * @return 条目列表（含被过滤掉/禁用的）
 */
QVector< DAItemEntry > DACheckListWidget::items() const
{
	DA_DC(d);
	QVector< DAItemEntry > res;
	res.reserve(d->mModel->rowCount());
	for (int i = 0; i < d->mModel->rowCount(); ++i) {
		res.append(d->entryOf(d->mModel->item(i)));
	}
	return res;
}

/**
 * @brief 预勾选若干条目
 * @param entries 要勾选的条目（按 DAItemEntry 相等匹配）
 * @note forced 条目始终勾选；不发 checkedEntriesChanged
 */
void DACheckListWidget::setCheckedEntries(const QVector< DAItemEntry >& entries)
{
	DA_D(d);
	d->mUpdating = true;
	for (int i = 0; i < d->mModel->rowCount(); ++i) {
		QStandardItem* item = d->mModel->item(i);
		// forced 优先：永远勾选
		if (d->itemForced(item)) {
			d->setCheckedSilently(item, true);
			continue;
		}
		bool matched           = false;
		const DAItemEntry cur  = d->entryOf(item);
		for (const DAItemEntry& e : entries) {
			if (e == cur) {
				matched = true;
				break;
			}
		}
		d->setCheckedSilently(item, matched && d->itemCheckable(item));
	}
	d->mUpdating = false;
}

/**
 * @brief 获取勾中的条目
 * @return 勾选条目（列表顺序；含被过滤掉/禁用的；含 forced）
 */
QVector< DAItemEntry > DACheckListWidget::checkedEntries() const
{
	DA_DC(d);
	QVector< DAItemEntry > res;
	for (int i = 0; i < d->mModel->rowCount(); ++i) {
		const QStandardItem* item = d->mModel->item(i);
		if (item && item->checkState() == Qt::Checked && d->itemCheckable(item)) {
			res.append(d->entryOf(item));
		}
	}
	return res;
}

/**
 * @brief 设置条目可用性
 * @param e 条目
 * @param enabled 是否可用（不可用时用户不能勾选）
 */
void DACheckListWidget::setEntryEnabled(const DAItemEntry& e, bool enabled)
{
	DA_D(d);
	QStandardItem* item = d->findItem(e);
	if (!item) {
		return;
	}
	Qt::ItemFlags flags = item->flags();
	if (enabled) {
		flags |= Qt::ItemIsEnabled;
	} else {
		flags &= ~Qt::ItemIsEnabled;
	}
	d->mUpdating = true;
	item->setFlags(flags);
	d->mUpdating = false;
}

/**
 * @brief 设置条目是否有复选框
 * @param e 条目
 * @param checkable false 时按未勾选处理
 */
void DACheckListWidget::setEntryCheckable(const DAItemEntry& e, bool checkable)
{
	DA_D(d);
	QStandardItem* item = d->findItem(e);
	if (!item) {
		return;
	}
	d->mUpdating = true;
	item->setData(checkable, kCheckableRole);
	Qt::ItemFlags flags = item->flags();
	if (checkable) {
		flags |= Qt::ItemIsUserCheckable;
	} else {
		flags &= ~Qt::ItemIsUserCheckable;
		// forced 优先：强制勾选项保持勾选
		if (!d->itemForced(item)) {
			d->setCheckedSilently(item, false);
		}
	}
	item->setFlags(flags);
	d->mUpdating = false;
}

/**
 * @brief 设置/取消强制勾选
 * @param e 条目
 * @param on true 时永远勾选且用户不能取消
 */
void DACheckListWidget::setEntryForcedChecked(const DAItemEntry& e, bool on)
{
	DA_D(d);
	QStandardItem* item = d->findItem(e);
	if (!item) {
		return;
	}
	d->mUpdating = true;
	item->setData(on, kForcedRole);
	if (on) {
		d->setCheckedSilently(item, true);
	}
	d->mUpdating = false;
}

/**
 * @brief 搜索框显隐
 * @param on 是否显示
 */
void DACheckListWidget::setSearchVisible(bool on)
{
	DA_D(d);
	d->mSearch->setVisible(on);
}

/**
 * @brief 设置搜索框占位文本
 * @param t 文本
 */
void DACheckListWidget::setSearchPlaceholderText(const QString& t)
{
	DA_D(d);
	d->mSearch->setPlaceholderText(t);
}

/**
 * @brief 全选按钮显隐
 * @param on 是否显示
 */
void DACheckListWidget::setSelectAllVisible(bool on)
{
	DA_D(d);
	d->mSelectAll->setVisible(on);
}

/**
 * @brief 反选按钮显隐
 * @param on 是否显示
 */
void DACheckListWidget::setInvertVisible(bool on)
{
	DA_D(d);
	d->mInvert->setVisible(on);
}

/**
 * @brief 打开/关闭键盘批量勾选
 * @param on 是否启用：Shift+点击区间连选、Ctrl+A 全选当前可见（默认 false）
 */
void DACheckListWidget::setKeyboardSelectionEnabled(bool on)
{
	DA_D(d);
	if (d->mKeyboardSelection == on)
		return;
	d->mKeyboardSelection = on;
	if (on) {
		if (!d->mSelectAllShortcut) {
			d->mSelectAllShortcut = new QShortcut(QKeySequence::SelectAll, d->mList);
			d->mSelectAllShortcut->setContext(Qt::WidgetWithChildrenShortcut);
			QObject::connect(d->mSelectAllShortcut, &QShortcut::activated, this, [ this ]() {
				DA_D(d);
				d->selectAllVisible();
			});
		}
	} else {
		d->mAnchorProxyRow = -1;
		if (d->mSelectAllShortcut) {
			d->mSelectAllShortcut->deleteLater();
			d->mSelectAllShortcut = nullptr;
		}
	}
}

/** @brief 键盘批量勾选是否启用 */
bool DACheckListWidget::keyboardSelectionEnabled() const
{
	DA_DC(d);
	return d->mKeyboardSelection;
}

/**
 * @brief 视图事件过滤：Shift+左键按下套用区间并吞掉事件
 * @details 只处理 press 与 dblclick；普通点击完全放行
 */
bool DACheckListWidget::eventFilter(QObject* watched, QEvent* event)
{
	DA_D(d);
	if (watched != d->mList->viewport() || !d->mKeyboardSelection)
		return QWidget::eventFilter(watched, event);
	if (event->type() == QEvent::MouseButtonDblClick) {
		auto* me = static_cast< QMouseEvent* >(event);
		if (me->button() == Qt::LeftButton)
			return true;  // 双击不再叠加一次翻转
		return QWidget::eventFilter(watched, event);
	}
	if (event->type() != QEvent::MouseButtonPress)
		return QWidget::eventFilter(watched, event);
	auto* me = static_cast< QMouseEvent* >(event);
	if (me->button() != Qt::LeftButton || !(me->modifiers() & Qt::ShiftModifier))
		return QWidget::eventFilter(watched, event);
	if (d->mAnchorProxyRow < 0)
		return QWidget::eventFilter(watched, event);
	const QModelIndex target = d->mList->indexAt(me->pos());
	if (!target.isValid())
		return QWidget::eventFilter(watched, event);
	const QStandardItem* anchorItem =
		d->mModel->itemFromIndex(d->mProxy->mapToSource(d->mProxy->index(d->mAnchorProxyRow, 0)));
	if (!anchorItem)
		return QWidget::eventFilter(watched, event);
	const bool targetChecked = (anchorItem->checkState() == Qt::Checked);
	const int targetRow      = target.row();
	d->applyRangeTo(d->mAnchorProxyRow, targetRow, targetChecked);
	d->mAnchorProxyRow = targetRow;
	d->mList->viewport()->update();
	return true;
}

/**
 * @brief 条目总数
 * @return 行数
 */
int DACheckListWidget::itemCount() const
{
	DA_DC(d);
	return d->mModel->rowCount();
}

/**
 * @brief 勾选总数（含被过滤掉/禁用的/forced 条目）
 * @return 勾选数
 */
int DACheckListWidget::checkedCount() const
{
	return checkedEntries().size();
}

}  // namespace DA

/*** End of inlined file: DACheckListWidget.cpp ***/


/*** Start of inlined file: DAStringListEditWidget.cpp ***/
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace DA
{

class DAStringListEditWidget::PrivateData
{
	DA_DECLARE_PUBLIC(DAStringListEditWidget)
public:
	PrivateData(DAStringListEditWidget* p);
	void init();
	// 统一刷新按钮可用状态与空状态提示
	void updateUiState();
	// 更新占位提示的位置与可见性
	void updatePlaceholder();
	// 当前行上移/下移，返回是否真的移动
	bool moveCurrent(int offset);
	void onAddClicked();
	void onRemoveClicked();

	QListWidget* mList { nullptr };
	QPushButton* mAdd { nullptr };
	QPushButton* mRemove { nullptr };
	QPushButton* mUp { nullptr };
	QPushButton* mDown { nullptr };
	QLabel* mPlaceholder { nullptr };
	AddMode mAddMode { AddMode::Inline };
	bool mReorderEnabled { false };
	bool mRemoveEnabled { true };
	bool mEditable { true };
	int mMaxCount { 0 };
	QString mPlaceholderText;
	bool mUpdating { false };  ///< 程序化增删期间屏蔽 itemChanged 回环
};

DAStringListEditWidget::PrivateData::PrivateData(DAStringListEditWidget* p) : q_ptr(p)
{
}

void DAStringListEditWidget::PrivateData::init()
{
	DA_Q(DAStringListEditWidget, q);
	QHBoxLayout* root = new QHBoxLayout(q);
	root->setContentsMargins(0, 0, 0, 0);
	root->setSpacing(4);

	mList = new QListWidget(q);
	mList->setObjectName(QStringLiteral("stringList"));
	mList->setSelectionMode(QAbstractItemView::SingleSelection);
	mList->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
	root->addWidget(mList, 1);

	// 空状态提示：叠在 viewport 上，不参与布局、不吃鼠标
	mPlaceholder = new QLabel(mList->viewport());
	mPlaceholder->setObjectName(QStringLiteral("placeholderLabel"));
	mPlaceholder->setAlignment(Qt::AlignCenter);
	mPlaceholder->setWordWrap(true);
	mPlaceholder->setAttribute(Qt::WA_TransparentForMouseEvents);
	mPlaceholder->setStyleSheet(QStringLiteral("color:#909399;background:transparent;"));
	mPlaceholder->hide();
	mList->viewport()->installEventFilter(q);

	QVBoxLayout* btnLayout = new QVBoxLayout();
	btnLayout->setContentsMargins(0, 0, 0, 0);
	btnLayout->setSpacing(2);
	mAdd = new QPushButton(q->tr("Add"), q);  // cn:添加
	mAdd->setObjectName(QStringLiteral("addButton"));
	mRemove = new QPushButton(q->tr("Remove"), q);  // cn:删除
	mRemove->setObjectName(QStringLiteral("removeButton"));
	mUp = new QPushButton(q->tr("Up"), q);  // cn:上移
	mUp->setObjectName(QStringLiteral("upButton"));
	mDown = new QPushButton(q->tr("Down"), q);  // cn:下移
	mDown->setObjectName(QStringLiteral("downButton"));
	btnLayout->addWidget(mAdd);
	btnLayout->addWidget(mRemove);
	btnLayout->addWidget(mUp);
	btnLayout->addWidget(mDown);
	btnLayout->addStretch();
	root->addLayout(btnLayout);

	QObject::connect(mAdd, &QPushButton::clicked, q, [ this ]() { onAddClicked(); });
	QObject::connect(mRemove, &QPushButton::clicked, q, [ this ]() { onRemoveClicked(); });
	QObject::connect(mUp, &QPushButton::clicked, q, [ this ]() { moveCurrent(-1); });
	QObject::connect(mDown, &QPushButton::clicked, q, [ this ]() { moveCurrent(1); });
	QObject::connect(mList, &QListWidget::itemChanged, q, [ this ]() {
		if (mUpdating) {
			return;
		}
		updateUiState();
		Q_EMIT q_func()->itemsChanged();
	});
	QObject::connect(mList, &QListWidget::currentRowChanged, q, [ this ](int) { updateUiState(); });
	updateUiState();
}

void DAStringListEditWidget::PrivateData::updateUiState()
{
	const int count      = mList->count();
	const bool overLimit = mMaxCount > 0 && count >= mMaxCount;
	const bool canInlineAdd = (mAddMode == AddMode::Inline) ? mEditable : true;
	mAdd->setEnabled(!overLimit && canInlineAdd);
	mRemove->setVisible(mRemoveEnabled);
	mRemove->setEnabled(mRemoveEnabled && mList->currentRow() >= 0);
	mUp->setVisible(mReorderEnabled);
	mDown->setVisible(mReorderEnabled);
	mUp->setEnabled(mReorderEnabled && mList->currentRow() > 0);
	mDown->setEnabled(mReorderEnabled && mList->currentRow() >= 0 && mList->currentRow() < count - 1);
	updatePlaceholder();
}

void DAStringListEditWidget::PrivateData::updatePlaceholder()
{
	const bool show = mList->count() == 0 && !mPlaceholderText.isEmpty();
	mPlaceholder->setText(mPlaceholderText);
	mPlaceholder->setGeometry(mList->viewport()->rect().adjusted(6, 6, -6, -6));
	mPlaceholder->setVisible(show);
}

bool DAStringListEditWidget::PrivateData::moveCurrent(int offset)
{
	DA_Q(DAStringListEditWidget, q);
	const int row = mList->currentRow();
	const int dst = row + offset;
	if (row < 0 || dst < 0 || dst >= mList->count()) {
		return false;
	}
	mUpdating = true;
	QListWidgetItem* item = mList->takeItem(row);
	mList->insertItem(dst, item);
	mList->setCurrentRow(dst);
	mUpdating = false;
	updateUiState();
	Q_EMIT q->itemsChanged();
	return true;
}

void DAStringListEditWidget::PrivateData::onAddClicked()
{
	DA_Q(DAStringListEditWidget, q);
	if (mAddMode == AddMode::External) {
		Q_EMIT q->addRequested();
		return;
	}
	if (mMaxCount > 0 && mList->count() >= mMaxCount) {
		return;
	}
	if (!mEditable) {
		return;
	}
	mUpdating = true;
	QListWidgetItem* item = new QListWidgetItem(QString(), mList);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	mList->setCurrentItem(item);
	mUpdating = false;
	// Inline：直接进入编辑，不插入 "New Item" 之类的占位文本
	mList->editItem(item);
	updateUiState();
	Q_EMIT q->itemsChanged();
}

void DAStringListEditWidget::PrivateData::onRemoveClicked()
{
	DA_Q(DAStringListEditWidget, q);
	const int row = mList->currentRow();
	if (row < 0) {
		return;
	}
	mUpdating = true;
	delete mList->takeItem(row);
	mUpdating = false;
	updateUiState();
	Q_EMIT q->itemsChanged();
}

//===============================================================
// DAStringListEditWidget
//===============================================================

/**
 * @brief 构造字符串列表编辑器
 * @param parent 父控件
 */
DAStringListEditWidget::DAStringListEditWidget(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
	DA_D(d);
	d->init();
}

/**
 * @brief 析构
 */
DAStringListEditWidget::~DAStringListEditWidget()
{
}

/**
 * @brief 整体替换列表（不发 itemsChanged）
 * @param items 新列表
 */
void DAStringListEditWidget::setItems(const QStringList& items)
{
	DA_D(d);
	d->mUpdating = true;
	d->mList->clear();
	for (const QString& s : items) {
		QListWidgetItem* item = new QListWidgetItem(s, d->mList);
		if (d->mEditable) {
			item->setFlags(item->flags() | Qt::ItemIsEditable);
		}
	}
	d->mUpdating = false;
	d->updateUiState();
}

/**
 * @brief 获取当前列表
 * @return 字符串列表
 */
QStringList DAStringListEditWidget::items() const
{
	DA_DC(d);
	QStringList res;
	for (int i = 0; i < d->mList->count(); ++i) {
		res.append(d->mList->item(i)->text());
	}
	return res;
}

/**
 * @brief 追加一条（External 模式下由调用方使用）
 * @param text 文本（允许空串）
 */
void DAStringListEditWidget::appendItem(const QString& text)
{
	DA_D(d);
	if (d->mMaxCount > 0 && d->mList->count() >= d->mMaxCount) {
		return;
	}
	d->mUpdating = true;
	QListWidgetItem* item = new QListWidgetItem(text, d->mList);
	if (d->mEditable) {
		item->setFlags(item->flags() | Qt::ItemIsEditable);
	}
	d->mList->setCurrentItem(item);
	d->mUpdating = false;
	d->updateUiState();
	Q_EMIT itemsChanged();
}

/**
 * @brief 设置 Add 按钮行为
 * @param m Inline/External
 */
void DAStringListEditWidget::setAddMode(AddMode m)
{
	DA_D(d);
	d->mAddMode = m;
	d->updateUiState();
}

/**
 * @brief 获取 Add 按钮行为
 * @return AddMode
 */
DAStringListEditWidget::AddMode DAStringListEditWidget::addMode() const
{
	DA_DC(d);
	return d->mAddMode;
}

/**
 * @brief 设置 Up/Down 按钮显隐
 * @param on 是否显示
 */
void DAStringListEditWidget::setReorderEnabled(bool on)
{
	DA_D(d);
	d->mReorderEnabled = on;
	d->updateUiState();
}

/**
 * @brief 获取 Up/Down 按钮显隐
 * @return 是否启用排序
 */
bool DAStringListEditWidget::isReorderEnabled() const
{
	DA_DC(d);
	return d->mReorderEnabled;
}

/**
 * @brief 设置 Remove 按钮显隐
 * @param on 是否显示
 */
void DAStringListEditWidget::setRemoveEnabled(bool on)
{
	DA_D(d);
	d->mRemoveEnabled = on;
	d->updateUiState();
}

/**
 * @brief 获取 Remove 按钮显隐
 * @return 是否启用删除
 */
bool DAStringListEditWidget::isRemoveEnabled() const
{
	DA_DC(d);
	return d->mRemoveEnabled;
}

/**
 * @brief 设置是否允许编辑条目文本
 * @param on 是否可编辑
 */
void DAStringListEditWidget::setEditable(bool on)
{
	DA_D(d);
	d->mEditable = on;
	for (int i = 0; i < d->mList->count(); ++i) {
		QListWidgetItem* item = d->mList->item(i);
		Qt::ItemFlags flags    = item->flags();
		if (on) {
			flags |= Qt::ItemIsEditable;
		} else {
			flags &= ~Qt::ItemIsEditable;
		}
		item->setFlags(flags);
	}
	d->mList->setEditTriggers(on ? (QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed)
								 : QAbstractItemView::NoEditTriggers);
	d->updateUiState();
}

/**
 * @brief 获取是否允许编辑
 * @return 是否可编辑
 */
bool DAStringListEditWidget::isEditable() const
{
	DA_DC(d);
	return d->mEditable;
}

/**
 * @brief 设置数量上限（<=0 不限）
 * @param n 上限
 */
void DAStringListEditWidget::setMaxCount(int n)
{
	DA_D(d);
	d->mMaxCount = n;
	d->updateUiState();
}

/**
 * @brief 获取数量上限
 * @return 上限（<=0 表示不限）
 */
int DAStringListEditWidget::maxCount() const
{
	DA_DC(d);
	return d->mMaxCount;
}

/**
 * @brief 设置空列表提示文本
 * @param t 文本
 */
void DAStringListEditWidget::setPlaceholderText(const QString& t)
{
	DA_D(d);
	d->mPlaceholderText = t;
	d->updatePlaceholder();
}

/**
 * @brief 获取空列表提示文本
 * @return 文本
 */
QString DAStringListEditWidget::placeholderText() const
{
	DA_DC(d);
	return d->mPlaceholderText;
}

/**
 * @brief 监听 viewport 尺寸变化以重排空状态提示
 * @param obj 事件目标
 * @param e 事件
 * @return 是否拦截
 */
bool DAStringListEditWidget::eventFilter(QObject* obj, QEvent* e)
{
	DA_D(d);
	if (d->mList && obj == d->mList->viewport() && e->type() == QEvent::Resize) {
		d->updatePlaceholder();
	}
	return QWidget::eventFilter(obj, e);
}

}  // namespace DA

/*** End of inlined file: DAStringListEditWidget.cpp ***/


/*** Start of inlined file: DAColorScalePreviewWidget.cpp ***/
#include <QPainter>
#include <QLinearGradient>
#include <QPaintEvent>
#include <algorithm>

namespace DA
{

DAColorScalePreviewWidget::DAColorScalePreviewWidget(QWidget* parent) : QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void DAColorScalePreviewWidget::setColors(const QList< QColor >& colors)
{
	if (mColors == colors) {
		return;
	}
	mColors = colors;
	updateGradientBrush();
	update();
	Q_EMIT colorsChanged(mColors);
}

QList< QColor > DAColorScalePreviewWidget::colors() const
{
	return mColors;
}

QSize DAColorScalePreviewWidget::sizeHint() const
{
	return QSize(160, 22);
}

QSize DAColorScalePreviewWidget::minimumSizeHint() const
{
	return QSize(48, 16);
}

void DAColorScalePreviewWidget::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	// 渐变端点跟随控件尺寸，尺寸变化时需重建
	updateGradientBrush();
}

/**
 * @brief 重建缓存渐变画刷
 *
 * 仅在颜色变化或尺寸变化时调用，绘制时直接用 mGradientBrush，避免在 paintEvent 中重复构造。
 * 渐变端点依赖控件尺寸（外框内缩 1px 的区域），因此尺寸变化同样需要重建。
 */
void DAColorScalePreviewWidget::updateGradientBrush()
{
	const bool valid = (mColors.size() == 2 || mColors.size() == 3)
					   && std::all_of(mColors.constBegin(), mColors.constEnd(), [](const QColor& c) { return c.isValid(); });
	mGradientValid = valid;
	if (!valid) {
		mGradientBrush = QBrush();
		return;
	}
	const QRect body = rect().adjusted(1, 1, -1, -1);
	QLinearGradient g(body.topLeft(), body.topRight());
	g.setColorAt(0.0, mColors.at(0));
	if (mColors.size() == 3) {
		g.setColorAt(0.5, mColors.at(1));
		g.setColorAt(1.0, mColors.at(2));
	} else {
		g.setColorAt(1.0, mColors.at(1));
	}
	mGradientBrush = QBrush(g);
}

void DAColorScalePreviewWidget::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing, false);
	const QRect body = rect().adjusted(1, 1, -1, -1);
	if (mGradientValid) {
		p.fillRect(body, mGradientBrush);
	} else {
		p.fillRect(body, palette().base());
	}
	p.setPen(palette().color(QPalette::Mid));
	p.drawRect(rect().adjusted(0, 0, -1, -1));
}

}  // namespace DA

/*** End of inlined file: DAColorScalePreviewWidget.cpp ***/


/*** Start of inlined file: DAStringFilterProxyModel.cpp ***/
namespace DA
{

/**
 * @brief 构造过滤代理
 * @param parent 父对象
 */
DAStringFilterProxyModel::DAStringFilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent)
{
	setFilterCaseSensitivity(Qt::CaseInsensitive);
}

/**
 * @brief 析构
 */
DAStringFilterProxyModel::~DAStringFilterProxyModel()
{
}

/**
 * @brief 设置过滤文本
 * @param text 文本（空串表示不过滤）
 */
void DAStringFilterProxyModel::setFilterText(const QString& text)
{
	if (mFilterText == text) {
		return;
	}
	mFilterText = text;
	invalidateFilter();
}

/**
 * @brief 获取过滤文本
 * @return 文本
 */
QString DAStringFilterProxyModel::filterText() const
{
	return mFilterText;
}

/**
 * @brief 设置大小写敏感
 * @param cs 大小写策略
 */
void DAStringFilterProxyModel::setFilterCaseSensitivity(Qt::CaseSensitivity cs)
{
	QSortFilterProxyModel::setFilterCaseSensitivity(cs);
	invalidateFilter();
}

/**
 * @brief 获取大小写敏感
 * @return 大小写策略
 */
Qt::CaseSensitivity DAStringFilterProxyModel::filterCaseSensitivity() const
{
	return QSortFilterProxyModel::filterCaseSensitivity();
}

/**
 * @brief 设置过滤列
 * @param column 列号，-1 表示所有列
 */
void DAStringFilterProxyModel::setFilterColumn(int column)
{
	if (mFilterColumn == column) {
		return;
	}
	mFilterColumn = column;
	invalidateFilter();
}

/**
 * @brief 获取过滤列
 * @return 列号（-1 = 所有列）
 */
int DAStringFilterProxyModel::filterColumn() const
{
	return mFilterColumn;
}

/**
 * @brief 设置过滤使用的数据角色
 * @param role 数据角色（默认 Qt::DisplayRole）
 */
void DAStringFilterProxyModel::setFilterRole(int role)
{
	if (mFilterRole == role) {
		return;
	}
	mFilterRole = role;
	invalidateFilter();
}

/**
 * @brief 获取过滤数据角色
 * @return 数据角色
 */
int DAStringFilterProxyModel::filterRole() const
{
	return mFilterRole;
}

/**
 * @brief 设置子节点命中时是否放行父节点
 * @param on 是否放行
 */
void DAStringFilterProxyModel::setIncludeParentsOfMatches(bool on)
{
	if (mIncludeParents == on) {
		return;
	}
	mIncludeParents = on;
	invalidateFilter();
}

/**
 * @brief 是否放行命中子节点的父节点
 * @return true = 放行
 */
bool DAStringFilterProxyModel::includeParentsOfMatches() const
{
	return mIncludeParents;
}

/**
 * @brief 过滤判定
 * @param source_row 源行号
 * @param source_parent 源父索引
 * @return 是否保留该行
 */
bool DAStringFilterProxyModel::matchesText(const QString& text, const QString& filter, Qt::CaseSensitivity cs)
{
	if (filter.isEmpty()) {
		return true;
	}
	return text.contains(filter, cs);
}

/**
 * @brief 纯文本的命中区间
 * @param text 文本
 * @param filter 过滤词；空串返回空数组（与 matchesText 返回 true 不对称）
 * @param cs 大小写
 * @return (start, length) 列表，UTF-16 下标、从左到右、互不重叠
 */
QVector< QPair< int, int > > DAStringFilterProxyModel::matchRangesIn(const QString& text,
																	 const QString& filter,
																	 Qt::CaseSensitivity cs)
{
	QVector< QPair< int, int > > out;
	if (filter.isEmpty())
		return out;
	int from = 0;
	int pos = 0;
	while ((pos = text.indexOf(filter, from, cs)) >= 0) {
		out.append(qMakePair(pos, static_cast< int >(filter.size())));
		from = pos + filter.size();
	}
	return out;
}

/**
 * @brief 单个代理单元格的命中区间
 * @param proxyIndex 代理索引
 * @return (start, length) 列表；无效索引/不参与的列/空过滤返回空
 */
QVector< QPair< int, int > > DAStringFilterProxyModel::matchRanges(const QModelIndex& proxyIndex) const
{
	const QModelIndex src = mapToSource(proxyIndex);
	if (!src.isValid())
		return {};
	if (mFilterText.isEmpty())
		return {};
	if (mFilterColumn >= 0 && src.column() != mFilterColumn)
		return {};
	return matchRangesIn(src.data(mFilterRole).toString(), mFilterText, filterCaseSensitivity());
}

bool DAStringFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
	if (mFilterText.isEmpty()) {
		return true;
	}
	if (rowMatches(source_row, source_parent)) {
		return true;
	}
	if (!mIncludeParents) {
		return false;
	}
	// 子节点命中则放行父节点（递归，避免只检查一层）
	const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
	if (!index.isValid()) {
		return false;
	}
	const int childCount = sourceModel()->rowCount(index);
	for (int i = 0; i < childCount; ++i) {
		if (filterAcceptsRow(i, index)) {
			return true;
		}
	}
	return false;
}

/**
 * @brief 单行自身是否命中（不含子节点）
 * @param source_row 源行号
 * @param source_parent 源父索引
 * @return 是否命中
 */
bool DAStringFilterProxyModel::rowMatches(int source_row, const QModelIndex& source_parent) const
{
	if (mFilterColumn >= 0) {
		const QModelIndex index = sourceModel()->index(source_row, mFilterColumn, source_parent);
		const QString text = index.isValid() ? index.data(mFilterRole).toString() : QString();
		return matchesText(text, mFilterText, filterCaseSensitivity());
	}
	const int columns = sourceModel()->columnCount(source_parent);
	for (int c = 0; c < columns; ++c) {
		const QModelIndex index = sourceModel()->index(source_row, c, source_parent);
		if (index.isValid() && matchesText(index.data(mFilterRole).toString(), mFilterText, filterCaseSensitivity())) {
			return true;
		}
	}
	return false;
}

}  // namespace DA

/*** End of inlined file: DAStringFilterProxyModel.cpp ***/

// ---- Qt RCC resources (generated by tools/Amalgamate.ps1) ----
// Qt 6 在 <QtCore/qtsymbolmacros.h>（由 qglobal.h 引入）中定义这两个宏；
// Qt 5 没有该头文件，但 Qt5 自带的 rcc 输出末尾会补一份 fallback 定义。
// 这里再补一次 fallback，使用 Qt 6 生成的资源代码也能在 Qt 5 下编译。
#ifndef QT_RCC_MANGLE_NAMESPACE
#  define QT_RCC_MANGLE_NAMESPACE(name) name
#  define QT_RCC_PREPEND_NAMESPACE(name) name
#endif

/*** Start of inlined file: qrc_dawidgetsicons.cpp ***/
static const unsigned char qt_resource_data[] = {
  // src/resources/text-underline.svg
  0x0,0x0,0x2,0x47,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x73,0x74,0x61,0x6e,0x64,0x61,0x6c,0x6f,0x6e,0x65,0x3d,0x22,0x6e,
  0x6f,0x22,0x3f,0x3e,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x73,0x76,
  0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,
  0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,0x2f,0x2f,0x45,
  0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,
  0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,0x73,0x2f,0x53,
  0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,0x67,0x31,0x31,
  0x2e,0x64,0x74,0x64,0x22,0x3e,0x3c,0x73,0x76,0x67,0x20,0x74,0x3d,0x22,0x31,0x36,
  0x37,0x30,0x34,0x32,0x33,0x31,0x32,0x30,0x39,0x39,0x34,0x22,0x20,0x63,0x6c,0x61,
  0x73,0x73,0x3d,0x22,0x69,0x63,0x6f,0x6e,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x31,0x30,0x32,0x34,0x20,0x31,0x30,0x32,0x34,
  0x22,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,
  0x78,0x6d,0x6c,0x6e,0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,
  0x67,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x34,0x37,0x38,0x36,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x77,0x69,0x64,0x74,0x68,0x3d,
  0x22,0x36,0x34,0x22,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x36,0x34,0x22,
  0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x32,0x35,0x36,0x20,0x36,
  0x34,0x68,0x31,0x32,0x38,0x76,0x34,0x34,0x38,0x48,0x32,0x35,0x36,0x7a,0x4d,0x36,
  0x34,0x30,0x20,0x36,0x34,0x68,0x31,0x32,0x38,0x76,0x34,0x34,0x38,0x48,0x36,0x34,
  0x30,0x7a,0x4d,0x35,0x31,0x32,0x20,0x37,0x36,0x38,0x63,0x2d,0x36,0x38,0x2e,0x34,
  0x20,0x30,0x2d,0x31,0x33,0x32,0x2e,0x37,0x2d,0x32,0x36,0x2e,0x36,0x2d,0x31,0x38,
  0x31,0x2d,0x37,0x35,0x2d,0x34,0x38,0x2e,0x34,0x2d,0x34,0x38,0x2e,0x34,0x2d,0x37,
  0x35,0x2d,0x31,0x31,0x32,0x2e,0x36,0x2d,0x37,0x35,0x2d,0x31,0x38,0x31,0x68,0x31,
  0x32,0x38,0x63,0x30,0x20,0x37,0x30,0x2e,0x36,0x20,0x35,0x37,0x2e,0x34,0x20,0x31,
  0x32,0x38,0x20,0x31,0x32,0x38,0x20,0x31,0x32,0x38,0x73,0x31,0x32,0x38,0x2d,0x35,
  0x37,0x2e,0x34,0x20,0x31,0x32,0x38,0x2d,0x31,0x32,0x38,0x68,0x31,0x32,0x38,0x63,
  0x30,0x20,0x36,0x38,0x2e,0x34,0x2d,0x32,0x36,0x2e,0x36,0x20,0x31,0x33,0x32,0x2e,
  0x37,0x2d,0x37,0x35,0x20,0x31,0x38,0x31,0x73,0x2d,0x31,0x31,0x32,0x2e,0x36,0x20,
  0x37,0x35,0x2d,0x31,0x38,0x31,0x20,0x37,0x35,0x7a,0x4d,0x36,0x34,0x20,0x38,0x39,
  0x36,0x68,0x38,0x39,0x36,0x76,0x36,0x34,0x48,0x36,0x34,0x7a,0x22,0x20,0x66,0x69,
  0x6c,0x6c,0x3d,0x22,0x23,0x37,0x32,0x37,0x32,0x37,0x32,0x22,0x20,0x70,0x2d,0x69,
  0x64,0x3d,0x22,0x34,0x37,0x38,0x37,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,0x68,0x3e,
  0x3c,0x2f,0x73,0x76,0x67,0x3e,
	// src/resources/bottomRight.svg
  0x0,0x0,0x2,0x46,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x65,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,0x3d,0x22,0x75,0x74,0x66,
  0x2d,0x38,0x22,0x3f,0x3e,0xd,0xa,0x3c,0x21,0x2d,0x2d,0x20,0x47,0x65,0x6e,0x65,
  0x72,0x61,0x74,0x6f,0x72,0x3a,0x20,0x41,0x64,0x6f,0x62,0x65,0x20,0x49,0x6c,0x6c,
  0x75,0x73,0x74,0x72,0x61,0x74,0x6f,0x72,0x20,0x31,0x36,0x2e,0x30,0x2e,0x30,0x2c,
  0x20,0x53,0x56,0x47,0x20,0x45,0x78,0x70,0x6f,0x72,0x74,0x20,0x50,0x6c,0x75,0x67,
  0x2d,0x49,0x6e,0x20,0x2e,0x20,0x53,0x56,0x47,0x20,0x56,0x65,0x72,0x73,0x69,0x6f,
  0x6e,0x3a,0x20,0x36,0x2e,0x30,0x30,0x20,0x42,0x75,0x69,0x6c,0x64,0x20,0x30,0x29,
  0x20,0x20,0x2d,0x2d,0x3e,0xd,0xa,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,
  0x20,0x73,0x76,0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,
  0x57,0x33,0x43,0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,
  0x2f,0x2f,0x45,0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,
  0x73,0x2f,0x53,0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,
  0x67,0x31,0x31,0x2e,0x64,0x74,0x64,0x22,0x3e,0xd,0xa,0x3c,0x73,0x76,0x67,0x20,
  0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,0x69,0x64,
  0x3d,0x22,0xe5,0x9b,0xbe,0xe5,0xb1,0x82,0x5f,0x31,0x22,0x20,0x78,0x6d,0x6c,0x6e,
  0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,
  0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,0x67,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x78,0x3d,0x22,0x30,0x70,0x78,
  0x22,0x20,0x79,0x3d,0x22,0x30,0x70,0x78,0x22,0xd,0xa,0x9,0x20,0x77,0x69,0x64,
  0x74,0x68,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x68,0x65,0x69,0x67,0x68,
  0x74,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,0x30,0x30,0x22,0x20,
  0x65,0x6e,0x61,0x62,0x6c,0x65,0x2d,0x62,0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,
  0x64,0x3d,0x22,0x6e,0x65,0x77,0x20,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,
  0x30,0x30,0x22,0x20,0x78,0x6d,0x6c,0x3a,0x73,0x70,0x61,0x63,0x65,0x3d,0x22,0x70,
  0x72,0x65,0x73,0x65,0x72,0x76,0x65,0x22,0x3e,0xd,0xa,0x3c,0x70,0x61,0x74,0x68,
  0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x35,0x32,0x38,0x30,0x43,0x31,0x22,0x20,
  0x64,0x3d,0x22,0x4d,0x31,0x31,0x2e,0x31,0x31,0x31,0x2c,0x31,0x38,0x38,0x2e,0x38,
  0x38,0x39,0x76,0x2d,0x36,0x32,0x2e,0x37,0x35,0x36,0x68,0x31,0x31,0x35,0x2e,0x30,
  0x32,0x32,0x6c,0x2d,0x30,0x2e,0x30,0x30,0x31,0x2d,0x31,0x31,0x35,0x2e,0x30,0x32,
  0x32,0x68,0x36,0x32,0x2e,0x37,0x35,0x37,0x76,0x31,0x37,0x37,0x2e,0x37,0x37,0x38,
  0x48,0x31,0x31,0x2e,0x31,0x31,0x31,0x7a,0x22,0x2f,0x3e,0xd,0xa,0x3c,0x2f,0x73,
  0x76,0x67,0x3e,0xd,0xa,
	// src/resources/topleft.svg
  0x0,0x0,0x2,0x3d,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x65,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,0x3d,0x22,0x75,0x74,0x66,
  0x2d,0x38,0x22,0x3f,0x3e,0xd,0xa,0x3c,0x21,0x2d,0x2d,0x20,0x47,0x65,0x6e,0x65,
  0x72,0x61,0x74,0x6f,0x72,0x3a,0x20,0x41,0x64,0x6f,0x62,0x65,0x20,0x49,0x6c,0x6c,
  0x75,0x73,0x74,0x72,0x61,0x74,0x6f,0x72,0x20,0x31,0x36,0x2e,0x30,0x2e,0x30,0x2c,
  0x20,0x53,0x56,0x47,0x20,0x45,0x78,0x70,0x6f,0x72,0x74,0x20,0x50,0x6c,0x75,0x67,
  0x2d,0x49,0x6e,0x20,0x2e,0x20,0x53,0x56,0x47,0x20,0x56,0x65,0x72,0x73,0x69,0x6f,
  0x6e,0x3a,0x20,0x36,0x2e,0x30,0x30,0x20,0x42,0x75,0x69,0x6c,0x64,0x20,0x30,0x29,
  0x20,0x20,0x2d,0x2d,0x3e,0xd,0xa,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,
  0x20,0x73,0x76,0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,
  0x57,0x33,0x43,0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,
  0x2f,0x2f,0x45,0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,
  0x73,0x2f,0x53,0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,
  0x67,0x31,0x31,0x2e,0x64,0x74,0x64,0x22,0x3e,0xd,0xa,0x3c,0x73,0x76,0x67,0x20,
  0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,0x69,0x64,
  0x3d,0x22,0xe5,0x9b,0xbe,0xe5,0xb1,0x82,0x5f,0x31,0x22,0x20,0x78,0x6d,0x6c,0x6e,
  0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,
  0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,0x67,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x78,0x3d,0x22,0x30,0x70,0x78,
  0x22,0x20,0x79,0x3d,0x22,0x30,0x70,0x78,0x22,0xd,0xa,0x9,0x20,0x77,0x69,0x64,
  0x74,0x68,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x68,0x65,0x69,0x67,0x68,
  0x74,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,0x30,0x30,0x22,0x20,
  0x65,0x6e,0x61,0x62,0x6c,0x65,0x2d,0x62,0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,
  0x64,0x3d,0x22,0x6e,0x65,0x77,0x20,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,
  0x30,0x30,0x22,0x20,0x78,0x6d,0x6c,0x3a,0x73,0x70,0x61,0x63,0x65,0x3d,0x22,0x70,
  0x72,0x65,0x73,0x65,0x72,0x76,0x65,0x22,0x3e,0xd,0xa,0x3c,0x70,0x61,0x74,0x68,
  0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x35,0x32,0x38,0x30,0x43,0x31,0x22,0x20,
  0x64,0x3d,0x22,0x4d,0x31,0x38,0x38,0x2e,0x38,0x38,0x39,0x2c,0x31,0x31,0x2e,0x31,
  0x31,0x31,0x76,0x36,0x32,0x2e,0x37,0x35,0x35,0x48,0x37,0x33,0x2e,0x38,0x36,0x37,
  0x76,0x31,0x31,0x35,0x2e,0x30,0x32,0x32,0x48,0x31,0x31,0x2e,0x31,0x31,0x31,0x56,
  0x31,0x31,0x2e,0x31,0x31,0x31,0x48,0x31,0x38,0x38,0x2e,0x38,0x38,0x39,0x7a,0x22,
  0x2f,0x3e,0xd,0xa,0x3c,0x2f,0x73,0x76,0x67,0x3e,0xd,0xa,
	// src/resources/error.svg
  0x0,0x0,0x5,0xe4,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x73,0x74,0x61,0x6e,0x64,0x61,0x6c,0x6f,0x6e,0x65,0x3d,0x22,0x6e,
  0x6f,0x22,0x3f,0x3e,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x73,0x76,
  0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,
  0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,0x2f,0x2f,0x45,
  0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,
  0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,0x73,0x2f,0x53,
  0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,0x67,0x31,0x31,
  0x2e,0x64,0x74,0x64,0x22,0x3e,0x3c,0x73,0x76,0x67,0x20,0x74,0x3d,0x22,0x31,0x37,
  0x38,0x32,0x36,0x33,0x38,0x31,0x33,0x35,0x37,0x33,0x32,0x22,0x20,0x63,0x6c,0x61,
  0x73,0x73,0x3d,0x22,0x69,0x63,0x6f,0x6e,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x31,0x30,0x32,0x34,0x20,0x31,0x30,0x32,0x34,
  0x22,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,
  0x78,0x6d,0x6c,0x6e,0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,
  0x67,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x37,0x32,0x31,0x33,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x77,0x69,0x64,0x74,0x68,0x3d,
  0x22,0x32,0x30,0x30,0x22,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x32,0x30,
  0x30,0x22,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x38,0x38,0x36,
  0x2e,0x37,0x38,0x34,0x20,0x37,0x34,0x36,0x2e,0x34,0x39,0x36,0x71,0x32,0x39,0x2e,
  0x36,0x39,0x36,0x20,0x33,0x30,0x2e,0x37,0x32,0x20,0x34,0x33,0x2e,0x35,0x32,0x20,
  0x35,0x36,0x2e,0x33,0x32,0x74,0x2d,0x34,0x2e,0x36,0x30,0x38,0x20,0x35,0x38,0x2e,
  0x33,0x36,0x38,0x71,0x2d,0x34,0x2e,0x30,0x39,0x36,0x20,0x36,0x2e,0x31,0x34,0x34,
  0x2d,0x31,0x31,0x2e,0x32,0x36,0x34,0x20,0x31,0x34,0x2e,0x38,0x34,0x38,0x74,0x2d,
  0x31,0x34,0x2e,0x38,0x34,0x38,0x20,0x31,0x36,0x2e,0x38,0x39,0x36,0x2d,0x31,0x35,
  0x2e,0x33,0x36,0x20,0x31,0x34,0x2e,0x38,0x34,0x38,0x2d,0x31,0x32,0x2e,0x38,0x20,
  0x39,0x2e,0x37,0x32,0x38,0x71,0x2d,0x32,0x35,0x2e,0x36,0x20,0x31,0x35,0x2e,0x33,
  0x36,0x2d,0x36,0x30,0x2e,0x34,0x31,0x36,0x20,0x38,0x2e,0x31,0x39,0x32,0x74,0x2d,
  0x36,0x32,0x2e,0x34,0x36,0x34,0x2d,0x33,0x34,0x2e,0x38,0x31,0x36,0x6c,0x2d,0x34,
  0x33,0x2e,0x30,0x30,0x38,0x2d,0x34,0x33,0x2e,0x30,0x30,0x38,0x2d,0x35,0x37,0x2e,
  0x33,0x34,0x34,0x2d,0x35,0x37,0x2e,0x33,0x34,0x34,0x2d,0x36,0x37,0x2e,0x35,0x38,
  0x34,0x2d,0x36,0x37,0x2e,0x35,0x38,0x34,0x2d,0x37,0x33,0x2e,0x37,0x32,0x38,0x2d,
  0x37,0x33,0x2e,0x37,0x32,0x38,0x2d,0x31,0x33,0x31,0x2e,0x30,0x37,0x32,0x20,0x31,
  0x33,0x31,0x2e,0x30,0x37,0x32,0x71,0x2d,0x36,0x30,0x2e,0x34,0x31,0x36,0x20,0x36,
  0x30,0x2e,0x34,0x31,0x36,0x2d,0x39,0x38,0x2e,0x33,0x30,0x34,0x20,0x39,0x39,0x2e,
  0x33,0x32,0x38,0x2d,0x33,0x38,0x2e,0x39,0x31,0x32,0x20,0x33,0x38,0x2e,0x39,0x31,
  0x32,0x2d,0x37,0x37,0x2e,0x33,0x31,0x32,0x20,0x34,0x38,0x2e,0x31,0x32,0x38,0x74,
  0x2d,0x36,0x38,0x2e,0x30,0x39,0x36,0x2d,0x31,0x37,0x2e,0x34,0x30,0x38,0x6c,0x2d,
  0x37,0x2e,0x31,0x36,0x38,0x2d,0x37,0x2e,0x31,0x36,0x38,0x2d,0x31,0x31,0x2e,0x32,
  0x36,0x34,0x2d,0x31,0x31,0x2e,0x32,0x36,0x34,0x2d,0x31,0x31,0x2e,0x32,0x36,0x34,
  0x2d,0x31,0x31,0x2e,0x32,0x36,0x34,0x71,0x2d,0x36,0x2e,0x31,0x34,0x34,0x2d,0x36,
  0x2e,0x31,0x34,0x34,0x2d,0x37,0x2e,0x31,0x36,0x38,0x2d,0x38,0x2e,0x31,0x39,0x32,
  0x2d,0x31,0x31,0x2e,0x32,0x36,0x34,0x2d,0x31,0x34,0x2e,0x33,0x33,0x36,0x2d,0x31,
  0x33,0x2e,0x33,0x31,0x32,0x2d,0x32,0x39,0x2e,0x31,0x38,0x34,0x74,0x32,0x2e,0x35,
  0x36,0x2d,0x32,0x39,0x2e,0x31,0x38,0x34,0x20,0x31,0x33,0x2e,0x38,0x32,0x34,0x2d,
  0x32,0x37,0x2e,0x36,0x34,0x38,0x20,0x32,0x30,0x2e,0x34,0x38,0x2d,0x32,0x34,0x2e,
  0x35,0x37,0x36,0x71,0x39,0x2e,0x32,0x31,0x36,0x2d,0x38,0x2e,0x31,0x39,0x32,0x20,
  0x33,0x32,0x2e,0x37,0x36,0x38,0x2d,0x33,0x30,0x2e,0x37,0x32,0x6c,0x35,0x35,0x2e,
  0x32,0x39,0x36,0x2d,0x35,0x37,0x2e,0x33,0x34,0x34,0x71,0x33,0x33,0x2e,0x37,0x39,
  0x32,0x2d,0x33,0x32,0x2e,0x37,0x36,0x38,0x20,0x37,0x35,0x2e,0x32,0x36,0x34,0x2d,
  0x37,0x33,0x2e,0x37,0x32,0x38,0x74,0x38,0x36,0x2e,0x35,0x32,0x38,0x2d,0x38,0x36,
  0x2e,0x30,0x31,0x36,0x71,0x2d,0x34,0x39,0x2e,0x31,0x35,0x32,0x2d,0x34,0x39,0x2e,
  0x31,0x35,0x32,0x2d,0x39,0x33,0x2e,0x36,0x39,0x36,0x2d,0x39,0x33,0x2e,0x31,0x38,
  0x34,0x74,0x2d,0x37,0x39,0x2e,0x38,0x37,0x32,0x2d,0x37,0x38,0x2e,0x38,0x34,0x38,
  0x2d,0x35,0x37,0x2e,0x38,0x35,0x36,0x2d,0x35,0x36,0x2e,0x38,0x33,0x32,0x2d,0x32,
  0x37,0x2e,0x36,0x34,0x38,0x2d,0x32,0x37,0x2e,0x31,0x33,0x36,0x71,0x2d,0x32,0x36,
  0x2e,0x36,0x32,0x34,0x2d,0x32,0x36,0x2e,0x36,0x32,0x34,0x2d,0x32,0x37,0x2e,0x31,
  0x33,0x36,0x2d,0x35,0x32,0x2e,0x37,0x33,0x36,0x74,0x31,0x37,0x2e,0x39,0x32,0x2d,
  0x35,0x32,0x2e,0x37,0x33,0x36,0x71,0x38,0x2e,0x31,0x39,0x32,0x2d,0x31,0x30,0x2e,
  0x32,0x34,0x20,0x32,0x33,0x2e,0x35,0x35,0x32,0x2d,0x32,0x34,0x2e,0x30,0x36,0x34,
  0x74,0x32,0x31,0x2e,0x35,0x30,0x34,0x2d,0x31,0x37,0x2e,0x39,0x32,0x71,0x33,0x30,
  0x2e,0x37,0x32,0x2d,0x32,0x30,0x2e,0x34,0x38,0x20,0x35,0x35,0x2e,0x32,0x39,0x36,
  0x2d,0x31,0x37,0x2e,0x39,0x32,0x74,0x34,0x39,0x2e,0x31,0x35,0x32,0x20,0x32,0x38,
  0x2e,0x31,0x36,0x6c,0x33,0x31,0x2e,0x37,0x34,0x34,0x20,0x33,0x31,0x2e,0x37,0x34,
  0x34,0x71,0x32,0x33,0x2e,0x35,0x35,0x32,0x20,0x32,0x33,0x2e,0x35,0x35,0x32,0x20,
  0x35,0x38,0x2e,0x33,0x36,0x38,0x20,0x35,0x37,0x2e,0x33,0x34,0x34,0x74,0x37,0x38,
  0x2e,0x33,0x33,0x36,0x20,0x37,0x36,0x2e,0x32,0x38,0x38,0x20,0x39,0x30,0x2e,0x36,
  0x32,0x34,0x20,0x38,0x38,0x2e,0x35,0x37,0x36,0x71,0x33,0x38,0x2e,0x39,0x31,0x32,
  0x2d,0x33,0x38,0x2e,0x39,0x31,0x32,0x20,0x37,0x36,0x2e,0x32,0x38,0x38,0x2d,0x37,
  0x35,0x2e,0x37,0x37,0x36,0x74,0x36,0x39,0x2e,0x36,0x33,0x32,0x2d,0x36,0x39,0x2e,
  0x31,0x32,0x20,0x35,0x38,0x2e,0x33,0x36,0x38,0x2d,0x35,0x37,0x2e,0x38,0x35,0x36,
  0x20,0x34,0x33,0x2e,0x35,0x32,0x2d,0x34,0x33,0x2e,0x30,0x30,0x38,0x71,0x32,0x34,
  0x2e,0x35,0x37,0x36,0x2d,0x32,0x33,0x2e,0x35,0x35,0x32,0x20,0x35,0x33,0x2e,0x32,
  0x34,0x38,0x2d,0x33,0x31,0x2e,0x32,0x33,0x32,0x74,0x35,0x35,0x2e,0x32,0x39,0x36,
  0x20,0x31,0x32,0x2e,0x38,0x71,0x31,0x2e,0x30,0x32,0x34,0x20,0x31,0x2e,0x30,0x32,
  0x34,0x20,0x36,0x2e,0x36,0x35,0x36,0x20,0x35,0x2e,0x31,0x32,0x74,0x31,0x31,0x2e,
  0x32,0x36,0x34,0x20,0x39,0x2e,0x32,0x31,0x36,0x20,0x31,0x30,0x2e,0x37,0x35,0x32,
  0x20,0x39,0x2e,0x37,0x32,0x38,0x20,0x37,0x2e,0x31,0x36,0x38,0x20,0x35,0x2e,0x36,
  0x33,0x32,0x71,0x32,0x37,0x2e,0x36,0x34,0x38,0x20,0x32,0x36,0x2e,0x36,0x32,0x34,
  0x20,0x32,0x37,0x2e,0x31,0x33,0x36,0x20,0x35,0x37,0x2e,0x38,0x35,0x36,0x74,0x2d,
  0x32,0x37,0x2e,0x31,0x33,0x36,0x20,0x35,0x37,0x2e,0x38,0x35,0x36,0x71,0x2d,0x31,
  0x38,0x2e,0x34,0x33,0x32,0x20,0x31,0x38,0x2e,0x34,0x33,0x32,0x2d,0x34,0x35,0x2e,
  0x35,0x36,0x38,0x20,0x34,0x36,0x2e,0x30,0x38,0x74,0x2d,0x36,0x30,0x2e,0x34,0x31,
  0x36,0x20,0x36,0x30,0x2e,0x34,0x31,0x36,0x2d,0x37,0x30,0x2e,0x31,0x34,0x34,0x20,
  0x36,0x39,0x2e,0x36,0x33,0x32,0x6c,0x2d,0x37,0x37,0x2e,0x38,0x32,0x34,0x20,0x37,
  0x37,0x2e,0x38,0x32,0x34,0x71,0x33,0x37,0x2e,0x38,0x38,0x38,0x20,0x33,0x36,0x2e,
  0x38,0x36,0x34,0x20,0x37,0x34,0x2e,0x32,0x34,0x20,0x37,0x32,0x2e,0x31,0x39,0x32,
  0x74,0x36,0x37,0x2e,0x35,0x38,0x34,0x20,0x36,0x36,0x2e,0x30,0x34,0x38,0x20,0x35,
  0x36,0x2e,0x33,0x32,0x20,0x35,0x36,0x2e,0x33,0x32,0x20,0x34,0x31,0x2e,0x34,0x37,
  0x32,0x20,0x34,0x31,0x2e,0x39,0x38,0x34,0x7a,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,
  0x22,0x37,0x32,0x31,0x34,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x35,0x31,
  0x35,0x31,0x35,0x31,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,0x68,0x3e,0x3c,0x2f,0x73,
  0x76,0x67,0x3e,
	// src/resources/AligmentRight.svg
  0x0,0x0,0x2,0xc,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x73,0x74,0x61,0x6e,0x64,0x61,0x6c,0x6f,0x6e,0x65,0x3d,0x22,0x6e,
  0x6f,0x22,0x3f,0x3e,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x73,0x76,
  0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,
  0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,0x2f,0x2f,0x45,
  0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,
  0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,0x73,0x2f,0x53,
  0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,0x67,0x31,0x31,
  0x2e,0x64,0x74,0x64,0x22,0x3e,0x3c,0x73,0x76,0x67,0x20,0x74,0x3d,0x22,0x31,0x37,
  0x31,0x33,0x32,0x38,0x30,0x35,0x39,0x32,0x31,0x33,0x37,0x22,0x20,0x63,0x6c,0x61,
  0x73,0x73,0x3d,0x22,0x69,0x63,0x6f,0x6e,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x31,0x30,0x32,0x34,0x20,0x31,0x30,0x32,0x34,
  0x22,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,
  0x78,0x6d,0x6c,0x6e,0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,
  0x67,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x31,0x36,0x31,0x37,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x77,0x69,0x64,0x74,0x68,0x3d,
  0x22,0x32,0x30,0x30,0x22,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x32,0x30,
  0x30,0x22,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x38,0x39,0x36,
  0x20,0x36,0x34,0x68,0x36,0x34,0x76,0x38,0x39,0x36,0x68,0x2d,0x36,0x34,0x7a,0x22,
  0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x37,0x32,0x37,0x32,0x37,0x32,0x22,0x20,
  0x70,0x2d,0x69,0x64,0x3d,0x22,0x31,0x36,0x31,0x38,0x22,0x3e,0x3c,0x2f,0x70,0x61,
  0x74,0x68,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x33,0x32,0x30,
  0x20,0x31,0x39,0x32,0x68,0x35,0x31,0x32,0x76,0x32,0x35,0x36,0x48,0x33,0x32,0x30,
  0x7a,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x42,0x33,0x42,0x33,0x42,0x33,
  0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x31,0x36,0x31,0x39,0x22,0x3e,0x3c,0x2f,
  0x70,0x61,0x74,0x68,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x36,
  0x34,0x20,0x35,0x37,0x36,0x68,0x37,0x36,0x38,0x76,0x32,0x35,0x36,0x48,0x36,0x34,
  0x7a,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x34,0x39,0x37,0x43,0x41,0x44,
  0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x31,0x36,0x32,0x30,0x22,0x3e,0x3c,0x2f,
  0x70,0x61,0x74,0x68,0x3e,0x3c,0x2f,0x73,0x76,0x67,0x3e,
	// src/resources/text-bold.svg
  0x0,0x0,0x3,0x43,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x73,0x74,0x61,0x6e,0x64,0x61,0x6c,0x6f,0x6e,0x65,0x3d,0x22,0x6e,
  0x6f,0x22,0x3f,0x3e,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x73,0x76,
  0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,
  0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,0x2f,0x2f,0x45,
  0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,
  0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,0x73,0x2f,0x53,
  0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,0x67,0x31,0x31,
  0x2e,0x64,0x74,0x64,0x22,0x3e,0x3c,0x73,0x76,0x67,0x20,0x74,0x3d,0x22,0x31,0x36,
  0x37,0x30,0x34,0x32,0x33,0x31,0x30,0x39,0x39,0x33,0x33,0x22,0x20,0x63,0x6c,0x61,
  0x73,0x73,0x3d,0x22,0x69,0x63,0x6f,0x6e,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x31,0x30,0x32,0x34,0x20,0x31,0x30,0x32,0x34,
  0x22,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,
  0x78,0x6d,0x6c,0x6e,0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,
  0x67,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x34,0x36,0x33,0x32,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x77,0x69,0x64,0x74,0x68,0x3d,
  0x22,0x36,0x34,0x22,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x36,0x34,0x22,
  0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x32,0x38,0x33,0x20,0x38,
  0x37,0x35,0x56,0x31,0x34,0x39,0x68,0x32,0x31,0x33,0x2e,0x31,0x63,0x36,0x33,0x2e,
  0x37,0x20,0x30,0x20,0x31,0x31,0x34,0x2e,0x36,0x20,0x31,0x35,0x2e,0x32,0x20,0x31,
  0x35,0x32,0x2e,0x37,0x20,0x34,0x35,0x2e,0x37,0x20,0x33,0x38,0x2e,0x31,0x20,0x33,
  0x30,0x2e,0x35,0x20,0x35,0x37,0x2e,0x31,0x20,0x37,0x30,0x2e,0x39,0x20,0x35,0x37,
  0x2e,0x31,0x20,0x31,0x32,0x31,0x2e,0x32,0x20,0x30,0x20,0x34,0x30,0x2d,0x31,0x31,
  0x2e,0x33,0x20,0x37,0x35,0x2e,0x33,0x2d,0x33,0x34,0x20,0x31,0x30,0x35,0x2e,0x39,
  0x2d,0x32,0x32,0x2e,0x36,0x20,0x33,0x30,0x2e,0x36,0x2d,0x35,0x33,0x2e,0x38,0x20,
  0x35,0x32,0x2e,0x33,0x2d,0x39,0x33,0x2e,0x34,0x20,0x36,0x35,0x2e,0x31,0x76,0x31,
  0x2e,0x39,0x63,0x34,0x39,0x2e,0x33,0x20,0x35,0x2e,0x36,0x20,0x38,0x38,0x2e,0x37,
  0x20,0x32,0x34,0x2e,0x33,0x20,0x31,0x31,0x38,0x2e,0x32,0x20,0x35,0x36,0x53,0x37,
  0x34,0x31,0x20,0x36,0x31,0x36,0x2e,0x37,0x20,0x37,0x34,0x31,0x20,0x36,0x36,0x35,
  0x2e,0x35,0x63,0x30,0x20,0x36,0x32,0x2e,0x35,0x2d,0x32,0x32,0x2e,0x34,0x20,0x31,
  0x31,0x33,0x2d,0x36,0x37,0x2e,0x32,0x20,0x31,0x35,0x31,0x2e,0x36,0x43,0x36,0x32,
  0x39,0x20,0x38,0x35,0x35,0x2e,0x37,0x20,0x35,0x37,0x31,0x2e,0x36,0x20,0x38,0x37,
  0x35,0x20,0x35,0x30,0x31,0x2e,0x37,0x20,0x38,0x37,0x35,0x48,0x32,0x38,0x33,0x7a,
  0x20,0x6d,0x39,0x33,0x2e,0x37,0x2d,0x36,0x34,0x34,0x76,0x32,0x32,0x37,0x2e,0x33,
  0x68,0x38,0x35,0x2e,0x32,0x63,0x34,0x35,0x2e,0x33,0x20,0x30,0x20,0x38,0x30,0x2e,
  0x38,0x2d,0x31,0x31,0x20,0x31,0x30,0x36,0x2e,0x35,0x2d,0x33,0x33,0x20,0x32,0x35,
  0x2e,0x38,0x2d,0x32,0x32,0x20,0x33,0x38,0x2e,0x36,0x2d,0x35,0x32,0x2e,0x36,0x20,
  0x33,0x38,0x2e,0x36,0x2d,0x39,0x31,0x2e,0x36,0x20,0x30,0x2d,0x36,0x38,0x2e,0x34,
  0x2d,0x34,0x35,0x2e,0x36,0x2d,0x31,0x30,0x32,0x2e,0x36,0x2d,0x31,0x33,0x36,0x2e,
  0x37,0x2d,0x31,0x30,0x32,0x2e,0x36,0x68,0x2d,0x39,0x33,0x2e,0x36,0x7a,0x20,0x6d,
  0x30,0x20,0x33,0x30,0x38,0x2e,0x39,0x56,0x37,0x39,0x33,0x68,0x31,0x31,0x32,0x2e,
  0x34,0x63,0x34,0x39,0x20,0x30,0x20,0x38,0x36,0x2e,0x39,0x2d,0x31,0x31,0x2e,0x33,
  0x20,0x31,0x31,0x33,0x2e,0x36,0x2d,0x33,0x34,0x20,0x32,0x36,0x2e,0x37,0x2d,0x32,
  0x32,0x2e,0x37,0x20,0x34,0x30,0x2d,0x35,0x34,0x2e,0x31,0x20,0x34,0x30,0x2d,0x39,
  0x34,0x2e,0x34,0x20,0x30,0x2d,0x38,0x33,0x2e,0x31,0x2d,0x35,0x36,0x2e,0x34,0x2d,
  0x31,0x32,0x34,0x2e,0x37,0x2d,0x31,0x36,0x39,0x2e,0x31,0x2d,0x31,0x32,0x34,0x2e,
  0x37,0x68,0x2d,0x39,0x36,0x2e,0x39,0x7a,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,
  0x23,0x37,0x32,0x37,0x32,0x37,0x32,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x34,
  0x36,0x33,0x33,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,0x68,0x3e,0x3c,0x2f,0x73,0x76,
  0x67,0x3e,
	// src/resources/AligmentCenter.svg
  0x0,0x0,0x2,0xd,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x73,0x74,0x61,0x6e,0x64,0x61,0x6c,0x6f,0x6e,0x65,0x3d,0x22,0x6e,
  0x6f,0x22,0x3f,0x3e,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x73,0x76,
  0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,
  0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,0x2f,0x2f,0x45,
  0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,
  0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,0x73,0x2f,0x53,
  0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,0x67,0x31,0x31,
  0x2e,0x64,0x74,0x64,0x22,0x3e,0x3c,0x73,0x76,0x67,0x20,0x74,0x3d,0x22,0x31,0x37,
  0x31,0x33,0x32,0x38,0x30,0x36,0x32,0x30,0x38,0x36,0x33,0x22,0x20,0x63,0x6c,0x61,
  0x73,0x73,0x3d,0x22,0x69,0x63,0x6f,0x6e,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x31,0x30,0x32,0x34,0x20,0x31,0x30,0x32,0x34,
  0x22,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,
  0x78,0x6d,0x6c,0x6e,0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,
  0x67,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x32,0x30,0x38,0x32,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x77,0x69,0x64,0x74,0x68,0x3d,
  0x22,0x32,0x30,0x30,0x22,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x32,0x30,
  0x30,0x22,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x36,0x34,0x20,
  0x34,0x38,0x30,0x68,0x38,0x39,0x36,0x76,0x36,0x34,0x48,0x36,0x34,0x7a,0x22,0x20,
  0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x37,0x32,0x37,0x32,0x37,0x32,0x22,0x20,0x70,
  0x2d,0x69,0x64,0x3d,0x22,0x32,0x30,0x38,0x33,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,
  0x68,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x38,0x33,0x32,0x20,
  0x32,0x35,0x36,0x76,0x35,0x31,0x32,0x48,0x35,0x37,0x36,0x56,0x32,0x35,0x36,0x7a,
  0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x42,0x33,0x42,0x33,0x42,0x33,0x22,
  0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x32,0x30,0x38,0x34,0x22,0x3e,0x3c,0x2f,0x70,
  0x61,0x74,0x68,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x34,0x34,
  0x38,0x20,0x31,0x32,0x38,0x76,0x37,0x36,0x38,0x48,0x31,0x39,0x32,0x56,0x31,0x32,
  0x38,0x7a,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x34,0x39,0x37,0x43,0x41,
  0x44,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x32,0x30,0x38,0x35,0x22,0x3e,0x3c,
  0x2f,0x70,0x61,0x74,0x68,0x3e,0x3c,0x2f,0x73,0x76,0x67,0x3e,
	// src/resources/right.svg
  0x0,0x0,0x2,0x56,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x65,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,0x3d,0x22,0x75,0x74,0x66,
  0x2d,0x38,0x22,0x3f,0x3e,0xd,0xa,0x3c,0x21,0x2d,0x2d,0x20,0x47,0x65,0x6e,0x65,
  0x72,0x61,0x74,0x6f,0x72,0x3a,0x20,0x41,0x64,0x6f,0x62,0x65,0x20,0x49,0x6c,0x6c,
  0x75,0x73,0x74,0x72,0x61,0x74,0x6f,0x72,0x20,0x31,0x36,0x2e,0x30,0x2e,0x30,0x2c,
  0x20,0x53,0x56,0x47,0x20,0x45,0x78,0x70,0x6f,0x72,0x74,0x20,0x50,0x6c,0x75,0x67,
  0x2d,0x49,0x6e,0x20,0x2e,0x20,0x53,0x56,0x47,0x20,0x56,0x65,0x72,0x73,0x69,0x6f,
  0x6e,0x3a,0x20,0x36,0x2e,0x30,0x30,0x20,0x42,0x75,0x69,0x6c,0x64,0x20,0x30,0x29,
  0x20,0x20,0x2d,0x2d,0x3e,0xd,0xa,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,
  0x20,0x73,0x76,0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,
  0x57,0x33,0x43,0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,
  0x2f,0x2f,0x45,0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,
  0x73,0x2f,0x53,0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,
  0x67,0x31,0x31,0x2e,0x64,0x74,0x64,0x22,0x3e,0xd,0xa,0x3c,0x73,0x76,0x67,0x20,
  0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,0x69,0x64,
  0x3d,0x22,0xe5,0x9b,0xbe,0xe5,0xb1,0x82,0x5f,0x31,0x22,0x20,0x78,0x6d,0x6c,0x6e,
  0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,
  0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,0x67,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x78,0x3d,0x22,0x30,0x70,0x78,
  0x22,0x20,0x79,0x3d,0x22,0x30,0x70,0x78,0x22,0xd,0xa,0x9,0x20,0x77,0x69,0x64,
  0x74,0x68,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x68,0x65,0x69,0x67,0x68,
  0x74,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,0x30,0x30,0x22,0x20,
  0x65,0x6e,0x61,0x62,0x6c,0x65,0x2d,0x62,0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,
  0x64,0x3d,0x22,0x6e,0x65,0x77,0x20,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,
  0x30,0x30,0x22,0x20,0x78,0x6d,0x6c,0x3a,0x73,0x70,0x61,0x63,0x65,0x3d,0x22,0x70,
  0x72,0x65,0x73,0x65,0x72,0x76,0x65,0x22,0x3e,0xd,0xa,0x3c,0x70,0x6f,0x6c,0x79,
  0x67,0x6f,0x6e,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x35,0x32,0x38,0x30,0x43,
  0x31,0x22,0x20,0x70,0x6f,0x69,0x6e,0x74,0x73,0x3d,0x22,0x31,0x32,0x36,0x2e,0x31,
  0x33,0x33,0x2c,0x31,0x31,0x2e,0x31,0x31,0x31,0x20,0x31,0x32,0x36,0x2e,0x31,0x33,
  0x33,0x2c,0x37,0x33,0x2e,0x38,0x36,0x37,0x20,0x31,0x32,0x36,0x2e,0x31,0x33,0x33,
  0x2c,0x31,0x38,0x38,0x2e,0x38,0x38,0x39,0x20,0x31,0x38,0x38,0x2e,0x38,0x38,0x39,
  0x2c,0x31,0x38,0x38,0x2e,0x38,0x38,0x39,0x20,0x31,0x38,0x38,0x2e,0x38,0x38,0x39,
  0x2c,0x31,0x31,0x2e,0x31,0x31,0x31,0x20,0x22,0x2f,0x3e,0xd,0xa,0x3c,0x2f,0x73,
  0x76,0x67,0x3e,0xd,0xa,
	// src/resources/center.svg
  0x0,0x0,0x2,0xc5,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x65,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,0x3d,0x22,0x75,0x74,0x66,
  0x2d,0x38,0x22,0x3f,0x3e,0xd,0xa,0x3c,0x21,0x2d,0x2d,0x20,0x47,0x65,0x6e,0x65,
  0x72,0x61,0x74,0x6f,0x72,0x3a,0x20,0x41,0x64,0x6f,0x62,0x65,0x20,0x49,0x6c,0x6c,
  0x75,0x73,0x74,0x72,0x61,0x74,0x6f,0x72,0x20,0x31,0x36,0x2e,0x30,0x2e,0x30,0x2c,
  0x20,0x53,0x56,0x47,0x20,0x45,0x78,0x70,0x6f,0x72,0x74,0x20,0x50,0x6c,0x75,0x67,
  0x2d,0x49,0x6e,0x20,0x2e,0x20,0x53,0x56,0x47,0x20,0x56,0x65,0x72,0x73,0x69,0x6f,
  0x6e,0x3a,0x20,0x36,0x2e,0x30,0x30,0x20,0x42,0x75,0x69,0x6c,0x64,0x20,0x30,0x29,
  0x20,0x20,0x2d,0x2d,0x3e,0xd,0xa,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,
  0x20,0x73,0x76,0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,
  0x57,0x33,0x43,0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,
  0x2f,0x2f,0x45,0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,
  0x73,0x2f,0x53,0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,
  0x67,0x31,0x31,0x2e,0x64,0x74,0x64,0x22,0x3e,0xd,0xa,0x3c,0x73,0x76,0x67,0x20,
  0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,0x69,0x64,
  0x3d,0x22,0xe5,0x9b,0xbe,0xe5,0xb1,0x82,0x5f,0x31,0x22,0x20,0x78,0x6d,0x6c,0x6e,
  0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,
  0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,0x67,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x78,0x3d,0x22,0x30,0x70,0x78,
  0x22,0x20,0x79,0x3d,0x22,0x30,0x70,0x78,0x22,0xd,0xa,0x9,0x20,0x77,0x69,0x64,
  0x74,0x68,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x68,0x65,0x69,0x67,0x68,
  0x74,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,0x30,0x30,0x22,0x20,
  0x65,0x6e,0x61,0x62,0x6c,0x65,0x2d,0x62,0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,
  0x64,0x3d,0x22,0x6e,0x65,0x77,0x20,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,
  0x30,0x30,0x22,0x20,0x78,0x6d,0x6c,0x3a,0x73,0x70,0x61,0x63,0x65,0x3d,0x22,0x70,
  0x72,0x65,0x73,0x65,0x72,0x76,0x65,0x22,0x3e,0xd,0xa,0x3c,0x70,0x6f,0x6c,0x79,
  0x67,0x6f,0x6e,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x35,0x32,0x38,0x30,0x43,
  0x31,0x22,0x20,0x70,0x6f,0x69,0x6e,0x74,0x73,0x3d,0x22,0x36,0x38,0x2e,0x36,0x32,
  0x32,0x2c,0x31,0x31,0x2e,0x31,0x31,0x31,0x20,0x36,0x38,0x2e,0x36,0x32,0x32,0x2c,
  0x31,0x38,0x38,0x2e,0x38,0x38,0x39,0x20,0x31,0x33,0x31,0x2e,0x33,0x37,0x38,0x2c,
  0x31,0x38,0x38,0x2e,0x38,0x38,0x39,0x20,0x31,0x33,0x31,0x2e,0x33,0x37,0x38,0x2c,
  0x37,0x33,0x2e,0x38,0x36,0x36,0x20,0x31,0x33,0x31,0x2e,0x33,0x37,0x38,0x2c,0x31,
  0x31,0x2e,0x31,0x31,0x31,0x20,0x22,0x2f,0x3e,0xd,0xa,0x3c,0x70,0x6f,0x6c,0x79,
  0x67,0x6f,0x6e,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x35,0x32,0x38,0x30,0x43,
  0x31,0x22,0x20,0x70,0x6f,0x69,0x6e,0x74,0x73,0x3d,0x22,0x31,0x38,0x38,0x2e,0x38,
  0x38,0x39,0x2c,0x37,0x32,0x2e,0x31,0x32,0x32,0x20,0x31,0x31,0x2e,0x31,0x31,0x31,
  0x2c,0x37,0x32,0x2e,0x31,0x32,0x32,0x20,0x31,0x31,0x2e,0x31,0x31,0x31,0x2c,0x31,
  0x33,0x34,0x2e,0x38,0x37,0x38,0x20,0x31,0x32,0x36,0x2e,0x31,0x33,0x34,0x2c,0x31,
  0x33,0x34,0x2e,0x38,0x37,0x38,0x20,0x31,0x38,0x38,0x2e,0x38,0x38,0x39,0x2c,0x31,
  0x33,0x34,0x2e,0x38,0x37,0x38,0x20,0x22,0x2f,0x3e,0xd,0xa,0x3c,0x2f,0x73,0x76,
  0x67,0x3e,0xd,0xa,
	// src/resources/bottomLeft.svg
  0x0,0x0,0x2,0x42,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x65,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,0x3d,0x22,0x75,0x74,0x66,
  0x2d,0x38,0x22,0x3f,0x3e,0xd,0xa,0x3c,0x21,0x2d,0x2d,0x20,0x47,0x65,0x6e,0x65,
  0x72,0x61,0x74,0x6f,0x72,0x3a,0x20,0x41,0x64,0x6f,0x62,0x65,0x20,0x49,0x6c,0x6c,
  0x75,0x73,0x74,0x72,0x61,0x74,0x6f,0x72,0x20,0x31,0x36,0x2e,0x30,0x2e,0x30,0x2c,
  0x20,0x53,0x56,0x47,0x20,0x45,0x78,0x70,0x6f,0x72,0x74,0x20,0x50,0x6c,0x75,0x67,
  0x2d,0x49,0x6e,0x20,0x2e,0x20,0x53,0x56,0x47,0x20,0x56,0x65,0x72,0x73,0x69,0x6f,
  0x6e,0x3a,0x20,0x36,0x2e,0x30,0x30,0x20,0x42,0x75,0x69,0x6c,0x64,0x20,0x30,0x29,
  0x20,0x20,0x2d,0x2d,0x3e,0xd,0xa,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,
  0x20,0x73,0x76,0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,
  0x57,0x33,0x43,0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,
  0x2f,0x2f,0x45,0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,
  0x73,0x2f,0x53,0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,
  0x67,0x31,0x31,0x2e,0x64,0x74,0x64,0x22,0x3e,0xd,0xa,0x3c,0x73,0x76,0x67,0x20,
  0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,0x69,0x64,
  0x3d,0x22,0xe5,0x9b,0xbe,0xe5,0xb1,0x82,0x5f,0x31,0x22,0x20,0x78,0x6d,0x6c,0x6e,
  0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,
  0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,0x67,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x78,0x3d,0x22,0x30,0x70,0x78,
  0x22,0x20,0x79,0x3d,0x22,0x30,0x70,0x78,0x22,0xd,0xa,0x9,0x20,0x77,0x69,0x64,
  0x74,0x68,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x68,0x65,0x69,0x67,0x68,
  0x74,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,0x30,0x30,0x22,0x20,
  0x65,0x6e,0x61,0x62,0x6c,0x65,0x2d,0x62,0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,
  0x64,0x3d,0x22,0x6e,0x65,0x77,0x20,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,
  0x30,0x30,0x22,0x20,0x78,0x6d,0x6c,0x3a,0x73,0x70,0x61,0x63,0x65,0x3d,0x22,0x70,
  0x72,0x65,0x73,0x65,0x72,0x76,0x65,0x22,0x3e,0xd,0xa,0x3c,0x70,0x61,0x74,0x68,
  0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x35,0x32,0x38,0x30,0x43,0x31,0x22,0x20,
  0x64,0x3d,0x22,0x4d,0x31,0x31,0x2e,0x31,0x31,0x31,0x2c,0x31,0x31,0x2e,0x31,0x31,
  0x31,0x68,0x36,0x32,0x2e,0x37,0x35,0x35,0x76,0x31,0x31,0x35,0x2e,0x30,0x32,0x32,
  0x6c,0x31,0x31,0x35,0x2e,0x30,0x32,0x32,0x2d,0x30,0x2e,0x30,0x30,0x31,0x76,0x36,
  0x32,0x2e,0x37,0x35,0x37,0x48,0x31,0x31,0x2e,0x31,0x31,0x31,0x56,0x31,0x31,0x2e,
  0x31,0x31,0x31,0x7a,0x22,0x2f,0x3e,0xd,0xa,0x3c,0x2f,0x73,0x76,0x67,0x3e,0xd,
  0xa,
	// src/resources/arrow-up.svg
  0x0,0x0,0x2,0xec,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x73,0x74,0x61,0x6e,0x64,0x61,0x6c,0x6f,0x6e,0x65,0x3d,0x22,0x6e,
  0x6f,0x22,0x3f,0x3e,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x73,0x76,
  0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,
  0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,0x2f,0x2f,0x45,
  0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,
  0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,0x73,0x2f,0x53,
  0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,0x67,0x31,0x31,
  0x2e,0x64,0x74,0x64,0x22,0x3e,0x3c,0x73,0x76,0x67,0x20,0x74,0x3d,0x22,0x31,0x37,
  0x38,0x32,0x36,0x33,0x38,0x31,0x38,0x30,0x36,0x35,0x39,0x22,0x20,0x63,0x6c,0x61,
  0x73,0x73,0x3d,0x22,0x69,0x63,0x6f,0x6e,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x31,0x30,0x32,0x34,0x20,0x31,0x30,0x32,0x34,
  0x22,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,
  0x78,0x6d,0x6c,0x6e,0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,
  0x67,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x37,0x34,0x33,0x34,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x77,0x69,0x64,0x74,0x68,0x3d,
  0x22,0x32,0x30,0x30,0x22,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x32,0x30,
  0x30,0x22,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x38,0x35,0x34,
  0x2e,0x30,0x31,0x36,0x20,0x37,0x33,0x39,0x2e,0x33,0x32,0x38,0x6c,0x2d,0x33,0x31,
  0x33,0x2e,0x33,0x34,0x34,0x2d,0x33,0x30,0x39,0x2e,0x32,0x34,0x38,0x2d,0x33,0x31,
  0x33,0x2e,0x33,0x34,0x34,0x20,0x33,0x30,0x39,0x2e,0x32,0x34,0x38,0x71,0x2d,0x31,
  0x34,0x2e,0x33,0x33,0x36,0x20,0x31,0x34,0x2e,0x33,0x33,0x36,0x2d,0x33,0x32,0x2e,
  0x37,0x36,0x38,0x20,0x32,0x31,0x2e,0x35,0x30,0x34,0x74,0x2d,0x33,0x37,0x2e,0x33,
  0x37,0x36,0x20,0x37,0x2e,0x31,0x36,0x38,0x2d,0x33,0x36,0x2e,0x38,0x36,0x34,0x2d,
  0x37,0x2e,0x31,0x36,0x38,0x2d,0x33,0x32,0x2e,0x32,0x35,0x36,0x2d,0x32,0x31,0x2e,
  0x35,0x30,0x34,0x71,0x2d,0x32,0x39,0x2e,0x36,0x39,0x36,0x2d,0x32,0x38,0x2e,0x36,
  0x37,0x32,0x2d,0x32,0x39,0x2e,0x36,0x39,0x36,0x2d,0x36,0x38,0x2e,0x36,0x30,0x38,
  0x74,0x32,0x39,0x2e,0x36,0x39,0x36,0x2d,0x36,0x38,0x2e,0x36,0x30,0x38,0x6c,0x33,
  0x37,0x36,0x2e,0x38,0x33,0x32,0x2d,0x33,0x37,0x33,0x2e,0x37,0x36,0x71,0x31,0x34,
  0x2e,0x33,0x33,0x36,0x2d,0x31,0x34,0x2e,0x33,0x33,0x36,0x20,0x33,0x34,0x2e,0x33,
  0x30,0x34,0x2d,0x32,0x32,0x2e,0x35,0x32,0x38,0x74,0x34,0x30,0x2e,0x34,0x34,0x38,
  0x2d,0x39,0x2e,0x32,0x31,0x36,0x20,0x33,0x39,0x2e,0x34,0x32,0x34,0x20,0x35,0x2e,
  0x31,0x32,0x20,0x33,0x31,0x2e,0x32,0x33,0x32,0x20,0x32,0x30,0x2e,0x34,0x38,0x6c,
  0x33,0x38,0x32,0x2e,0x39,0x37,0x36,0x20,0x33,0x37,0x39,0x2e,0x39,0x30,0x34,0x71,
  0x32,0x38,0x2e,0x36,0x37,0x32,0x20,0x32,0x38,0x2e,0x36,0x37,0x32,0x20,0x32,0x38,
  0x2e,0x36,0x37,0x32,0x20,0x36,0x38,0x2e,0x36,0x30,0x38,0x74,0x2d,0x32,0x38,0x2e,
  0x36,0x37,0x32,0x20,0x36,0x38,0x2e,0x36,0x30,0x38,0x71,0x2d,0x31,0x34,0x2e,0x33,
  0x33,0x36,0x20,0x31,0x34,0x2e,0x33,0x33,0x36,0x2d,0x33,0x32,0x2e,0x37,0x36,0x38,
  0x20,0x32,0x31,0x2e,0x35,0x30,0x34,0x74,0x2d,0x33,0x37,0x2e,0x33,0x37,0x36,0x20,
  0x37,0x2e,0x31,0x36,0x38,0x2d,0x33,0x36,0x2e,0x38,0x36,0x34,0x2d,0x37,0x2e,0x31,
  0x36,0x38,0x2d,0x33,0x32,0x2e,0x32,0x35,0x36,0x2d,0x32,0x31,0x2e,0x35,0x30,0x34,
  0x7a,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x37,0x34,0x33,0x35,0x22,0x20,0x66,
  0x69,0x6c,0x6c,0x3d,0x22,0x23,0x35,0x31,0x35,0x31,0x35,0x31,0x22,0x3e,0x3c,0x2f,
  0x70,0x61,0x74,0x68,0x3e,0x3c,0x2f,0x73,0x76,0x67,0x3e,
	// src/resources/AligmentHCenter.svg
  0x0,0x0,0x1,0xce,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x73,0x74,0x61,0x6e,0x64,0x61,0x6c,0x6f,0x6e,0x65,0x3d,0x22,0x6e,
  0x6f,0x22,0x3f,0x3e,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x73,0x76,
  0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,
  0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,0x2f,0x2f,0x45,
  0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,
  0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,0x73,0x2f,0x53,
  0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,0x67,0x31,0x31,
  0x2e,0x64,0x74,0x64,0x22,0x3e,0x3c,0x73,0x76,0x67,0x20,0x63,0x6c,0x61,0x73,0x73,
  0x3d,0x22,0x69,0x63,0x6f,0x6e,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,0x78,0x3d,
  0x22,0x30,0x20,0x30,0x20,0x31,0x30,0x32,0x34,0x20,0x31,0x30,0x32,0x34,0x22,0x20,
  0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,0x78,0x6d,
  0x6c,0x6e,0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,
  0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,0x67,0x22,
  0x20,0x78,0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,
  0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,
  0x31,0x39,0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x77,0x69,0x64,0x74,
  0x68,0x3d,0x22,0x32,0x30,0x30,0x22,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3d,0x22,
  0x32,0x30,0x30,0x22,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x34,
  0x38,0x30,0x20,0x36,0x34,0x68,0x36,0x34,0x76,0x38,0x39,0x36,0x68,0x2d,0x36,0x34,
  0x7a,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x37,0x32,0x37,0x32,0x37,0x32,
  0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,0x68,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,
  0x3d,0x22,0x4d,0x32,0x35,0x36,0x20,0x31,0x39,0x32,0x68,0x35,0x31,0x32,0x76,0x32,
  0x35,0x36,0x48,0x32,0x35,0x36,0x7a,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,
  0x42,0x33,0x42,0x33,0x42,0x33,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,0x68,0x3e,0x3c,
  0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x31,0x32,0x38,0x20,0x35,0x37,0x36,
  0x68,0x37,0x36,0x38,0x76,0x32,0x35,0x36,0x48,0x31,0x32,0x38,0x7a,0x22,0x20,0x66,
  0x69,0x6c,0x6c,0x3d,0x22,0x23,0x34,0x39,0x37,0x43,0x41,0x44,0x22,0x3e,0x3c,0x2f,
  0x70,0x61,0x74,0x68,0x3e,0x3c,0x2f,0x73,0x76,0x67,0x3e,0xd,0xa,
	// src/resources/left.svg
  0x0,0x0,0x2,0x51,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x65,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,0x3d,0x22,0x75,0x74,0x66,
  0x2d,0x38,0x22,0x3f,0x3e,0xd,0xa,0x3c,0x21,0x2d,0x2d,0x20,0x47,0x65,0x6e,0x65,
  0x72,0x61,0x74,0x6f,0x72,0x3a,0x20,0x41,0x64,0x6f,0x62,0x65,0x20,0x49,0x6c,0x6c,
  0x75,0x73,0x74,0x72,0x61,0x74,0x6f,0x72,0x20,0x31,0x36,0x2e,0x30,0x2e,0x30,0x2c,
  0x20,0x53,0x56,0x47,0x20,0x45,0x78,0x70,0x6f,0x72,0x74,0x20,0x50,0x6c,0x75,0x67,
  0x2d,0x49,0x6e,0x20,0x2e,0x20,0x53,0x56,0x47,0x20,0x56,0x65,0x72,0x73,0x69,0x6f,
  0x6e,0x3a,0x20,0x36,0x2e,0x30,0x30,0x20,0x42,0x75,0x69,0x6c,0x64,0x20,0x30,0x29,
  0x20,0x20,0x2d,0x2d,0x3e,0xd,0xa,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,
  0x20,0x73,0x76,0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,
  0x57,0x33,0x43,0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,
  0x2f,0x2f,0x45,0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,
  0x73,0x2f,0x53,0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,
  0x67,0x31,0x31,0x2e,0x64,0x74,0x64,0x22,0x3e,0xd,0xa,0x3c,0x73,0x76,0x67,0x20,
  0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,0x69,0x64,
  0x3d,0x22,0xe5,0x9b,0xbe,0xe5,0xb1,0x82,0x5f,0x31,0x22,0x20,0x78,0x6d,0x6c,0x6e,
  0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,
  0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,0x67,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x78,0x3d,0x22,0x30,0x70,0x78,
  0x22,0x20,0x79,0x3d,0x22,0x30,0x70,0x78,0x22,0xd,0xa,0x9,0x20,0x77,0x69,0x64,
  0x74,0x68,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x68,0x65,0x69,0x67,0x68,
  0x74,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,0x30,0x30,0x22,0x20,
  0x65,0x6e,0x61,0x62,0x6c,0x65,0x2d,0x62,0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,
  0x64,0x3d,0x22,0x6e,0x65,0x77,0x20,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,
  0x30,0x30,0x22,0x20,0x78,0x6d,0x6c,0x3a,0x73,0x70,0x61,0x63,0x65,0x3d,0x22,0x70,
  0x72,0x65,0x73,0x65,0x72,0x76,0x65,0x22,0x3e,0xd,0xa,0x3c,0x70,0x6f,0x6c,0x79,
  0x67,0x6f,0x6e,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x35,0x32,0x38,0x30,0x43,
  0x31,0x22,0x20,0x70,0x6f,0x69,0x6e,0x74,0x73,0x3d,0x22,0x31,0x31,0x2e,0x31,0x31,
  0x31,0x2c,0x31,0x31,0x2e,0x31,0x31,0x31,0x20,0x31,0x31,0x2e,0x31,0x31,0x31,0x2c,
  0x31,0x38,0x38,0x2e,0x38,0x38,0x39,0x20,0x37,0x33,0x2e,0x38,0x36,0x37,0x2c,0x31,
  0x38,0x38,0x2e,0x38,0x38,0x39,0x20,0x37,0x33,0x2e,0x38,0x36,0x37,0x2c,0x37,0x33,
  0x2e,0x38,0x36,0x36,0x20,0x37,0x33,0x2e,0x38,0x36,0x37,0x2c,0x31,0x31,0x2e,0x31,
  0x31,0x31,0x20,0x22,0x2f,0x3e,0xd,0xa,0x3c,0x2f,0x73,0x76,0x67,0x3e,0xd,0xa,

	// src/resources/open.svg
  0x0,0x0,0x6,0x1e,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x73,0x74,0x61,0x6e,0x64,0x61,0x6c,0x6f,0x6e,0x65,0x3d,0x22,0x6e,
  0x6f,0x22,0x3f,0x3e,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x73,0x76,
  0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,
  0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,0x2f,0x2f,0x45,
  0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,
  0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,0x73,0x2f,0x53,
  0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,0x67,0x31,0x31,
  0x2e,0x64,0x74,0x64,0x22,0x3e,0x3c,0x73,0x76,0x67,0x20,0x74,0x3d,0x22,0x31,0x37,
  0x31,0x32,0x32,0x34,0x36,0x38,0x30,0x38,0x38,0x38,0x33,0x22,0x20,0x63,0x6c,0x61,
  0x73,0x73,0x3d,0x22,0x69,0x63,0x6f,0x6e,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x31,0x30,0x32,0x34,0x20,0x31,0x30,0x32,0x34,
  0x22,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,
  0x78,0x6d,0x6c,0x6e,0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,
  0x67,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x39,0x35,0x31,0x37,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x77,0x69,0x64,0x74,0x68,0x3d,
  0x22,0x32,0x30,0x30,0x22,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x32,0x30,
  0x30,0x22,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x38,0x36,0x33,
  0x2e,0x39,0x36,0x38,0x20,0x39,0x31,0x33,0x2e,0x32,0x33,0x32,0x61,0x34,0x2e,0x35,
  0x33,0x36,0x20,0x34,0x2e,0x35,0x33,0x36,0x20,0x30,0x20,0x30,0x20,0x31,0x2d,0x34,
  0x2e,0x35,0x34,0x34,0x20,0x34,0x2e,0x35,0x32,0x38,0x48,0x31,0x35,0x32,0x2e,0x39,
  0x38,0x34,0x61,0x34,0x2e,0x35,0x34,0x34,0x20,0x34,0x2e,0x35,0x34,0x34,0x20,0x30,
  0x20,0x30,0x20,0x31,0x2d,0x34,0x2e,0x35,0x35,0x32,0x2d,0x34,0x2e,0x35,0x32,0x38,
  0x56,0x33,0x36,0x32,0x2e,0x35,0x33,0x36,0x63,0x30,0x2d,0x32,0x2e,0x35,0x30,0x34,
  0x20,0x32,0x2e,0x30,0x34,0x2d,0x34,0x2e,0x35,0x33,0x36,0x20,0x34,0x2e,0x35,0x35,
  0x32,0x2d,0x34,0x2e,0x35,0x33,0x36,0x68,0x37,0x30,0x36,0x2e,0x34,0x34,0x63,0x32,
  0x2e,0x35,0x30,0x34,0x20,0x30,0x20,0x34,0x2e,0x35,0x34,0x34,0x20,0x32,0x2e,0x30,
  0x33,0x32,0x20,0x34,0x2e,0x35,0x34,0x34,0x20,0x34,0x2e,0x35,0x33,0x36,0x76,0x35,
  0x35,0x30,0x2e,0x36,0x39,0x36,0x7a,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,
  0x33,0x43,0x34,0x30,0x34,0x37,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x39,0x35,
  0x31,0x38,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,0x68,0x3e,0x3c,0x70,0x61,0x74,0x68,
  0x20,0x64,0x3d,0x22,0x4d,0x38,0x31,0x39,0x2e,0x32,0x35,0x36,0x20,0x33,0x32,0x31,
  0x2e,0x34,0x37,0x32,0x48,0x35,0x30,0x36,0x2e,0x31,0x39,0x32,0x76,0x2d,0x34,0x34,
  0x2e,0x37,0x32,0x61,0x34,0x34,0x2e,0x37,0x32,0x20,0x34,0x34,0x2e,0x37,0x32,0x20,
  0x30,0x20,0x30,0x20,0x30,0x2d,0x34,0x34,0x2e,0x37,0x32,0x2d,0x34,0x34,0x2e,0x37,
  0x32,0x68,0x2d,0x32,0x36,0x38,0x2e,0x33,0x32,0x61,0x34,0x34,0x2e,0x37,0x32,0x20,
  0x34,0x34,0x2e,0x37,0x32,0x20,0x30,0x20,0x30,0x20,0x30,0x2d,0x34,0x34,0x2e,0x37,
  0x32,0x20,0x34,0x34,0x2e,0x37,0x32,0x76,0x31,0x37,0x38,0x2e,0x38,0x38,0x38,0x61,
  0x34,0x34,0x2e,0x37,0x32,0x20,0x34,0x34,0x2e,0x37,0x32,0x20,0x30,0x20,0x30,0x20,
  0x31,0x20,0x34,0x34,0x2e,0x37,0x32,0x2d,0x34,0x34,0x2e,0x37,0x32,0x68,0x36,0x32,
  0x36,0x2e,0x31,0x30,0x34,0x61,0x34,0x34,0x2e,0x37,0x32,0x20,0x34,0x34,0x2e,0x37,
  0x32,0x20,0x30,0x20,0x30,0x20,0x31,0x20,0x34,0x34,0x2e,0x37,0x31,0x32,0x20,0x34,
  0x34,0x2e,0x37,0x32,0x56,0x33,0x36,0x36,0x2e,0x31,0x38,0x34,0x61,0x34,0x34,0x2e,
  0x37,0x32,0x20,0x34,0x34,0x2e,0x37,0x32,0x20,0x30,0x20,0x30,0x20,0x30,0x2d,0x34,
  0x34,0x2e,0x37,0x31,0x32,0x2d,0x34,0x34,0x2e,0x37,0x31,0x32,0x7a,0x22,0x20,0x66,
  0x69,0x6c,0x6c,0x3d,0x22,0x23,0x33,0x43,0x34,0x30,0x34,0x37,0x22,0x20,0x70,0x2d,
  0x69,0x64,0x3d,0x22,0x39,0x35,0x31,0x39,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,0x68,
  0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x32,0x35,0x31,0x2e,0x38,
  0x37,0x32,0x20,0x33,0x35,0x38,0x68,0x35,0x36,0x33,0x2e,0x34,0x37,0x32,0x76,0x34,
  0x38,0x33,0x2e,0x37,0x36,0x38,0x48,0x32,0x35,0x31,0x2e,0x38,0x37,0x32,0x56,0x33,
  0x35,0x38,0x7a,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x43,0x43,0x43,0x43,
  0x43,0x43,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x39,0x35,0x32,0x30,0x22,0x3e,
  0x3c,0x2f,0x70,0x61,0x74,0x68,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,
  0x4d,0x36,0x36,0x39,0x2e,0x38,0x38,0x20,0x31,0x32,0x35,0x2e,0x35,0x33,0x36,0x6c,
  0x31,0x30,0x2e,0x33,0x30,0x34,0x20,0x32,0x39,0x2e,0x38,0x36,0x34,0x63,0x34,0x33,
  0x2e,0x30,0x30,0x38,0x2d,0x31,0x35,0x2e,0x39,0x30,0x34,0x20,0x39,0x30,0x2e,0x30,
  0x31,0x36,0x2d,0x31,0x31,0x2e,0x38,0x39,0x36,0x20,0x31,0x33,0x32,0x2e,0x33,0x35,
  0x32,0x20,0x31,0x31,0x2e,0x33,0x31,0x32,0x20,0x34,0x32,0x2e,0x36,0x30,0x38,0x20,
  0x32,0x33,0x2e,0x33,0x35,0x32,0x20,0x37,0x35,0x2e,0x34,0x34,0x20,0x36,0x33,0x2e,
  0x33,0x32,0x20,0x39,0x32,0x2e,0x34,0x32,0x34,0x20,0x31,0x31,0x32,0x2e,0x35,0x33,
  0x36,0x6c,0x32,0x37,0x2e,0x38,0x34,0x38,0x2d,0x31,0x30,0x2e,0x33,0x30,0x34,0x63,
  0x2d,0x31,0x39,0x2e,0x36,0x34,0x2d,0x35,0x36,0x2e,0x38,0x38,0x38,0x2d,0x35,0x37,
  0x2e,0x36,0x38,0x38,0x2d,0x31,0x30,0x33,0x2e,0x31,0x33,0x36,0x2d,0x31,0x30,0x37,
  0x2e,0x31,0x33,0x36,0x2d,0x31,0x33,0x30,0x2e,0x32,0x34,0x38,0x2d,0x34,0x39,0x2e,
  0x37,0x32,0x38,0x2d,0x32,0x37,0x2e,0x32,0x33,0x32,0x2d,0x31,0x30,0x35,0x2e,0x30,
  0x35,0x36,0x2d,0x33,0x31,0x2e,0x39,0x32,0x2d,0x31,0x35,0x35,0x2e,0x37,0x39,0x32,
  0x2d,0x31,0x33,0x2e,0x31,0x36,0x7a,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,
  0x32,0x41,0x32,0x45,0x33,0x33,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x39,0x35,
  0x32,0x31,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,0x68,0x3e,0x3c,0x70,0x61,0x74,0x68,
  0x20,0x64,0x3d,0x22,0x4d,0x38,0x35,0x38,0x2e,0x33,0x34,0x34,0x20,0x32,0x35,0x39,
  0x2e,0x35,0x33,0x36,0x6c,0x38,0x37,0x2e,0x34,0x38,0x20,0x38,0x31,0x2e,0x31,0x39,
  0x32,0x20,0x32,0x30,0x2e,0x36,0x38,0x2d,0x31,0x31,0x39,0x2e,0x36,0x32,0x34,0x7a,
  0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x32,0x41,0x32,0x45,0x33,0x33,0x22,
  0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x39,0x35,0x32,0x32,0x22,0x3e,0x3c,0x2f,0x70,
  0x61,0x74,0x68,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x31,0x39,
  0x35,0x2e,0x35,0x34,0x34,0x20,0x34,0x30,0x37,0x2e,0x39,0x35,0x32,0x68,0x35,0x39,
  0x33,0x2e,0x38,0x30,0x38,0x76,0x34,0x37,0x30,0x2e,0x30,0x34,0x38,0x48,0x31,0x39,
  0x35,0x2e,0x35,0x34,0x34,0x56,0x34,0x30,0x37,0x2e,0x39,0x35,0x32,0x7a,0x22,0x20,
  0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x44,0x42,0x44,0x42,0x44,0x42,0x22,0x20,0x70,
  0x2d,0x69,0x64,0x3d,0x22,0x39,0x35,0x32,0x33,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,
  0x68,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x38,0x36,0x33,0x2e,
  0x39,0x36,0x38,0x20,0x39,0x30,0x32,0x2e,0x38,0x34,0x38,0x61,0x34,0x34,0x2e,0x37,
  0x32,0x38,0x20,0x34,0x34,0x2e,0x37,0x32,0x38,0x20,0x30,0x20,0x30,0x20,0x31,0x2d,
  0x34,0x34,0x2e,0x37,0x31,0x32,0x20,0x34,0x34,0x2e,0x37,0x31,0x32,0x48,0x31,0x39,
  0x33,0x2e,0x31,0x35,0x32,0x61,0x34,0x34,0x2e,0x37,0x32,0x20,0x34,0x34,0x2e,0x37,
  0x32,0x20,0x30,0x20,0x30,0x20,0x31,0x2d,0x34,0x34,0x2e,0x37,0x32,0x2d,0x34,0x34,
  0x2e,0x37,0x31,0x32,0x6c,0x31,0x32,0x38,0x2d,0x33,0x36,0x37,0x2e,0x32,0x31,0x36,
  0x61,0x34,0x34,0x2e,0x37,0x32,0x20,0x34,0x34,0x2e,0x37,0x32,0x20,0x30,0x20,0x30,
  0x20,0x31,0x20,0x34,0x34,0x2e,0x37,0x32,0x2d,0x34,0x34,0x2e,0x37,0x32,0x68,0x36,
  0x32,0x36,0x2e,0x31,0x30,0x34,0x61,0x34,0x34,0x2e,0x37,0x32,0x20,0x34,0x34,0x2e,
  0x37,0x32,0x20,0x30,0x20,0x30,0x20,0x31,0x20,0x34,0x34,0x2e,0x37,0x31,0x32,0x20,
  0x34,0x34,0x2e,0x37,0x32,0x6c,0x2d,0x31,0x32,0x38,0x20,0x33,0x36,0x37,0x2e,0x32,
  0x31,0x36,0x7a,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x45,0x35,0x43,0x42,
  0x35,0x45,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x39,0x35,0x32,0x34,0x22,0x3e,
  0x3c,0x2f,0x70,0x61,0x74,0x68,0x3e,0x3c,0x2f,0x73,0x76,0x67,0x3e,
	// src/resources/font-size-add.svg
  0x0,0x0,0x2,0x63,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x73,0x74,0x61,0x6e,0x64,0x61,0x6c,0x6f,0x6e,0x65,0x3d,0x22,0x6e,
  0x6f,0x22,0x3f,0x3e,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x73,0x76,
  0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,
  0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,0x2f,0x2f,0x45,
  0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,
  0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,0x73,0x2f,0x53,
  0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,0x67,0x31,0x31,
  0x2e,0x64,0x74,0x64,0x22,0x3e,0x3c,0x73,0x76,0x67,0x20,0x74,0x3d,0x22,0x31,0x36,
  0x37,0x30,0x34,0x32,0x33,0x30,0x31,0x30,0x36,0x34,0x36,0x22,0x20,0x63,0x6c,0x61,
  0x73,0x73,0x3d,0x22,0x69,0x63,0x6f,0x6e,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x31,0x30,0x32,0x34,0x20,0x31,0x30,0x32,0x34,
  0x22,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,
  0x78,0x6d,0x6c,0x6e,0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,
  0x67,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x34,0x34,0x37,0x37,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x77,0x69,0x64,0x74,0x68,0x3d,
  0x22,0x36,0x34,0x22,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x36,0x34,0x22,
  0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x37,0x31,0x38,0x20,0x39,
  0x36,0x30,0x48,0x36,0x31,0x34,0x2e,0x33,0x6c,0x2d,0x37,0x34,0x2e,0x31,0x2d,0x31,
  0x39,0x32,0x2e,0x37,0x48,0x32,0x33,0x37,0x2e,0x36,0x4c,0x31,0x36,0x37,0x2e,0x37,
  0x20,0x39,0x36,0x30,0x48,0x36,0x34,0x6c,0x32,0x37,0x36,0x2e,0x38,0x2d,0x37,0x30,
  0x34,0x68,0x31,0x30,0x30,0x2e,0x34,0x4c,0x37,0x31,0x38,0x20,0x39,0x36,0x30,0x7a,
  0x4d,0x35,0x31,0x31,0x2e,0x31,0x20,0x36,0x38,0x37,0x2e,0x38,0x4c,0x34,0x30,0x31,
  0x2e,0x33,0x20,0x33,0x39,0x35,0x2e,0x31,0x63,0x2d,0x33,0x2e,0x34,0x2d,0x39,0x2e,
  0x34,0x2d,0x37,0x2e,0x32,0x2d,0x32,0x35,0x2e,0x37,0x2d,0x31,0x31,0x2e,0x33,0x2d,
  0x34,0x39,0x2e,0x31,0x68,0x2d,0x32,0x2e,0x33,0x63,0x2d,0x33,0x2e,0x34,0x20,0x32,
  0x31,0x2e,0x32,0x2d,0x37,0x2e,0x34,0x20,0x33,0x37,0x2e,0x36,0x2d,0x31,0x31,0x2e,
  0x37,0x20,0x34,0x39,0x2e,0x31,0x4c,0x32,0x36,0x37,0x2e,0x31,0x20,0x36,0x38,0x37,
  0x2e,0x38,0x68,0x32,0x34,0x34,0x7a,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,
  0x37,0x32,0x37,0x32,0x37,0x32,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x34,0x34,
  0x37,0x38,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,0x68,0x3e,0x3c,0x70,0x61,0x74,0x68,
  0x20,0x64,0x3d,0x22,0x4d,0x38,0x30,0x30,0x20,0x36,0x34,0x4c,0x36,0x34,0x30,0x20,
  0x32,0x35,0x36,0x68,0x33,0x32,0x30,0x7a,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,
  0x23,0x34,0x39,0x37,0x43,0x41,0x44,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x34,
  0x34,0x37,0x39,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,0x68,0x3e,0x3c,0x2f,0x73,0x76,
  0x67,0x3e,
	// src/resources/bottom.svg
  0x0,0x0,0x2,0x56,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x65,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,0x3d,0x22,0x75,0x74,0x66,
  0x2d,0x38,0x22,0x3f,0x3e,0xd,0xa,0x3c,0x21,0x2d,0x2d,0x20,0x47,0x65,0x6e,0x65,
  0x72,0x61,0x74,0x6f,0x72,0x3a,0x20,0x41,0x64,0x6f,0x62,0x65,0x20,0x49,0x6c,0x6c,
  0x75,0x73,0x74,0x72,0x61,0x74,0x6f,0x72,0x20,0x31,0x36,0x2e,0x30,0x2e,0x30,0x2c,
  0x20,0x53,0x56,0x47,0x20,0x45,0x78,0x70,0x6f,0x72,0x74,0x20,0x50,0x6c,0x75,0x67,
  0x2d,0x49,0x6e,0x20,0x2e,0x20,0x53,0x56,0x47,0x20,0x56,0x65,0x72,0x73,0x69,0x6f,
  0x6e,0x3a,0x20,0x36,0x2e,0x30,0x30,0x20,0x42,0x75,0x69,0x6c,0x64,0x20,0x30,0x29,
  0x20,0x20,0x2d,0x2d,0x3e,0xd,0xa,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,
  0x20,0x73,0x76,0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,
  0x57,0x33,0x43,0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,
  0x2f,0x2f,0x45,0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,
  0x73,0x2f,0x53,0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,
  0x67,0x31,0x31,0x2e,0x64,0x74,0x64,0x22,0x3e,0xd,0xa,0x3c,0x73,0x76,0x67,0x20,
  0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,0x69,0x64,
  0x3d,0x22,0xe5,0x9b,0xbe,0xe5,0xb1,0x82,0x5f,0x31,0x22,0x20,0x78,0x6d,0x6c,0x6e,
  0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,
  0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,0x67,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x78,0x3d,0x22,0x30,0x70,0x78,
  0x22,0x20,0x79,0x3d,0x22,0x30,0x70,0x78,0x22,0xd,0xa,0x9,0x20,0x77,0x69,0x64,
  0x74,0x68,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x68,0x65,0x69,0x67,0x68,
  0x74,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,0x30,0x30,0x22,0x20,
  0x65,0x6e,0x61,0x62,0x6c,0x65,0x2d,0x62,0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,
  0x64,0x3d,0x22,0x6e,0x65,0x77,0x20,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,
  0x30,0x30,0x22,0x20,0x78,0x6d,0x6c,0x3a,0x73,0x70,0x61,0x63,0x65,0x3d,0x22,0x70,
  0x72,0x65,0x73,0x65,0x72,0x76,0x65,0x22,0x3e,0xd,0xa,0x3c,0x70,0x6f,0x6c,0x79,
  0x67,0x6f,0x6e,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x35,0x32,0x38,0x30,0x43,
  0x31,0x22,0x20,0x70,0x6f,0x69,0x6e,0x74,0x73,0x3d,0x22,0x37,0x33,0x2e,0x38,0x36,
  0x36,0x2c,0x31,0x32,0x36,0x2e,0x31,0x33,0x33,0x20,0x31,0x31,0x2e,0x31,0x31,0x31,
  0x2c,0x31,0x32,0x36,0x2e,0x31,0x33,0x33,0x20,0x31,0x31,0x2e,0x31,0x31,0x31,0x2c,
  0x31,0x38,0x38,0x2e,0x38,0x38,0x39,0x20,0x31,0x38,0x38,0x2e,0x38,0x38,0x39,0x2c,
  0x31,0x38,0x38,0x2e,0x38,0x38,0x39,0x20,0x31,0x38,0x38,0x2e,0x38,0x38,0x39,0x2c,
  0x31,0x32,0x36,0x2e,0x31,0x33,0x32,0x20,0x22,0x2f,0x3e,0xd,0xa,0x3c,0x2f,0x73,
  0x76,0x67,0x3e,0xd,0xa,
	// src/resources/AligmentBottom.svg
  0x0,0x0,0x2,0xc,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x73,0x74,0x61,0x6e,0x64,0x61,0x6c,0x6f,0x6e,0x65,0x3d,0x22,0x6e,
  0x6f,0x22,0x3f,0x3e,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x73,0x76,
  0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,
  0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,0x2f,0x2f,0x45,
  0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,
  0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,0x73,0x2f,0x53,
  0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,0x67,0x31,0x31,
  0x2e,0x64,0x74,0x64,0x22,0x3e,0x3c,0x73,0x76,0x67,0x20,0x74,0x3d,0x22,0x31,0x37,
  0x31,0x33,0x32,0x38,0x30,0x35,0x35,0x34,0x30,0x35,0x34,0x22,0x20,0x63,0x6c,0x61,
  0x73,0x73,0x3d,0x22,0x69,0x63,0x6f,0x6e,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x31,0x30,0x32,0x34,0x20,0x31,0x30,0x32,0x34,
  0x22,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,
  0x78,0x6d,0x6c,0x6e,0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,
  0x67,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x31,0x34,0x36,0x32,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x77,0x69,0x64,0x74,0x68,0x3d,
  0x22,0x32,0x30,0x30,0x22,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x32,0x30,
  0x30,0x22,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x36,0x34,0x20,
  0x38,0x39,0x36,0x68,0x38,0x39,0x36,0x76,0x36,0x34,0x48,0x36,0x34,0x7a,0x22,0x20,
  0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x37,0x32,0x37,0x32,0x37,0x32,0x22,0x20,0x70,
  0x2d,0x69,0x64,0x3d,0x22,0x31,0x34,0x36,0x33,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,
  0x68,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x31,0x39,0x32,0x20,
  0x38,0x33,0x32,0x56,0x36,0x34,0x68,0x32,0x35,0x36,0x76,0x37,0x36,0x38,0x7a,0x22,
  0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x34,0x39,0x37,0x43,0x41,0x44,0x22,0x20,
  0x70,0x2d,0x69,0x64,0x3d,0x22,0x31,0x34,0x36,0x34,0x22,0x3e,0x3c,0x2f,0x70,0x61,
  0x74,0x68,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x35,0x37,0x36,
  0x20,0x38,0x33,0x32,0x56,0x33,0x32,0x30,0x68,0x32,0x35,0x36,0x76,0x35,0x31,0x32,
  0x7a,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x42,0x33,0x42,0x33,0x42,0x33,
  0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x31,0x34,0x36,0x35,0x22,0x3e,0x3c,0x2f,
  0x70,0x61,0x74,0x68,0x3e,0x3c,0x2f,0x73,0x76,0x67,0x3e,
	// src/resources/AligmentLeft.svg
  0x0,0x0,0x2,0xc,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x73,0x74,0x61,0x6e,0x64,0x61,0x6c,0x6f,0x6e,0x65,0x3d,0x22,0x6e,
  0x6f,0x22,0x3f,0x3e,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x73,0x76,
  0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,
  0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,0x2f,0x2f,0x45,
  0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,
  0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,0x73,0x2f,0x53,
  0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,0x67,0x31,0x31,
  0x2e,0x64,0x74,0x64,0x22,0x3e,0x3c,0x73,0x76,0x67,0x20,0x74,0x3d,0x22,0x31,0x37,
  0x31,0x33,0x32,0x38,0x30,0x36,0x31,0x34,0x34,0x32,0x39,0x22,0x20,0x63,0x6c,0x61,
  0x73,0x73,0x3d,0x22,0x69,0x63,0x6f,0x6e,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x31,0x30,0x32,0x34,0x20,0x31,0x30,0x32,0x34,
  0x22,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,
  0x78,0x6d,0x6c,0x6e,0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,
  0x67,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x31,0x39,0x32,0x37,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x77,0x69,0x64,0x74,0x68,0x3d,
  0x22,0x32,0x30,0x30,0x22,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x32,0x30,
  0x30,0x22,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x36,0x34,0x20,
  0x36,0x34,0x68,0x36,0x34,0x76,0x38,0x39,0x36,0x48,0x36,0x34,0x7a,0x22,0x20,0x66,
  0x69,0x6c,0x6c,0x3d,0x22,0x23,0x37,0x32,0x37,0x32,0x37,0x32,0x22,0x20,0x70,0x2d,
  0x69,0x64,0x3d,0x22,0x31,0x39,0x32,0x38,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,0x68,
  0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x31,0x39,0x32,0x20,0x31,
  0x39,0x32,0x68,0x35,0x31,0x35,0x76,0x32,0x35,0x36,0x48,0x31,0x39,0x32,0x7a,0x22,
  0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x42,0x33,0x42,0x33,0x42,0x33,0x22,0x20,
  0x70,0x2d,0x69,0x64,0x3d,0x22,0x31,0x39,0x32,0x39,0x22,0x3e,0x3c,0x2f,0x70,0x61,
  0x74,0x68,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x31,0x39,0x32,
  0x20,0x35,0x37,0x36,0x68,0x37,0x36,0x38,0x76,0x32,0x35,0x36,0x48,0x31,0x39,0x32,
  0x7a,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x34,0x39,0x37,0x43,0x41,0x44,
  0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x31,0x39,0x33,0x30,0x22,0x3e,0x3c,0x2f,
  0x70,0x61,0x74,0x68,0x3e,0x3c,0x2f,0x73,0x76,0x67,0x3e,
	// src/resources/arrow-down.svg
  0x0,0x0,0x3,0xde,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x65,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,0x3d,0x22,0x75,0x74,0x66,
  0x2d,0x38,0x22,0x3f,0x3e,0xd,0xa,0x3c,0x21,0x2d,0x2d,0x20,0x47,0x65,0x6e,0x65,
  0x72,0x61,0x74,0x6f,0x72,0x3a,0x20,0x41,0x64,0x6f,0x62,0x65,0x20,0x49,0x6c,0x6c,
  0x75,0x73,0x74,0x72,0x61,0x74,0x6f,0x72,0x20,0x31,0x36,0x2e,0x30,0x2e,0x30,0x2c,
  0x20,0x53,0x56,0x47,0x20,0x45,0x78,0x70,0x6f,0x72,0x74,0x20,0x50,0x6c,0x75,0x67,
  0x2d,0x49,0x6e,0x20,0x2e,0x20,0x53,0x56,0x47,0x20,0x56,0x65,0x72,0x73,0x69,0x6f,
  0x6e,0x3a,0x20,0x36,0x2e,0x30,0x30,0x20,0x42,0x75,0x69,0x6c,0x64,0x20,0x30,0x29,
  0x20,0x20,0x2d,0x2d,0x3e,0xd,0xa,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,
  0x20,0x73,0x76,0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,
  0x57,0x33,0x43,0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,
  0x2f,0x2f,0x45,0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,
  0x73,0x2f,0x53,0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,
  0x67,0x31,0x31,0x2e,0x64,0x74,0x64,0x22,0x3e,0xd,0xa,0x3c,0x73,0x76,0x67,0x20,
  0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,0x69,0x64,
  0x3d,0x22,0xe5,0x9b,0xbe,0xe5,0xb1,0x82,0x5f,0x31,0x22,0x20,0x78,0x6d,0x6c,0x6e,
  0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,
  0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,0x67,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x78,0x3d,0x22,0x30,0x70,0x78,
  0x22,0x20,0x79,0x3d,0x22,0x30,0x70,0x78,0x22,0xd,0xa,0x9,0x20,0x77,0x69,0x64,
  0x74,0x68,0x3d,0x22,0x32,0x30,0x30,0x2e,0x33,0x39,0x31,0x70,0x78,0x22,0x20,0x68,
  0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x76,0x69,
  0x65,0x77,0x42,0x6f,0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x2e,0x33,
  0x39,0x31,0x20,0x32,0x30,0x30,0x22,0x20,0x65,0x6e,0x61,0x62,0x6c,0x65,0x2d,0x62,
  0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,0x64,0x3d,0x22,0x6e,0x65,0x77,0x20,0x30,
  0x20,0x30,0x20,0x32,0x30,0x30,0x2e,0x33,0x39,0x31,0x20,0x32,0x30,0x30,0x22,0x20,
  0x78,0x6d,0x6c,0x3a,0x73,0x70,0x61,0x63,0x65,0x3d,0x22,0x70,0x72,0x65,0x73,0x65,
  0x72,0x76,0x65,0x22,0x3e,0xd,0xa,0x3c,0x70,0x61,0x74,0x68,0x20,0x66,0x69,0x6c,
  0x6c,0x3d,0x22,0x23,0x35,0x31,0x35,0x31,0x35,0x31,0x22,0x20,0x64,0x3d,0x22,0x4d,
  0x31,0x36,0x31,0x2e,0x34,0x39,0x36,0x2c,0x34,0x39,0x2e,0x37,0x38,0x34,0x63,0x33,
  0x2e,0x37,0x33,0x32,0x2d,0x33,0x2e,0x37,0x33,0x33,0x2c,0x38,0x2e,0x32,0x33,0x32,
  0x2d,0x35,0x2e,0x36,0x2c,0x31,0x33,0x2e,0x35,0x2d,0x35,0x2e,0x36,0x63,0x35,0x2e,
  0x32,0x36,0x36,0x2c,0x30,0x2c,0x39,0x2e,0x37,0x36,0x36,0x2c,0x31,0x2e,0x38,0x36,
  0x36,0x2c,0x31,0x33,0x2e,0x35,0x2c,0x35,0x2e,0x36,0x63,0x33,0x2e,0x38,0x36,0x36,
  0x2c,0x33,0x2e,0x37,0x33,0x33,0x2c,0x35,0x2e,0x38,0x2c,0x38,0x2e,0x32,0x2c,0x35,
  0x2e,0x38,0x2c,0x31,0x33,0x2e,0x34,0xd,0xa,0x9,0x63,0x30,0x2c,0x35,0x2e,0x32,
  0x2d,0x31,0x2e,0x39,0x33,0x34,0x2c,0x39,0x2e,0x36,0x36,0x37,0x2d,0x35,0x2e,0x38,
  0x2c,0x31,0x33,0x2e,0x34,0x6c,0x2d,0x37,0x34,0x2e,0x38,0x30,0x31,0x2c,0x37,0x34,
  0x2e,0x34,0x63,0x2d,0x31,0x2e,0x36,0x2c,0x31,0x2e,0x38,0x36,0x37,0x2d,0x33,0x2e,
  0x36,0x2c,0x33,0x2e,0x31,0x36,0x36,0x2d,0x36,0x2c,0x33,0x2e,0x39,0x63,0x2d,0x32,
  0x2e,0x33,0x39,0x39,0x2c,0x30,0x2e,0x37,0x33,0x32,0x2d,0x34,0x2e,0x39,0x33,0x34,
  0x2c,0x31,0x2e,0x30,0x33,0x33,0x2d,0x37,0x2e,0x36,0x2c,0x30,0x2e,0x38,0x39,0x39,
  0x73,0x2d,0x35,0x2e,0x33,0x2d,0x30,0x2e,0x37,0x2d,0x37,0x2e,0x39,0x2d,0x31,0x2e,
  0x37,0xd,0xa,0x9,0x73,0x2d,0x34,0x2e,0x38,0x33,0x33,0x2d,0x32,0x2e,0x34,0x33,
  0x34,0x2d,0x36,0x2e,0x37,0x2d,0x34,0x2e,0x33,0x6c,0x2d,0x37,0x33,0x2e,0x36,0x2d,
  0x37,0x33,0x2e,0x32,0x63,0x2d,0x33,0x2e,0x38,0x36,0x37,0x2d,0x33,0x2e,0x37,0x33,
  0x33,0x2d,0x35,0x2e,0x38,0x2d,0x38,0x2e,0x32,0x2d,0x35,0x2e,0x38,0x2d,0x31,0x33,
  0x2e,0x34,0x63,0x30,0x2d,0x35,0x2e,0x32,0x2c,0x31,0x2e,0x39,0x33,0x33,0x2d,0x39,
  0x2e,0x36,0x36,0x37,0x2c,0x35,0x2e,0x38,0x2d,0x31,0x33,0x2e,0x34,0x63,0x31,0x2e,
  0x38,0x36,0x37,0x2d,0x31,0x2e,0x38,0x36,0x36,0x2c,0x33,0x2e,0x39,0x36,0x37,0x2d,
  0x33,0x2e,0x32,0x36,0x37,0x2c,0x36,0x2e,0x33,0x2d,0x34,0x2e,0x32,0xd,0xa,0x9,
  0x63,0x32,0x2e,0x33,0x33,0x33,0x2d,0x30,0x2e,0x39,0x33,0x34,0x2c,0x34,0x2e,0x37,
  0x33,0x33,0x2d,0x31,0x2e,0x34,0x2c,0x37,0x2e,0x32,0x2d,0x31,0x2e,0x34,0x63,0x32,
  0x2e,0x34,0x36,0x37,0x2c,0x30,0x2c,0x34,0x2e,0x39,0x2c,0x30,0x2e,0x34,0x36,0x36,
  0x2c,0x37,0x2e,0x33,0x2c,0x31,0x2e,0x34,0x63,0x32,0x2e,0x34,0x2c,0x30,0x2e,0x39,
  0x33,0x33,0x2c,0x34,0x2e,0x35,0x33,0x33,0x2c,0x32,0x2e,0x33,0x33,0x33,0x2c,0x36,
  0x2e,0x34,0x2c,0x34,0x2e,0x32,0x6c,0x36,0x31,0x2e,0x32,0x2c,0x36,0x30,0x2e,0x34,
  0x4c,0x31,0x36,0x31,0x2e,0x34,0x39,0x36,0x2c,0x34,0x39,0x2e,0x37,0x38,0x34,0x7a,
  0x22,0x2f,0x3e,0xd,0xa,0x3c,0x2f,0x73,0x76,0x67,0x3e,0xd,0xa,
	// src/resources/top.svg
  0x0,0x0,0x2,0x51,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x65,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,0x3d,0x22,0x75,0x74,0x66,
  0x2d,0x38,0x22,0x3f,0x3e,0xd,0xa,0x3c,0x21,0x2d,0x2d,0x20,0x47,0x65,0x6e,0x65,
  0x72,0x61,0x74,0x6f,0x72,0x3a,0x20,0x41,0x64,0x6f,0x62,0x65,0x20,0x49,0x6c,0x6c,
  0x75,0x73,0x74,0x72,0x61,0x74,0x6f,0x72,0x20,0x31,0x36,0x2e,0x30,0x2e,0x30,0x2c,
  0x20,0x53,0x56,0x47,0x20,0x45,0x78,0x70,0x6f,0x72,0x74,0x20,0x50,0x6c,0x75,0x67,
  0x2d,0x49,0x6e,0x20,0x2e,0x20,0x53,0x56,0x47,0x20,0x56,0x65,0x72,0x73,0x69,0x6f,
  0x6e,0x3a,0x20,0x36,0x2e,0x30,0x30,0x20,0x42,0x75,0x69,0x6c,0x64,0x20,0x30,0x29,
  0x20,0x20,0x2d,0x2d,0x3e,0xd,0xa,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,
  0x20,0x73,0x76,0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,
  0x57,0x33,0x43,0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,
  0x2f,0x2f,0x45,0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,
  0x73,0x2f,0x53,0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,
  0x67,0x31,0x31,0x2e,0x64,0x74,0x64,0x22,0x3e,0xd,0xa,0x3c,0x73,0x76,0x67,0x20,
  0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,0x69,0x64,
  0x3d,0x22,0xe5,0x9b,0xbe,0xe5,0xb1,0x82,0x5f,0x31,0x22,0x20,0x78,0x6d,0x6c,0x6e,
  0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,
  0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,0x67,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x78,0x3d,0x22,0x30,0x70,0x78,
  0x22,0x20,0x79,0x3d,0x22,0x30,0x70,0x78,0x22,0xd,0xa,0x9,0x20,0x77,0x69,0x64,
  0x74,0x68,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x68,0x65,0x69,0x67,0x68,
  0x74,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,0x30,0x30,0x22,0x20,
  0x65,0x6e,0x61,0x62,0x6c,0x65,0x2d,0x62,0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,
  0x64,0x3d,0x22,0x6e,0x65,0x77,0x20,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,
  0x30,0x30,0x22,0x20,0x78,0x6d,0x6c,0x3a,0x73,0x70,0x61,0x63,0x65,0x3d,0x22,0x70,
  0x72,0x65,0x73,0x65,0x72,0x76,0x65,0x22,0x3e,0xd,0xa,0x3c,0x70,0x6f,0x6c,0x79,
  0x67,0x6f,0x6e,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x35,0x32,0x38,0x30,0x43,
  0x31,0x22,0x20,0x70,0x6f,0x69,0x6e,0x74,0x73,0x3d,0x22,0x31,0x31,0x2e,0x31,0x31,
  0x31,0x2c,0x31,0x31,0x2e,0x31,0x31,0x31,0x20,0x31,0x31,0x2e,0x31,0x31,0x31,0x2c,
  0x37,0x33,0x2e,0x38,0x36,0x36,0x20,0x37,0x33,0x2e,0x38,0x36,0x37,0x2c,0x37,0x33,
  0x2e,0x38,0x36,0x36,0x20,0x31,0x38,0x38,0x2e,0x38,0x38,0x39,0x2c,0x37,0x33,0x2e,
  0x38,0x36,0x36,0x20,0x31,0x38,0x38,0x2e,0x38,0x38,0x39,0x2c,0x31,0x31,0x2e,0x31,
  0x31,0x31,0x20,0x22,0x2f,0x3e,0xd,0xa,0x3c,0x2f,0x73,0x76,0x67,0x3e,0xd,0xa,

	// src/resources/topRight.svg
  0x0,0x0,0x2,0x3f,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x65,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,0x3d,0x22,0x75,0x74,0x66,
  0x2d,0x38,0x22,0x3f,0x3e,0xd,0xa,0x3c,0x21,0x2d,0x2d,0x20,0x47,0x65,0x6e,0x65,
  0x72,0x61,0x74,0x6f,0x72,0x3a,0x20,0x41,0x64,0x6f,0x62,0x65,0x20,0x49,0x6c,0x6c,
  0x75,0x73,0x74,0x72,0x61,0x74,0x6f,0x72,0x20,0x31,0x36,0x2e,0x30,0x2e,0x30,0x2c,
  0x20,0x53,0x56,0x47,0x20,0x45,0x78,0x70,0x6f,0x72,0x74,0x20,0x50,0x6c,0x75,0x67,
  0x2d,0x49,0x6e,0x20,0x2e,0x20,0x53,0x56,0x47,0x20,0x56,0x65,0x72,0x73,0x69,0x6f,
  0x6e,0x3a,0x20,0x36,0x2e,0x30,0x30,0x20,0x42,0x75,0x69,0x6c,0x64,0x20,0x30,0x29,
  0x20,0x20,0x2d,0x2d,0x3e,0xd,0xa,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,
  0x20,0x73,0x76,0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,
  0x57,0x33,0x43,0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,
  0x2f,0x2f,0x45,0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,
  0x73,0x2f,0x53,0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,
  0x67,0x31,0x31,0x2e,0x64,0x74,0x64,0x22,0x3e,0xd,0xa,0x3c,0x73,0x76,0x67,0x20,
  0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,0x69,0x64,
  0x3d,0x22,0xe5,0x9b,0xbe,0xe5,0xb1,0x82,0x5f,0x31,0x22,0x20,0x78,0x6d,0x6c,0x6e,
  0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,
  0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,0x67,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x78,0x3d,0x22,0x30,0x70,0x78,
  0x22,0x20,0x79,0x3d,0x22,0x30,0x70,0x78,0x22,0xd,0xa,0x9,0x20,0x77,0x69,0x64,
  0x74,0x68,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x68,0x65,0x69,0x67,0x68,
  0x74,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,0x30,0x30,0x22,0x20,
  0x65,0x6e,0x61,0x62,0x6c,0x65,0x2d,0x62,0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,
  0x64,0x3d,0x22,0x6e,0x65,0x77,0x20,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x20,0x32,
  0x30,0x30,0x22,0x20,0x78,0x6d,0x6c,0x3a,0x73,0x70,0x61,0x63,0x65,0x3d,0x22,0x70,
  0x72,0x65,0x73,0x65,0x72,0x76,0x65,0x22,0x3e,0xd,0xa,0x3c,0x70,0x61,0x74,0x68,
  0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x35,0x32,0x38,0x30,0x43,0x31,0x22,0x20,
  0x64,0x3d,0x22,0x4d,0x31,0x38,0x38,0x2e,0x38,0x38,0x39,0x2c,0x31,0x38,0x38,0x2e,
  0x38,0x38,0x39,0x68,0x2d,0x36,0x32,0x2e,0x37,0x35,0x36,0x56,0x37,0x33,0x2e,0x38,
  0x36,0x37,0x48,0x31,0x31,0x2e,0x31,0x31,0x31,0x56,0x31,0x31,0x2e,0x31,0x31,0x31,
  0x68,0x31,0x37,0x37,0x2e,0x37,0x37,0x38,0x56,0x31,0x38,0x38,0x2e,0x38,0x38,0x39,
  0x7a,0x22,0x2f,0x3e,0xd,0xa,0x3c,0x2f,0x73,0x76,0x67,0x3e,0xd,0xa,
	// src/resources/text-italic.svg
  0x0,0x0,0x1,0xb9,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x73,0x74,0x61,0x6e,0x64,0x61,0x6c,0x6f,0x6e,0x65,0x3d,0x22,0x6e,
  0x6f,0x22,0x3f,0x3e,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x73,0x76,
  0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,
  0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,0x2f,0x2f,0x45,
  0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,
  0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,0x73,0x2f,0x53,
  0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,0x67,0x31,0x31,
  0x2e,0x64,0x74,0x64,0x22,0x3e,0x3c,0x73,0x76,0x67,0x20,0x74,0x3d,0x22,0x31,0x36,
  0x37,0x30,0x34,0x32,0x32,0x39,0x39,0x36,0x37,0x37,0x39,0x22,0x20,0x63,0x6c,0x61,
  0x73,0x73,0x3d,0x22,0x69,0x63,0x6f,0x6e,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x31,0x30,0x32,0x34,0x20,0x31,0x30,0x32,0x34,
  0x22,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,
  0x78,0x6d,0x6c,0x6e,0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,
  0x67,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x34,0x31,0x36,0x36,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x77,0x69,0x64,0x74,0x68,0x3d,
  0x22,0x36,0x34,0x22,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x36,0x34,0x22,
  0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x34,0x34,0x36,0x2e,0x36,
  0x20,0x31,0x39,0x32,0x76,0x36,0x34,0x68,0x31,0x32,0x38,0x2e,0x35,0x6c,0x2d,0x32,
  0x35,0x35,0x20,0x35,0x31,0x32,0x48,0x31,0x39,0x32,0x76,0x36,0x34,0x68,0x33,0x38,
  0x35,0x2e,0x34,0x76,0x2d,0x36,0x34,0x48,0x34,0x34,0x37,0x2e,0x39,0x6c,0x32,0x35,
  0x35,0x2d,0x35,0x31,0x32,0x48,0x38,0x33,0x32,0x76,0x2d,0x36,0x34,0x7a,0x22,0x20,
  0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x37,0x32,0x37,0x32,0x37,0x32,0x22,0x20,0x70,
  0x2d,0x69,0x64,0x3d,0x22,0x34,0x31,0x36,0x37,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,
  0x68,0x3e,0x3c,0x2f,0x73,0x76,0x67,0x3e,
	// src/resources/AligmentTop.svg
  0x0,0x0,0x2,0xc,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x73,0x74,0x61,0x6e,0x64,0x61,0x6c,0x6f,0x6e,0x65,0x3d,0x22,0x6e,
  0x6f,0x22,0x3f,0x3e,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x73,0x76,
  0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,
  0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,0x2f,0x2f,0x45,
  0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,
  0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,0x73,0x2f,0x53,
  0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,0x67,0x31,0x31,
  0x2e,0x64,0x74,0x64,0x22,0x3e,0x3c,0x73,0x76,0x67,0x20,0x74,0x3d,0x22,0x31,0x37,
  0x31,0x33,0x32,0x38,0x30,0x36,0x30,0x34,0x37,0x39,0x31,0x22,0x20,0x63,0x6c,0x61,
  0x73,0x73,0x3d,0x22,0x69,0x63,0x6f,0x6e,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x31,0x30,0x32,0x34,0x20,0x31,0x30,0x32,0x34,
  0x22,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,
  0x78,0x6d,0x6c,0x6e,0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,
  0x67,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x31,0x37,0x37,0x32,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x77,0x69,0x64,0x74,0x68,0x3d,
  0x22,0x32,0x30,0x30,0x22,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x32,0x30,
  0x30,0x22,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x36,0x34,0x20,
  0x36,0x34,0x68,0x38,0x39,0x36,0x76,0x36,0x34,0x48,0x36,0x34,0x7a,0x22,0x20,0x66,
  0x69,0x6c,0x6c,0x3d,0x22,0x23,0x37,0x32,0x37,0x32,0x37,0x32,0x22,0x20,0x70,0x2d,
  0x69,0x64,0x3d,0x22,0x31,0x37,0x37,0x33,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,0x68,
  0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x31,0x39,0x32,0x20,0x39,
  0x36,0x30,0x56,0x31,0x39,0x32,0x68,0x32,0x35,0x36,0x76,0x37,0x36,0x38,0x7a,0x22,
  0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x34,0x39,0x37,0x43,0x41,0x44,0x22,0x20,
  0x70,0x2d,0x69,0x64,0x3d,0x22,0x31,0x37,0x37,0x34,0x22,0x3e,0x3c,0x2f,0x70,0x61,
  0x74,0x68,0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x35,0x37,0x36,
  0x20,0x37,0x30,0x34,0x56,0x31,0x39,0x32,0x68,0x32,0x35,0x36,0x76,0x35,0x31,0x32,
  0x7a,0x22,0x20,0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x42,0x33,0x42,0x33,0x42,0x33,
  0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x31,0x37,0x37,0x35,0x22,0x3e,0x3c,0x2f,
  0x70,0x61,0x74,0x68,0x3e,0x3c,0x2f,0x73,0x76,0x67,0x3e,
	// src/resources/font-size-sub.svg
  0x0,0x0,0x2,0x59,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x73,0x74,0x61,0x6e,0x64,0x61,0x6c,0x6f,0x6e,0x65,0x3d,0x22,0x6e,
  0x6f,0x22,0x3f,0x3e,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x73,0x76,
  0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,
  0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,0x2f,0x2f,0x45,
  0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,
  0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,0x73,0x2f,0x53,
  0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,0x67,0x31,0x31,
  0x2e,0x64,0x74,0x64,0x22,0x3e,0x3c,0x73,0x76,0x67,0x20,0x74,0x3d,0x22,0x31,0x36,
  0x37,0x30,0x34,0x32,0x33,0x30,0x30,0x35,0x39,0x33,0x34,0x22,0x20,0x63,0x6c,0x61,
  0x73,0x73,0x3d,0x22,0x69,0x63,0x6f,0x6e,0x22,0x20,0x76,0x69,0x65,0x77,0x42,0x6f,
  0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x31,0x30,0x32,0x34,0x20,0x31,0x30,0x32,0x34,
  0x22,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,
  0x78,0x6d,0x6c,0x6e,0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,
  0x67,0x22,0x20,0x70,0x2d,0x69,0x64,0x3d,0x22,0x34,0x33,0x32,0x30,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x77,0x69,0x64,0x74,0x68,0x3d,
  0x22,0x36,0x34,0x22,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x36,0x34,0x22,
  0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x36,0x30,0x30,0x20,0x39,
  0x36,0x30,0x68,0x2d,0x38,0x35,0x6c,0x2d,0x36,0x30,0x2e,0x38,0x2d,0x31,0x35,0x38,
  0x2e,0x32,0x68,0x2d,0x32,0x34,0x38,0x4c,0x31,0x34,0x39,0x20,0x39,0x36,0x30,0x48,
  0x36,0x34,0x6c,0x32,0x32,0x36,0x2e,0x39,0x2d,0x35,0x37,0x38,0x68,0x38,0x32,0x2e,
  0x33,0x4c,0x36,0x30,0x30,0x20,0x39,0x36,0x30,0x7a,0x4d,0x34,0x33,0x30,0x2e,0x34,
  0x20,0x37,0x33,0x36,0x2e,0x35,0x6c,0x2d,0x39,0x30,0x2d,0x32,0x34,0x30,0x2e,0x33,
  0x63,0x2d,0x32,0x2e,0x38,0x2d,0x37,0x2e,0x37,0x2d,0x35,0x2e,0x39,0x2d,0x32,0x31,
  0x2e,0x31,0x2d,0x39,0x2e,0x32,0x2d,0x34,0x30,0x2e,0x33,0x68,0x2d,0x31,0x2e,0x39,
  0x63,0x2d,0x32,0x2e,0x38,0x20,0x31,0x37,0x2e,0x34,0x2d,0x36,0x20,0x33,0x30,0x2e,
  0x38,0x2d,0x39,0x2e,0x36,0x20,0x34,0x30,0x2e,0x33,0x6c,0x2d,0x38,0x39,0x2e,0x32,
  0x20,0x32,0x34,0x30,0x2e,0x33,0x68,0x31,0x39,0x39,0x2e,0x39,0x7a,0x22,0x20,0x66,
  0x69,0x6c,0x6c,0x3d,0x22,0x23,0x37,0x32,0x37,0x32,0x37,0x32,0x22,0x20,0x70,0x2d,
  0x69,0x64,0x3d,0x22,0x34,0x33,0x32,0x31,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,0x68,
  0x3e,0x3c,0x70,0x61,0x74,0x68,0x20,0x64,0x3d,0x22,0x4d,0x38,0x30,0x30,0x20,0x32,
  0x35,0x36,0x4c,0x36,0x34,0x30,0x20,0x36,0x34,0x68,0x33,0x32,0x30,0x7a,0x22,0x20,
  0x66,0x69,0x6c,0x6c,0x3d,0x22,0x23,0x34,0x39,0x37,0x43,0x41,0x44,0x22,0x20,0x70,
  0x2d,0x69,0x64,0x3d,0x22,0x34,0x33,0x32,0x32,0x22,0x3e,0x3c,0x2f,0x70,0x61,0x74,
  0x68,0x3e,0x3c,0x2f,0x73,0x76,0x67,0x3e,
	// src/resources/arrow-right.svg
  0x0,0x0,0x3,0xe8,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x65,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,0x3d,0x22,0x75,0x74,0x66,
  0x2d,0x38,0x22,0x3f,0x3e,0xd,0xa,0x3c,0x21,0x2d,0x2d,0x20,0x47,0x65,0x6e,0x65,
  0x72,0x61,0x74,0x6f,0x72,0x3a,0x20,0x41,0x64,0x6f,0x62,0x65,0x20,0x49,0x6c,0x6c,
  0x75,0x73,0x74,0x72,0x61,0x74,0x6f,0x72,0x20,0x31,0x36,0x2e,0x30,0x2e,0x30,0x2c,
  0x20,0x53,0x56,0x47,0x20,0x45,0x78,0x70,0x6f,0x72,0x74,0x20,0x50,0x6c,0x75,0x67,
  0x2d,0x49,0x6e,0x20,0x2e,0x20,0x53,0x56,0x47,0x20,0x56,0x65,0x72,0x73,0x69,0x6f,
  0x6e,0x3a,0x20,0x36,0x2e,0x30,0x30,0x20,0x42,0x75,0x69,0x6c,0x64,0x20,0x30,0x29,
  0x20,0x20,0x2d,0x2d,0x3e,0xd,0xa,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,
  0x20,0x73,0x76,0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,
  0x57,0x33,0x43,0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,
  0x2f,0x2f,0x45,0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,
  0x73,0x2f,0x53,0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,
  0x67,0x31,0x31,0x2e,0x64,0x74,0x64,0x22,0x3e,0xd,0xa,0x3c,0x73,0x76,0x67,0x20,
  0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,0x69,0x64,
  0x3d,0x22,0xe5,0x9b,0xbe,0xe5,0xb1,0x82,0x5f,0x31,0x22,0x20,0x78,0x6d,0x6c,0x6e,
  0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,
  0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,0x67,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x78,0x3d,0x22,0x30,0x70,0x78,
  0x22,0x20,0x79,0x3d,0x22,0x30,0x70,0x78,0x22,0xd,0xa,0x9,0x20,0x77,0x69,0x64,
  0x74,0x68,0x3d,0x22,0x32,0x30,0x30,0x2e,0x33,0x39,0x31,0x70,0x78,0x22,0x20,0x68,
  0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x76,0x69,
  0x65,0x77,0x42,0x6f,0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x2e,0x33,
  0x39,0x31,0x20,0x32,0x30,0x30,0x22,0x20,0x65,0x6e,0x61,0x62,0x6c,0x65,0x2d,0x62,
  0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,0x64,0x3d,0x22,0x6e,0x65,0x77,0x20,0x30,
  0x20,0x30,0x20,0x32,0x30,0x30,0x2e,0x33,0x39,0x31,0x20,0x32,0x30,0x30,0x22,0x20,
  0x78,0x6d,0x6c,0x3a,0x73,0x70,0x61,0x63,0x65,0x3d,0x22,0x70,0x72,0x65,0x73,0x65,
  0x72,0x76,0x65,0x22,0x3e,0xd,0xa,0x3c,0x70,0x61,0x74,0x68,0x20,0x66,0x69,0x6c,
  0x6c,0x3d,0x22,0x23,0x35,0x31,0x35,0x31,0x35,0x31,0x22,0x20,0x64,0x3d,0x22,0x4d,
  0x34,0x39,0x2e,0x39,0x38,0x2c,0x33,0x38,0x2e,0x37,0x63,0x2d,0x33,0x2e,0x37,0x33,
  0x33,0x2d,0x33,0x2e,0x37,0x33,0x32,0x2d,0x35,0x2e,0x36,0x2d,0x38,0x2e,0x32,0x33,
  0x32,0x2d,0x35,0x2e,0x36,0x2d,0x31,0x33,0x2e,0x35,0x63,0x30,0x2d,0x35,0x2e,0x32,
  0x36,0x36,0x2c,0x31,0x2e,0x38,0x36,0x36,0x2d,0x39,0x2e,0x37,0x36,0x36,0x2c,0x35,
  0x2e,0x36,0x2d,0x31,0x33,0x2e,0x35,0x63,0x33,0x2e,0x37,0x33,0x33,0x2d,0x33,0x2e,
  0x38,0x36,0x36,0x2c,0x38,0x2e,0x32,0x2d,0x35,0x2e,0x38,0x2c,0x31,0x33,0x2e,0x34,
  0x2d,0x35,0x2e,0x38,0xd,0xa,0x9,0x63,0x35,0x2e,0x32,0x2c,0x30,0x2c,0x39,0x2e,
  0x36,0x36,0x37,0x2c,0x31,0x2e,0x39,0x33,0x34,0x2c,0x31,0x33,0x2e,0x34,0x2c,0x35,
  0x2e,0x38,0x6c,0x37,0x34,0x2e,0x34,0x2c,0x37,0x34,0x2e,0x38,0x30,0x31,0x63,0x31,
  0x2e,0x38,0x36,0x37,0x2c,0x31,0x2e,0x36,0x2c,0x33,0x2e,0x31,0x36,0x36,0x2c,0x33,
  0x2e,0x36,0x2c,0x33,0x2e,0x39,0x2c,0x36,0x63,0x30,0x2e,0x37,0x33,0x32,0x2c,0x32,
  0x2e,0x33,0x39,0x39,0x2c,0x31,0x2e,0x30,0x33,0x33,0x2c,0x34,0x2e,0x39,0x33,0x34,
  0x2c,0x30,0x2e,0x38,0x39,0x39,0x2c,0x37,0x2e,0x36,0xd,0xa,0x9,0x63,0x2d,0x30,
  0x2e,0x31,0x33,0x34,0x2c,0x32,0x2e,0x36,0x36,0x36,0x2d,0x30,0x2e,0x37,0x2c,0x35,
  0x2e,0x32,0x39,0x39,0x2d,0x31,0x2e,0x37,0x2c,0x37,0x2e,0x39,0x63,0x2d,0x31,0x2c,
  0x32,0x2e,0x36,0x2d,0x32,0x2e,0x34,0x33,0x34,0x2c,0x34,0x2e,0x38,0x33,0x33,0x2d,
  0x34,0x2e,0x33,0x2c,0x36,0x2e,0x37,0x6c,0x2d,0x37,0x33,0x2e,0x32,0x2c,0x37,0x33,
  0x2e,0x36,0x63,0x2d,0x33,0x2e,0x37,0x33,0x33,0x2c,0x33,0x2e,0x38,0x36,0x37,0x2d,
  0x38,0x2e,0x32,0x2c,0x35,0x2e,0x38,0x2d,0x31,0x33,0x2e,0x34,0x2c,0x35,0x2e,0x38,
  0x63,0x2d,0x35,0x2e,0x32,0x2c,0x30,0x2d,0x39,0x2e,0x36,0x36,0x37,0x2d,0x31,0x2e,
  0x39,0x33,0x33,0x2d,0x31,0x33,0x2e,0x34,0x2d,0x35,0x2e,0x38,0xd,0xa,0x9,0x63,
  0x2d,0x31,0x2e,0x38,0x36,0x36,0x2d,0x31,0x2e,0x38,0x36,0x36,0x2d,0x33,0x2e,0x32,
  0x36,0x37,0x2d,0x33,0x2e,0x39,0x36,0x37,0x2d,0x34,0x2e,0x32,0x2d,0x36,0x2e,0x33,
  0x63,0x2d,0x30,0x2e,0x39,0x33,0x34,0x2d,0x32,0x2e,0x33,0x33,0x34,0x2d,0x31,0x2e,
  0x34,0x2d,0x34,0x2e,0x37,0x33,0x33,0x2d,0x31,0x2e,0x34,0x2d,0x37,0x2e,0x32,0x73,
  0x30,0x2e,0x34,0x36,0x36,0x2d,0x34,0x2e,0x39,0x2c,0x31,0x2e,0x34,0x2d,0x37,0x2e,
  0x33,0x63,0x30,0x2e,0x39,0x33,0x33,0x2d,0x32,0x2e,0x34,0x2c,0x32,0x2e,0x33,0x33,
  0x33,0x2d,0x34,0x2e,0x35,0x33,0x33,0x2c,0x34,0x2e,0x32,0x2d,0x36,0x2e,0x34,0x6c,
  0x36,0x30,0x2e,0x34,0x30,0x31,0x2d,0x36,0x31,0x2e,0x32,0xd,0xa,0x9,0x4c,0x34,
  0x39,0x2e,0x39,0x38,0x2c,0x33,0x38,0x2e,0x37,0x7a,0x22,0x2f,0x3e,0xd,0xa,0x3c,
  0x2f,0x73,0x76,0x67,0x3e,0xd,0xa,
	// src/resources/arrow-left.svg
  0x0,0x0,0x3,0xea,
  0x3c,
  0x3f,0x78,0x6d,0x6c,0x20,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,
  0x30,0x22,0x20,0x65,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,0x3d,0x22,0x75,0x74,0x66,
  0x2d,0x38,0x22,0x3f,0x3e,0xd,0xa,0x3c,0x21,0x2d,0x2d,0x20,0x47,0x65,0x6e,0x65,
  0x72,0x61,0x74,0x6f,0x72,0x3a,0x20,0x41,0x64,0x6f,0x62,0x65,0x20,0x49,0x6c,0x6c,
  0x75,0x73,0x74,0x72,0x61,0x74,0x6f,0x72,0x20,0x31,0x36,0x2e,0x30,0x2e,0x30,0x2c,
  0x20,0x53,0x56,0x47,0x20,0x45,0x78,0x70,0x6f,0x72,0x74,0x20,0x50,0x6c,0x75,0x67,
  0x2d,0x49,0x6e,0x20,0x2e,0x20,0x53,0x56,0x47,0x20,0x56,0x65,0x72,0x73,0x69,0x6f,
  0x6e,0x3a,0x20,0x36,0x2e,0x30,0x30,0x20,0x42,0x75,0x69,0x6c,0x64,0x20,0x30,0x29,
  0x20,0x20,0x2d,0x2d,0x3e,0xd,0xa,0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,
  0x20,0x73,0x76,0x67,0x20,0x50,0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,
  0x57,0x33,0x43,0x2f,0x2f,0x44,0x54,0x44,0x20,0x53,0x56,0x47,0x20,0x31,0x2e,0x31,
  0x2f,0x2f,0x45,0x4e,0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,
  0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x47,0x72,0x61,0x70,0x68,0x69,0x63,
  0x73,0x2f,0x53,0x56,0x47,0x2f,0x31,0x2e,0x31,0x2f,0x44,0x54,0x44,0x2f,0x73,0x76,
  0x67,0x31,0x31,0x2e,0x64,0x74,0x64,0x22,0x3e,0xd,0xa,0x3c,0x73,0x76,0x67,0x20,
  0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x3d,0x22,0x31,0x2e,0x31,0x22,0x20,0x69,0x64,
  0x3d,0x22,0xe5,0x9b,0xbe,0xe5,0xb1,0x82,0x5f,0x31,0x22,0x20,0x78,0x6d,0x6c,0x6e,
  0x73,0x3d,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,
  0x2e,0x6f,0x72,0x67,0x2f,0x32,0x30,0x30,0x30,0x2f,0x73,0x76,0x67,0x22,0x20,0x78,
  0x6d,0x6c,0x6e,0x73,0x3a,0x78,0x6c,0x69,0x6e,0x6b,0x3d,0x22,0x68,0x74,0x74,0x70,
  0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,0x2e,0x6f,0x72,0x67,0x2f,0x31,0x39,
  0x39,0x39,0x2f,0x78,0x6c,0x69,0x6e,0x6b,0x22,0x20,0x78,0x3d,0x22,0x30,0x70,0x78,
  0x22,0x20,0x79,0x3d,0x22,0x30,0x70,0x78,0x22,0xd,0xa,0x9,0x20,0x77,0x69,0x64,
  0x74,0x68,0x3d,0x22,0x32,0x30,0x30,0x2e,0x33,0x39,0x31,0x70,0x78,0x22,0x20,0x68,
  0x65,0x69,0x67,0x68,0x74,0x3d,0x22,0x32,0x30,0x30,0x70,0x78,0x22,0x20,0x76,0x69,
  0x65,0x77,0x42,0x6f,0x78,0x3d,0x22,0x30,0x20,0x30,0x20,0x32,0x30,0x30,0x2e,0x33,
  0x39,0x31,0x20,0x32,0x30,0x30,0x22,0x20,0x65,0x6e,0x61,0x62,0x6c,0x65,0x2d,0x62,
  0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,0x64,0x3d,0x22,0x6e,0x65,0x77,0x20,0x30,
  0x20,0x30,0x20,0x32,0x30,0x30,0x2e,0x33,0x39,0x31,0x20,0x32,0x30,0x30,0x22,0x20,
  0x78,0x6d,0x6c,0x3a,0x73,0x70,0x61,0x63,0x65,0x3d,0x22,0x70,0x72,0x65,0x73,0x65,
  0x72,0x76,0x65,0x22,0x3e,0xd,0xa,0x3c,0x70,0x61,0x74,0x68,0x20,0x66,0x69,0x6c,
  0x6c,0x3d,0x22,0x23,0x35,0x31,0x35,0x31,0x35,0x31,0x22,0x20,0x64,0x3d,0x22,0x4d,
  0x31,0x35,0x30,0x2e,0x34,0x31,0x32,0x2c,0x31,0x36,0x31,0x2e,0x32,0x39,0x39,0x63,
  0x33,0x2e,0x37,0x33,0x33,0x2c,0x33,0x2e,0x37,0x33,0x32,0x2c,0x35,0x2e,0x36,0x2c,
  0x38,0x2e,0x32,0x33,0x32,0x2c,0x35,0x2e,0x36,0x2c,0x31,0x33,0x2e,0x35,0x63,0x30,
  0x2c,0x35,0x2e,0x32,0x36,0x36,0x2d,0x31,0x2e,0x38,0x36,0x36,0x2c,0x39,0x2e,0x37,
  0x36,0x36,0x2d,0x35,0x2e,0x36,0x2c,0x31,0x33,0x2e,0x35,0xd,0xa,0x9,0x63,0x2d,
  0x33,0x2e,0x37,0x33,0x33,0x2c,0x33,0x2e,0x38,0x36,0x36,0x2d,0x38,0x2e,0x32,0x2c,
  0x35,0x2e,0x38,0x2d,0x31,0x33,0x2e,0x34,0x2c,0x35,0x2e,0x38,0x73,0x2d,0x39,0x2e,
  0x36,0x36,0x37,0x2d,0x31,0x2e,0x39,0x33,0x34,0x2d,0x31,0x33,0x2e,0x34,0x2d,0x35,
  0x2e,0x38,0x6c,0x2d,0x37,0x34,0x2e,0x34,0x2d,0x37,0x34,0x2e,0x38,0x30,0x31,0x63,
  0x2d,0x31,0x2e,0x38,0x36,0x37,0x2d,0x31,0x2e,0x36,0x2d,0x33,0x2e,0x31,0x36,0x36,
  0x2d,0x33,0x2e,0x36,0x2d,0x33,0x2e,0x39,0x2d,0x36,0x63,0x2d,0x30,0x2e,0x37,0x33,
  0x32,0x2d,0x32,0x2e,0x33,0x39,0x39,0x2d,0x31,0x2e,0x30,0x33,0x33,0x2d,0x34,0x2e,
  0x39,0x33,0x34,0x2d,0x30,0x2e,0x38,0x39,0x39,0x2d,0x37,0x2e,0x36,0xd,0xa,0x9,
  0x63,0x30,0x2e,0x31,0x33,0x34,0x2d,0x32,0x2e,0x36,0x36,0x36,0x2c,0x30,0x2e,0x37,
  0x2d,0x35,0x2e,0x32,0x39,0x39,0x2c,0x31,0x2e,0x37,0x2d,0x37,0x2e,0x39,0x63,0x31,
  0x2d,0x32,0x2e,0x36,0x2c,0x32,0x2e,0x34,0x33,0x34,0x2d,0x34,0x2e,0x38,0x33,0x33,
  0x2c,0x34,0x2e,0x33,0x2d,0x36,0x2e,0x37,0x6c,0x37,0x33,0x2e,0x32,0x2d,0x37,0x33,
  0x2e,0x36,0x63,0x33,0x2e,0x37,0x33,0x33,0x2d,0x33,0x2e,0x38,0x36,0x37,0x2c,0x38,
  0x2e,0x32,0x2d,0x35,0x2e,0x38,0x2c,0x31,0x33,0x2e,0x34,0x2d,0x35,0x2e,0x38,0x73,
  0x39,0x2e,0x36,0x36,0x37,0x2c,0x31,0x2e,0x39,0x33,0x33,0x2c,0x31,0x33,0x2e,0x34,
  0x2c,0x35,0x2e,0x38,0xd,0xa,0x9,0x63,0x31,0x2e,0x38,0x36,0x36,0x2c,0x31,0x2e,
  0x38,0x36,0x36,0x2c,0x33,0x2e,0x32,0x36,0x37,0x2c,0x33,0x2e,0x39,0x36,0x37,0x2c,
  0x34,0x2e,0x31,0x39,0x39,0x2c,0x36,0x2e,0x33,0x63,0x30,0x2e,0x39,0x33,0x34,0x2c,
  0x32,0x2e,0x33,0x33,0x34,0x2c,0x31,0x2e,0x34,0x2c,0x34,0x2e,0x37,0x33,0x33,0x2c,
  0x31,0x2e,0x34,0x2c,0x37,0x2e,0x32,0x73,0x2d,0x30,0x2e,0x34,0x36,0x37,0x2c,0x34,
  0x2e,0x39,0x2d,0x31,0x2e,0x34,0x2c,0x37,0x2e,0x33,0x63,0x2d,0x30,0x2e,0x39,0x33,
  0x33,0x2c,0x32,0x2e,0x34,0x2d,0x32,0x2e,0x33,0x33,0x33,0x2c,0x34,0x2e,0x35,0x33,
  0x33,0x2d,0x34,0x2e,0x31,0x39,0x39,0x2c,0x36,0x2e,0x34,0xd,0xa,0x9,0x6c,0x2d,
  0x36,0x30,0x2e,0x34,0x30,0x31,0x2c,0x36,0x31,0x2e,0x32,0x4c,0x31,0x35,0x30,0x2e,
  0x34,0x31,0x32,0x2c,0x31,0x36,0x31,0x2e,0x32,0x39,0x39,0x7a,0x22,0x2f,0x3e,0xd,
  0xa,0x3c,0x2f,0x73,0x76,0x67,0x3e,0xd,0xa,

};

static const unsigned char qt_resource_name[] = {
  // DAWidgetsIcons
  0x0,0xe,
  0x6,0xc0,0xef,0xd3,
  0x0,0x44,
  0x0,0x41,0x0,0x57,0x0,0x69,0x0,0x64,0x0,0x67,0x0,0x65,0x0,0x74,0x0,0x73,0x0,0x49,0x0,0x63,0x0,0x6f,0x0,0x6e,0x0,0x73,
	// text-underline.svg
  0x0,0x12,
  0x2,0x62,0x15,0x87,
  0x0,0x74,
  0x0,0x65,0x0,0x78,0x0,0x74,0x0,0x2d,0x0,0x75,0x0,0x6e,0x0,0x64,0x0,0x65,0x0,0x72,0x0,0x6c,0x0,0x69,0x0,0x6e,0x0,0x65,0x0,0x2e,0x0,0x73,0x0,0x76,
  0x0,0x67,
	// bottomRight.svg
  0x0,0xf,
  0xa,0x9a,0xae,0x87,
  0x0,0x62,
  0x0,0x6f,0x0,0x74,0x0,0x74,0x0,0x6f,0x0,0x6d,0x0,0x52,0x0,0x69,0x0,0x67,0x0,0x68,0x0,0x74,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// topleft.svg
  0x0,0xb,
  0xb,0x21,0x94,0x87,
  0x0,0x74,
  0x0,0x6f,0x0,0x70,0x0,0x6c,0x0,0x65,0x0,0x66,0x0,0x74,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// error.svg
  0x0,0x9,
  0x9,0x65,0x83,0xe7,
  0x0,0x65,
  0x0,0x72,0x0,0x72,0x0,0x6f,0x0,0x72,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// AligmentRight.svg
  0x0,0x11,
  0xb,0x5c,0xd4,0xa7,
  0x0,0x41,
  0x0,0x6c,0x0,0x69,0x0,0x67,0x0,0x6d,0x0,0x65,0x0,0x6e,0x0,0x74,0x0,0x52,0x0,0x69,0x0,0x67,0x0,0x68,0x0,0x74,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,

	// text-bold.svg
  0x0,0xd,
  0x3,0xb9,0xbc,0x7,
  0x0,0x74,
  0x0,0x65,0x0,0x78,0x0,0x74,0x0,0x2d,0x0,0x62,0x0,0x6f,0x0,0x6c,0x0,0x64,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// AligmentCenter.svg
  0x0,0x12,
  0x0,0x7f,0x8a,0xa7,
  0x0,0x41,
  0x0,0x6c,0x0,0x69,0x0,0x67,0x0,0x6d,0x0,0x65,0x0,0x6e,0x0,0x74,0x0,0x43,0x0,0x65,0x0,0x6e,0x0,0x74,0x0,0x65,0x0,0x72,0x0,0x2e,0x0,0x73,0x0,0x76,
  0x0,0x67,
	// right.svg
  0x0,0x9,
  0xd,0xf7,0xab,0x27,
  0x0,0x72,
  0x0,0x69,0x0,0x67,0x0,0x68,0x0,0x74,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// center.svg
  0x0,0xa,
  0xa,0xc8,0x62,0x67,
  0x0,0x63,
  0x0,0x65,0x0,0x6e,0x0,0x74,0x0,0x65,0x0,0x72,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// bottomLeft.svg
  0x0,0xe,
  0x6,0xa1,0x84,0xc7,
  0x0,0x62,
  0x0,0x6f,0x0,0x74,0x0,0x74,0x0,0x6f,0x0,0x6d,0x0,0x4c,0x0,0x65,0x0,0x66,0x0,0x74,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// arrow-up.svg
  0x0,0xc,
  0x9,0xd0,0x77,0x87,
  0x0,0x61,
  0x0,0x72,0x0,0x72,0x0,0x6f,0x0,0x77,0x0,0x2d,0x0,0x75,0x0,0x70,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// AligmentHCenter.svg
  0x0,0x13,
  0x1,0x63,0xe7,0x27,
  0x0,0x41,
  0x0,0x6c,0x0,0x69,0x0,0x67,0x0,0x6d,0x0,0x65,0x0,0x6e,0x0,0x74,0x0,0x48,0x0,0x43,0x0,0x65,0x0,0x6e,0x0,0x74,0x0,0x65,0x0,0x72,0x0,0x2e,0x0,0x73,
  0x0,0x76,0x0,0x67,
	// left.svg
  0x0,0x8,
  0xb,0xd7,0x54,0x87,
  0x0,0x6c,
  0x0,0x65,0x0,0x66,0x0,0x74,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// open.svg
  0x0,0x8,
  0x6,0xc1,0x54,0x7,
  0x0,0x6f,
  0x0,0x70,0x0,0x65,0x0,0x6e,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// font-size-add.svg
  0x0,0x11,
  0xf,0x50,0xc7,0x67,
  0x0,0x66,
  0x0,0x6f,0x0,0x6e,0x0,0x74,0x0,0x2d,0x0,0x73,0x0,0x69,0x0,0x7a,0x0,0x65,0x0,0x2d,0x0,0x61,0x0,0x64,0x0,0x64,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,

	// bottom.svg
  0x0,0xa,
  0xb,0x55,0x77,0xa7,
  0x0,0x62,
  0x0,0x6f,0x0,0x74,0x0,0x74,0x0,0x6f,0x0,0x6d,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// AligmentBottom.svg
  0x0,0x12,
  0x1,0xe6,0x8f,0x67,
  0x0,0x41,
  0x0,0x6c,0x0,0x69,0x0,0x67,0x0,0x6d,0x0,0x65,0x0,0x6e,0x0,0x74,0x0,0x42,0x0,0x6f,0x0,0x74,0x0,0x74,0x0,0x6f,0x0,0x6d,0x0,0x2e,0x0,0x73,0x0,0x76,
  0x0,0x67,
	// AligmentLeft.svg
  0x0,0x10,
  0x7,0xbd,0xe3,0x67,
  0x0,0x41,
  0x0,0x6c,0x0,0x69,0x0,0x67,0x0,0x6d,0x0,0x65,0x0,0x6e,0x0,0x74,0x0,0x4c,0x0,0x65,0x0,0x66,0x0,0x74,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// arrow-down.svg
  0x0,0xe,
  0x6,0xc,0x7,0x87,
  0x0,0x61,
  0x0,0x72,0x0,0x72,0x0,0x6f,0x0,0x77,0x0,0x2d,0x0,0x64,0x0,0x6f,0x0,0x77,0x0,0x6e,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// top.svg
  0x0,0x7,
  0xb,0x63,0x5a,0x27,
  0x0,0x74,
  0x0,0x6f,0x0,0x70,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// topRight.svg
  0x0,0xc,
  0x2,0x9b,0xeb,0x27,
  0x0,0x74,
  0x0,0x6f,0x0,0x70,0x0,0x52,0x0,0x69,0x0,0x67,0x0,0x68,0x0,0x74,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// text-italic.svg
  0x0,0xf,
  0xc,0x20,0x5a,0xe7,
  0x0,0x74,
  0x0,0x65,0x0,0x78,0x0,0x74,0x0,0x2d,0x0,0x69,0x0,0x74,0x0,0x61,0x0,0x6c,0x0,0x69,0x0,0x63,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// AligmentTop.svg
  0x0,0xf,
  0x4,0xa5,0xf1,0x47,
  0x0,0x41,
  0x0,0x6c,0x0,0x69,0x0,0x67,0x0,0x6d,0x0,0x65,0x0,0x6e,0x0,0x74,0x0,0x54,0x0,0x6f,0x0,0x70,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// font-size-sub.svg
  0x0,0x11,
  0x2,0x42,0xc7,0x87,
  0x0,0x66,
  0x0,0x6f,0x0,0x6e,0x0,0x74,0x0,0x2d,0x0,0x73,0x0,0x69,0x0,0x7a,0x0,0x65,0x0,0x2d,0x0,0x73,0x0,0x75,0x0,0x62,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,

	// arrow-right.svg
  0x0,0xf,
  0xf,0x22,0x69,0x47,
  0x0,0x61,
  0x0,0x72,0x0,0x72,0x0,0x6f,0x0,0x77,0x0,0x2d,0x0,0x72,0x0,0x69,0x0,0x67,0x0,0x68,0x0,0x74,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,
	// arrow-left.svg
  0x0,0xe,
  0x8,0xfa,0x38,0xa7,
  0x0,0x61,
  0x0,0x72,0x0,0x72,0x0,0x6f,0x0,0x77,0x0,0x2d,0x0,0x6c,0x0,0x65,0x0,0x66,0x0,0x74,0x0,0x2e,0x0,0x73,0x0,0x76,0x0,0x67,

};

static const unsigned char qt_resource_struct[] = {
  // :
  0x0,0x0,0x0,0x0,0x0,0x2,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x1,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  // :/DAWidgetsIcons
  0x0,0x0,0x0,0x0,0x0,0x2,0x0,0x0,0x0,0x1a,0x0,0x0,0x0,0x2,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  // :/DAWidgetsIcons/AligmentCenter.svg
  0x0,0x0,0x0,0xec,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x12,0x15,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe1,
  // :/DAWidgetsIcons/AligmentHCenter.svg
  0x0,0x0,0x1,0x88,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x1e,0x7f,
0x0,0x0,0x1,0xa2,0x6,0x83,0xae,0xef,
  // :/DAWidgetsIcons/AligmentBottom.svg
  0x0,0x0,0x2,0x22,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x2d,0x89,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe1,
  // :/DAWidgetsIcons/font-size-sub.svg
  0x0,0x0,0x3,0xe,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x3d,0xf0,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe8,
  // :/DAWidgetsIcons/text-underline.svg
  0x0,0x0,0x0,0x22,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xeb,
  // :/DAWidgetsIcons/topRight.svg
  0x0,0x0,0x2,0xa8,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x37,0xe0,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xec,
  // :/DAWidgetsIcons/text-bold.svg
  0x0,0x0,0x0,0xcc,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0xe,0xce,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xea,
  // :/DAWidgetsIcons/AligmentTop.svg
  0x0,0x0,0x2,0xea,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x3b,0xe0,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe3,
  // :/DAWidgetsIcons/arrow-down.svg
  0x0,0x0,0x2,0x72,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x31,0xa9,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe3,
  // :/DAWidgetsIcons/bottomLeft.svg
  0x0,0x0,0x1,0x48,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x19,0x49,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe5,
  // :/DAWidgetsIcons/open.svg
  0x0,0x0,0x1,0xca,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x22,0xa6,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe8,
  // :/DAWidgetsIcons/AligmentLeft.svg
  0x0,0x0,0x2,0x4c,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x2f,0x99,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe2,
  // :/DAWidgetsIcons/arrow-left.svg
  0x0,0x0,0x3,0x5a,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x44,0x39,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe3,
  // :/DAWidgetsIcons/error.svg
  0x0,0x0,0x0,0x8c,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x6,0xd6,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe7,
  // :/DAWidgetsIcons/arrow-up.svg
  0x0,0x0,0x1,0x6a,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x1b,0x8f,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe4,
  // :/DAWidgetsIcons/bottomRight.svg
  0x0,0x0,0x0,0x4c,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x2,0x4b,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe6,
  // :/DAWidgetsIcons/center.svg
  0x0,0x0,0x1,0x2e,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x16,0x80,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe6,
  // :/DAWidgetsIcons/topleft.svg
  0x0,0x0,0x0,0x70,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x4,0x95,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xec,
  // :/DAWidgetsIcons/bottom.svg
  0x0,0x0,0x2,0x8,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x2b,0x2f,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe5,
  // :/DAWidgetsIcons/AligmentRight.svg
  0x0,0x0,0x0,0xa4,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0xc,0xbe,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe2,
  // :/DAWidgetsIcons/top.svg
  0x0,0x0,0x2,0x94,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x35,0x8b,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xeb,
  // :/DAWidgetsIcons/left.svg
  0x0,0x0,0x1,0xb4,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x20,0x51,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe8,
  // :/DAWidgetsIcons/text-italic.svg
  0x0,0x0,0x2,0xc6,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x3a,0x23,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xea,
  // :/DAWidgetsIcons/right.svg
  0x0,0x0,0x1,0x16,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x14,0x26,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xea,
  // :/DAWidgetsIcons/arrow-right.svg
  0x0,0x0,0x3,0x36,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x40,0x4d,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe4,
  // :/DAWidgetsIcons/font-size-add.svg
  0x0,0x0,0x1,0xe0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x28,0xc8,
0x0,0x0,0x1,0x9f,0x10,0xce,0xf,0xe7,

};

#ifdef QT_NAMESPACE
#  define QT_RCC_PREPEND_NAMESPACE(name) ::QT_NAMESPACE::name
#  define QT_RCC_MANGLE_NAMESPACE0(x) x
#  define QT_RCC_MANGLE_NAMESPACE1(a, b) a##_##b
#  define QT_RCC_MANGLE_NAMESPACE2(a, b) QT_RCC_MANGLE_NAMESPACE1(a,b)
#  define QT_RCC_MANGLE_NAMESPACE(name) QT_RCC_MANGLE_NAMESPACE2( \
		QT_RCC_MANGLE_NAMESPACE0(name), QT_RCC_MANGLE_NAMESPACE0(QT_NAMESPACE))
#else
#   define QT_RCC_PREPEND_NAMESPACE(name) name
#   define QT_RCC_MANGLE_NAMESPACE(name) name
#endif

#ifdef QT_NAMESPACE
namespace QT_NAMESPACE {
#endif

bool qRegisterResourceData(int, const unsigned char *, const unsigned char *, const unsigned char *);
bool qUnregisterResourceData(int, const unsigned char *, const unsigned char *, const unsigned char *);

#ifdef QT_NAMESPACE
}
#endif

int QT_RCC_MANGLE_NAMESPACE(qInitResources_dawidgetsicons)();
int QT_RCC_MANGLE_NAMESPACE(qInitResources_dawidgetsicons)()
{
	int version = 3;
	QT_RCC_PREPEND_NAMESPACE(qRegisterResourceData)
		(version, qt_resource_struct, qt_resource_name, qt_resource_data);
	return 1;
}

int QT_RCC_MANGLE_NAMESPACE(qCleanupResources_dawidgetsicons)();
int QT_RCC_MANGLE_NAMESPACE(qCleanupResources_dawidgetsicons)()
{
	int version = 3;
	QT_RCC_PREPEND_NAMESPACE(qUnregisterResourceData)
	   (version, qt_resource_struct, qt_resource_name, qt_resource_data);
	return 1;
}

#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif

namespace {
   struct initializer {
	   initializer() { QT_RCC_MANGLE_NAMESPACE(qInitResources_dawidgetsicons)(); }
	   ~initializer() { QT_RCC_MANGLE_NAMESPACE(qCleanupResources_dawidgetsicons)(); }
   } dummy;
}

#ifdef __clang__
#   pragma clang diagnostic pop
#endif

/*** End of inlined file: qrc_dawidgetsicons.cpp ***/


#ifdef _MSC_VER
#pragma warning(pop)
#pragma pop_macro("_CRT_SECURE_NO_WARNINGS")
#endif


#ifndef DAWIDGETS_AMALGAM_H
#define DAWIDGETS_AMALGAM_H

// -----------------------------------------------------------------------------
// DAWidgets single-file amalgamation.
//
// 本文件由 tools/Amalgamate.ps1 自动生成，包含 DAWidgets 全部公开头文件。
// Do not edit this generated file by hand.
// -----------------------------------------------------------------------------


/*** Start of inlined file: DAWidgetsAmalgamTemplatePublicHeaders.h ***/
// DAWidgets 单文件合并模板 —— 公共头文件清单
//
// 本文件只被合并工具读取，不要直接编译。Amalgamate 会把下面列出的所有头文件
// 依次内联进 src-amalgamate/DAWidgets.h；头文件之间通过裸文件名互相包含
// （例如 "DAWidgetsGlobals.h"），合并工具会递归展开并自动去重，因此这里的
// 顺序只影响输出排版，不影响编译结果。
//
// 新增公开类时：把新的 .h 加到这里，把对应的 .cpp 加到
// DAWidgetsAmalgamTemplate.cpp，然后重新运行 tools/Amalgamate.ps1。


/*** Start of inlined file: DAWidgetsGlobals.h ***/
#ifndef DAWIDGETSGLOBALS_H
#define DAWIDGETSGLOBALS_H
#include <memory>

// ---- 符号导出/导入宏 ----
// 不依赖 CMake GenerateExportHeader,手动实现,便于 qmake 等其他构建系统集成。
//
// 三种场景:
//   1. 静态构建 (DA_BUILT_AS_STATIC): 导出宏为空
//   2. 共享构建-编译库 (DAWIDGETS_EXPORTS): 符号导出
//   3. 共享构建-使用者侧: 符号导入
#if defined(DA_BUILT_AS_STATIC)
#define DAWIDGETS_EXPORT
#elif defined(DAWIDGETS_EXPORTS)
#if defined(_MSC_VER)
#define DAWIDGETS_EXPORT __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
#define DAWIDGETS_EXPORT __attribute__((visibility("default")))
#else
#define DAWIDGETS_EXPORT
#endif
#else
#if defined(_MSC_VER)
#define DAWIDGETS_EXPORT __declspec(dllimport)
#elif defined(__GNUC__) || defined(__clang__)
#define DAWIDGETS_EXPORT
#else
#define DAWIDGETS_EXPORT
#endif
#endif

/**
 * @def DA_DECLARE_PRIVATE
 * @brief 模仿Q_DECLARE_PRIVATE，但不用前置声明而是作为一个内部类
 *
 * 例如:
 *
 * @code
 * //header
 * class A
 * {
 *  DA_DECLARE_PRIVATE(A)
 * };
 * @endcode
 *
 * 其展开效果为：
 *
 * @code
 * class A{
 *  class PrivateData;
 *  friend class A::PrivateData;
 *  std::unique_ptr< PrivateData > d_ptr;
 * }
 * @endcode
 *
 * 这样前置声明了一个内部类PrivateData，在cpp文件中建立这个内部类的实现
 *
 * @code
 * //cpp
 * class A::PrivateData{
 *  DA_DECLARE_PUBLIC(A)
 *  PrivateData(A* p):q_ptr(p){
 *  }
 * };
 *
 * A::A():d_ptr(new PrivateData(this)){
 * }
 * @endcode
 *
 */
#ifndef DA_DECLARE_PRIVATE
#define DA_DECLARE_PRIVATE(classname)                                                                                  \
	class PrivateData;                                                                                                 \
	friend class classname::PrivateData;                                                                               \
	std::unique_ptr< PrivateData > d_ptr;                                                                              \
	inline PrivateData* d_func()                                                                                       \
	{                                                                                                                  \
		return (d_ptr.get());                                                                                          \
	}                                                                                                                  \
	inline const PrivateData* d_func() const                                                                           \
	{                                                                                                                  \
		return (d_ptr.get());                                                                                          \
	}
#endif

/**
 * @def DA_DECLARE_PUBLIC
 * @brief 模仿Q_DECLARE_PUBLIC
 *
 * 配套DA_DECLARE_PRIVATE使用
 */
#ifndef DA_DECLARE_PUBLIC
#define DA_DECLARE_PUBLIC(classname)                                                                                   \
	friend class classname;                                                                                            \
	classname* q_ptr { nullptr };                                                                                      \
	inline classname* q_func()                                                                                         \
	{                                                                                                                  \
		return (static_cast< classname* >(q_ptr));                                                                     \
	}                                                                                                                  \
	inline const classname* q_func() const                                                                             \
	{                                                                                                                  \
		return (static_cast< const classname* >(q_ptr));                                                               \
	}
#endif

/**
 * @def  DA_PIMPL_CONSTRUCT
 *
 * 配套DA_DECLARE_PRIVATE使用,在构造函数中构建privatedata
 */
#ifndef DA_PIMPL_CONSTRUCT
#define DA_PIMPL_CONSTRUCT d_ptr(std::make_unique< PrivateData >(this))
#endif

/**
 *@def DA_D
 *@brief impl获取指针，参考Q_D
 */
#ifndef DA_D
#define DA_D(pointerName) PrivateData* pointerName = d_func()
#endif

/**
 *@def DA_DC
 *@brief impl获取指针，参考Q_D
 */
#ifndef DA_DC
#define DA_DC(pointerName) const PrivateData* pointerName = d_func()
#endif

/**
 *@def DA_Q
 *@brief impl获取指针，参考Q_Q
 */
#ifndef DA_Q
#define DA_Q(classname, pointerName) classname* pointerName = q_func()
#endif

/**
 *@def DA_QC
 *@brief impl获取指针，参考Q_Q
 */
#ifndef DA_QC
#define DA_QC(classname, pointerName) const classname* pointerName = q_func()
#endif

namespace DA {

/**
 * @brief 注册 Qt RCC 图标资源（内部接口）
 * @details 控件构造函数调用一次，保证静态库链接时 qrc 资源目标不会被丢弃。
 *          Q_INIT_RESOURCE 的展开必须在全局作用域完成，故这里只做一层转发，
 *          实现见 DAWidgetsResources.cpp。
 */
void ensureResourcesInitialized();

}  // namespace DA

#endif  // DAWIDGETSGLOBALS_H

/*** End of inlined file: DAWidgetsGlobals.h ***/


/*** Start of inlined file: DAQt5Qt6Compat.h ***/
#ifndef DAQT5QT6COMPAT_H
#define DAQT5QT6COMPAT_H

//
// 本头文件提供 Qt5/Qt6 差异兼容宏。
//
// 维护契约:
//   此处的宏定义必须与 data-workbench 项目的 src/DAGlobals.h 中对应宏
//   逐字一致。两侧均以 #ifndef 守卫,任一翻译单元先 include 谁都安全
//   (first-include-wins)。不可独立修改本文件中的宏定义——若需变更,
//   需同步更新 data-workbench 的 DAGlobals.h。
//
// 引入策略:
//   默认仅在 .cpp 文件中按需 include,避免通过公共头传递,从而
//   data-workbench 引入 DAWidgets 时不会被动引入新的兼容宏。
//

#include <QtGlobal>

/**
 *@def Qt5Qt6Compat_QXXEvent_Pos
 *@brief QXXEvent_Pos函数的兼容宏
 *
 *'pos' is deprecated: Use position().toPoint()
 *
 *qevent.h:740:5: note: 'pos' has been explicitly marked deprecated here
 *
 *qglobal.h:369:44: note: expanded from macro 'QT_DEPRECATED_VERSION_X_6_0'
 *
 *qglobal.h:281:33: note: expanded from macro 'QT_DEPRECATED_X'
 *
 */
#ifndef Qt5Qt6Compat_QXXEvent_Pos
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define Qt5Qt6Compat_QXXEvent_Pos(valuePtr) valuePtr->pos()
#else
#define Qt5Qt6Compat_QXXEvent_Pos(valuePtr) valuePtr->position().toPoint()
#endif
#endif

/**
 *@def Qt5Qt6Compat_QXXEvent_x
 *@brief QXXEvent_x函数的兼容宏
 *
 * warning: 'int QMouseEvent::x() const' is deprecated: Use position()
 *
 * QMouseEvent::x
 */
#ifndef Qt5Qt6Compat_QXXEvent_x
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define Qt5Qt6Compat_QXXEvent_x(valuePtr) valuePtr->pos().x()
#else
#define Qt5Qt6Compat_QXXEvent_x(valuePtr) valuePtr->position().x()
#endif
#endif

/**
 *@def Qt5Qt6Compat_QXXEvent_y
 *@brief QXXEvent_y函数的兼容宏
 *
 * warning: 'int QMouseEvent::y() const' is deprecated: Use position()
 *
 * QMouseEvent::y
 */
#ifndef Qt5Qt6Compat_QXXEvent_y
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define Qt5Qt6Compat_QXXEvent_y(valuePtr) valuePtr->pos().y()
#else
#define Qt5Qt6Compat_QXXEvent_y(valuePtr) valuePtr->position().y()
#endif
#endif

/**
 * @def Qt5Qt6Compat_fontMetrics_width
 * @brief QFontMetrics的字体宽度适配
 */
#ifndef Qt5Qt6Compat_fontMetrics_width
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
#define Qt5Qt6Compat_fontMetrics_width(fontMeter, str) fontMeter.width(str)
#else
#define Qt5Qt6Compat_fontMetrics_width(fontMeter, str) fontMeter.horizontalAdvance(str)
#endif
#endif

#ifndef Qt5Qt6Compat_Connect_ButtonGroupClicked_int
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
#define Qt5Qt6Compat_Connect_ButtonGroupClicked_int(buttonGroup, funName)                                              \
	do {                                                                                                               \
		connect(buttonGroup, QOverload< int >::of(&QButtonGroup::buttonClicked), this, &funName);                      \
	} while (0)
#else
#define Qt5Qt6Compat_Connect_ButtonGroupClicked_int(buttonGroup, funName)                                              \
	do {                                                                                                               \
		connect(buttonGroup, &QButtonGroup::idClicked, this, &funName);                                                \
	} while (0)
#endif
#endif

#endif  // DAQT5QT6COMPAT_H

/*** End of inlined file: DAQt5Qt6Compat.h ***/

// ---- Selector 选择器家族 ----

/*** Start of inlined file: DAFlowLayout.h ***/
#ifndef DAFLOWLAYOUT_H
#define DAFLOWLAYOUT_H

#include <QLayout>
#include <QList>
#include <QRect>
#include <QStyle>

class QWidget;

namespace DA
{
/**
 * @brief 流式布局，子项按行排列、自动换行。
 *
 * 与 Qt 官方 Flow Layout 示例一致，额外支持 #trailingExpanding：开启后
 * 最后一个布局项会占据当前行的剩余宽度（用于在 DAChipsEdit 末尾嵌入自适应
 * 宽度的输入框）；剩余宽度不足时自动换行并占满整行。
 */
class DAWIDGETS_EXPORT DAFlowLayout : public QLayout
{
	DA_DECLARE_PRIVATE(DAFlowLayout)
public:
	// 构造流式布局并挂载到父控件
	explicit DAFlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
	// 构造流式布局（无父控件，-1 表示使用 style 默认间距）
	explicit DAFlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
	// 析构，释放所有布局项
	~DAFlowLayout() override;

	// 添加布局项到末尾
	void addItem(QLayoutItem *item) override;
	// 在指定位置插入控件
	void insertWidget(int index, QWidget *widget);
	// 获取水平间距（-1 表示使用 style 默认值）
	int horizontalSpacing() const;
	// 获取垂直间距（-1 表示使用 style 默认值）
	int verticalSpacing() const;
	// 返回布局可扩展的方向
	Qt::Orientations expandingDirections() const override;
	// 是否支持 height-for-width 布局（流式布局必须返回 true）
	bool hasHeightForWidth() const override;
	// 根据给定宽度计算所需高度
	int heightForWidth(int width) const override;
	// 获取布局项数量
	int count() const override;
	// 获取指定索引的布局项
	QLayoutItem *itemAt(int index) const override;
	// 获取最小尺寸
	QSize minimumSize() const override;
	// 执行布局，排列所有子项
	void setGeometry(const QRect &rect) override;
	// 获取推荐尺寸
	QSize sizeHint() const override;
	// 移除并返回指定索引的布局项
	QLayoutItem *takeAt(int index) override;

	// 设置末尾项是否占满剩余宽度（用于内嵌自适应输入框）
	void setTrailingExpanding(bool on);
	// 获取末尾项是否占满剩余宽度
	bool trailingExpanding() const;

private:
	int doLayout(const QRect &rect, bool testOnly) const;
	int smartSpacing(QStyle::PixelMetric pm) const;
};
}  // namespace DA

#endif // DAFLOWLAYOUT_H

/*** End of inlined file: DAFlowLayout.h ***/



/*** Start of inlined file: DAChip.h ***/
#ifndef DACHIP_H
#define DACHIP_H

#include <QFrame>

class QLabel;
class QToolButton;

namespace DA
{
/**
 * @brief 单个标签胶囊控件，显示文本 + 可选关闭按钮。
 *
 * 独立可用，亦作为 DAChipsEdit 的子项。可通过 qss 定制：
 *
 * ```css
 * DA--DAChip { background: #ecf5ff; border:1px solid #d9ecff;
 *          border-radius:4px; }
 * DA--DAChip:hover { background: #d9ecff; }
 * QToolButton#DAChipCloseButton { border:none; color:#909399; }
 * QToolButton#DAChipCloseButton:hover { color:#f56c6c; }
 * ```
 */
class DAWIDGETS_EXPORT DAChip : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
	Q_PROPERTY(bool closable READ closable WRITE setClosable NOTIFY closableChanged)
	DA_DECLARE_PRIVATE(DAChip)
public:
	// 构造带文本的标签胶囊
	explicit DAChip(const QString &text, QWidget *parent = nullptr);
	// 构造空文本的标签胶囊
	explicit DAChip(QWidget *parent = nullptr);
	// 析构，释放内部资源
	~DAChip();

	// 获取标签显示文本
	QString text() const;
	// 设置标签显示文本
	void setText(const QString &text);

	// 是否显示关闭按钮
	bool closable() const;
	// 设置是否显示关闭按钮
	void setClosable(bool on);

Q_SIGNALS:
	/**
	 * @brief 关闭按钮被点击时发射
	 */
	void closeRequested();
	/**
	 * @brief 标签整体被点击时发射
	 */
	void clicked();
	/**
	 * @brief 显示文本变化时发射
	 * @param text 当前文本
	 */
	void textChanged(const QString &text);
	/**
	 * @brief closable 属性变化时发射
	 * @param closable 是否可关闭
	 */
	void closableChanged(bool closable);

protected:
	void mousePressEvent(QMouseEvent *e) override;

private:
	void init();
};
}  // namespace DA

#endif // DACHIP_H

/*** End of inlined file: DAChip.h ***/


/*** Start of inlined file: DAChipsEdit.h ***/
#ifndef DACHIPSEDIT_H
#define DACHIPSEDIT_H

#include <QFrame>
#include <QList>
#include <QSet>
#include <QStringList>

class QLineEdit;
class QKeyEvent;
class QRegularExpression;

namespace DA
{
class DAChip;
class DAFlowLayout;

/**
 * @brief 流式多标签输入文本框。
 *
 * 行为对标 Element UI el-select 的 multiple + allow-create 输入态：
 * 输入文本后按回车或分隔符提交为一个标签（DAChip），Backspace 在空输入时
 * 删除末尾标签，每个标签可点击 × 关闭。支持去重、限量、自定义分隔符。
 *
 * 分隔符同时提供枚举重载（编译期类型安全）与字符串重载（运行时/UI 配置）。
 *
 * 扩展点（供 DASelectComboBox 等组合控件使用）：inputText()/clearInput()
 * 读写内嵌输入框文本，inputTextChanged/inputFocusChanged/inputKeyEvent
 * 信号暴露输入态与按键事件，便于外部拦截回车/方向键实现下拉候选逻辑。
 */
class DAWIDGETS_EXPORT DAChipsEdit : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged)
	Q_PROPERTY(QString placeholder READ placeholder WRITE setPlaceholder)
	Q_PROPERTY(bool allowCreate READ allowCreate WRITE setAllowCreate)
	Q_PROPERTY(bool allowDuplicate READ allowDuplicate WRITE setAllowDuplicate)
	Q_PROPERTY(int maxCount READ maxCount WRITE setMaxCount)
	DA_DECLARE_PRIVATE(DAChipsEdit)
public:
	/// 提交标签的触发字符。Enter 由回车键触发，不参与文本内分隔检测。
	enum class Separator {
		Comma,     ///< ','
		Semicolon, ///< ';'
		Space,     ///< ' '
		Enter,     ///< 回车键
		Tab        ///< '\t'（按 Tab 提交，阻止焦点跳转）
	};
	Q_ENUM(Separator)

	// 构造流式多标签输入文本框
	explicit DAChipsEdit(QWidget *parent = nullptr);
	// 析构，释放内部资源
	~DAChipsEdit();

	// 获取当前所有标签文本列表
	QStringList tags() const;
	// 设置标签列表，替换当前所有标签
	void setTags(const QStringList &tags);

	// 追加一个标签到末尾
	void appendTag(const QString &text);
	// 在指定位置插入标签，索引越界返回 false
	bool insertTag(int index, const QString &text);
	// 移除第一个匹配文本的标签
	void removeTag(const QString &text);
	// 移除指定索引处的标签
	void removeAt(int index);
	// 清空所有标签
	void clear();

	// 获取占位提示文本
	QString placeholder() const;
	// 设置占位提示文本
	void setPlaceholder(const QString &text);

	// 是否允许添加非候选列表中的文本作为标签
	bool allowCreate() const;
	// 设置是否允许创建新标签
	void setAllowCreate(bool on);

	// 是否允许重复标签
	bool allowDuplicate() const;
	// 设置是否允许重复标签
	void setAllowDuplicate(bool on);

	// 获取标签数量上限，-1 表示无限制
	int maxCount() const;
	// 设置标签数量上限，-1 表示无限制
	void setMaxCount(int count);

	// 枚举重载（推荐，C++ 编译期）
	void setSeparators(const QList<Separator> &separators);
	// 获取当前分隔符枚举列表
	QList<Separator> separators() const;
	// 字符串重载：以给定字符集合中任意字符作为分隔符（不限枚举）
	void setSeparators(const QString &chars);

	// 内嵌输入框访问（供 DASelectComboBox 等组合控件使用）
	QString inputText() const;
	// 清空内嵌输入框文本
	void clearInput();

	/// 单行高度（输入框 sizeHint + layout 上下 margin），供固定高度模式估算
	int rowHeightHint() const;

	// 推荐尺寸，依据当前标签与布局计算
	QSize sizeHint() const override;
	// 最小推荐尺寸，保证输入框至少可显示
	QSize minimumSizeHint() const override;

Q_SIGNALS:
	/**
	 * @brief 添加标签后发射
	 * @param text 新增标签的文本
	 */
	void tagAdded(const QString &text);
	/**
	 * @brief 移除标签后发射
	 * @param text 被移除标签的文本
	 */
	void tagRemoved(const QString &text);
	/**
	 * @brief 标签列表变化时发射
	 * @param tags 当前的标签列表
	 */
	void tagsChanged(const QStringList &tags);
	/**
	 * @brief 内嵌输入框文本变化时发射
	 * @param text 当前输入框文本
	 */
	void inputTextChanged(const QString &text);
	/**
	 * @brief 内嵌输入框焦点变化时发射
	 * @param focused 是否获得焦点
	 */
	void inputFocusChanged(bool focused);
	/// 按键事件拦截：外部可将 *handled 置 true 阻止 DAChipsEdit 默认处理。
	void inputKeyEvent(QKeyEvent *event, bool *handled);

protected:
	bool eventFilter(QObject *watched, QEvent *event) override;
	void focusInEvent(QFocusEvent *event) override;

private:
	void rebuildChips();
	void addChipWidget(const QString &text);
	void commitInput();
	void onInputEdited(const QString &text);
	QSet<QChar> separatorChars() const;
	QRegularExpression separatorRegex() const;
};
}  // namespace DA

#endif // DACHIPSEDIT_H

/*** End of inlined file: DAChipsEdit.h ***/


/*** Start of inlined file: DASelectComboBox.h ***/
#ifndef DASELECTCOMBOBOX_H
#define DASELECTCOMBOBOX_H

#include <QComboBox>
#include <QStringList>

class QListWidget;
class QListWidgetItem;
class QScrollArea;
class QWidget;

namespace DA
{
class DAChipsEdit;

/**
 * @brief 多标签选择下拉框，对标 Element UI el-select 的
 * multiple + filterable + allow-create + default-first-option 完整形态。
 *
 * 继承 QComboBox，保留原生箭头按钮、CC_ComboBox 样式边框与 showPopup 键盘
 * 助记键（F4 / Alt+Down）；编辑区替换为 DAChipsEdit 实现标签流式输入。下拉
 * 候选用自定义 QListWidget，支持输入实时过滤、方向键导航、回车选中
 * （default-first-option）；无匹配且 allowCreate 时回车创建。
 *
 * 高度策略 #heightMode：GrowWithTags（默认，随标签换行增高，el-select 风格）
 * 或 FixedHeightScrollable（固定行数，超出滚动）。
 */
class DAWIDGETS_EXPORT DASelectComboBox : public QComboBox
{
	Q_OBJECT
	Q_PROPERTY(QStringList options READ options WRITE setOptions)
	Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged)
	Q_PROPERTY(bool allowCreate READ allowCreate WRITE setAllowCreate)
	Q_PROPERTY(bool filterable READ filterable WRITE setFilterable)
	Q_PROPERTY(QString placeholder READ placeholder WRITE setPlaceholder)
	Q_PROPERTY(HeightMode heightMode READ heightMode WRITE setHeightMode)
	Q_PROPERTY(int fixedRowCount READ fixedRowCount WRITE setFixedRowCount)
	DA_DECLARE_PRIVATE(DASelectComboBox)
public:
	/// 高度策略
	enum class HeightMode {
		GrowWithTags,          ///< 随标签换行自动增高（el-select multiple 风格）
		FixedHeightScrollable  ///< 固定显示 fixedRowCount 行，超出在编辑区内滚动
	};
	Q_ENUM(HeightMode)

	// 构造多标签选择下拉框
	explicit DASelectComboBox(QWidget *parent = nullptr);
	// 析构，释放内部资源
	~DASelectComboBox();

	// 获取候选选项列表
	QStringList options() const;
	// 设置候选选项列表，替换当前所有选项
	void setOptions(const QStringList &options);

	// 获取已选标签列表
	QStringList tags() const;
	// 设置已选标签列表，替换当前所有标签
	void setTags(const QStringList &tags);
	// 清空所有已选标签与候选选项
	void clear();

	// 是否允许在无匹配时创建新标签
	bool allowCreate() const;
	// 设置是否允许创建新标签
	void setAllowCreate(bool on);
	// 下拉列表是否支持输入过滤
	bool filterable() const;
	// 设置下拉列表是否支持输入过滤
	void setFilterable(bool on);

	// 获取输入框占位提示文本
	QString placeholder() const;
	// 设置输入框占位提示文本
	void setPlaceholder(const QString &text);

	// 获取当前高度策略
	HeightMode heightMode() const;
	// 设置高度策略（随标签增高或固定行数滚动）
	void setHeightMode(HeightMode mode);
	// 获取固定高度模式下的显示行数
	int fixedRowCount() const;
	// 设置固定高度模式下的显示行数
	void setFixedRowCount(int rows);

	// 推荐尺寸，依据当前标签数量与高度策略计算
	QSize sizeHint() const override;
	// 最小推荐尺寸，保证至少一行可显示
	QSize minimumSizeHint() const override;

Q_SIGNALS:
	/**
	 * @brief 已选标签列表变化时发射
	 * @param tags 当前的标签列表
	 */
	void tagsChanged(const QStringList &tags);
	/**
	 * @brief 从候选列表选中某项时发射
	 * @param text 被选中项的文本
	 */
	void optionSelected(const QString &text);
	/**
	 * @brief 用户创建新标签时发射
	 * @param text 新创建标签的文本
	 */
	void created(const QString &text);

protected:
	void resizeEvent(QResizeEvent *event) override;
	void showPopup() override;
	bool eventFilter(QObject *watched, QEvent *event) override;

private:
	void applyHeightMode();
	void applyDefaultStyle();
	void showCustomPopup();
	void hideCustomPopup();
	void refreshPopup(const QString &filter);
	void pickCurrent();
	void onInputTextChanged(const QString &text);
	void onInputFocusChanged(bool focused);
	void onInputKey(QKeyEvent *event, bool *handled);
	void onPopupItemClicked(QListWidgetItem *item);
};
}  // namespace DA

#endif // DASELECTCOMBOBOX_H

/*** End of inlined file: DASelectComboBox.h ***/

// ---- Collapse 折叠面板家族 ----

/*** Start of inlined file: DACollapseLayout.h ***/
#ifndef DACOLLAPSELAYOUT_H
#define DACOLLAPSELAYOUT_H

#include <QLayout>
#include <QVector>

namespace DA
{
/**
 * @brief 折叠面板专用垂直布局器
 *
 * 从上到下依次排列子项,每个子项取其 sizeHint 的自然高度,
 * 不压缩、不拉伸、不调用 qGeomCalc()。
 * 空间不足时子项溢出底部而非被压缩,从根本上消除展开/折叠动画期间的抖动。
 *
 * 替换 DACollapseWidget 和 DACollapseItem 中的 QVBoxLayout。
 */
class DAWIDGETS_EXPORT DACollapseLayout : public QLayout
{
	Q_OBJECT
public:
	explicit DACollapseLayout(QWidget* parent = nullptr);
	~DACollapseLayout() override;

	void addItem(QLayoutItem* item) override;
	QLayoutItem* itemAt(int index) const override;
	QLayoutItem* takeAt(int index) override;
	int count() const override;
	void setGeometry(const QRect& rect) override;
	QSize sizeHint() const override;
	QSize minimumSize() const override;
	QSize maximumSize() const override;
	Qt::Orientations expandingDirections() const override;
	bool hasHeightForWidth() const override;
	int heightForWidth(int width) const override;
	void invalidate() override;

	void insertWidget(int index, QWidget* widget);
	void addStretch(int stretch = 0);

private:
	QVector<QLayoutItem*> m_items;
	mutable bool m_dirty;
	mutable QSize m_sizeHint;
	mutable QSize m_minSize;

	void recalc() const;
};
}  // namespace DA

#endif  // DACOLLAPSELAYOUT_H

/*** End of inlined file: DACollapseLayout.h ***/



/*** Start of inlined file: DACollapseItem.h ***/
#ifndef DACOLLAPSEITEM_H
#define DACOLLAPSEITEM_H

#include <QFrame>

class QLabel;
class QPropertyAnimation;
class QVBoxLayout;
class QColor;

namespace DA
{
/**
 * @brief 折叠面板的单个条目,对标 Element UI el-collapse-item。
 *
 * 由标题栏(header)与内容区(content)两部分构成。点击标题栏在展开/折叠
 * 之间切换,展开时内容区从 0 高度动画过渡到自然高度(可通过 setAnimation 关闭)。
 *
 * 通常不单独使用,而是 add 进 DACollapse 容器由其协调互斥;但也可独立使用,
 * 此时 setExpanded 直接控制自身展开状态。
 *
 * qss 定制示例:
 * ```css
 * DA--DACollapseItem { border:1px solid #ebeef5; border-radius:4px; }
 * DA--DACollapseItem::header { background:#fafafa; }
 * DA--DACollapseItem::header:hover { background:#f5f7fa; }
 * QLabel#DACollapseItemArrow { color:#909399; }
 * QLabel#DACollapseItemTitle { color:#303133; font-weight:bold; }
 * DA--DACollapseItem[disabled="true"]::header { color:#c0c4cc; }
 * ```
 *
 * 对应 Element UI:
 *   name      -> name()      / setName()
 *   title     -> title()     / setTitle()        (title slot -> setTitleWidget)
 *   disabled  -> disabled()  / setDisabled()
 */
class DAWIDGETS_EXPORT DACollapseItem : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(bool disabled READ disabled WRITE setDisabled NOTIFY disabledChanged)
	Q_PROPERTY(bool expanded READ expanded WRITE setExpanded NOTIFY expandChanged)
	Q_PROPERTY(bool animationEnabled READ animationEnabled WRITE setAnimationEnabled NOTIFY animationEnabledChanged)
	DA_DECLARE_PRIVATE(DACollapseItem)
public:
	// 构造空标题的折叠条目
	explicit DACollapseItem(QWidget *parent = nullptr);
	// 构造指定标题的折叠条目
	explicit DACollapseItem(const QString &title, QWidget *parent = nullptr);
	// 析构,释放标题栏与内容区子控件
	~DACollapseItem();

	// 获取标题文本
	QString title() const;
	// 设置标题文本
	void setTitle(const QString &title);

	// 获取面板标识(供容器 activeNames 引用)
	QString name() const;
	// 设置面板标识
	void setName(const QString &name);

	// 是否禁用(禁用后不可点击展开)
	bool disabled() const;
	// 启用/禁用面板交互
	void setDisabled(bool on);

	// 是否处于展开态
	bool expanded() const;
	// 展开/折叠面板(可带动画)
	void setExpanded(bool on);

	// 动画开关(默认开)
	bool animationEnabled() const;
	// 设置是否启用展开/折叠动画
	void setAnimationEnabled(bool on);

	// 自定义标题内容(对标 title slot);传入后取代默认标题文本
	void setTitleWidget(QWidget *widget);
	// 获取自定义标题控件(未设置时返回 nullptr)
	QWidget *titleWidget() const;

	// 设置内容控件(取代默认内容布局)
	void setContentWidget(QWidget *widget);
	// 获取内容控件(未设置时返回 nullptr)
	QWidget *contentWidget() const;

	// 内容区布局,可直接 addWidget 追加多个控件
	QVBoxLayout *contentLayout() const;

	// 推荐尺寸
	QSize sizeHint() const override;

Q_SIGNALS:
	/**
	 * @brief 标题文本变化时发射
	 * @param title 新标题文本
	 */
	void titleChanged(const QString &title);
	/**
	 * @brief 面板标识变化时发射
	 * @param name 新面板标识
	 */
	void nameChanged(const QString &name);
	/**
	 * @brief 禁用状态变化时发射
	 * @param disabled 是否禁用
	 */
	void disabledChanged(bool disabled);
	/**
	 * @brief 展开/折叠状态变化时发射
	 * @param expanded 是否展开
	 */
	void expandChanged(bool expanded);
	/**
	 * @brief 动画开关变化时发射
	 * @param on 是否启用动画
	 */
	void animationEnabledChanged(bool on);

protected:
	bool eventFilter(QObject *watched, QEvent *event) override;

private:
	void init();
	void updateArrow();
	void updateHeaderVisualState();
	int contentNaturalHeight() const;
	void doExpand(bool on);
	void onHeaderClicked();
	void onAnimationFinished();
};
}  // namespace DA

#endif // DACOLLAPSEITEM_H

/*** End of inlined file: DACollapseItem.h ***/


/*** Start of inlined file: DACollapseWidget.h ***/
#ifndef DACOLLAPSEWIDGET_H
#define DACOLLAPSEWIDGET_H

#include <QStringList>
#include <QWidget>

namespace DA
{
class DACollapseItem;
class DACollapseLayout;

/**
 * @brief 折叠面板容器,对标 Element UI el-collapse。
 *
 * 纵向排列多个 DACollapseItem,并协调展开状态:
 *  - 非手风琴模式(默认):多个面板可同时展开,activeNames 为列表。
 *  - 手风琴模式:同时只能展开一个,展开新面板时折叠其余,activeNames 取首个。
 *
 * 对应 Element UI:
 *   value/v-model  -> activeNames() / setActiveNames()
 *   accordion      -> accordion()   / setAccordion()
 *   change(event)  -> activeNamesChanged(QStringList)
 *
 * 用法:
 * ```cpp
 * auto *c = new DA::DACollapse;
 * c->addItem(QStringLiteral("一致性"), QStringLiteral("1"));
 * c->addItem(QStringLiteral("反馈"), QStringLiteral("2"));
 * c->setActiveNames({ QStringLiteral("1") });
 * ```
 */
class DAWIDGETS_EXPORT DACollapseWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(bool accordion READ accordion WRITE setAccordion NOTIFY accordionChanged)
	Q_PROPERTY(QStringList activeNames READ activeNames WRITE setActiveNames NOTIFY activeNamesChanged)
	DA_DECLARE_PRIVATE(DACollapseWidget)
public:
	// 构造折叠面板容器
	explicit DACollapseWidget(QWidget* parent = nullptr);
	// 析构,释放内部条目与布局
	~DACollapseWidget();

	// 是否启用手风琴模式(同时仅一个面板展开)
	bool accordion() const;
	// 启用/禁用手风琴模式
	void setAccordion(bool on);

	// 获取当前激活(展开)面板的 name 列表
	QStringList activeNames() const;
	// 设置激活面板(手风琴模式下仅取首个生效);会同步展开/折叠子项
	void setActiveNames(const QStringList& names);

	// 添加一个已有的条目(接管所有权)
	void addItem(DACollapseItem* item);
	// 便捷创建并添加条目,返回新建实例以便设置内容
	DACollapseItem* addItem(const QString& title, const QString& name = QString());

	// 按 name 查找条目
	DACollapseItem* item(const QString& name) const;
	// 获取全部条目列表
	QList< DACollapseItem* > items() const;

	// 移除并销毁条目
	void removeItem(DACollapseItem* item);
	// 清空并销毁所有条目
	void clear();

	// 在指定索引处插入普通控件(非 DACollapseItem,不参与 activeNames/accordion 协调)
	void insertWidget(int index, QWidget* widget);
	// 末尾(弹簧前)添加普通控件
	void addWidget(QWidget* widget);
	// 移除普通控件(不销毁,调用方负责所有权)
	void removeWidget(QWidget* widget);
	// 返回普通控件的索引,不存在返回 -1
	int indexOf(QWidget* widget) const;

	// 推荐尺寸
	QSize sizeHint() const override;

Q_SIGNALS:
	/**
	 * @brief 手风琴模式切换时发射
	 * @param on 切换后是否启用手风琴模式
	 */
	void accordionChanged(bool on);
	/**
	 * @brief 激活面板集合变化时发射
	 * @param activeNames 当前激活面板的 name 列表
	 */
	void activeNamesChanged(const QStringList& activeNames);

private:
	void init();
	void activate(DACollapseItem* item, bool on);
	void onItemExpandChanged();
};
}  // namespace DA

#endif  // DACOLLAPSEWIDGET_H

/*** End of inlined file: DACollapseWidget.h ***/

// ---- 光标 / 遮罩 / 撤销历史等工具组件 ----

/*** Start of inlined file: DACursorScoped.h ***/
#ifndef DACURSORSCOPED_H
#define DACURSORSCOPED_H
#include <QCursor>

namespace DA
{
/**
 * @brief 用于图标保护，防止异常导致图标无法恢复
 */
class DAWIDGETS_EXPORT DACursorScoped
{
public:
	// 构造光标守卫，构造时立即设置指定光标
	DACursorScoped(const QCursor& cur);
	// 析构时自动恢复原光标，保证异常路径下也能恢复
	virtual ~DACursorScoped();
	// 设置新的覆盖光标，叠加到应用级光标栈
	void setCursor(const QCursor& cur);
	// 手动释放，提前恢复光标
	void release();
};
}  // end namespace DA

#endif  // DACURSORSCOPED_H

/*** End of inlined file: DACursorScoped.h ***/



/*** Start of inlined file: DAWaitCursorScoped.h ***/
#ifndef DAWAITCURSORSCOPED_H
#define DAWAITCURSORSCOPED_H
#include <QCursor>

namespace DA
{
/**
 * @brief 一个等待鼠标的保护类
 */
class DAWIDGETS_EXPORT DAWaitCursorScoped : public DACursorScoped
{
public:
	// 构造等待光标守卫，构造时立即显示等待沙漏
	DAWaitCursorScoped();
	// 析构时恢复原光标
	virtual ~DAWaitCursorScoped();
};
}  // end namespace DA

#ifndef DA_WAIT_CURSOR_SCOPED
#define DA_WAIT_CURSOR_SCOPED()                                                                                        \
	DA::DAWaitCursorScoped __da__wait__cursor;                                                                         \
	Q_UNUSED(__da__wait__cursor)
#endif

#ifndef DA_WAIT_CURSOR_SCOPED_NS
#define DA_WAIT_CURSOR_SCOPED_NS()                                                                                     \
	DA::DAWaitCursorScoped __da__wait__cursor;                                                                         \
	Q_UNUSED(__da__wait__cursor)
#endif
#endif  // DAWAITCURSORSCOPED_H

/*** End of inlined file: DAWaitCursorScoped.h ***/


/*** Start of inlined file: DAAbstractWidgetOverlay.h ***/
#ifndef DAABSTRACTWIDGETOVERLAY_H
#define DAABSTRACTWIDGETOVERLAY_H

#include <QWidget>
class QPainter;
class QRegion;

namespace DA
{

/**
 * @brief 参照QwtWidgetOverlay的Widget Overlay
 *
 * 遮罩层的主要用例是为了避免对下层窗口进行繁重的重绘操作。
 * 与绘图画布结合使用时，覆盖层可以避免重新绘制，因为画布的内容可以从其后备存储中恢复。
 */
class DAWIDGETS_EXPORT DAAbstractWidgetOverlay : public QWidget
{
	DA_DECLARE_PRIVATE(DAAbstractWidgetOverlay)
	Q_OBJECT
public:
	/*!
		   \brief 遮罩模式

	   当使用遮罩时，下层窗口仅会为覆盖层的遮罩区域接收绘制事件。
	   否则，Qt 会触发完整的重绘。在性能较低的硬件（如嵌入式系统）上，
	   或者在远程桌面上使用光栅绘制引擎时，位块传输是一个明显的操作，需要避免。

		是否以及如何使用遮罩取决于遮罩计算的复杂度和遮罩可以排除多少像素。

	   默认设置为 MaskHint。

		\sa setMaskMode(), maskMode()
	  */
	enum MaskMode
	{
		//! 不使用遮罩。
		NoMask,

		/*!
		   \brief 使用 maskHint() 作为遮罩

			   在许多情况下，快速近似就足够了，没有必要构建更详细的遮罩
			   （例如文本的边界矩形）。
			 */
		MaskHint,

		/*!
		   \brief 通过检查 alpha 值来计算遮罩

			   有时无法给出快速近似，需要通过绘制覆盖层并测试结果来计算遮罩。

			   当有一个有效的 maskHint() 时，只会检查该近似范围内的像素。
			 */
		AlphaMask
	};

	/*!
	   \brief 渲染模式

		为了计算 alpha 遮罩，覆盖层已经被绘制到一个临时的 QImage 上。
		可以避免两次渲染覆盖层，而是复制这个缓冲区来绘制覆盖层。

	   在使用光栅绘制引擎（QWS、Windows）的图形系统上，这通常只意味着复制一些内存。
	   在 X11 上，这会生成一个昂贵的操作来构建一个像素图，对于简单的覆盖层可能不推荐。

		\note 当 maskMode() != AlphaMask 时，渲染模式无效。
	  */
	enum RenderMode
	{
		//! 使用光栅绘制引擎时复制缓冲区。（默认）
		AutoRenderMode,

		//! 总是复制缓冲区
		CopyAlphaMask,

		//! 从不复制缓冲区
		DrawOverlay
	};

public:
	explicit DAAbstractWidgetOverlay(QWidget* parent);
	~DAAbstractWidgetOverlay();

	void setMaskMode(MaskMode mode);
	MaskMode getMaskMode() const;

	void setRenderMode(RenderMode mode);
	RenderMode getRenderMode() const;

	virtual bool eventFilter(QObject* object, QEvent* event) override;

public:
	// 矩形的遮罩区域计算
	static QRegion maskRegion(const QRect& r, int penWidth);
	// 针对水平线或竖直线的遮罩区域计算
	static QRegion maskRegionVOrHLine(const QLine& VOrHLine, int penWidth);

protected:
	// 绘制遮罩层的主要业务实现函数
	virtual void drawOverlay(QPainter* painter) const = 0;

	// 计算遮罩区的近似值
	virtual QRegion maskHint() const;

	virtual void paintEvent(QPaintEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
public Q_SLOTS:
	void updateOverlay();

private:
	void updateMask();
	void draw(QPainter* painter) const;
};
}  // end ns da
#endif  // DAWIDGETOVERLAY_H

/*** End of inlined file: DAAbstractWidgetOverlay.h ***/


/*** Start of inlined file: DAUndoHistoryWidget.h ***/
#ifndef DAUNDOHISTORYWIDGET_H
#define DAUNDOHISTORYWIDGET_H
#include <QWidget>

class QUndoGroup;
class QUndoView;
class QSpinBox;

namespace DA
{
/**
 * @brief 撤销历史停靠面板（005 T1.21，widget 在 DAGui、APP 只做挂载接线，D0）
 *
 * 内容用 Qt 内置 QUndoView 绑到全局 QUndoGroup（DAInterface 的 DACommandInterface
 * 持有，APP 注入——DAGui 不依赖 DAInterface/L4，只认 QUndoGroup）：跟随当前激活
 * 页面的撤销栈、点任意条目前进/回退到该状态、显示命令描述（文案来自各命令 setText
 * 的 tr()，红线5：面板里不允许空白条目）。
 *
 * 撤销深度调节（R19）：面板提供 QSpinBox 入口，改动只发 undoLimitChangeRequested
 * 信号——配置读取/写回（ui.undo-stack-limit，dawork-config.json）与对各
 * 数据页 QUndoStack 的施加都在 APP 侧完成（D0 分层：严禁 DAGui→APP 反向依赖，
 * DAGui 只暴露 undoLimit()/setUndoLimit(int) 口径的接口）。
 */
class DAWIDGETS_EXPORT DAUndoHistoryWidget : public QWidget
{
	Q_OBJECT
public:
	explicit DAUndoHistoryWidget(QWidget* parent = nullptr);
	~DAUndoHistoryWidget();

	// 绑定全局撤销组（APP 挂载时注入；视图自动跟随组内激活栈）
	void setUndoGroup(QUndoGroup* group);
	QUndoGroup* undoGroup() const;

	// 撤销深度（SpinBox 当前值；APP 启动读配置后经 setUndoLimit 注入，不触发信号）
	int undoLimit() const;
	void setUndoLimit(int limit);

Q_SIGNALS:
	// 用户在面板调整了撤销深度（APP 接线：写回配置并施加到全部数据页撤销栈）
	void undoLimitChangeRequested(int limit);

private:
	QUndoView* mView { nullptr };
	QSpinBox* mSpinLimit { nullptr };
	bool mUpdatingLimit { false };  ///< setUndoLimit 期间屏蔽 valueChanged 回环
};
}  // end of namespace DA
#endif  // DAUNDOHISTORYWIDGET_H

/*** End of inlined file: DAUndoHistoryWidget.h ***/

// ---- Property Editing 属性编辑家族 ----

/*** Start of inlined file: DAPropertyItemWidget.h ***/
#ifndef DAPROPERTYITEMWIDGET_H
#define DAPROPERTYITEMWIDGET_H

#include <QFrame>
#include <QString>

namespace DA
{
/**
 * @brief 属性项控件，支持Inline和Below两种布局模式
 *
 * Inline模式：属性名标签在左侧（固定宽度，可换行），编辑Widget在右侧同一行（顶部对齐）
 * Below模式：属性名标签在顶部，编辑Widget在下方占满整行宽度
 *
 * 描述文本作为属性名标签的tooltip显示。
 *
 * @code
 * // Inline模式使用示例
 * DAPropertyItemWidget* item = new DAPropertyItemWidget(this);
 * item->setPropertyName("画笔颜色");
 * item->setPropertyDescription("设置曲线的画笔颜色");
 * item->setEditorWidget(new DAColorPickerButton(this));
 * item->setLayoutMode(DAPropertyItemWidget::InlineLayout);
 * @endcode
 */
class DAWIDGETS_EXPORT DAPropertyItemWidget : public QFrame
{
	Q_OBJECT
	DA_DECLARE_PRIVATE(DAPropertyItemWidget)
	Q_PROPERTY(int propertyId READ propertyId WRITE setPropertyId)
	Q_PROPERTY(QString propertyName READ propertyName WRITE setPropertyName)
	Q_PROPERTY(QString propertyDescription READ propertyDescription WRITE setPropertyDescription)
	Q_PROPERTY(LayoutMode layoutMode READ layoutMode WRITE setLayoutMode)
	Q_PROPERTY(bool frameVisible READ isFrameVisible WRITE setFrameVisible)
public:
	// 布局模式枚举
	enum LayoutMode {
		InlineLayout,  ///< Inline模式：左侧属性名 + 右侧编辑Widget
		BelowLayout    ///< Below模式：顶部属性名 + 下方编辑Widget
	};

	// 构造属性项控件，默认采用 Inline 布局
	explicit DAPropertyItemWidget(QWidget* parent = nullptr);
	// 析构属性项控件
	~DAPropertyItemWidget();

	// 属性ID
	int propertyId() const;
	void setPropertyId(int id);

	// 属性名称
	QString propertyName() const;
	void setPropertyName(const QString& name);

	// 属性描述（作为tooltip）
	QString propertyDescription() const;
	void setPropertyDescription(const QString& desc);

	// 编辑Widget
	QWidget* editorWidget() const;
	void setEditorWidget(QWidget* widget);

	// 布局模式
	LayoutMode layoutMode() const;
	void setLayoutMode(LayoutMode mode);

	// Frame边界可见性
	bool isFrameVisible() const;
	void setFrameVisible(bool visible);

	// 属性名标签宽度（仅Inline模式有效）
	void setNameLabelWidth(int width);

Q_SIGNALS:
	/**
	 * @brief 编辑值变化信号
	 * @param propertyId 属性ID
	 */
	void valueChanged(int propertyId);

private:
	void updateLayout();
};
}  // namespace DA
#endif // DAPROPERTYITEMWIDGET_H

/*** End of inlined file: DAPropertyItemWidget.h ***/



/*** Start of inlined file: DATimeFormatEditWidget.h ***/
#ifndef DATIMEFORMATEDITWIDGET_H
#define DATIMEFORMATEDITWIDGET_H

#include <QWidget>
#include <QDateTime>
class QComboBox;
namespace Ui
{
class DATimeFormatEditWidget;
}

namespace DA
{

///
/// \brief 时间设置窗口 用于设置时间的格式
///
class DAWIDGETS_EXPORT DATimeFormatEditWidget : public QWidget
{
	Q_OBJECT

public:
	// 构造时间格式编辑窗口，预置常用格式选项
	explicit DATimeFormatEditWidget(QWidget* parent = nullptr);
	// 析构，释放 ui 资源
	~DATimeFormatEditWidget();
	// 获取当前选中的时间格式字符串
	QString getTimeFormat() const;
	// 设置时间格式文本，匹配下拉框中已有的选项
	void setTimeFormatText(const QString& str);
private Q_SLOTS:
	void onCurrentTextChanged(const QString& text);

private:
	void setComboxTimeFormat(QComboBox* comb);

private:
	Ui::DATimeFormatEditWidget* ui;
	QDateTime m_currentDateTime;
};
}  // end DA
#endif  // DATIMEFORMATEDITWIDGET_H

/*** End of inlined file: DATimeFormatEditWidget.h ***/


/*** Start of inlined file: DAAligmentEditWidget.h ***/
#ifndef DAALIGMENTEDITWIDGET_H
#define DAALIGMENTEDITWIDGET_H

#include <QWidget>

class QAbstractButton;
namespace Ui
{
class DAAligmentEditWidget;
}
namespace DA
{
/**
 * @brief 对齐编辑窗口
 */
class DAWIDGETS_EXPORT DAAligmentEditWidget : public QWidget
{
	Q_OBJECT

public:
	// 构造对齐编辑窗口，初始化按钮组映射
	explicit DAAligmentEditWidget(QWidget* parent = nullptr);
	// 析构，释放 ui 资源
	~DAAligmentEditWidget();
	// 设置对齐方式
	Qt::Alignment getCurrentAlignment() const;
public Q_SLOTS:
	// 设置对齐，此函数发射alignmentChanged信号
	void setCurrentAlignment(Qt::Alignment al);
Q_SIGNALS:
	/**
	 * @brief 对齐发生变化
	 * @param al
	 */
	void alignmentChanged(Qt::Alignment al);

protected:
	void changeEvent(QEvent* e);
protected Q_SLOTS:
	void onButtonGroupClicked(int id);

private:
	Ui::DAAligmentEditWidget* ui;
};
}  // end da
#endif  // DAALIGMENTEDITWIDGET_H

/*** End of inlined file: DAAligmentEditWidget.h ***/


/*** Start of inlined file: DAAligmentPositionEditWidget.h ***/
#ifndef DAALIGMENTPOSITIONEDITWIDGET_H
#define DAALIGMENTPOSITIONEDITWIDGET_H

#include <QWidget>

namespace Ui
{
class DAAligmentPositionEditWidget;
}
namespace DA
{
/**
 * @brief The DAAligmentPositionEditWidget class
 */
class DAWIDGETS_EXPORT DAAligmentPositionEditWidget : public QWidget
{
	Q_OBJECT

public:
	// 构造对齐位置编辑窗口，初始化九宫格按钮组映射
	explicit DAAligmentPositionEditWidget(QWidget* parent = nullptr);
	// 析构，释放 ui 资源
	~DAAligmentPositionEditWidget();
	// 设置aligment
	Qt::Alignment getAligmentPosition() const;
	// 把输入的al进行过滤，得到本控件支持的组合
	static Qt::Alignment acceptAligment(Qt::Alignment al);
public Q_SLOTS:
	// 设置对齐位置，此函数会发射 aligmentPositionChanged 信号
	void setAligmentPosition(Qt::Alignment al);
Q_SIGNALS:
	/**
	 * @brief 对齐位置变化时发射
	 * @param al 变化后的对齐位置
	 */
	void aligmentPositionChanged(Qt::Alignment al);

protected:
	void changeEvent(QEvent* e);
private Q_SLOTS:
	void onButtonClicked(int id);

private:
	Ui::DAAligmentPositionEditWidget* ui;
};
}  // end DA
#endif  // DAALIGMENTPOSITIONEDITWIDGET_H

/*** End of inlined file: DAAligmentPositionEditWidget.h ***/


/*** Start of inlined file: DATextAlignmentEditWidget.h ***/
#ifndef DATEXTALIGNMENTEDITWIDGET_H
#define DATEXTALIGNMENTEDITWIDGET_H

#include <QWidget>

namespace DA
{
/**
 * @brief 文本对齐编辑控件：水平一组（左/居中/右）+ 垂直一组（顶端/居中/底端）
 *
 * 两行互斥按钮组，各自独立选择；值以 Qt::Alignment 的水平位 + 垂直位组合表示。
 * 通用控件：只认 Qt::Alignment，不含业务语义（表格单元格、图表文本、标签排版均可复用）。
 * 与 DAAligmentEditWidget 的区别：后者是单选语义（水平垂直共用一个按钮组），
 * 本控件把水平与垂直拆成两组，可分别设置（如「右对齐 + 顶端对齐」）。
 */
class DAWIDGETS_EXPORT DATextAlignmentEditWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)
public:
	explicit DATextAlignmentEditWidget(QWidget* parent = nullptr);
	~DATextAlignmentEditWidget() override;

	// 当前对齐（水平位 AlignLeft/AlignHCenter/AlignRight + 垂直位 AlignTop/AlignVCenter/AlignBottom）
	Qt::Alignment alignment() const;
	// 当前水平对齐位
	Qt::Alignment horizontalAlignment() const;
	// 当前垂直对齐位
	Qt::Alignment verticalAlignment() const;

public Q_SLOTS:
	// 设置对齐：缺水平位/垂直位的那一半保持当前值；与当前值相同不发信号
	void setAlignment(Qt::Alignment al);

Q_SIGNALS:
	// 对齐发生变化（用户点击或 setAlignment 改变值时发出）
	void alignmentChanged(Qt::Alignment al);

protected:
	void changeEvent(QEvent* e) override;

private:
	DA_DECLARE_PRIVATE(DATextAlignmentEditWidget)
};
}  // namespace DA
#endif  // DATEXTALIGNMENTEDITWIDGET_H

/*** End of inlined file: DATextAlignmentEditWidget.h ***/


/*** Start of inlined file: DAFilePathEditWidget.h ***/
#ifndef DAFILEPATHEDITWIDGET_H
#define DAFILEPATHEDITWIDGET_H

#include <QWidget>

namespace Ui
{
class DAFilePathEditWidget;
}

namespace DA
{

/**
 * @brief 文件/目录路径编辑窗口，可通过此窗口选中一个路径
 *
 * 形如下面的选择按钮
 * --------------  ----
 * |C:/src      | |    |
 * --------------  ----
 *
 * 三种模式共用同一套「文本框 + 浏览按钮」布局：
 * - OpenFile：选择已存在文件（QFileDialog::ExistingFile）
 * - SaveFile：选择保存路径（AcceptSave + 默认后缀，保留 Qt 自带覆盖确认）
 * - ExistingDirectory：选择目录（Directory + ShowDirsOnly）
 */
class DAWIDGETS_EXPORT DAFilePathEditWidget : public QWidget
{
	Q_OBJECT
	DA_DECLARE_PRIVATE(DAFilePathEditWidget)
public:
	/// 浏览模式
	enum class Mode
	{
		OpenFile,          ///< 选择已存在文件
		SaveFile,          ///< 选择保存文件路径
		ExistingDirectory  ///< 选择已存在目录
	};

	// 构造文件路径编辑窗口
	explicit DAFilePathEditWidget(QWidget* parent = nullptr);
	// 析构，释放 ui 资源
	~DAFilePathEditWidget();

	// 设置过滤
	void setNameFilter(const QString& filter);
	void setNameFilters(const QStringList& filters);
	QStringList nameFilters() const;

	// 设置/获取浏览模式（默认 OpenFile）
	void setMode(Mode m);
	Mode mode() const;

	// 设置/获取文件对话框的起始目录（空表示由对话框决定）
	void setStartDirectory(const QString& dir);
	QString startDirectory() const;

	// 设置/获取 SaveFile 模式的默认后缀（不带点，如 "txt"）
	void setDefaultSuffix(const QString& suffix);
	QString defaultSuffix() const;

	// 获取路径
	QString getFilePath() const;
	void setFilePath(const QString& v);
	// 文本框占位提示
	void setPlaceholderText(const QString& t);
Q_SIGNALS:
	/**
	 * @brief 选中路径后会发射此信号
	 * @param p 路径
	 */
	void selectedPath(const QString& p);
	/**
	 * @brief 文本框内容每次变化都发射（用于设置页即时标记 dirty）
	 * @param p 当前文本
	 */
	void pathChanged(const QString& p);
private Q_SLOTS:
	void onToolButtonOpenClicked();
	void onLineEditEditingFinished();

protected:
	void changeEvent(QEvent* e);

private:
	Ui::DAFilePathEditWidget* ui;
};
}

#endif  // DAFILEPATHEDITWIDGET_H

/*** End of inlined file: DAFilePathEditWidget.h ***/


/*** Start of inlined file: DATextEditWidget.h ***/
#ifndef DATEXTEDITWIDGET_H
#define DATEXTEDITWIDGET_H

#include <QString>
#include <QWidget>

class QAction;
class QPlainTextEdit;
class QToolBar;

namespace DA
{

/**
 * @brief 通用文本编辑/查看控件（只读、等宽、换行、工具动作）
 *
 * 内容默认做成 QWidget：不拥有 OK/Cancel/Close，需要模态时由调用方套 10 行
 * QDialog 壳（标题 + 本控件 + QDialogButtonBox）。
 *
 * 工具动作由调用方注入（如“另存为”），控件只负责展示与转发触发；
 * 不内置任何业务动作。
 */
class DAWIDGETS_EXPORT DATextEditWidget : public QWidget
{
	Q_OBJECT
	DA_DECLARE_PRIVATE(DATextEditWidget)
public:
	explicit DATextEditWidget(QWidget* parent = nullptr);
	~DATextEditWidget();

	// 只读
	void setReadOnly(bool on);
	bool isReadOnly() const;

	// 等宽字体
	void setMonospace(bool on);
	bool isMonospace() const;

	// 自动换行
	void setLineWrapEnabled(bool on);
	bool isLineWrapEnabled() const;

	// 文本（setText 不发 textChanged）
	void setText(const QString& text);
	QString text() const;

	// 占位提示
	void setPlaceholderText(const QString& t);

	// 工具条显隐
	void setToolbarVisible(bool on);
	bool isToolbarVisible() const;
	// 注入工具动作（加入工具条后由调用方负责生命周期）
	void addToolAction(QAction* action);

Q_SIGNALS:
	/**
	 * @brief 用户编辑导致文本变化（setText 不发射）
	 */
	void textChanged();

private:
	Q_DISABLE_COPY(DATextEditWidget)
};
}  // namespace DA

#endif  // DATEXTEDITWIDGET_H

/*** End of inlined file: DATextEditWidget.h ***/


/*** Start of inlined file: DABrushStyleComboBox.h ***/
#ifndef DABRUSHSTYLECOMBOBOX_H
#define DABRUSHSTYLECOMBOBOX_H

#include <QComboBox>
#include <QAbstractItemDelegate>
#include <QBrush>
#include <QGradient>
#include <QPixmap>
namespace DA
{
/**
 * @brief Qt::BrushStyle for QComboBox
 * @todo 目前仅实现常规填充，渐变的还未实现
 */
class DAWIDGETS_EXPORT DABrushStyleComboBox : public QComboBox
{
	Q_OBJECT
	DA_DECLARE_PRIVATE(DABrushStyleComboBox)
public:
	// 默认构造以ShowNoBrushStyle|ShowPaintStyle构造，基本颜色为黑色
	DABrushStyleComboBox(QWidget* parent = nullptr);
	// 析构函数，释放私有数据
	~DABrushStyleComboBox();
	// 画笔样式转换为字符串
	static QString brushStyleToString(Qt::BrushStyle s);
	// 显示无画刷
	void setShowNoBrushStyle(bool v);
	// 是否显示了无画刷选项
	bool isShowNoBrushStyle() const;
	// 设置画刷颜色，此设置会影响ShowPaintStyle
	bool isColorBrush() const;
	void setBrushColor(const QColor& v);
	// 获取当前用于渲染样式的画刷颜色
	QColor getBrushColor() const;
	// 重建item
	void rebuildItems();
	// 生成icon
	QIcon generateBrushStyleIcon(Qt::BrushStyle s);
	// 更新icon
	void updateIcons();
	// 是否在样式上显示文字
	void setStyleTextVisible(bool on);
	// 返回是否在样式上显示文字
	bool isStyleTextVisible() const;
	// 获取当前的画刷样式
	Qt::BrushStyle getCurrentBrushStyle() const;
	// 获取画刷样式
	Qt::BrushStyle getBrushStyle(int index) const;
public Q_SLOTS:
	// 设置当前的样式 此函数会发射currentBrushStyleChanged信号
	void setCurrentBrushStyle(Qt::BrushStyle s);
Q_SIGNALS:
	/**
	 * @brief 画刷样式发生变更
	 * @param s
	 */
	void currentBrushStyleChanged(Qt::BrushStyle s);
private Q_SLOTS:
	// 原信号对应的槽
	void onCurrentIndexChanged(int index);

protected:
	void addItem(Qt::BrushStyle s);

private:
};
}

#endif  // DABRUSHSTYLECOMBOBOX_H

/*** End of inlined file: DABrushStyleComboBox.h ***/


/*** Start of inlined file: DAPenStyleComboBox.h ***/
#ifndef DAPENSTYLECOMBOBOX_H
#define DAPENSTYLECOMBOBOX_H

#include <QComboBox>
#include <QAbstractItemDelegate>
#define DAPENSTYLECOMBOBOX_USE_DELEGATE 0

namespace DA
{

/**
 * @brief Qt::PenStyle for QComboBox
 */
class DAWIDGETS_EXPORT DAPenStyleComboBox : public QComboBox
{
	Q_OBJECT
#if !DAPENSTYLECOMBOBOX_USE_DELEGATE
	DA_DECLARE_PRIVATE(DAPenStyleComboBox)
#endif
public:
	// 构造函数
	DAPenStyleComboBox(QWidget* parent = nullptr);
	// 析构函数，释放私有数据
	~DAPenStyleComboBox();

	// 画笔样式转换为字符串
	static QString penStyleToString(Qt::PenStyle s);

	// 生成icon
	QIcon generatePenIcon(Qt::PenStyle s) const;
	// 绘制画笔
	static void drawPenStyle(QPainter* painter, const QRect& rect, const QPen& pen);
	// 刷新所有item
	void updateItems();
	// 是否在样式上显示文字
	void setStyleTextVisible(bool on);
	// 返回是否在样式上显示文字
	bool isStyleTextVisible() const;
	// 重建所有items
	void rebuildItems();
	// 获取当前的画笔类型
	Qt::PenStyle getCurrentPenStyle() const;

public Q_SLOTS:
	// 设置画笔
	void setPen(const QPen& p);
	// 设置绘制的线条的颜色
	void setPenColor(const QColor& c);
	// 设置绘制的画笔宽度
	void setPenLineWidth(int w);
	// 设置当前的画笔样式
	void setCurrentPenStyle(Qt::PenStyle s);

protected:
	// 添加item
	void addItem(Qt::PenStyle s);
private Q_SLOTS:
	// 当下拉框索引变化时同步发射画笔样式变更信号
	void onCurrentIndexChanged(int index);
Q_SIGNALS:
	/**
	 * @brief 画笔样式改变
	 * @param s
	 */
	void currentPenStyleChanged(Qt::PenStyle s);
};

}  // namespace DA
#endif  // DAPENSTYLECOMBOBOX_H

/*** End of inlined file: DAPenStyleComboBox.h ***/


/*** Start of inlined file: DAColorGridWidget.h ***/
#ifndef DACOLORGRIDWIDGET_H
#define DACOLORGRIDWIDGET_H

#include <QWidget>
#include <QList>
#include <QColor>

class QToolButton;
class QGridLayout;
class QButtonGroup;
class QSpacerItem;

namespace DA
{
/**
 * @brief 基础颜色网格控件。
 *
 * 使用 QGridLayout + QButtonGroup(exclusive) 排列一组颜色色块按钮,
 * 每个色块为 QToolButton(IconOnly),颜色通过 QPixmap icon 显示。
 *
 * 是 DAColorPaletteGridWidget 和 DAColorMenu 的基础构件。
 */
class DAWIDGETS_EXPORT DAColorGridWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
	DA_DECLARE_PRIVATE(DAColorGridWidget)
public:
	using FunColorBtn = std::function<void(QToolButton*)>;

	// 构造颜色网格控件
	explicit DAColorGridWidget(QWidget* par = nullptr);
	// 析构函数
	~DAColorGridWidget();

	// 设置网格列数，颜色按钮按行优先排列
	void setColumnCount(int c);
	// 获取网格列数
	int columnCount() const;

	// 设置颜色列表，会触发网格重建
	void setColorList(const QList<QColor>& cls);
	// 获取当前颜色列表
	QList<QColor> getColorList() const;

	// 获取颜色间距（水平和垂直的统一值）
	int spacing() const;
	// 设置颜色间距（水平和垂直统一设置）
	void setSpacing(int v);

	// 获取颜色数量
	int colorCount() const;

	// 设置色块图标尺寸
	void setColorIconSize(const QSize& s);
	// 获取色块图标尺寸
	QSize colorIconSize() const;

	// 设置色块是否可选中（checkable）
	void setColorCheckable(bool on = true);
	// 返回色块是否可选中
	bool isColorCheckable() const;

	// 获取当前被选中的颜色（基于buttonGroup的checked状态）
	QColor currentCheckedColor() const;
	// 根据索引获取对应位置的色块按钮
	QToolButton* colorButton(int index) const;

	// 设置垂直间距
	void setVerticalSpacing(int v);
	// 获取垂直间距
	int verticalSpacing() const;
	// 设置水平间距
	void setHorizontalSpacing(int v);
	// 获取水平间距
	int horizontalSpacing() const;

	// 清除所有色块的选中状态
	void clearCheckedState();

	// 遍历所有色块按钮并执行回调函数
	void iterationColorBtns(FunColorBtn fn);

	// 设置指定行的最小高度
	void setRowMinimumHeight(int row, int minSize);
	// 在右侧添加水平弹簧，使色块左对齐
	void setHorizontalSpacerToRight(bool on = true);

	// 推荐尺寸
	QSize sizeHint() const override;

Q_SIGNALS:
	/**
	 * @brief 颜色被点击时发射
	 * @param c 被点击的颜色
	 */
	void colorClicked(const QColor& c);
	/**
	 * @brief 颜色按钮被按下时发射
	 * @param c 被按下的颜色
	 */
	void colorPressed(const QColor& c);
	/**
	 * @brief 颜色按钮被释放时发射
	 * @param c 被释放的颜色
	 */
	void colorReleased(const QColor& c);
	/**
	 * @brief 颜色按钮选中状态切换时发射
	 * @param c 切换的颜色
	 * @param on 是否选中
	 */
	void colorToggled(const QColor& c, bool on);

private:
	void updateGridColor(bool isRemoveSpacer = false);
	void setColorAt(const QColor& clr, int r, int c);
	static QIcon makeColorIcon(const QColor& clr, const QSize& size);
};
}  // namespace DA

#endif  // DACOLORGRIDWIDGET_H

/*** End of inlined file: DAColorGridWidget.h ***/


/*** Start of inlined file: DAColorPaletteGridWidget.h ***/
#ifndef DACOLORPALETTEGRIDWIDGET_H
#define DACOLORPALETTEGRIDWIDGET_H

#include <QWidget>
#include <QList>
#include <QColor>

namespace DA
{
class DAColorGridWidget;

/**
 * @brief Office 风格色板控件。
 *
 * 上方一行标准主色,下方 5 行(3 行浅色 + 2 行深色)由主色通过 lighter 因子
 * 自动生成的调色板。内部组合两个 DAColorGridWidget。
 *
 * 默认主色列表为 10 色 Office 风格(深红/红/橙/黄/黄绿/绿/浅蓝/蓝/深蓝/紫)。
 */
class DAWIDGETS_EXPORT DAColorPaletteGridWidget : public QWidget
{
	Q_OBJECT
	DA_DECLARE_PRIVATE(DAColorPaletteGridWidget)
public:
	// 构造默认Office风格色板
	explicit DAColorPaletteGridWidget(QWidget* par = nullptr);
	// 构造自定义主色色板
	explicit DAColorPaletteGridWidget(const QList<QColor>& cls, QWidget* par = nullptr);
	// 析构函数
	~DAColorPaletteGridWidget();

	// 设置主色列表，会重新生成调色板
	void setColorList(const QList<QColor>& cls);
	// 获取主色列表
	QList<QColor> colorList() const;

	// 设置浅色/深色变体的 lighter/darker 因子列表
	void setFactor(const QList<int>& factor);
	// 获取浅色/深色变体因子列表
	QList<int> factor() const;

	// 设置色块图标尺寸
	void setColorIconSize(const QSize& s);
	// 获取色块图标尺寸
	QSize colorIconSize() const;

	// 设置色块是否可选中
	void setColorCheckable(bool on = true);
	// 返回色块是否可选中
	bool isColorCheckable() const;

Q_SIGNALS:
	/**
	 * @brief 点击任意色块时发射
	 * @param c 被点击的颜色
	 */
	void colorClicked(const QColor& c);

private:
	void init();
	QList<QColor> makeColorPalette(const QList<QColor>& cls) const;
	void onMainColorClicked(const QColor& c);
	void onPaletteColorClicked(const QColor& c);
};
}  // namespace DA

#endif  // DACOLORPALETTEGRIDWIDGET_H

/*** End of inlined file: DAColorPaletteGridWidget.h ***/


/*** Start of inlined file: DAColorMenu.h ***/
#ifndef DACOLORMENU_H
#define DACOLORMENU_H

#include <QMenu>
#include <QColor>
#include <QList>

class QLabel;
class QWidgetAction;
class QColorDialog;

namespace DA
{
class DAColorPaletteGridWidget;
class DAColorGridWidget;

/**
 * @brief 颜色选择菜单。
 *
 * 结构:标题 Label → 主题色板(DAColorPaletteGridWidget)→ (可选)"None" Action →
 * "Custom Color" Action → 自定义颜色记录网格(DAColorGridWidget)。
 *
 * 选色后发射 selectedColor 信号并自动隐藏菜单。
 * "Custom Color" 弹出 QColorDialog,选中的颜色记录到自定义色网格(最多 10 个,FIFO)。
 */
class DAWIDGETS_EXPORT DAColorMenu : public QMenu
{
	Q_OBJECT
	DA_DECLARE_PRIVATE(DAColorMenu)
public:
	// 构造默认主题色的颜色菜单
	explicit DAColorMenu(QWidget* parent = nullptr);
	// 构造带标题的颜色菜单
	explicit DAColorMenu(const QString& title, QWidget* parent = nullptr);
	// 析构函数
	~DAColorMenu();

	// 获取主题色板的 QWidgetAction
	QWidgetAction* themeColorsPaletteAction() const;
	// 获取自定义颜色记录网格的 QWidgetAction
	QWidgetAction* getCustomColorsWidgetAction() const;
	// 获取"自定义颜色"Action（点击弹出QColorDialog）
	QAction* customColorAction() const;

	// 获取主题色板网格控件
	DAColorPaletteGridWidget* colorPaletteGridWidget() const;
	// 获取自定义颜色记录网格控件
	DAColorGridWidget* customColorsWidget() const;

	// 启用或禁用"无颜色"Action
	void enableNoneColorAction(bool on = true);
	// 获取"无颜色"Action
	QAction* noneColorAction() const;

public Q_SLOTS:
	// 发射 selectedColor 信号并隐藏菜单
	void emitSelectedColor(const QColor& c);

Q_SIGNALS:
	/**
	 * @brief 用户选色后发射
	 * @param c 选中的颜色，无效颜色表示"无颜色"
	 */
	void selectedColor(const QColor& c);

private:
	void init(const QList<QColor>& themeCls);
	void onCustomColorActionTriggered(bool on = false);
	void onNoneColorActionTriggered(bool on = false);
	QColor getColorByDialog();
	void recordCustomColor(const QColor& c);
	static QIcon createNoneColorIcon();
};
}  // namespace DA

#endif  // DACOLORMENU_H

/*** End of inlined file: DAColorMenu.h ***/


/*** Start of inlined file: DAColorPickerButton.h ***/
#ifndef DACOLORPICKERBUTTON_H
#define DACOLORPICKERBUTTON_H

#include <QToolButton>
#include <QColor>
#include <QMargins>

class QStylePainter;
class QStyleOptionToolButton;

namespace DA
{
class DAColorMenu;

/**
 * @brief 颜色拾取按钮。
 *
 * 继承 QToolButton,完全自绘:在按钮区域内绘制颜色块(可选图标+文本+颜色条布局)。
 * 默认 WithColorMenu 模式自动创建 DAColorMenu,点击弹出选色;
 * NoColorMenu 模式下点击发射 colorClicked 信号,由外部处理选色逻辑。
 *
 * 无效颜色(QColor())显示为红色斜杠+黑框的"无色"标记。
 *
 * qss 定制示例:
 * ```css
 * DA--DAColorPickerButton { border:1px solid #dcdfe6; border-radius:3px; }
 * DA--DAColorPickerButton:hover { border:1px solid #409eff; }
 * ```
 */
class DAWIDGETS_EXPORT DAColorPickerButton : public QToolButton
{
	Q_OBJECT
	DA_DECLARE_PRIVATE(DAColorPickerButton)
public:
	/// 按钮样式:是否自带颜色菜单
	enum ColorToolButtonStyle {
		WithColorMenu,  ///< 默认:自动构建 DAColorMenu
		NoColorMenu     ///< 不带颜色菜单
	};

	explicit DAColorPickerButton(QWidget* parent = nullptr);
	// 构造指定样式的颜色拾取按钮
	explicit DAColorPickerButton(ColorToolButtonStyle style, QWidget* parent = nullptr);
	// 析构函数
	~DAColorPickerButton();

	// 获取当前颜色
	QColor color() const;

	// 获取绘制边距
	QMargins margins() const;
	// 设置绘制边距，控制颜色块与按钮边框的间距
	void setMargins(const QMargins& mg);

	// 获取按钮样式模式
	ColorToolButtonStyle colorToolButtonStyle() const;
	// 设置按钮样式模式，决定是否自动创建颜色菜单
	void setColorToolButtonStyle(ColorToolButtonStyle s);

	// 获取关联的颜色菜单（WithColorMenu模式下自动创建）
	DAColorMenu* colorMenu() const;
	// 创建并关联颜色菜单，返回创建的菜单指针
	DAColorMenu* createColorMenu();

	/// 绘制"无颜色"标记:红色对角线+黑色边框
	static void paintNoneColor(QPainter* p, const QRect& colorRect);

Q_SIGNALS:
	/**
	 * @brief 按钮被点击时发射（NoColorMenu模式下使用）
	 * @param color 当前颜色
	 * @param checked 按钮是否处于选中状态
	 */
	void colorClicked(const QColor& color, bool checked = false);
	/**
	 * @brief 颜色改变时发射
	 * @param color 新颜色
	 */
	void colorChanged(const QColor& color);

public Q_SLOTS:
	void setColor(const QColor& c);

protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	QSize sizeHint() const override;

	virtual void calcRect(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
	virtual void paintButton(QStylePainter* p, const QStyleOptionToolButton& opt);
	virtual void paintIcon(QStylePainter* p, const QRect& iconRect, const QStyleOptionToolButton& opt);
	virtual void paintText(QStylePainter* p, const QRect& textRect, const QStyleOptionToolButton& opt);
	virtual void paintColor(QStylePainter* p, const QRect& colorRect, const QColor& color, const QStyleOptionToolButton& opt);

private:
	void onButtonClicked(bool checked = false);
};
}  // namespace DA

#endif  // DACOLORPICKERBUTTON_H

/*** End of inlined file: DAColorPickerButton.h ***/


/*** Start of inlined file: DAPenEditWidget.h ***/
#ifndef DAPENEDITWIDGET_H
#define DAPENEDITWIDGET_H

#include <QWidget>
#include <QPen>

namespace DA
{
/**
 * @brief 画笔编辑窗口
 * 颜色按钮+线型选择+线宽
 * ┏┓┍━━━━┓┍━━┓
 * ┗┛┗━━━━┛┗━━┛
 */
class DAWIDGETS_EXPORT DAPenEditWidget : public QWidget
{
	Q_OBJECT
	DA_DECLARE_PRIVATE(DAPenEditWidget)
public:
	// 构造函数
	explicit DAPenEditWidget(QWidget* parent = nullptr);
	// 构造并设置初始画笔
	explicit DAPenEditWidget(const QPen& p, QWidget* parent = nullptr);
	// 析构函数
	~DAPenEditWidget();
	// 重新翻译UI文本（多语言支持）
	void retranslateUi();
	// 获取当前的画笔
	QPen getCurrentPen() const;
	// 是否在样式上显示文字
	void setLineStyleTextVisible(bool on);
	// 返回是否在样式上显示文字
	bool isLineStyleTextVisible() const;
public Q_SLOTS:
	// 设置画笔,设置画笔会触发penChanged信号
	void setCurrentPen(const QPen& p);
Q_SIGNALS:
	/**
	 * @brief 画笔改变信号
	 * @param p
	 */
	void penChanged(const QPen& p);

protected Q_SLOTS:
	void onColorChanged(const QColor& c);
	void onPenWidthValueChanged(double w);
	void onPenStyleChanged(Qt::PenStyle s);

private:
	void initConnect();
};
}  // namespace DA
#endif  // DAPENEDITWIDGET_H

/*** End of inlined file: DAPenEditWidget.h ***/


/*** Start of inlined file: DABrushEditWidget.h ***/
#ifndef DABRUSHEDITWIDGET_H
#define DABRUSHEDITWIDGET_H

#include <QWidget>
#include <QBrush>
#include <QGradient>
#include <QPixmap>

namespace DA
{
class DAColorMenu;

/**
 * @brief 画刷编辑窗口
 *
 * 颜色按钮 + 画刷样式下拉框。支持纯色、图案、渐变(Linear/Radial/Conical)、
 * 纹理(QPixmap)四种填充模式。
 *
 * - 纯色/图案:颜色按钮选色,下拉框选 NoBrush/SolidPattern/Dense1-7/Hor/Ver/Cross/Diag 等
 * - 渐变:下拉框选 LinearGradientPattern/RadialGradientPattern/ConicalGradientPattern,
 *   自动弹出渐变编辑对话框(类型+起始色+结束色+预览)
 * - 纹理:下拉框选 TexturePattern,自动弹出文件对话框选择图片
 *
 * 颜色与样式联动:颜色有效时若样式为 NoBrush 自动改为 SolidPattern;
 * 颜色无效(清空)时样式自动改为 NoBrush。
 */
class DAWIDGETS_EXPORT DABrushEditWidget : public QWidget
{
	Q_OBJECT

public:
	// 构造画刷编辑窗口
	explicit DABrushEditWidget(QWidget* parent = nullptr);
	// 析构函数
	~DABrushEditWidget();

	// 获取当前画刷
	QBrush getCurrentBrush() const;
	// 设置是否在样式上显示文字
	void setStyleTextVisible(bool on);
	// 返回是否在样式上显示文字
	bool isStyleTextVisible() const;
	// 获取当前画刷样式
	Qt::BrushStyle getCurrentBrushStyle() const;
	// 获取当前画刷颜色（纯色/图案模式下有效）
	QColor getCurrentBrushColor() const;

public Q_SLOTS:
	// 设置当前画刷，支持纯色/图案/渐变/纹理四种模式
	void setCurrentBrush(const QBrush& b);
	// 设置当前画刷样式
	void setCurrentBrushStyle(Qt::BrushStyle s);

Q_SIGNALS:
	/**
	 * @brief 画刷改变时发射
	 * @param b 新画刷
	 */
	void brushChanged(const QBrush& b);

private Q_SLOTS:
	void onColorChanged(const QColor& c);
	void onBrushStyleChanged(Qt::BrushStyle s);

private:
	class PrivateData;
	PrivateData* d;
	void editGradient(Qt::BrushStyle style);
	void editTexture();
};
}  // namespace DA

#endif  // DABRUSHEDITWIDGET_H

/*** End of inlined file: DABrushEditWidget.h ***/


/*** Start of inlined file: DAFontEditPannelWidget.h ***/
#ifndef DAFONTEDITPANNELWIDGET_H
#define DAFONTEDITPANNELWIDGET_H

#include <QWidget>
#include <QColor>
#include <QFont>

namespace DA
{
/**
 * @brief 字体编辑面板
 *
 * 字体选择 + 字号 + 粗体/斜体/下划线 + 字体颜色。
 * 通过 currentFontChanged / currentFontColorChanged 信号通知变更。
 */
class DAWIDGETS_EXPORT DAFontEditPannelWidget : public QWidget
{
	Q_OBJECT
public:
	// 布局模式：TwoRowsLayout为默认两排布局，SingleRowLayout为单排布局（适合工具栏）
	enum LayoutMode
	{
		TwoRowsLayout,   ///< 两排布局：第一排字体族，第二排样式按钮+字号（默认）
		SingleRowLayout  ///< 单排布局：所有控件一行排列，适合工具栏等高度受限场景
	};
	Q_ENUM(LayoutMode)
	Q_PROPERTY(LayoutMode layoutMode READ layoutMode WRITE setLayoutMode)

public:
	// 构造字体编辑面板
	explicit DAFontEditPannelWidget(QWidget* parent = nullptr);
	// 析构函数
	~DAFontEditPannelWidget();

	// 设置布局模式，默认TwoRowsLayout，切换时重建布局
	void setLayoutMode(LayoutMode m);
	// 获取布局模式
	LayoutMode layoutMode() const;

	// 获取当前字体颜色
	QColor getCurrentFontColor() const;
	// 设置当前字体颜色
	void setCurrentFontColor(const QColor& c);

	// 设置当前字体
	void setCurrentFont(const QFont& f);
	// 获取当前字体
	QFont getCurrentFont();

	// 设置字体粗细（Qt5用0-99，Qt6用QFont::Weight枚举，内部做兼容处理）
	void setFontWeight(int w);
	// 获取字体粗细
	int getFontWeight() const;

private Q_SLOTS:
	void onComboBoxFontSizeTextChanged(const QString& t);
	void onFontSizeAdd();
	void onFontSizeSub();
	void signalEmitFontChanged();

Q_SIGNALS:
	/**
	 * @brief 字体改变时发射
	 * @param f 新字体
	 */
	void currentFontChanged(const QFont& f);
	/**
	 * @brief 字体颜色改变时发射
	 * @param c 新颜色
	 */
	void currentFontColorChanged(const QColor& c);

private:
	class PrivateData;
	PrivateData* d;
};
}  // namespace DA

#endif  // DAFONTEDITPANNELWIDGET_H

/*** End of inlined file: DAFontEditPannelWidget.h ***/


/*** Start of inlined file: DAShapeEditPannelWidget.h ***/
#ifndef DASHAPEEDITPANNELWIDGET_H
#define DASHAPEEDITPANNELWIDGET_H

#include <QWidget>
#include <QBrush>
#include <QPen>

namespace DA
{
/**
 * @brief 形状编辑面板
 *
 * 组合 DABrushEditWidget(背景填充)和 DAPenEditWidget(边框画笔),
 * 发射 backgroundBrushChanged / borderPenChanged 信号。
 */
class DAWIDGETS_EXPORT DAShapeEditPannelWidget : public QWidget
{
	Q_OBJECT

public:
	// 构造形状编辑面板
	explicit DAShapeEditPannelWidget(QWidget* parent = nullptr);
	// 析构函数
	~DAShapeEditPannelWidget();

	// 获取当前背景画刷
	QBrush getBackgroundBrush() const;
	// 获取当前边框画笔
	QPen getBorderPen() const;

public Q_SLOTS:
	// 设置背景画刷
	void setBackgroundBrush(const QBrush& b);
	// 设置边框画笔
	void setBorderPen(const QPen& v);

Q_SIGNALS:
	/**
	 * @brief 背景画刷改变时发射
	 * @param b 新画刷
	 */
	void backgroundBrushChanged(const QBrush& b);
	/**
	 * @brief 边框画笔改变时发射
	 * @param p 新画笔
	 */
	void borderPenChanged(const QPen& p);

private:
	class PrivateData;
	PrivateData* d;
};
}  // namespace DA

#endif  // DASHAPEEDITPANNELWIDGET_H

/*** End of inlined file: DAShapeEditPannelWidget.h ***/

// ---- Item Selection & Preview 条目选择与预览家族 ----

/*** Start of inlined file: DAItemListEditWidget.h ***/
#ifndef DAITEMLISTEDITWIDGET_H
#define DAITEMLISTEDITWIDGET_H

#include <QWidget>
#include <QString>
#include <QVariant>
#include <QVector>

namespace DA
{

/**
 * @brief 通用候选/选中条目（text 展示、data 标识）
 *
 * data 无效时按 text 判同；调用方一般用 data 携带位置索引等真实标识，
 * 避免同名条目（如重名列）混淆。
 */
struct DAWIDGETS_EXPORT DAItemEntry
{
	QString text;    ///< 展示文本
	QVariant data;   ///< 标识数据（无效则退化为按 text 判同）
	bool operator==(const DAItemEntry& o) const;
	bool operator!=(const DAItemEntry& o) const;
};

/**
 * @brief 通用条目选择编辑控件（选中之列表 + 添加 + 移除所有）
 *
 * 面向「从一组候选中挑选若干条」的场景（典型如表格列选择）：
 * - 列表展示已选条目，每行右端一个删除按钮，单击即移除该条目；
 * - 添加按钮为带菜单的 QToolButton（MenuButtonPopup）：点箭头下拉候选菜单
 *   （候选 = availableItems 中尚未选中者，可由调用方设定哪些条目出现），
 *   单击按钮主体弹出 DAItemListPickDialog（可搜索、可多选一次性添加）；
 * - 「移除所有」按钮清空列表，带二次确认。
 *
 * 控件只认 DAItemEntry（text + data），不绑定任何业务语义，列选择/字段选择等
 * 场景均可复用；条目变化统一发 selectedItemsChanged()。
 */
class DAWIDGETS_EXPORT DAItemListEditWidget : public QWidget
{
	Q_OBJECT
	DA_DECLARE_PRIVATE(DAItemListEditWidget)
public:
	explicit DAItemListEditWidget(QWidget* parent = nullptr);
	~DAItemListEditWidget();

	// 设置候选条目（添加菜单与添加对话框的数据源；通常一次性给全量）
	void setAvailableItems(const QVector< DAItemEntry >& items);
	// 获取候选条目
	QVector< DAItemEntry > availableItems() const;

	// 设置已选条目（整体替换，不发 selectedItemsChanged）
	void setSelectedItems(const QVector< DAItemEntry >& items);
	// 获取已选条目（列表顺序 = 添加顺序）
	QVector< DAItemEntry > selectedItems() const;

	// 显示 Up/Down 按钮（按钮移动当前行，不使用拖拽；默认关闭）
	void setReorderEnabled(bool on);
	bool isReorderEnabled() const;

	// 覆盖默认文案（集成侧 i18n 用；默认为英文 tr 文本）
	void setAddButtonText(const QString& t);
	void setRemoveAllButtonText(const QString& t);
	// 覆盖「移除所有」二次确认的标题与正文
	void setRemoveAllConfirmText(const QString& title, const QString& text);

Q_SIGNALS:
	/**
	 * @brief 已选条目变化（增/删/清空均发射；setSelectedItems 不发射）
	 */
	void selectedItemsChanged();

private:
	Q_DISABLE_COPY(DAItemListEditWidget)
};
}  // namespace DA

#endif  // DAITEMLISTEDITWIDGET_H

/*** End of inlined file: DAItemListEditWidget.h ***/



/*** Start of inlined file: DAItemListPickDialog.h ***/
#ifndef DAITEMLISTPICKDIALOG_H
#define DAITEMLISTPICKDIALOG_H

#include <QDialog>
#include <QVector>

namespace DA
{

/**
 * @brief 通用条目勾选对话框（搜索 + 多选一次性添加）
 *
 * 配合 DAItemListEditWidget 的添加按钮使用：列出候选条目（复选框列表），
 * 顶部搜索框按展示文本子串过滤（大小写不敏感，过滤不丢失勾选状态），
 * 确定后由 checkedEntries() 取回勾中的条目。
 */
class DAWIDGETS_EXPORT DAItemListPickDialog : public QDialog
{
	Q_OBJECT
	DA_DECLARE_PRIVATE(DAItemListPickDialog)
public:
	explicit DAItemListPickDialog(QWidget* parent = nullptr);
	~DAItemListPickDialog();

	// 设置候选条目（全部未勾选）
	void setItems(const QVector< DAItemEntry >& items);
	// 预勾选若干条目（按 DAItemEntry 相等匹配）
	void setCheckedEntries(const QVector< DAItemEntry >& entries);
	// 取回勾中的条目（列表顺序）
	QVector< DAItemEntry > checkedEntries() const;

	// 覆盖默认文案（集成侧 i18n 用）
	void setSearchPlaceholderText(const QString& t);

private:
	Q_DISABLE_COPY(DAItemListPickDialog)
};
}  // namespace DA

#endif  // DAITEMLISTPICKDIALOG_H

/*** End of inlined file: DAItemListPickDialog.h ***/


/*** Start of inlined file: DACheckListWidget.h ***/
#ifndef DACHECKLISTWIDGET_H
#define DACHECKLISTWIDGET_H

#include <QWidget>
#include <QVector>

namespace DA
{

/**
 * @brief 通用勾选列表（搜索 + 勾选 + 全选/反选）
 *
 * 面向「从候选里挑选若干条」的平铺列表场景（合并列、分组键、列可见性、
 * 子 Agent 工具白名单等）。控件只认 DAItemEntry（text + data），不绑定业务语义。
 *
 * 勾选语义（调用方必须按此理解）：
 * 1. forced-checked 条目永远出现在 checkedEntries()，用户不能取消，全选/反选都不动它；
 * 2. 全选只作用于「当前可见 + 可勾选 + 非 forced + 可用」的条目，反选同理；
 * 3. 搜索只隐藏行，不改勾选状态；被过滤掉的已勾选条目仍然出现在 checkedEntries() 里；
 * 4. setEntryEnabled(false) 且非 forced 的条目不能被勾选；若已勾选，控件保留状态，
 *    是否清理由调用方用 forced 语义表达，控件不猜；
 * 5. setEntryCheckable(false) 的条目没有复选框、按未勾选处理，checkedEntries() 不含它；
 * 6. setItems/setCheckedEntries 整体替换，不发 checkedEntriesChanged；用户操作才发。
 */
class DAWIDGETS_EXPORT DACheckListWidget : public QWidget
{
	Q_OBJECT
	DA_DECLARE_PRIVATE(DACheckListWidget)
public:
	explicit DACheckListWidget(QWidget* parent = nullptr);
	~DACheckListWidget();

	// 设置候选条目（全部未勾选、可用、可勾选、非 forced；不发信号）
	void setItems(const QVector< DAItemEntry >& items);
	// 获取全部候选条目（含过滤掉的、禁用的）
	QVector< DAItemEntry > items() const;
	// 预勾选若干条目（按 DAItemEntry 相等匹配，forced 条目始终勾选；不发信号）
	void setCheckedEntries(const QVector< DAItemEntry >& entries);
	// 获取勾中的条目（列表顺序；含被过滤掉/禁用的；含 forced）
	QVector< DAItemEntry > checkedEntries() const;

	// 条目可用性（不可用的条目不能勾选）
	void setEntryEnabled(const DAItemEntry& e, bool enabled);
	// 条目是否有复选框（false 时按未勾选处理）
	void setEntryCheckable(const DAItemEntry& e, bool checkable);
	// 强制勾选：永远勾选且用户不能取消，全选/反选不动它
	void setEntryForcedChecked(const DAItemEntry& e, bool on);

	// 搜索框显隐与占位文本
	void setSearchVisible(bool on);
	void setSearchPlaceholderText(const QString& t);
	// 全选/反选按钮显隐
	void setSelectAllVisible(bool on);
	void setInvertVisible(bool on);

	// 条目总数 / 勾选总数
	int itemCount() const;
	int checkedCount() const;
Q_SIGNALS:
	/**
	 * @brief 用户操作导致勾选变化（全选/反选合并为一次）
	 */
	void checkedEntriesChanged();

private:
	Q_DISABLE_COPY(DACheckListWidget)
};
}  // namespace DA

#endif  // DACHECKLISTWIDGET_H

/*** End of inlined file: DACheckListWidget.h ***/


/*** Start of inlined file: DAStringListEditWidget.h ***/
#ifndef DASTRINGLISTEDITWIDGET_H
#define DASTRINGLISTEDITWIDGET_H

#include <QString>
#include <QStringList>
#include <QWidget>

namespace DA
{

/**
 * @brief 纯字符串列表编辑器（可自由输入、可编辑、可排序）
 *
 * 面向 QStringList 类数据（表单 list 字段、模块搜索路径等）：
 * - Inline 模式：Add 追加一行空字符串并立即进入编辑，不插入占位文本；
 * - External 模式：Add 只发 addRequested()，由调用方决定加什么（如目录选择），
 *   再调用 appendItem() 落入列表；
 * - 可选的 Remove / Up / Down 按钮，排序用按钮而不是拖拽。
 *
 * 空字符串允许存在（调用方可自行校验）；setItems 整体替换不发 itemsChanged。
 */
class DAWIDGETS_EXPORT DAStringListEditWidget : public QWidget
{
	Q_OBJECT
	DA_DECLARE_PRIVATE(DAStringListEditWidget)
public:
	/// Add 按钮行为
	enum class AddMode
	{
		Inline,   ///< 追加可编辑空行
		External  ///< 只发 addRequested()，由调用方追加
	};

	explicit DAStringListEditWidget(QWidget* parent = nullptr);
	~DAStringListEditWidget();

	// 整体替换（不发 itemsChanged）
	void setItems(const QStringList& items);
	// 获取当前列表
	QStringList items() const;
	// 追加一条（External 模式下由调用方在 addRequested 后使用；发 itemsChanged）
	void appendItem(const QString& text);

	// Add 按钮行为（默认 Inline）
	void setAddMode(AddMode m);
	AddMode addMode() const;

	// Up/Down 按钮显隐
	void setReorderEnabled(bool on);
	bool isReorderEnabled() const;

	// Remove 按钮显隐
	void setRemoveEnabled(bool on);
	bool isRemoveEnabled() const;

	// 是否允许编辑条目文本
	void setEditable(bool on);
	bool isEditable() const;

	// 数量上限（<=0 表示不限）
	void setMaxCount(int n);
	int maxCount() const;

	// 空列表时的提示文本
	void setPlaceholderText(const QString& t);
	QString placeholderText() const;

Q_SIGNALS:
	/**
	 * @brief 列表内容变化（增/删/排序/编辑，appendItem 也发；setItems 不发）
	 */
	void itemsChanged();
	/**
	 * @brief External 模式下点击 Add
	 */
	void addRequested();

protected:
	bool eventFilter(QObject* obj, QEvent* e) override;

private:
	Q_DISABLE_COPY(DAStringListEditWidget)
};
}  // namespace DA

#endif  // DASTRINGLISTEDITWIDGET_H

/*** End of inlined file: DAStringListEditWidget.h ***/


/*** Start of inlined file: DAColorScalePreviewWidget.h ***/
#ifndef DACOLORSCALEPREVIEWWIDGET_H
#define DACOLORSCALEPREVIEWWIDGET_H

#include <QWidget>
#include <QColor>
#include <QList>
#include <QBrush>

namespace DA
{

/**
 * @brief 色阶渐变预览条（2/3 色水平渐变）
 *
 * 条件格式色阶编辑等场景的效果预览：按传入的 2 个（首尾）或 3 个（首/中/尾）
 * 颜色绘制水平线性渐变（三色时中点色停在 0.5 位置），外框 1px 中性描边。
 * 颜色数不为 2/3 或存在无效色时绘制空白态。
 */
class DAWIDGETS_EXPORT DAColorScalePreviewWidget : public QWidget
{
	Q_OBJECT
public:
	explicit DAColorScalePreviewWidget(QWidget* parent = nullptr);

	// 设置渐变色（2 或 3 个有效色生效，其余为空白态）
	void setColors(const QList< QColor >& colors);
	// 获取当前渐变色
	QList< QColor > colors() const;

	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

Q_SIGNALS:
	/**
	 * @brief 渐变色变化
	 * @param colors 新渐变色
	 */
	void colorsChanged(const QList< QColor >& colors);

protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

private:
	// 按当前颜色和控件尺寸重建缓存渐变画刷
	void updateGradientBrush();

private:
	QList< QColor > mColors;        ///< 渐变色（2/3 个有效色，否则空白态）
	QBrush mGradientBrush;          ///< 预构建的渐变画刷，绘制时直接使用
	bool mGradientValid { false };  ///< 缓存画刷是否有效（颜色不合法时为空白态）
};
}  // namespace DA

#endif  // DACOLORSCALEPREVIEWWIDGET_H

/*** End of inlined file: DAColorScalePreviewWidget.h ***/


/*** Start of inlined file: DAStringFilterProxyModel.h ***/
#ifndef DASTRINGFILTERPROXYMODEL_H
#define DASTRINGFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QString>

namespace DA
{

/**
 * @brief 按展示文本过滤的通用代理模型（列表/树通用）
 *
 * - 过滤目标：行的 Qt::DisplayRole 文本；可限定单列，默认 -1 表示所有列都参与匹配；
 * - 大小写：默认不敏感，可切换；
 * - 树形模型：setIncludeParentsOfMatches(true) 让命中子节点的父节点也放行，
 *   否则父节点会被单独过滤掉、子节点无法显示；
 * - 空过滤文本表示不过滤（全部放行）。
 */
class DAWIDGETS_EXPORT DAStringFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:
	explicit DAStringFilterProxyModel(QObject* parent = nullptr);
	~DAStringFilterProxyModel();

	// 过滤文本（空串 = 不过滤）
	void setFilterText(const QString& text);
	QString filterText() const;

	// 大小写敏感（默认不敏感）
	void setFilterCaseSensitivity(Qt::CaseSensitivity cs);
	Qt::CaseSensitivity filterCaseSensitivity() const;

	// 过滤列（-1 = 所有列）
	void setFilterColumn(int column);
	int filterColumn() const;

	// 过滤使用的数据角色（默认 Qt::DisplayRole）
	void setFilterRole(int role);
	int filterRole() const;

	// 子节点命中时是否放行父节点（树形模型用，默认 false）
	void setIncludeParentsOfMatches(bool on);
	bool includeParentsOfMatches() const;

	// 单段文本的匹配语义（空过滤恒为 true）；供不能挂代理的视图复用同一实现
	static bool matchesText(const QString& text, const QString& filter, Qt::CaseSensitivity cs);

protected:
	bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

private:
	// 单行自身是否命中（不含子节点）
	bool rowMatches(int source_row, const QModelIndex& source_parent) const;

	QString mFilterText;
	int mFilterColumn { -1 };
	int mFilterRole { Qt::DisplayRole };
	bool mIncludeParents { false };
};

}  // namespace DA

#endif  // DASTRINGFILTERPROXYMODEL_H

/*** End of inlined file: DAStringFilterProxyModel.h ***/

/*** End of inlined file: DAWidgetsAmalgamTemplatePublicHeaders.h ***/

#endif // DAWIDGETS_AMALGAM_H


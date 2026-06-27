# DAWidgets 组件补充规划

> 本文档是 DAWidgets 库的长期开发路线图。Element UI 提供组件交互范式的参照，Qt 原生控件现状决定"是否需要补"，GitHub 开源生态决定"是否需要自研"。
>
> 调研时间：2026-06。开源仓库 star 数为时点值，实施前应复查最新状态。
>
> 决策原则：
> - Qt 原生已充分覆盖的，**不做**（避免重复造轮子）。
> - 有成熟开源实现且协议兼容的，**评估引入/封装**而非自研。
> - Web 特有、桌面端需求弱的，**标记暂缓**。
> - 桌面端有实际需求且无成熟实现的，**自研**。

## 图例

| 标记 | 含义 |
|------|------|
| ✅ 已实现 | 当前版本已交付 |
| 🟢 自研 | 无成熟开源替代，需自行实现 |
| 🟡 封装 | 有开源实现可参考/封装 |
| ⏸️ 暂缓 | Web 特有或桌面端需求弱，暂不实施 |
| ❌ 不做 | Qt 原生已充分覆盖，无需补充 |

---

## 一、基础组件

| 组件 | 对应 Element | Qt 原生 | 决策 | 说明 |
|------|-------------|---------|------|------|
| Button | Button | QPushButton | ❌ | 原生充足，仅样式差异 |
| Link | Link | — | ❌ | QLabel + hyperlink 即可，不值得封装 |
| Icon | Icon | QIcon/QSvgWidget | ❌ | 原生充足 |

> 基础组件 Qt 已完备，本库不在此层补充。

---

## 二、表单组件

| 组件 | 对应 Element | Qt 原生 | 决策 | 说明 / 开源替代 |
|------|-------------|---------|------|------|
| **Select（多标签）** | Select (multiple) | QComboBox（单选/勾选式） | ✅ 已实现 | DASelectComboBox + DAChipsEdit + DAChip + DAFlowLayout |
| Radio | Radio | QRadioButton | ❌ | 原生充足 |
| Checkbox | Checkbox | QCheckBox | ❌ | 原生充足 |
| Input | Input | QLineEdit/QTextEdit | ❌ | 原生充足 |
| InputNumber | InputNumber | QSpinBox/QDoubleSpinBox | ❌ | 原生充足（步长/范围/精度均支持） |
| **Switch** | Switch | —（QCheckBox 可替代但形态不同） | 🟢 自研 | 桌面端现代 UI 常见双态开关，QCheckBox 视觉不符。参考 ElaWidgetTools 的 ElaToggle |
| **Rate** | Rate | — | 🟢 自研 | 星级评分，可半选/只读/清空。Qt 无原生等价物 |
| **Slider（带刻度）** | Slider | QSlider（无刻度标注） | 🟢 自研 | QSlider 缺刻度文字标注、缺双向 range slider。双向滑块可参考 ThisIsClark/Qt-RangeSlider（~115★） |
| Cascader | Cascader | — | 🟢 自研 | 多级联动下拉，Qt 无等价物。实现复杂度高 |
| Transfer | Transfer | — | ⏸️ 暂缓 | 穿梭框是 Web 表单特有模式，桌面端可用双 QListView+按钮替代，优先级低 |
| DatePicker | DatePicker | QDateEdit/QCalendarWidget | ❌ | 单日期原生充足 |
| **DateRangePicker** | DatePicker (range) | — | 🟢 自研 | 日期范围选择，Qt 无原生双日历范围选择。中等工作量 |
| TimePicker | TimePicker | QTimeEdit | ❌ | 原生充足 |
| DateTimePicker | DateTimePicker | QDateTimeEdit | ❌ | 原生充足 |
| ColorPicker | ColorPicker | QColorDialog | 🟡 封装 | QColorDialog 是模态对话框；Element 风格的弹出式取色板可参考 ksnip/kColorPicker（~48★）封装 |
| Upload | Upload | — | ⏸️ 暂缓 | 拖拽上传+进度，桌面端通常用 QFileDialog，Web 风格上传组件桌面需求弱 |
| Form | Form | QFormLayout | ❌ | 原生布局充足，校验逻辑属业务层 |

---

## 三、数据展示

| 组件 | 对应 Element | Qt 原生 | 决 | 说明 / 开源替代 |
|------|-------------|---------|------|------|
| Table | Table | QTableView/QTableWidget | ❌ | 原生充足（排序/筛选/编辑均支持） |
| Tag | Tag | ✅ DAChip | ✅ 已实现 | 已作为 Selector 子组件交付，可独立使用 |
| Progress | Progress | QProgressBar | ❌ | 原生充足 |
| Tree | Tree | QTreeWidget/QTreeView | ❌ | 原生充足（勾选/拖拽/编辑均支持） |
| Pagination | Pagination | — | 🟢 自研 | Qt 无原生分页控件，但需求常见。可基于 QSpinBox+QPushButton 组合封装 |
| **Badge** | Badge | — | 🟢 自研 | 角标/数字徽标，实现简单，复用价值高 |
| Avatar | Avatar | QLabel(QPixmap) | ❌ | QLabel 即可，不值得封装 |
| **Skeleton** | Skeleton | — | 🟢 自研 | 骨架屏加载占位，现代 UI 标配。Qt 无等价物 |
| **Empty** | Empty | — | 🟢 自研 | 空状态占位（图标+文字+操作按钮），实现简单 |
| Descriptions | Descriptions | — | 🟡 封装 | 键值对描述列表，可用 QGridLayout 封装 |
| Result | Result | — | 🟢 自研 | 操作结果页（图标+标题+详情+操作），实现简单 |

---

## 四、提示/反馈

| 组件 | 对应 Element | Qt 原生 | 决策 | 说明 |
|------|-------------|---------|------|------|
| Alert | Alert | QMessageBox | 🟢 自研 | 内联告警条（页面内常驻，非模态弹窗），QMessageBox 是模态的，形态不同 |
| Loading | Loading | QProgressDialog | ❌ | 原生充足 |
| Message | Message | — | 🟢 自研 | 顶部短暂浮层提示，QMessageBox 太重。可基于 QFrame+QTimer 自绘 |
| MessageBox | MessageBox | QMessageBox | ❌ | 原生充足 |
| Notification | Notification | QSystemTrayIcon::showMessage | 🟡 封装 | 桌面通知系统已有，可封装统一 API |
| **Tooltip 增强** | Tooltip | QToolTip | ❌ | 原生充足 |

---

## 五、导航

| 组件 | 对应 Element | Qt 原生 | 决策 | 说明 / 开源替代 |
|------|-------------|---------|------|------|
| NavMenu | NavMenu | QMenuBar | ❌ | 桌面端菜单栏形态不同，原生充足 |
| **Tabs（可关闭/拖拽）** | Tabs | QTabWidget（无关闭/拖拽） | 🟢 自研 | QTabWidget 缺 tab 关闭按钮与拖拽重排，这两个是高频需求 |
| Breadcrumb | Breadcrumb | — | 🟢 自研 | 面包屑导航，实现简单 |
| PageHeader | PageHeader | — | 🟡 封装 | 页头（返回按钮+标题+操作区），QHBoxLayout 封装 |
| Dropdown | Dropdown | QMenu | ❌ | 原生充足 |
| **Steps** | Steps | — | 🟢 自研 | 步骤条（横/纵向流程进度），Qt 无等价物，表单向导场景常见 |

---

## 六、其他

| 组件 | 对应 Element | Qt 原生 | 决策 | 说明 / 开源替代 |
|------|-------------|---------|------|------|------|
| Dialog | Dialog | QDialog | ❌ | 原生充足 |
| Popover/Popconfirm | Popover | — | 🟡 封装 | 弹出气泡，可基于 QWidget+QTimer 封装 |
| Card | Card | QGroupBox | ❌ | 原生充足 |
| **Collapse** | Collapse | QToolBox | 🟡 封装 | QToolBox 形态接近但只能单展开；Element 风格可多展开/手风琴。可封装增强 |
| **Carousel** | Carousel | — | 🟢 自研 | 轮播图，Qt 无原生。可参考 Qt 官方 examples/widgets/effects/autoscalingpixmap |
| **Timeline** | Timeline | — | 🟡 封装 | 时间线，可参考 iwxyi/Qt-Timeline-Widget（~80★） |
| **Divider（带文字）** | Divider | QFrame(QFrame::HLine) | 🟢 自研 | 原生分割线无文字嵌入，实现简单 |
| Calendar | Calendar | QCalendarWidget | ❌ | 原生充足 |
| **Image（预览）** | Image | QLabel(QPixmap) | 🟢 自研 | 可缩放/旋转的图片预览器，QLabel 不够。参考 QLabel + QGraphicsView 思路 |
| **Backtop** | Backtop | — | ⏸️ 暂缓 | 回到顶部，依赖滚动容器场景，桌面端优先级低 |
| Drawer | Drawer | — | 🟢 自研 | 侧滑抽屉面板，现代 UI 常见，Qt 无原生等价物 |

---

## 实施优先级建议

按"实现成本低 / 复用价值高 / 无替代"三维度排序，建议分三批：

### 第一批（低难度高频，快速见效）
1. **DASwitch** — 双态开关， QPainter 自绘，1 个类
2. **DABadge** — 角标，QPainter 自绘 + 事件过滤器叠加到父控件，1 个类
3. **DADivider** — 带文字分割线，QFrame 派生，1 个类
4. **DAEmpty** — 空状态，QWidget 容器（图标+文字+按钮），1 个类
5. **DARate** — 星级评分，mouseMove + paintEvent，1 个类

### 第二批（中难度，表单/导航补全）
6. **DASteps** — 步骤条，横向/纵向，paintEvent 自绘 + 数据模型
7. **DATabs** — 可关闭/可拖拽标签页，QTabWidget 派生 + 事件过滤器
8. **DASlider** — 带刻度滑块 + DARangeSlider 双向滑块
9. **DAPagination** — 分页器，组合控件
10. **DASkeleton** — 骨架屏，QPainter 动画
11. **DABreadcrumb** — 面包屑
12. **DAResult** — 结果页

### 第三批（较高难度，按需实施）
13. **DADrawer** — 侧滑抽屉
14. **DACascader** — 级联选择器
15. **DADateRangePicker** — 日期范围
16. **DACarousel** — 轮播
17. **DAImagePreview** — 图片预览
18. **DAAlert** — 内联告警条
19. **DAMessage** — 浮层提示
20. **DATimeline** — 时间线（可参考开源封装）

### 暂缓（Web 特有或需求弱）
- Transfer 穿梭框、Upload 上传、Backtop 回到顶部

---

## 命名约定

所有新增控件延续 `DA` 前缀 + Element 组件名 PascalCase：
- `DASwitch`、`DABadge`、`DADivider`、`DAEmpty`、`DARate`
- `DASteps`、`DATabs`、`DASlider`/`DARangeSlider`、`DAPagination`
- `DASkeleton`、`DABreadcrumb`、`DAResult`、`DADrawer`、`DACascader`
- `DADateRangePicker`、`DACarousel`、`DAImagePreview`、`DAAlert`、`DAMessage`、`DATimeline`

## 开源参考库

实施时可参考（注意协议兼容性，优先 MIT）：
- **ElaWidgetTools**（~2400★, MIT）— C++ Fluent UI 控件集，Switch/Slider/Tab 等实现可参考
- **ThisIsClark/Qt-RangeSlider**（~115★）— 双向滑块
- **ksnip/kColorPicker**（~48★）— 弹出式取色板
- **iwxyi/Qt-Timeline-Widget**（~80★）— 时间线

> 引用任何开源实现前，必须核对 LICENSE 与本库 MIT 协议兼容，并在 README 注明来源。

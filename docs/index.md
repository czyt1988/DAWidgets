---
title: DAWidgets — Qt 控件补充库
description: 受 Element UI 启发的 Qt QWidget 补充控件库，为桌面应用带来 Web 风格的组件交互。
hide:
  - navigation
  - toc
---

<div class="daw-hero">
<div class="daw-hero-bg"></div>
<div class="daw-hero-inner">

<p class="daw-hero-badge">开源 &middot; MIT 许可证</p>

<h1 class="daw-hero-title">Qt 控件补充库</h1>

<p class="daw-hero-desc">
受 Element UI 启发的 Qt QWidget 补充控件库，
为桌面应用带来 Web 风格的组件交互体验。
</p>

<div class="daw-hero-pills">
<span>C++17</span>
<span>CMake</span>
<span>Qt 5 / Qt 6</span>
<span>Element UI 风格</span>
</div>

<div class="daw-hero-actions">
<a href="introduction/" class="daw-btn daw-btn-primary">快速开始</a>
<a href="https://github.com/czyt1988/DAWidgets" class="daw-btn daw-btn-ghost">GitHub</a>
</div>

</div>
</div>

<div class="daw-stats-bar">
<div class="daw-stat">
<strong>24+</strong>
<span>公开类</span>
</div>
<div class="daw-stat-divider"></div>
<div class="daw-stat">
<strong>5</strong>
<span>组件家族</span>
</div>
<div class="daw-stat-divider"></div>
<div class="daw-stat">
<strong>Qt 5 &amp; 6</strong>
<span>双兼容</span>
</div>
<div class="daw-stat-divider"></div>
<div class="daw-stat">
<strong>MIT</strong>
<span>商用友好</span>
</div>
</div>

<div class="daw-container">

<div class="daw-section-head">
<h2>核心特性</h2>
<p>为 Qt 桌面应用提供现代化的控件补充方案。</p>
</div>

<div class="daw-card-grid">

<div class="daw-card">
<div class="daw-card-icon">
<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="6 9 12 15 18 9"></polyline></svg>
</div>
<h3>Selector 选择器</h3>
<p>对标 <code>el-select</code> 的多标签下拉选择框，支持筛选、创建、默认选中、两种高度模式。含 <code>DAChipsEdit</code>、<code>DAChip</code>、<code>DAFlowLayout</code>。</p>
</div>

<div class="daw-card">
<div class="daw-card-icon">
<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="6" rx="1"></rect><rect x="3" y="13" width="18" height="8" rx="1"></rect></svg>
</div>
<h3>Collapse 折叠面板</h3>
<p>对标 <code>el-collapse</code> 的折叠面板容器与条目，支持手风琴模式、动画展开/折叠、自定义标题与内容区。</p>
</div>

<div class="daw-card">
<div class="daw-card-icon">
<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="13.5" cy="6.5" r="2.5"></circle><circle cx="19" cy="13" r="2"></circle><circle cx="6" cy="12" r="3"></circle><circle cx="10" cy="20" r="2"></circle></svg>
</div>
<h3>属性编辑器</h3>
<p>属性项容器 + 对齐方式编辑器、文件路径选择器、时间格式编辑器、最近文件管理器，快速构建属性面板。</p>
</div>

<div class="daw-card">
<div class="daw-card-icon">
<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"></circle><path d="M12 2a14.5 14.5 0 0 0 0 20 14.5 14.5 0 0 0 0-20"></path><path d="M2 12h20"></path></svg>
</div>
<h3>颜色/画笔编辑</h3>
<p>颜色网格、调色板、颜色菜单、拾取按钮，画笔/画刷样式下拉框，完整的画笔、画刷、字体、形状编辑面板。</p>
</div>

<div class="daw-card">
<div class="daw-card-icon">
<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"></path></svg>
</div>
<h3>RAII 光标守卫</h3>
<p><code>DACursorScoped</code> 与 <code>DAWaitCursorScoped</code> 提供异常安全的光标管理，作用域结束自动恢复。</p>
</div>

<div class="daw-card">
<div class="daw-card-icon">
<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"></path></svg>
</div>
<h3>PIMPL + 双兼容</h3>
<p>统一的 <code>DA_</code> PIMPL 宏套件，Qt5/Qt6 差异兼容宏，静态/动态库双模式导出，开箱即用。</p>
</div>

</div>
</div>

<div class="daw-footer">
<div class="daw-footer-inner">

<h2>开始使用</h2>
<p>阅读文档或浏览源代码。</p>

<div class="daw-footer-links">
<a href="introduction/" class="daw-btn daw-btn-primary">阅读文档</a>
<a href="en/introduction/" class="daw-btn daw-btn-ghost">English Docs</a>
<a href="https://github.com/czyt1988/DAWidgets" class="daw-btn daw-btn-ghost">GitHub</a>
</div>

</div>
</div>

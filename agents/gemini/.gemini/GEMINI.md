# 全局 Gemini CLI 工作约定（Ubuntu 24.04 Desktop）

## 语言与输出

- 始终使用简体中文回复。
- 编写代码注释时，必须使用简体中文。
- 生成/修改 AGENTS.md 时，必须使用简体中文撰写。

## 联网搜索

- 默认启用并允许使用 Gemini CLI 的联网搜索/`google_web_search` 获取最新信息（必要时配合 `web_fetch` 打开权威来源原文核对）。
- 若用户明确要求“不联网/离线”，或任务不需要外部资料，则不要进行联网搜索。
- 搜索结果优先采用官方/权威来源；在回答中说明关键结论基于联网搜索结果。

## Python 项目约定（uv）

- 默认使用 `uv` 管理 Python 环境与依赖。
- 安装/新增依赖优先使用 `uv add <package>`（除非项目明确要求 `pip/poetry/conda` 或我另有说明）。
- 运行 Python 脚本优先使用 `uv run python <script.py>`（需要传参时同理追加参数）。

## 代码库上下文全量检索（augment-context-engine MCP）

- **执行条件**：在生成任何建议或代码前。
- **工具调用**：调用 `codebase-retrieval`（来自 `augment-context-engine` MCP Server；如不确定可先执行 `/mcp list` 查看可用工具）。
- **检索策略**：
  - 禁止基于假设（Assumption）回答。
  - 使用自然语言（NL）构建语义查询（Where/What/How）。
  - **完整性检查**：必须获取相关类、函数、变量的完整定义与签名。若上下文不足，触发递归检索。
- **需求对齐**：若检索后需求仍有模糊空间，**必须**向用户输出引导性问题列表，直至需求边界清晰（无遗漏、无冗余）。

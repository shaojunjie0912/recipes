# MCP 安装与配置（Codex CLI / Claude Code / Gemini CLI）

本文只覆盖“官方推荐”的两类方式：

1. 用各自 CLI 的 `mcp add/list/remove` 子命令管理（优先推荐）
2. 直接编辑对应配置文件（适合批量/可审计/可共享）

> 说明：不同 MCP Server 可能是 `stdio`（本地进程）或 `http/sse`（远程服务）。三款 CLI 都支持这几类传输。

## Codex CLI（OpenAI）

### 方式 A（推荐）：命令行安装/管理

Codex CLI 内置 `codex mcp` 用来管理 MCP Server：

```bash
# 列出已配置的 MCP servers
codex mcp list

# 删除 server
codex mcp remove <name>
```

添加本地 `stdio` server（示例：Filesystem Server）：

```bash
codex mcp add filesystem -- npx -y @modelcontextprotocol/server-filesystem .
```

### 示例：你当前已安装的两款 MCP（augment-context-engine / tavily）

命令行安装（与当前环境一致；密钥用占位符）：

```bash
# augment-context-engine
codex mcp add augment-context-engine \
  --env AUGMENT_API_TOKEN=YOUR_AUGMENT_API_TOKEN \
  --env AUGMENT_API_URL=YOUR_AUGMENT_API_URL \
  -- auggie --mcp

# tavily
codex mcp add tavily \
  --env TAVILY_API_KEY=YOUR_TAVILY_API_KEY \
  -- npx -y tavily-mcp@latest
```

等价 `~/.codex/config.toml` 片段：

```toml
[mcp_servers."augment-context-engine"]
command = "auggie"
args = ["--mcp"]

[mcp_servers."augment-context-engine".env]
AUGMENT_API_TOKEN = "${AUGMENT_API_TOKEN}"
AUGMENT_API_URL = "${AUGMENT_API_URL}"

[mcp_servers.tavily]
command = "npx"
args = ["-y", "tavily-mcp@latest"]

[mcp_servers.tavily.env]
TAVILY_API_KEY = "${TAVILY_API_KEY}"
```

添加远程“可流式 HTTP” server：

```bash
# 如果服务要求 Bearer Token，可让 Codex 从环境变量读取
codex mcp add my-remote --url https://example.com/mcp --bearer-token-env-var MY_MCP_TOKEN
```

### 方式 B：编辑配置文件（TOML）

Codex 官方配置文件路径为：`~/.codex/config.toml`。

在 `config.toml` 里添加 `mcp_servers`：

```toml
[mcp_servers.filesystem]
command = "npx"
args = ["-y", "@modelcontextprotocol/server-filesystem", "."]

[mcp_servers.my_remote]
url = "https://example.com/mcp"
bearer_token_env_var = "MY_MCP_TOKEN"
# http_headers = { "X-Example" = "value" }
```

> 建议：密钥尽量用环境变量注入（如 `${TAVILY_API_KEY}`），不要硬编码进仓库。

### 校验

```bash
codex mcp list
```

## Claude Code（Anthropic）

### 方式 A（推荐）：命令行安装/管理

Claude Code 内置 `claude mcp`：

```bash
# 列出已配置的 MCP servers
claude mcp list

# 删除 server
claude mcp remove <name>
```

添加 `stdio` server（示例：Filesystem Server）：

```bash
# scope：local（默认）、user、project
claude mcp add --transport stdio --scope project filesystem -- npx -y @modelcontextprotocol/server-filesystem .
```

### 示例：你当前已安装的两款 MCP（augment-context-engine / tavily）

命令行安装（与当前环境一致；密钥用占位符）：

```bash
# augment-context-engine
claude mcp add --transport stdio augment-context-engine \
  -e AUGMENT_API_TOKEN=YOUR_AUGMENT_API_TOKEN \
  -e AUGMENT_API_URL=YOUR_AUGMENT_API_URL \
  -- auggie --mcp

# tavily
claude mcp add --transport stdio tavily \
  -e TAVILY_API_KEY=YOUR_TAVILY_API_KEY \
  -- npx -y tavily-mcp@latest
```

等价 `.mcp.json`（项目根目录，可共享给团队）：

```json
{
  "mcpServers": {
    "augment-context-engine": {
      "type": "stdio",
      "command": "auggie",
      "args": ["--mcp"],
      "env": {
        "AUGMENT_API_TOKEN": "YOUR_AUGMENT_API_TOKEN",
        "AUGMENT_API_URL": "YOUR_AUGMENT_API_URL"
      }
    },
    "tavily": {
      "type": "stdio",
      "command": "npx",
      "args": ["-y", "tavily-mcp@latest"],
      "env": {
        "TAVILY_API_KEY": "YOUR_TAVILY_API_KEY"
      }
    }
  }
}
```

添加远程 HTTP/SSE server：

```bash
claude mcp add --transport http sentry https://mcp.sentry.dev/mcp
```

如果需要设置环境变量/请求头：

```bash
# stdio：用 -e/--env
claude mcp add --transport stdio airtable -e AIRTABLE_API_KEY=YOUR_KEY -- npx -y airtable-mcp-server

# http/sse：用 -H/--header
claude mcp add --transport http my-remote -H "Authorization: Bearer YOUR_TOKEN" https://example.com/mcp
```

### 方式 B：编辑配置文件（JSON）

Claude Code 的“可共享项目配置”通常放在项目根目录：`.mcp.json`。

最小示例：

```json
{
  "mcpServers": {
    "filesystem": {
      "type": "stdio",
      "command": "npx",
      "args": ["-y", "@modelcontextprotocol/server-filesystem", "."],
      "env": {}
    }
  }
}
```

> 备注：`--scope local/user` 的配置会写入用户侧配置文件（常见为 `~/.claude.json`）。

### 校验

```bash
claude mcp list
```

## Gemini CLI（Google）

### 方式 A（推荐）：命令行安装/管理

Gemini CLI 内置 `gemini mcp`：

```bash
gemini mcp list
gemini mcp remove <name>
```

添加 `stdio` server（示例：Filesystem Server；默认 scope=project）：

```bash
gemini mcp add filesystem npx -y @modelcontextprotocol/server-filesystem .
```

添加到用户级（写入 `~/.gemini/settings.json`）：

```bash
gemini mcp add -s user filesystem npx -y @modelcontextprotocol/server-filesystem .
```

添加远程 HTTP/SSE server（并设置请求头）：

```bash
gemini mcp add -t http my-remote https://example.com/mcp -H "Authorization: Bearer YOUR_TOKEN"
gemini mcp add -t sse  my-sse    https://example.com/sse -H "X-Api-Key: YOUR_KEY"
```

设置环境变量：

```bash
gemini mcp add my-stdio -e API_KEY=YOUR_KEY npx -y some-mcp-server@latest
```

### 方式 B：编辑配置文件（JSON）

Gemini CLI 支持两种位置：

- 项目级：`.gemini/settings.json`
- 用户级：`~/.gemini/settings.json`

最小示例：

```json
{
  "mcpServers": {
    "filesystem": {
      "command": "npx",
      "args": ["-y", "@modelcontextprotocol/server-filesystem", "."]
    }
  }
}
```

### 校验

```bash
gemini mcp list
```

## 常见问题（最短排查路径）

1. 先手动跑一遍 server 命令，确认能启动：`npx -y <server-pkg> ...`
2. 再用 `*/mcp list` 确认 CLI 已读到配置（Codex/Claude/Gemini 分别是 `codex/claude/gemini mcp list`）
3. 密钥优先走环境变量，不要写进仓库；必要时用各 CLI 的 `-e/--env` 注入

## 参考（官方/权威）

- OpenAI Codex 配置：`https://developers.openai.com/codex/config-basic/`
- OpenAI Codex 样例配置：`https://developers.openai.com/codex/config-sample`
- MCP（协议）文档：`https://modelcontextprotocol.io/`
- Gemini CLI MCP 文档：`https://google-gemini.github.io/gemini-cli/docs/tools/mcp-server.html`
- Firebase Studio（含 Gemini CLI 的 `settings.json` 示例）：`https://firebase.google.com/docs/studio/mcp-servers`

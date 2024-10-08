-- Please check `lua/core/settings.lua` to view the full list of configurable settings
local settings = {}

settings["use_ssh"] = true
settings["colorscheme"] = "catppuccin-latte"
settings["use_copilot"] = false
settings["format_notify"] = false

settings["disabled_plugins"] = {
	"ray-x/go.nvim",
	"lewis6991/gitsigns.nvim",
	"MeanderingProgrammer/render-markdown.nvim",
}
settings["lsp_inlayhints"] = false
settings["lsp_deps"] = function(defaults)
	return {
		"bashls",
		"clangd",
		"jsonls",
		"lua_ls",
		"pylsp",
		"taplo",
		"sqlls",
		"cmake",
	}
end
settings["null_ls_deps"] = function(defaults)
	return {
		"clang_format",
		"prettier",
		"shfmt",
		"stylua",
		"vint",
		"cmake_format",
	}
end

settings["dap_deps"] = function(defaults)
	return {
		"codelldb", -- C-Family
		"python", -- Python (debugpy)
	}
end

settings["treesitter_deps"] = function()
	return {
		"bash",
		"c",
		"cpp",
		"json",
		"lua",
		"make",
		"markdown",
		"markdown_inline",
		"python",
		"toml",
		"cmake",
	}
end

settings["dashboard_image"] = {
	[[                                                     ]],
	[[  ███╗   ██╗███████╗ ██████╗ ██╗   ██╗██╗███╗   ███╗ ]],
	[[  ████╗  ██║██╔════╝██╔═══██╗██║   ██║██║████╗ ████║ ]],
	[[  ██╔██╗ ██║█████╗  ██║   ██║██║   ██║██║██╔████╔██║ ]],
	[[  ██║╚██╗██║██╔══╝  ██║   ██║╚██╗ ██╔╝██║██║╚██╔╝██║ ]],
	[[  ██║ ╚████║███████╗╚██████╔╝ ╚████╔╝ ██║██║ ╚═╝ ██║ ]],
	[[  ╚═╝  ╚═══╝╚══════╝ ╚═════╝   ╚═══╝  ╚═╝╚═╝     ╚═╝ ]],
	[[                                                     ]],
}
return settings

local settings = {}

settings["use_ssh"] = true
settings["colorscheme"] = "catppuccin-latte"
settings["use_copilot"] = true
-- settings["format_notify"] = false

settings["disabled_plugins"] = {
	"ray-x/go.nvim",
	"lewis6991/gitsigns.nvim",
	"MeanderingProgrammer/render-markdown.nvim",
}

settings["lsp_inlayhints"] = false

settings["lsp_deps"] = function(defaults)
	return {
		"clangd",
		"lua_ls",
		"pylsp",
	}
end

settings["null_ls_deps"] = function(defaults)
	return {
		"clang_format",
		"stylua",
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
		"c",
		"cpp",
		"lua",
		"python",
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

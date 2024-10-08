return function()
	local leet_arg = "leetcode.nvim"
	require("leetcode").setup({
		cn = { enabled = true },
		arg = leet_arg,
		injector = {
			["cpp"] = {
				before = { "#include <bits/stdc++.h>", "using namespace std;" },
			},
		},
	})
end

return function()
	local leet_arg = "leetcode.nvim"
	require("leetcode").setup({
		arg = leet_arg,
		cn = {
			enabled = true,
		},
		lang = "cpp",
		injector = {
			["cpp"] = {
				before = {
					"#include <bits/stdc++.h>",
					"",
					-- 链表节点 & 节点 & 二叉树节点
					'#include "include/list_node.hpp"',
					'#include "include/node.hpp"',
					'#include "include/tree_node.hpp"',
					"",
					"using namespace std;",
				},
				after = { "int main() {", "    return 0;", "}" },
			},
		},
	})
end

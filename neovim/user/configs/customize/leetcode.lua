return function()
	local leet_arg = "leetcode.nvim"
	require("leetcode").setup({
		arg = leet_arg,
		cn = {
			enabled = true,
		},
		lang = "cpp",
		injector = {
			["python3"] = {
				before = {
					"from typing import List, Dict, Tuple, Literal",
					"from functools import cache",
					"from math import inf",
				},
				after = {
					'if __name__ == "__main__":',
					"    pass",
				},
			},
			["cpp"] = {
				before = {
					"#include <algorithm>",
					"#include <array>",
					"#include <climits>",
					"#include <cmath>",
					"#include <cstdint>",
					"#include <functional>",
					"#include <iostream>",
					"#include <list>",
					"#include <map>",
					"#include <numeric>",
					"#include <queue>",
					"#include <set>",
					"#include <stack>",
					"#include <string>",
					"#include <unordered_map>",
					"#include <unordered_set>",
					"#include <utility>",
					"#include <vector>",
					"",
					-- 链表节点 & 节点 & 二叉树节点
					'#include "include/list_node.hpp"',
					'#include "include/node.hpp"',
					'#include "include/tree_node.hpp"',
					"",
					"using namespace std;",
				},
				-- after = {
				-- 	"int main() {",
				-- 	"    return 0;",
				-- 	"}",
				-- },
			},
		},
		description = {
			width = "45%",
		},
	})
end

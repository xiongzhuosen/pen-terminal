#!/usr/bin/env bash
# tree_cat_such_as.sh —— 针对 /home/kali/such_as/ 的先 tree 再 cat 脚本

set -euo pipefail

DIR="src/"

# 1. 目录结构
echo "========== 目录结构 =========="
tree -C -n -- "$DIR"
echo

# 2. 逐个 cat 普通文件
echo "========== 文件内容 =========="
while IFS= read -r -d '' file; do
    echo
    echo "----- $file -----"
    cat -- "$file"
done < <(find "$DIR" -type f -print0)

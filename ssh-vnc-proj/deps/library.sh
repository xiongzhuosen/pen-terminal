#!/bin/bash
for pkg in *.deb *.udeb; do
    dpkg-deb -x "$pkg" .     # 点号表示“当前目录”，覆盖同名文件
done
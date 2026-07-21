# c-text-stats

一个使用 C11 编写的命令行文本统计工具，用于统计文件中的换行符、单词和字节数量。

## 功能

- 默认输出换行符、单词和字节数量；
- 支持短选项、组合短选项和对应的长选项；
- 支持同时统计多个文件并输出合计；
- 支持从标准输入读取内容；
- 使用流式处理，不需要将整个文件载入内存。

统计规则与常见的 `wc` 工具一致：行数是 `\n` 的数量，单词由空白字符分隔，`-c` 统计原始字节数。

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 使用

```text
c-text-stats [OPTION]... FILE...
```

示例：

```bash
c-text-stats README.md
c-text-stats -l README.md
c-text-stats -wc README.md
c-text-stats --lines --words README.md
c-text-stats README.md LICENSE
c-text-stats --version
printf "hello world\n" | c-text-stats -
```

## 测试

```bash
ctest --test-dir build --output-on-failure
```

## 项目结构

```text
include/    公开接口
src/        程序入口和统计算法
tests/      单元测试
```

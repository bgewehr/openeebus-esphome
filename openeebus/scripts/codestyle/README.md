# Codestyle check

This script automatically checks if there's any issues with the codestyle,
utilizing the clang-format tool.

## .main_branch

This scripts supports different names for the main branch of the repository.
By default it expects `origin/main` but a different branch name can be
specified in the `.main_branch` file in the repository root.

Example of content of .main_branch:
```
origin/main
```

## Ignore files

To ignore files from being included in the format diff check, you can add the
file `.format_diff_ignore` in the repository root folder.

This file should contain a pattern per file as would be returned by the find
command (with `./` prefix to the path).

---
./src/file.c
./src/third_party/*
---

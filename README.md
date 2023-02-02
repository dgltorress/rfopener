# Random File Opener

Simple **command-line tool** for *Windows* used to **open a random file** selected from a directory and its subdirectories.

The LICENSE applies to all source code **unless otherwise specified**.

## Overview

To open a random file, simply use:

```shell
rfopener
```

at the **directory that contains your pool of files** (whether they be in the root directory or any subdirectory).

This command will:
1. **Recursively iterate** through the working directory and its subdirectories up to the specified depth.
2. **Store** each path **into memory**.
3. **Pick a random path** and **open its corresponding file**.
4. Ask whether to repeat step 3 (**open another** random file).

There is a **soft cap** of **50000 stored paths** and **10 depth levels** so as to avoid any unwanted mishaps.

**Detailed example**: open a random MP4 or WEBM file contained within up to 4 (inclusive) levels of depth inside the `C:\Users\ME\Videos` folder. Allow the program to store more than 50000 (soft cap) relative paths into memory. 

```shell
rfopener -r "C:\Users\ME\Videos" -e "mp4;webm" -d 4 -nc
```

## Usage

`rfopener` `[-opts]`

Options:

`-h`, `--help` Show help.

`-r`, `--root` `[DIRECTORY]` ***Relative* or *absolute* path** to the **root directory**. Defaults to the **current working directory**.

`-e`, `--extensions` `[extension1;extension2;...;extensionN]` Enables **file extension whitelisting**. Only files that exactly match any of the specified extensions (separated by a *semicolon*) will be taken into account. **Empty strings** are accounted for as well.

`-d`, `--depth` `[DEPTH]` **Maximum (inclusive) levels of depth** the recursive iterator is allowed to reach (min. 0, max. 10, default. 5). 0 equals to the working directory.

`-nc`, `--nocap` **Disable soft caps** for storage of relative paths in memory (max. 50000 paths) and depth levels (max. 10 levels). Caps are **enabled by default**.

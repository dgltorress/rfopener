# Random File Opener

Simple **command-line tool** for *Windows* used to **open a random file** selected from a directory and its subdirectories.

The LICENSE applies to all source code **unless otherwise specified**.

## Overview

To open a random file, simply use:

```shell
rfopener
```

on the **directory that contains your pool of files** (whether they be in the root directory or any subdirectory).

This command will:
1. **Recursively iterate** through the working directory and its subdirectories up to the specified depth.
2. **Store** each path **into memory**.
3. **Pick a random path** and **open its corresponding file**.
4. Ask whether to **repeat** step 3. Use the <kbd>Enter</kbd> or <kbd>Space</kbd> keys to open more files, or <kbd>Esc</kbd> or <kbd>Backspace</kbd> to exit.

By default, there are **soft caps** of **50000 stored paths** and **10 depth levels**. Both can be disabled.

**Detailed example**: open a random MP4 or WEBM file contained within up to 4 (inclusive) levels of depth inside the `C:\Users\ME\Videos` folder. Allow the program to store more than 50000 relative paths into memory. 

```shell
rfopener -r "C:\Users\ME\Videos" -e "mp4;webm" -d 4 -nc
```

## Playlist mode

Alternatively, the program can **shuffle** a series of file paths and allow **sequential access** to them. To achieve this, use:

```shell
rfopener -p
```

The playlist can be navigated in the following manner:
- Forward: <kbd>&rarr;</kbd> and <kbd>&darr;</kbd> keys.
- Backwards: <kbd>&larr;</kbd> and <kbd>&uarr;</kbd> keys.

Both directions loop back to the opposite end of the playlist.

## Usage

`rfopener` `[-opts]`

Options:

`-h`, `--help` Show the help message.

`-r`, `--root` `directory` ***Relative* or *absolute* path** to the **root directory**. Defaults to the **current working directory**.

`-p`, `--playlist` Enable
***<span style="color:#f94144">P</span>
<span style="color:#277da1">L</span>
<span style="color:#f3722c">A</span>
<span style="color:#577590">Y</span>
<span style="color:#f8961e">L</span>
<span style="color:#4d908e">I</span>
<span style="color:#f9844a">S</span>
<span style="color:#43aa8b">T</span>&nbsp;
<span style="color:#f9c74f">M</span>
<span style="color:#90be6d">O</span>
<span style="color:#ef476f">D</span>
<span style="color:#2a9d8f">E</span>***,
where files will be **shuffled** and accessible **sequentially**.

`-x`, `--exclude` `directory1;directory2;...;directoryN` Enables **directory blacklisting**. Directories matching any of the specified absolute or relative paths will be skipped.

`-e`, `--extensions` `extension1;extension2;...;extensionN` Enables **file extension whitelisting**. Only files that exactly match any of the specified extensions will be taken into account.

**Empty strings** are accounted for as well, but require a separator be explicitly included.

`-d`, `--depth` `levels` **Maximum (inclusive) levels of depth** the recursive iterator is allowed to reach (min. 0, max. 10, default. 5). 0 equals to the working directory.

`-nc`, `--nocap` **Disable soft caps** for storage of relative paths in memory (max. 50000 paths) and depth levels (max. 10 levels). Caps are **enabled by default**.

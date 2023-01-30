# Random File Opener

Simple **command-line tool** for *Windows* used to **open a random file** selected from a directory and its subdirectories.

The LICENSE applies to all source code **unless otherwise specified**.

## Overview

This tool works by performing two steps:

1. **Scan**. It recursively gets the **paths to all files in a directory** and its subdirectories up to a specified depth. It then **dumps the canonical directory path and relative file paths** into a `.txt` file (*scan file*), and the **amount of stored paths** into another file (*size file*).

    Example with the current working directory:
    ```shell
    rfopener -s .
    ```

2. **Open**. It **reads the *scan file*** and randomly picks a path, whose corresponding file is promptly opened. There are currently two ways it can proceed with this task:
    - **Direct Mode** (*default*). It directly reads the file up to a previously selected line.
    - **Memory Mode**. It stores all read paths into memory and then picks a random index. Grants constant complexity to subsequent accesses to random files, but cost could potentially be prohibitive.

    Example with the current working directory:
    ```shell
    rfopener -o .
    ```

## Usage

```shell
rfopener [-opts]

Options:
-h, --help              Show this help message.
-s, --scan [DIRECTORY]          Recursively scan and store file paths from the specified directory and its subdirectories into a file within said directory.
        -d, --depth [DEPTH]             Maximum (inclusive) depth the recursive iterator is allowed to reach. min. 0, max. 8, default 2 (0 equals to the working directory).
-o, --open [DIRECTORY]          Opens a file corresponding to a random path from an already created scan file located in the specified directory.
        -m, --memory            Whether to enable Memory Mode, which stores all read paths into memory. Disabled by default.
```
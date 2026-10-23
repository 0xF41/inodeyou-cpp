# inodeyou-cpp

**Compare raw-volume and mounted-filesystem inode inventories, with full paths for discrepancies.**

`inodeyou-cpp` is a Linux forensic proof of concept for identifying files and directories that may be omitted from filesystem enumeration. It compares two views of the same directory tree:

1. **Raw-volume view:** The Sleuth Kit (TSK) parses filesystem metadata from the backing block device.
2. **Mounted view:** The program walks the directory through normal filesystem interfaces.

Differences are investigation candidates, not proof of malware. Both views still depend on the running kernel, and live filesystem changes can produce discrepancies.

## Features

- Reports full mounted paths alongside unsigned 64-bit inode numbers.
- Retains multiple names per inode, including hard links.
- Distinguishes raw-only inodes, mounted-only inodes, and differing names for shared inodes.
- Collects raw-view paths directly through TSK; no subsequent `find` lookup is needed.
- Supports an explicit mount root for filesystems mounted somewhere other than `/`.
- Inventories symbolic links without following them during traversal.
- Escapes non-ASCII and control bytes in reported paths for terminal-safe output.
- Returns distinct exit codes for matching inventories, differences, and incomplete scans.
- Flushes pending writes before collection without dropping system-wide filesystem caches.

## Build

On Ubuntu or Debian:

```sh
sudo apt update
sudo apt install build-essential cmake libtsk-dev e2fsprogs

git clone https://github.com/0xF41/inodeyou-cpp.git
cd inodeyou-cpp

cmake -S . -B build
cmake --build build
```

The executable is in **`build/inodeyou`**. 

## Usage

```text
sudo ./build/inodeyou <volume> [directory] [--mount-root <mountpoint>]
```

| Argument | Meaning |
| --- | --- |
| `volume` | The block device containing the mounted filesystem, such as a partition or logical volume. |
| `directory` | The directory tree to compare. Defaults to the mount root when omitted. |
| `--mount-root` | Where the volume's filesystem root is mounted. Defaults to `/`. |

Display usage without root privileges:

```sh
./build/inodeyou --help
```

### Filesystem mounted at `/`

First identify the device backing the directory:

```sh
findmnt -T /var/lib/demo -o SOURCE,TARGET,FSTYPE
```

If the output identifies `/dev/sda2`, mounted at `/`:

```sh
sudo ./build/inodeyou /dev/sda2 /var/lib/demo
echo $?
```

Use an existing, small directory with minimal activity for the initial comparison.

### Filesystem mounted elsewhere

For `/dev/sdb1` mounted at `/mnt/data`:

```sh
sudo ./build/inodeyou /dev/sdb1 /mnt/data/demo --mount-root /mnt/data
```

The program walks `/mnt/data/demo` through the mounted view and resolves `/demo` inside the raw volume. Both inventories report paths under `/mnt/data/demo`.

The mount root must represent the filesystem root, not a bind-mounted subdirectory. The program checks the backing device identity and requires the starting inode resolved by TSK to match the mounted starting directory.

## Reading the results

The following examples are illustrative; inode numbers and inventory sizes vary.

### Matching inventories

```text
[*] TSK inventory: 2 inodes.
[*] Mounted inventory: 2 inodes.
[*] No differences found between TSK and filesystem inode/path inventories.
```

This means the collected inode/path inventories match. It does not establish whole-system integrity.

### Raw-only inode

```text
[!] RAW-ONLY: hidden-entry candidate | inode=12345
    Path: "/var/lib/demo/probe_caraxes.txt"
```

TSK observed the inode, but the mounted walk did not. The path comes from raw-volume directory records, so it can be reported even when normal enumeration omits the entry.

### Mounted-only inode

```text
[!] MOUNTED-ONLY: reverse discrepancy | inode=12346
    Path: "/var/lib/demo/new-file.txt"
```

The mounted walk observed an inode absent from the TSK inventory. Investigate this separately from raw-only candidates; it is not automatically evidence of a hidden file.

### Different names for a shared inode

```text
[!] NAME-ONLY IN RAW VIEW | inode=12347
    Path: "/var/lib/demo/second-link.txt"
```

The inode exists in both inventories, but this path appears only in the raw view. The reverse is reported as `NAME-ONLY IN MOUNTED VIEW`. This preserves differences that an inode-only comparison would miss, such as an omitted hard-link name. Renames between the two walks can also produce name discrepancies.

Counts refer to unique inodes, not filenames. A single inode can have several reported paths.

## Exit codes

### Status codes

| Code | Meaning |
| --- | --- |
| `0` | Collection completed and the inode/path inventories matched. |
| `1` | Collection completed and differences were found. |
| `2` | Invalid input, collection failure, or incomplete scan. |

**Behaviour change:** earlier versions returned `0` even when inode differences were printed. Scripts should use the new exit-code meanings.

When collection fails, the program reports `INCOMPLETE` and suppresses the comparison instead of treating a partial inventory as a list of hidden entries.

## Scope and limitations

- **Linux block-device workflow:** the CLI requires the block device backing the mounted scan directory. Raw-device parsing is used; whole-disk images with partition offsets and standalone image files are not supported CLI inputs.
- **Filesystem support:** parsing depends on the installed TSK version. The automated integration test exercises ext4; other filesystem formats have not been established as working by that test.
- **Live observations:** the two walks are sequential, not an atomic snapshot. Writes, renames, deletions, and inode reuse can affect results. Flushing pending writes does not eliminate those races.
- **Nested mounts and bind mounts:** the mounted walk checks the mount identity of each directory and stops if it crosses a mount boundary. A changed directory identity, symlink substitution, or traversal cycle also produces an incomplete result.
- **Traversal bounds:** mounted traversal is capped at 1,000,000 entries and depth 256; TSK directory cycles are rejected. An inaccessible directory or missing metadata makes the scan incomplete.
- **Inventory scope:** the starting directory itself is excluded from both inventories. TSK unallocated entries and virtual entries are excluded; this is not a deleted-file recovery tool.
- **Shared trust boundary:** raw-device reads and mounted enumeration both rely on the same running kernel. A compromised kernel may manipulate either view.

## Credits

Adapted from the original **inodeyou** project:

- [Original repository and Python implementation](https://bitbucket.org/unixist/inodeyou/src/master/inodeyou.py)
- Historical documentation: `hxxp[://]www[.]unixist[.]com/security/detecting-hidden-files/index[.]html` (previously unavailable).

Raw filesystem parsing is provided by [The Sleuth Kit](https://www.sleuthkit.org/).

## License

GPL-3.0-only. See [LICENSE](LICENSE).

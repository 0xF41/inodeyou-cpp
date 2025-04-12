# inodeyou

**inodeyou-cpp** is a forensic tool to detect files and directories that may be hidden by malware on unix systems.

**inodeyou-cpp** uses The Sleuth Kit (TSK) C library to compare the view of the filesystem inodes by two methods:

1. Finding inodes visible to the disk's view of the file system (with `read()`) via TSK library
2. Finding inodes visible from the user's view of the mounted file system (with `getdent()`, `stat()`, etc.).

With the results from both methods, we can cross-check to see if there are any inodes that are seen by the disk view of the file system (with TSK) and not from the user's view of the mounted file system (with the ls command). If so, those inodes might be hidden by a rootkit/malware.

**Adapted from inodeyou:**

- **`inodeyou` documentation:** hxxp[://]www[.]unixist[.]com/security/detecting-hidden-files/index[.]html (defanged as link is broken)
- **`inodeyou` original repository:** https://bitbucket.org/unixist/inodeyou/src/master/inodeyou.py

## Build & Installation
For Debian-based distributions that use the following file systems (NTFS, FAT, exFAT, HFS+, Ext3, Ext4, UFS and YAFFS2) are supported.

**Prerequisites & Dependencies**

```
sudo apt update
sudo apt install g++ libtsk-dev cmake
```

**Installation**
```
git clone https://github.com/0xf41/inodeyou-cpp.git
cd inodeyou-cpp
./build.sh
```

## Usage

The program scans for hidden inodes recursively from `[directory]` in the filesystem mounted on `<volume>`.

```
sudo ./inodeyou <volume> [directory]
```

**Example**

```
sudo ./inodeyou /dev/sda1 /home/user1
```

**Expected output (for no anomalies):**

```
[*] TSK inodes populated with 77894 inodes.
[*] Filesystem inodes populated with 77894 inodes.
[*] No differences found between TSK and filesystem inodes.
```

## Disclaimer

This program is intended to be a proof-of-concept and to be used for educational and research purposes only. It may produce many false positives and should not be used as a definitive tool for detecting malware.

---

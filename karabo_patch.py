from os import getenv, rename
import re
from sys import exit
from packaging import version

MIN_VERSION = "2.11.5"


def is_version_ok():
    karabo = getenv('KARABO')
    if not karabo:
        raise RuntimeError("KARABO variable is not defined")

    with open(f"{karabo}/VERSION", 'r') as f:
        karabo_version = f.read()
        return (version.parse(karabo_version) > version.parse(MIN_VERSION))

    return False


def patch_karabo():
    karabo = getenv('KARABO')
    fname1 = f"{karabo}/include/karabo/util/karaboDll.hh"
    fname2 = f"{fname1}_"

    rename(fname1, fname2)

    with open(fname2, 'r') as in_file:
        with open(fname1, 'w') as out_file:
            for line in in_file:
                if re.search("typedef.*uint64", line):
                    continue
                out_file.write(line)


def main():
    """If Karabo is older than 2.11.5, then karaboDll.hh needs to be patched,
    to solve a typedef clash with OpenCV's interface.h.
    """

    if not is_version_ok():
        patch_karabo()


if __name__ == "__main__":
    exit(main())

# aesd-assignments
This repo contains public starter source code, scripts, and documentation for Advanced Embedded Software Development (ECEN-5713) and Advanced Embedded Linux Development assignments University of Colorado, Boulder.

## GitHub + Repos

### Setting up SSH keys

See instructions in [Setting-up-SSH-Access-To-your-Repo](https://github.com/cu-ecen-aeld/aesd-assignments/wiki/Setting-up-SSH-Access-To-your-Repo) for details.

### Sources
- [Generating a new SSH key and adding it to the ssh-agent](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent)
- [Testing your SSH connection](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/testing-your-ssh-connection)

```bash
ssh-keygen -t ed25519 -C "sergey.nikolaienkov+coursera@gmail.com" -f ~/.ssh/github-coursera-ed25519
```

Add `.pub` in [github.com/settings/keys](https://github.com/settings/keys).

```bash
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/github-coursera-ed25519
# Test if it works.
ssh -T git@github.com
```

```bash
# Clone the "private" repo
mkdir assignments && cd assignments
git init .
git remote add origin git@github.com:sergeynv-coursera/coursera-advanced-embedded-linux-development-specialization.git
git fetch origin
git checkout origin/main
git submodule update --init --recursive

# Add other remotes
git remote add base https://github.com/cu-ecen-aeld/aesd-assignments.git
git remote add a3+ git@github.com:cu-ecen-aeld/assignments-3-and-later-sergeynv-coursera.git
# git remote add a2 git@github.com:cu-ecen-aeld/assignment-2-sergeynv-coursera.git
# git remote add a1 git@github.com:cu-ecen-aeld/assignment-1-sergeynv-coursera.git
```

## References

### Setup
- [GCloud](https://github.com/cu-ecen-aeld/aesd-assignments/wiki/Installing-a-Google-Cloud-Platform-(GCP)-based-actions-runner-or-build-system)
- [ARM GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
  - Download `AArch64 GNU/Linux target (aarch64-none-linux-gnu)`
    - Note: not available for Mac!


```bash
# Look for the most recent "AArch64 GNU/Linux target (aarch64-none-linux-gnu)" at
# https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
DOWNLOAD_URL="https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-aarch64-none-linux-gnu.tar.xz?rev=22c39fc25e5541818967b4ff5a09ef3e&hash=E7676169CE35FC2AAECF4C121E426083871CA6E5"

# Parse out the filename from the URL.
# Remove prefix.
FILENAME="arm-gnu-toolchain-${DOWNLOAD_URL#*arm-gnu-toolchain-}"
# Remove suffix.
FILENAME="${FILENAME%.tar.xz*}.tar.xz"

wget -O "${FILENAME}" "$DOWNLOAD_URL"

tar -xvf ${FILENAME}
# Remove the archive?
# rm ${FILENAME}
```

### Assignments

#### Course 1

##### Week 1
- [Assignment 1](https://www.coursera.org/learn/linux-system-programming-introduction-to-buildroot/supplement/bnixD/assignment-1-instructions)
- [Assignment 2](https://www.coursera.org/learn/linux-system-programming-introduction-to-buildroot/supplement/U1Beh/assignment-2-instructions)

##### Week 2
- [Assignment 3, Part 1](https://www.coursera.org/learn/linux-system-programming-introduction-to-buildroot/supplement/Nh4LM/assignment-3-part-1-instructions)
- [Assignment 3, Part 2](https://www.coursera.org/learn/linux-system-programming-introduction-to-buildroot/supplement/YGf42/assignment-3-part-2-instructions)

##### Week 3
- [Assignment 4, Part 1](https://www.coursera.org/learn/linux-system-programming-introduction-to-buildroot/supplement/GT0Ld/assignment-4-part-1-instructions)
- [Assignment 4, Part 2](https://www.coursera.org/learn/linux-system-programming-introduction-to-buildroot/supplement/fdk6R/assignment-4-part-2-instructions)

##### Week 4
- [Assignment 5, Part 1](https://www.coursera.org/learn/linux-system-programming-introduction-to-buildroot/supplement/c7ZrG/assignment-5-part-1-instructions)
  - [Sockets (PDF)](https://d3c33hcgiwev3.cloudfront.net/BKfAy6NTSyinwMujU2socA_0ee2c6e43dec4ce99488f4f3ff90d8f1_Sockets.pdf?Expires=1705104000&Signature=Uk9~sx2YRhs1LxI9uE0pPkojOZ49gp5Eh9jxVXVAZuz7DdE9o4bdvH-Jw3gRonaNgL5IcSC50~vUZlhMwtmuX~7eurf4jX5wjKYcurVAhiqGjYNio7bEz4ZlvpxAoA19Ck2Uc62a-3an45c1Qg7eqiR8TmBKi5mzLbVQcLsX8sg_&Key-Pair-Id=APKAJLTNE6QMUY6HBC5A)
  - [Signals (PDF)](https://d3c33hcgiwev3.cloudfront.net/67c2eKNUSSC3NnijVMkgXg_7bce9fe876d94bf8a592d3e77fe0c4f1_Signal-Management.pdf?Expires=1705104000&Signature=jRrDmLdi8LZjPLoh7WIBBnHFe5RZ2ZPDqjCs-SwQ639fHhzaCDEdRUnd-KY16Wg2uQPSddVE8zsyFejms3Xkt2nIv1qYAyqQC~BhQ2jWebWf-3-wlxX~sBn0bcyL35cJlCOLxEFJYZdAdgO-aanu5EQ0kJJvQJReluySxTagwcM_&Key-Pair-Id=APKAJLTNE6QMUY6HBC5A)
  - [Processes and Daemons (PDF)](https://d3c33hcgiwev3.cloudfront.net/ZkK8S2gSTN2CvEtoEozd1g_c10c3f0aadb74a849ba52fc90af058f1_Processes-and-Daemons.pdf?Expires=1705190400&Signature=damMfNjv1-DFb8KQi17lw6ZXNwq7zjVFL8fB5EXBIwvPGmfZ9DIECPECD-egvQ-zmrf0o~33xj44dJ1pgU0jWyvMUzDCo-okEGg1gXxJX6ugD3UhkSyMiR3xm0cLg9KdB-U9D0r6NNFOmbCbnPWLenzRrhh0jr24M0nOrQ0CTYc_&Key-Pair-Id=APKAJLTNE6QMUY6HBC5A)
- [Assignment 5, Part 2](https://www.coursera.org/learn/linux-system-programming-introduction-to-buildroot/supplement/oLYlx/assignment-5-part-2-instructions)

### Testing

The basis of the automated test implementation for this repository comes from [https://github.com/cu-ecen-aeld/assignment-autotest/](https://github.com/cu-ecen-aeld/assignment-autotest/)

The assignment-autotest directory contains scripts useful for automated testing  Use
```
git submodule update --init --recursive
```
to synchronize after cloning and before starting each assignment, as discussed in the assignment instructions.

As a part of the assignment instructions, you will setup your assignment repo to perform automated testing using github actions.  See [this page](https://github.com/cu-ecen-aeld/aesd-assignments/wiki/Setting-up-Github-Actions) for details.

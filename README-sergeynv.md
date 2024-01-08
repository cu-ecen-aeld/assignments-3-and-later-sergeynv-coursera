### GitHub + Repos

#### Sources
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
git remote add asignments-base https://github.com/cu-ecen-aeld/aesd-assignments.git
git remote add asignments-1 git@github.com:cu-ecen-aeld/assignment-1-sergeynv-coursera.git
git remote add asignments-2 git@github.com:cu-ecen-aeld/assignment-2-sergeynv-coursera.git
git remote add asignments-3 git@github.com:cu-ecen-aeld/assignments-3-and-later-sergeynv-coursera.git
```

### Course 1

#### Setup
- [GCloud](https://github.com/cu-ecen-aeld/aesd-assignments/wiki/Installing-a-Google-Cloud-Platform-(GCP)-based-actions-runner-or-build-system)
- [ARM GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
  - Download `AArch64 GNU/Linux target (aarch64-none-linux-gnu)`
    - Note: not available for Mac!

#### Week 1
- [Assignment 1 Instructions](https://www.coursera.org/learn/linux-system-programming-introduction-to-buildroot/supplement/bnixD/assignment-1-instructions)
- [Assignment 2 Instructions](https://www.coursera.org/learn/linux-system-programming-introduction-to-buildroot/supplement/U1Beh/assignment-2-instructions)

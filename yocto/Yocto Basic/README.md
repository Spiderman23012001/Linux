## 1. Architecture
![image](./img/architecture.png)
- Source mirror
    - Upstream
    - Local projects
    - SCMs
- Config 
    - **conf/local.conf**
    - **conf/bblayers.conf**
    - meta/your-layer/conf/machine/machine.conf
    - meta/your-layer/conf/distro/distro.conf
    - bitbake.conf
    - meta/your-layer/conf/layer.conf
    - meta/conf/auto.conf
- Source Fetching
- Patch Application
- Configuration/Compile/Autoreconf as needed
## 2. Environment Setup and Downloading Poky Reference Distribution
- Build host package: `sudo apt-get install build-essential chrpath cpio debianutils diffstat file gawk gcc git iputils-ping libacl1 liblz4-tool locales python3 python3-git python3-jinja2 python3-pexpect python3-pip python3-subunit socat texinfo unzip wget xz-utils zstd`
- Use Git to Clone Poky: `git clone git://git.yoctoproject.org/poky -b scarthgap`
- Install extension in VSCode:
    - Code runner
    - Device Tree
    - QML
    - Bash IDE
    - Bitbake
    - vscode-pdf
    - Yocto Project Bitbake
- Find `find . -name "*.txt"`
## 3. Yocto Project Basic Configuration and Examining Poky Source
- Initialize the Build Environment
    - `source oe-init-build-env`
- Operator:
    - `=:` Assigns a value to a variable (overwrites the current value).

    - `+=`: Adds a value to a variable (string or list).

    - `?=`: Assigns a value to a variable only if the variable does not have a value.

    - `??=`: Assigns a value to a variable if the variable does not have a value or has an empty string.

    - `:=`: Assigns an immediate value to a variable, performing the calculation right when assigned.

    - `-=`: Removes an element from a variable (for strings or lists).

- `poky/build/conf`
    - `BBLAYERS`: Lists the layers used in the system.
    - 

## 4. Example in docker
- Dropbear
    - Install
        - sudo apt install bzip2 bunzip2 chrpath diffstat lz4
        - bitbake dropbear
    - Error:
        - `ERROR: quilt-native-0.67-r0 do_fetch: Bitbake Fetcher Error: NetworkAccess('https://download.savannah.gnu.org/releases/quilt/quilt-0.67.tar.gz', "/usr/bin/env wget -t 2 -T 100 -O /home/user/Android/Sdk/yocto/poky/build/downloads/quilt-0.67.tar.gz.tmp -P `
        - Solution
            - `conf/local.conf`: 
            - Add `BB_NO_NETWORK = "0"`
            - Add `CONNECTIVITY_CHECK_URIS = "https://www.yoctoproject.org/connectivity.html"`

## 5. UART 
- Add raspberrypi class:
    - Search meta-.. in `https://layers.openembedded.org/layerindex/branch/master/layers/`
    - `git clone git://git.yoctoproject.org/meta-raspberrypi -b scarthgap`
    

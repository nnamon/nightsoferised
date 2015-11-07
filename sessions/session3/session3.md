Session 3 - Night of Decompildation
==========================

In this session, we're going to look at reversing byte code of
interpreted/virtual machine based programming languages.

Objectives
----------
1. Java Reversing Set Up
2. Java Crackme
3. Python Reversing Set Up
4. Python Crackme

1. Java Reversing Set Up
------------------------

First, we need to install the Java runtime and development kit in our vagrant
environment.

```console
vagrant@erised:~$ sudo apt-get install openjdk-7-jre-headless openjdk-7-jdk
Reading package lists... Done
Building dependency tree
Reading state information... Done
...
Processing triggers for ca-certificates (20141019ubuntu0.14.04.1) ...
Updating certificates in /etc/ssl/certs... 0 added, 0 removed; done.
Running hooks in /etc/ca-certificates/update.d....
done.
done.
```

2. Java Crackme
---------------

We'll work on the Java crackme in the bears directory.

3. Python Reversing Set Up
--------------------------

First, we'll need to install pip.

```console
vagrant@erised:~$ sudo apt-get install python-pip
Reading package lists... Done
Building dependency tree
Reading state information... Done
The following extra packages will be installed:
  build-essential dpkg-dev g++ g++-4.8 libalgorithm-diff-perl
  libalgorithm-diff-xs-perl libalgorithm-merge-perl libdpkg-perl
  libfile-fcntllock-perl libstdc++-4.8-dev python-chardet-whl python-colorama
  python-colorama-whl python-distlib python-distlib-whl python-html5lib
  python-html5lib-whl python-pip-whl python-requests-whl python-setuptools
  python-setuptools-whl python-six-whl python-urllib3-whl python-wheel
  python3-pkg-resources
Suggested packages:
  debian-keyring g++-multilib g++-4.8-multilib gcc-4.8-doc libstdc++6-4.8-dbg
  libstdc++-4.8-doc python-genshi python-lxml python3-setuptools
...
```

Next, we will use pip to install uncompyle2.

```console
vagrant@erised:~$ sudo pip install uncompyle2
Downloading/unpacking uncompyle2
  Downloading uncompyle2-1.1.tar.gz (351kB): 351kB downloaded
  Running setup.py (path:/tmp/pip_build_root/uncompyle2/setup.py) egg_info for package uncompyle2

    warning: no files found matching 'MANIFEST'
    warning: no files found matching 'ANNOUNCE'
    warning: no files found matching 'CHANGES'
    warning: no files found matching 'TODO'
    warning: no files found matching 'uncompyle'
    warning: no previously-included files matching '*~' found anywhere in distribution
    warning: no previously-included files matching '.*~' found anywhere in distribution
Installing collected packages: uncompyle2
  Running setup.py install for uncompyle2
    changing mode of build/scripts-2.7/uncompyle2 from 644 to 755

    warning: no files found matching 'MANIFEST'
    warning: no files found matching 'ANNOUNCE'
    warning: no files found matching 'CHANGES'
    warning: no files found matching 'TODO'
    warning: no files found matching 'uncompyle'
    warning: no previously-included files matching '*~' found anywhere in distribution
    warning: no previously-included files matching '.*~' found anywhere in distribution
    changing mode of /usr/local/bin/uncompyle2 to 755
Successfully installed uncompyle2
Cleaning up...
vagrant@erised:~$
```

4. Python Crackme
-----------------

We'll work on the Python crackme in the morty directory.

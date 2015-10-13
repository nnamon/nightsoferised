Session 0 - Setup
=================

Introduction
------------

So, welcome to this little series of binary fun I am codenaming Erised. Because
Esrever is really hard to pronounce.

Anyway, these are a few things you have to get ready before the session starts
tomorrow.

Vagrant
-------

1. Install [Vagrant](https://www.vagrantup.com/) on the OS of your choice.

I recommend Virtualbox as the provider. Look up the instructions in the manual
on getting Vagrant up and running.

Initialise a base box. We will be using Ubuntu 14.04 64 bit as our main
reversing environment.

```
amon@Evanna:~/sproink/erised/environment$ vagrant init ubuntu/trusty64; vagrant up --provider virtualbox
A `Vagrantfile` has been placed in this directory. You are now
ready to `vagrant up` your first virtual environment! Please read
the comments in the Vagrantfile as well as documentation on
`vagrantup.com` for more information on using Vagrant.
Bringing machine 'default' up with 'virtualbox' provider...
==> default: Box 'ubuntu/trusty64' could not be found. Attempting to find and install...
    default: Box Provider: virtualbox
    default: Box Version: >= 0
==> default: Loading metadata for box 'ubuntu/trusty64'
    default: URL: https://vagrantcloud.com/ubuntu/trusty64
==> default: Adding box 'ubuntu/trusty64' (v20151008.0.0) for provider: virtualbox
    default: Downloading: https://atlas.hashicorp.com/ubuntu/boxes/trusty64/versions/20151008.0.0/providers/virtualbox.box
==> default: Successfully added box 'ubuntu/trusty64' (v20151008.0.0) for 'virtualbox'!
==> default: Importing base box 'ubuntu/trusty64'...
==> default: Matching MAC address for NAT networking...
==> default: Checking if box 'ubuntu/trusty64' is up to date...
==> default: Setting the name of the VM: environment_default_1444746381814_57899
==> default: Clearing any previously set forwarded ports...
==> default: Clearing any previously set network interfaces...
==> default: Preparing network interfaces based on configuration...
    default: Adapter 1: nat
==> default: Forwarding ports...
    default: 22 => 2222 (adapter 1)
==> default: Booting VM...
==> default: Waiting for machine to boot. This may take a few minutes...
    default: SSH address: 127.0.0.1:2222
    default: SSH username: vagrant
    default: SSH auth method: private key
    default: Warning: Connection timeout. Retrying...
    default: Warning: Remote connection disconnect. Retrying...
==> default: Machine booted and ready!
==> default: Checking for guest additions in VM...
==> default: Mounting shared folders...
    default: /vagrant => /home/amon/sproink/erised/environment
```

Now, test that your set up works by through SSH.

```
amon@Evanna:~/sproink/erised/environment$ vagrant ssh
Welcome to Ubuntu 14.04.3 LTS (GNU/Linux 3.13.0-65-generic x86_64)

 * Documentation:  https://help.ubuntu.com/

  System information as of Tue Oct 13 14:26:54 UTC 2015

  System load:  0.75              Processes:           92
  Usage of /:   2.9% of 39.34GB   Users logged in:     0
  Memory usage: 15%               IP address for eth0: 10.0.2.15
  Swap usage:   0%

  Graph this data and manage this system at:
    https://landscape.canonical.com/

  Get cloud support with Ubuntu Advantage Cloud Guest:
    http://www.ubuntu.com/business/services/cloud

0 packages can be updated.
0 updates are security updates.


vagrant@vagrant-ubuntu-trusty-64:~$ uname -a
Linux vagrant-ubuntu-trusty-64 3.13.0-65-generic #106-Ubuntu SMP Fri Oct 2 22:08:27 UTC 2015 x86_64 x86_64 x86_64 GNU/Linux
```

Switch Ubuntu apt sources to use Singapore mirrors.

```
vagrant@vagrant-ubuntu-trusty-64:~$ sudo cp /etc/apt/sources.list /etc/apt/sources.list.bak
vagrant@vagrant-ubuntu-trusty-64:~$ sudo sh -c 'sed -e "s:archive.ubuntu.com:sg.archive.ubuntu.com:g" /etc/apt/sources.list.bak > /etc/apt/sources.list'
```

Now, update Ubuntu.

```
vagrant@vagrant-ubuntu-trusty-64:~$ sudo apt-get update
... blah ...
vagrant@vagrant-ubuntu-trusty-64:~$ sudo apt-get upgrade
Reading package lists... Done
Building dependency tree
Reading state information... Done
Calculating upgrade... Done
0 upgraded, 0 newly installed, 0 to remove and 0 not upgraded.
vagrant@vagrant-ubuntu-trusty-64:~$
```

Install these for now:

```
vagrant@vagrant-ubuntu-trusty-64:~$ sudo apt-get install git curl gdb-multiarch python
```

And that's all you need to do for Session 0!


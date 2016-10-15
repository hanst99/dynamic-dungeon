# -*- mode: ruby -*-
# vi: set ft=ruby :

# All Vagrant configuration is done below. The "2" in Vagrant.configure
# configures the configuration version (we support older styles for
# backwards compatibility). Please don't change it unless you know what
# you're doing.
Vagrant.configure('2') do |config|
  config.vm.box = 'terrywang/archlinux'
  config.vm.provider 'virtualbox' do |vb|
    vb.gui = false
    # Reserve a little more memory so we don't run out when building large things
    vb.memory = '2048'
  end
  config.vm.provision :shell, path: 'box/provision.sh'
end

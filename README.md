# LYOS
自己动手写一个操作系统相信是每个程序员的梦想，这个例子是一个简单的实践。

---------------------------------

### 在虚拟机上运行
- sudo apt install virtualbox
- sudo apt install g++ binutils libc6-dev-i386 xorriso
- make mykernel.iso

第一次运行自己先手动新建一个名为LYOS的虚拟机，类型和版本都选Other，内存分配默认64M即可，并选择不添加虚拟硬盘，然后启动。这时会让选择一个启动盘，选择我们`make mykernel.iso`生成的mykernel.iso即可启动。
以后再运行，只需`make run`即可。

### 在物理机上运行
- 修改原来操作系统的/boot/grub/grub.cfg文件。末尾添加如下内容
```
### BEGIN LYOS ###
menuentry 'LYOS' {
        multiboot /boot/mykernel.bin
        boot
}
### END LYOS ###
```
- make install 
- 重启机器，选择LYOS

在物理机上运行借助了本来的引导程序，添加了一个启动项。
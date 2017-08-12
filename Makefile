GPPPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings
#g++参数：32位模式
ASPARAMS = --32
#as参数：32位模式
LDPARAMS = -melf_i386
#ld参数：链接生成32位文件
objects = obj/loader.o \
		  obj/gdt.o \
		  obj/kernel.o

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	g++ $(GPPPARAMS) -o $@ -c $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

lyoskernel.bin: linker.ld $(objects)		#生成最终可执行的kernel
	ld $(LDPARAMS) -T $< -o $@ $(objects)
	
lyoskernel.iso: lyoskernel.bin				#生成光盘镜像文件
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp lyoskernel.bin iso/boot/lyoskernel.bin
	echo 'set timeout=0' 					  > iso/boot/grub/grub.cfg
	echo 'set default=0' 					 >> iso/boot/grub/grub.cfg
	echo '' 								 >> iso/boot/grub/grub.cfg
	echo 'menuentry "LYOS" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/lyoskernel.bin' 	 >> iso/boot/grub/grub.cfg
	echo '  boot' 							 >> iso/boot/grub/grub.cfg
	echo '}' 								 >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=lyoskernel.iso iso
	rm -rf iso

run: lyoskernel.iso							#在VirtualBox上运行
	(killall VirtualBox && sleep 1) || true
	VirtualBox --startvm "LYOS" &
	
close:										#关闭VirtualBox
	(killall VirtualBox && sleep 1) || true

install: lyoskernel.bin						#安装内核
	sudo cp $< /boot/lyoskernel.bin

.PHONY: clean								#清除编译生成的文件
clean:
	rm -rf obj lyoskernel.iso lyoskernel.bin

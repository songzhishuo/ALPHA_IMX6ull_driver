# 本节演示了最基本的字符设备驱动的模型

在整个驱动中并不能自动创建设备结点，因此在insmod 之后需要手动的通过 mknod命令去创建结点

```shell
insmod chr_dev_hello.ko    //注册设备驱动

cat /proc/devices        //查看设备结点的设备号

mknod /dev/chr_dev_hello c [主设备号] [次设备号]     //创建设备文件

cat 'hello' > /dev/chr_dev_hello

```

## 注:
正点原子自带的zImage 中已经集成了led的驱动，因此需要先关闭led的闪烁控制，否则看不到效果
``` shell
echo none > /sys/class/leds/red/trigger
```

#include "linux/kernel.h"
#include "linux/module.h"
#include "linux/fs.h"
#include "linux/init.h"
#include "linux/types.h"
#include "linux/errno.h"
#include "linux/uaccess.h"
#include "linux/kdev_t.h"
#define MAX_SIZE 1024

static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *file, char __user *user, size_t t, loff_t *f);
static ssize_t my_write(struct file *file, const char __user *user, size_t t, loff_t *f);

static char message[MAX_SIZE] = "succeed!";
static int device_num = 0;//�豸��
static int counter = 0;//������
static int mutex = 0;//������
static char* devName = "myDevice";//�豸��

struct file_operations pStruct =
{ open:my_open, release:my_release, read:my_read, write:my_write, };

// ע��ģ��
int init_module()
{
	int ret;
	/* ��һ��������ָ��ע����豸�����豸����ϵͳ���䣬
	 * �ڶ������������豸ע��ʱ���豸���֣�
	 * ������������ָ��file_operations��ָ�룬
	 * �����豸��Ϊ0����ʱ��ϵͳһ�������õ��豸�Ŵ���ģ�� */
	ret = register_chrdev(0, devName, &pStruct);
	if (ret < 0)
	{
		printk("regist failure!\n");
		return -1;
	}
	else
	{
		printk("the device has been registered!\n");
		device_num = ret;
		printk("the virtual device's major number %d.\n", device_num);
		printk("------'mknod /dev/myCharDev c %d 0'-------\n", device_num);
		printk("Use \"rmmod\" to remove the module\n");

		return 0;
	}
}

// ע��ģ��
void cleanup_module()
{
	unregister_chrdev(device_num, devName);
	printk("unregister it success!\n");
}

//���豸
static int my_open(struct inode *inode, struct file *file)
{
        if(mutex)
                return -EBUSY;
        mutex = 1;//����
	printk("main  device : %d\n", MAJOR(inode->i_rdev));
	printk("slave device : %d\n", MINOR(inode->i_rdev));
	printk("%d times to call the device\n", ++counter);
	try_module_get(THIS_MODULE);
	return 0;
}

// ÿ��ʹ�������ͷ�
static int my_release(struct inode *inode, struct file *file)
{
	printk("Device released!\n");
	module_put(THIS_MODULE);
        mutex = 0;//����
	return 0;
}

//������
static ssize_t my_read(struct file *file, char __user *user, size_t t, loff_t *f)
{
	if(copy_to_user(user,message,sizeof(message))) // �ں˿ռ䵽�û��ռ�ĸ��� 
	{
		return -EFAULT;
	}
	return sizeof(message);
}

//д����
static ssize_t my_write(struct file *file, const char __user *user, size_t t, loff_t *f)
{
	if(copy_from_user(message,user,sizeof(message))) //�û��ռ䵽�ں˿ռ�ĸ��� 
	{
		return -EFAULT;
	}
	return sizeof(message);
}



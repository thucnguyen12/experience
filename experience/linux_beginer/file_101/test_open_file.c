#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

MODULE_AUTHOR ("THUCND");
MODULE_LICENSE("GPL");

extern int errno;

static int __init simple_init(void)
{
	printk(KERN_INFO "HELLO KERNEL\n");
}

static void __exit simple_exit(void)
{
	printk(KERN_INFO "goobye\n")
}

module_init(simple_init);
module_exit(simple_exit);

int main()
{
	int fd = open (/mnt/d/tai_lieu/test.txt, O_RONLY | OCREAT);
	printf ("fd = %d\n", fd);
	if (fd == -1){
	        // print which type of error have in a code
        	printf("Error Number % d\n", errno);
  
        	// print program detail "Success or failure"
        	perror("Program");
	}
	char test[25];
	size_t byte_write = write (fd, "testing write to file", 22);
	off_t position = lseek (fd, 10, SEEK_SET);
	size_t byte_read = read(fd, test, sizeof(test));
	printf ("data read: %s\r\n", test);
	close(fd);
	return 0;
}

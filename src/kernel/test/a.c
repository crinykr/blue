void test_thread(){
	int i = 0;

	while (1) {
		printk("hello %d", i++);

		sleep(1000);
	}
}

#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "terminal.h"
#include "keyboard.h"
#include "filesystem.h"
#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}

int filesystem_test();
/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here
// exception test1

/* 
 * divide_zero_test
 *   DESCRIPTION: Performs a division by zero to test error handling.
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: 
 *     - FAIL indicating that the division by zero test failed.
 *   SIDE EFFECTS: None
 */
int divide_zero_test(){
	TEST_HEADER;
	clear();
	
	int i,j;
	i = 2;
	j = 0;
	i /= j; 
	return FAIL;
}

//paging test
/* 
 * paging_test
 *   DESCRIPTION: Tests memory paging by accessing memory addresses and printing their contents.
 *   INPUTS: None
 *   OUTPUTS: Prints the contents of memory addresses.
 *   RETURN VALUE: 
 *     - PASS indicating that the paging test passed successfully.
 *   SIDE EFFECTS: None
 */
int paging_test(){
	clear();
	TEST_HEADER;
	uint32_t  addr = 0x400000;
	int i;

	for (i=0;i<10;++i){
		addr = addr + 50*i;
		printf("Content at address %#x: %#x\n",addr,(*(uint32_t *)addr));
	}
	while (addr < 0x700000){
		addr += 0x100000;
		printf("Content at address %#x: %#x\n",addr,(*(uint32_t *)addr));
	}
	for (i=0;i<5;++i){
		addr = 0xB8000 + 50*i;
		printf("Content at address %#x: %#x\n",addr,(*(uint32_t *)addr));
	}	
	//edge cases
		printf("Edge cases:\n");
	addr = 0xB8FFF;
		printf("Content at address %#x: %#x\n",addr,(*(uint8_t *)addr));
	addr = 0x7FFFFF;
		printf("Content at address %#x: %#x\n",addr,(*(uint8_t *)addr));
	// addr = 0xb9000;
	// 	printf("Content at address %#x: %#x\n",addr,(*(uint32_t *)addr));

	return PASS;
	}

//exception test2
/* 
 * null_address_test
 *   DESCRIPTION: Tests dereferencing a null pointer to trigger a null pointer exception.
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: 
 *     - FAIL indicating that the null address test failed.
 *   SIDE EFFECTS: None
 */
int null_address_test(){
	TEST_HEADER;
	clear();

	int* addr = NULL;
	int invalid;
	invalid = *addr;
	return FAIL;
}

//system call test
/* 
 * syscall_test
 *   DESCRIPTION: Tests system call functionality by triggering a software interrupt.
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: 
 *     - FAIL indicating that the syscall test failed.
 *   SIDE EFFECTS: None
 */
int syscall_test(){
	TEST_HEADER;
	clear();
	__asm__("int	$0x80"); 
	return FAIL;
}


/*
 * rtc_test
 *   DESCRIPTION: Test function to verify the functionality of the real-time clock (RTC).
 *   INPUTS: None
 *   OUTPUTS: Prints messages indicating success or failure of RTC operations.
 *   RETURN VALUE: PASS if RTC operations succeed, FAIL otherwise.
 *   SIDE EFFECTS: May change the frequency of the RTC and read from it.
 */
int rtc_test(){
	TEST_HEADER;
	clear();
	int32_t freq,i,buf;
	uint8_t* filename = (uint8_t*)"rtc";
	if (!rtc_open(filename)){
		printf("rtc_open success! The frequency has been set to 2 Hz.\n");
	}
	else	return FAIL;
	for (freq = 2; freq < 2048; freq = freq * 2 ){
		rtc_write(0,&freq,sizeof(int32_t));	//change the frequency between 2 to 2^10
		for (i = 0;i < freq; ++i){
			rtc_read(0,&buf,sizeof(int32_t));	//read rtc
			printf("1");
		}
		clear();
	}
	if (!rtc_close(0)){
		printf("\nrtc_close success!\n");
	}
	else	return FAIL;

	return PASS;
}


/*
 * terminal_test
 *   DESCRIPTION: Test function to verify the functionality of the terminal driver.
 *   INPUTS: None
 *   OUTPUTS: Reads input from the terminal and writes it back to the terminal indefinitely.
 *   RETURN VALUE: This function does not return.
 *   SIDE EFFECTS: Interacts with the terminal driver.
 */
int terminal_test()
{
	TEST_HEADER;
	clear();
	char buf[129];	//random large number for enough space
	while (1){
		memset(buf, 0, 129);
		terminal_read(0, buf, 128);
		terminal_write(0, buf, 128);
	}
	return PASS;
}

/*
 * filename_test_long
 *   DESCRIPTION: Test function to verify the functionality of file operations with a long filename.
 *   INPUTS: None
 *   OUTPUTS: Prints the content of a file with a long filename to the console.
 *   RETURN VALUE: PASS if the test passes, FAIL otherwise.
 *   SIDE EFFECTS: Interacts with file operations.
 */
int filename_test_long() {
	TEST_HEADER;
	clear();

	int i;
	const uint8_t fname[32] = "verylargetextwithverylongname.tx";//filename length restricted to 32
	uint8_t buf[10000];	//random large number for enough space

	open_file(fname);
	read_file(0, buf, 10000);

	while (i < 10000 && buf[i] != NULL) {
		printf("%c", buf[i]);
		i++;
	}

	i = 0;
	printf("\nFILENAME: ");
	while (i < 32) {
		printf("%c", fname[i]);
		i++;
	}
	printf("\n");

	close_file(0);

	return PASS;
}

/*
 * filename_test_short
 *   DESCRIPTION: Test function to verify the functionality of file operations with a short filename.
 *   INPUTS: None
 *   OUTPUTS: Prints the content of a file with a short filename to the console.
 *   RETURN VALUE: PASS if the test passes, FAIL otherwise.
 *   SIDE EFFECTS: Interacts with file operations.
 */
int filename_test_short() {
	TEST_HEADER;
	clear();

	int i;
	const uint8_t fname[10] = "frame0.txt";//filename length restricted to 32
	uint8_t buf[10000];	//random large number for enough space

	open_file(fname);
	read_file(0, buf, 10000);

	while (i < 10000 && buf[i] != NULL) {
		printf("%c", buf[i]);
		i++;
	}

	i = 0;
	printf("\nFILENAME: ");
	while (i < 10) {
		printf("%c", fname[i]);
		i++;
	}
	printf("\n");

	close_file(0);

	return PASS;
}

/*
 * filename_test_exe
 *   DESCRIPTION: Test function to verify the functionality of file operations with an executable filename.
 *   INPUTS: None
 *   OUTPUTS: Prints the content of an executable file to the console.
 *   RETURN VALUE: PASS if the test passes, FAIL otherwise.
 *   SIDE EFFECTS: Interacts with file operations.
 */
int filename_test_exe() {
	TEST_HEADER;
	clear();

	int i;
	const uint8_t fname[32] = "ls";	//filename length restricted to 32
	uint8_t buf[6000];//random large number for enough space


	open_file(fname);
	read_file(0, buf, 6000);

	while (i < 6000) {
		if (buf[i] != NULL) {
			printf("%c", buf[i]);
		}
		if (buf[i] == 'Z') {
			i = 6000;
		}
		i++;
	}

	i = 0;
	printf("\nFILENAME: ");
	while (i < 2) {
		printf("%c", fname[i]);
		i++;
	}
	printf("\n");

	close_file(0);

	return PASS;
}

/*
 * read_dir_test
 *   DESCRIPTION: Test function to verify the functionality of reading a directory.
 *   INPUTS: None
 *   OUTPUTS: Prints the content of the directory to the console.
 *   RETURN VALUE: PASS if the test passes, FAIL otherwise.
 *   SIDE EFFECTS: Interacts with directory operations.
 */
int read_dir_test() {
	TEST_HEADER;
	clear();

	const uint8_t fname[32] = ".";	//filename length restricted to 32
	uint8_t buf[2000];//random large number for enough space

	int i = 0;

	open_dir(fname);

	while (i < 20) {
		read_dir(0, buf, 2000);
		i++;
	}
	printf("\n");

	close_dir(0);

	return PASS;
}


/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
/*launch tests*/
void launch_tests(){
	/*ckpt1*/
	// TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("divide_zero", divide_zero_test());
	// TEST_OUTPUT("paging_test", paging_test());
	// TEST_OUTPUT("null_address_test", null_address_test());
	// TEST_OUTPUT("syscall_test", syscall_test());

	/*ckpt2*/
	// TEST_OUTPUT("rtc_test", rtc_test());
	// TEST_OUTPUT("terminal_test", terminal_test());
	TEST_OUTPUT("filesys_test_short", filename_test_short());
	// TEST_OUTPUT("filesys_test_long", filename_test_long());
	// TEST_OUTPUT("filesys_test_exe", filename_test_exe());

	// TEST_OUTPUT("read_dir_test", read_dir_test());


}


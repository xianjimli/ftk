#include "ftk_util.h"
static const char* normalize_path_name(const char* in, char* out)
{
	if(in != NULL)
	{
		strcpy(out, in);
		ftk_normalize_path(out);
		return out;
	}
	else
	{
		return NULL;
	}
}

static void test_strtol()
{
	int i = 0;
	const char* p1 = NULL;
	char* p2 = NULL;
	const char* strs[] = 
	{
		"1",
		"12",
		"123",
		"+1",
		"+12",
		"+123",
		"-1",
		"-12",
		"-123",
		"1234ABCD",
		"-1END",
		"-12END",
		"-123END",
		"0END",
		NULL
	};

	for(i = 0; strs[i] != NULL; i++)
	{
		assert(ftk_strtol(strs[i], &p1, 8) == strtol(strs[i], &p2, 8));
		assert(p1 == p2);
		assert(ftk_strtol(strs[i], &p1, 10) == strtol(strs[i], &p2, 10));
		assert(p1 == p2);
		assert(ftk_strtol(strs[i], &p1, 16) == strtol(strs[i], &p2, 16));
		assert(p1 == p2);
	}

	return;
}

static void test_atoi_itoa()
{
	int i = 0;
	char str[32] = {0};
	const char* strs[] = 
	{
		"1",
		"12",
		"123",
		"+1",
		"+12",
		"+123",
		"-1",
		"-12",
		"-123",
		"1234567",
		"0",
		NULL
	};

	for(i = 0; strs[i] != NULL; i++)
	{
		assert(ftk_atoi(strs[i]) == atoi(strs[i]));
	}

	assert(strcmp(ftk_itoa(str, sizeof(str), 0), "0") == 0);
	assert(strcmp(ftk_itoa(str, sizeof(str), 1), "1") == 0);
	assert(strcmp(ftk_itoa(str, sizeof(str), 12), "12") == 0);
	assert(strcmp(ftk_itoa(str, sizeof(str), 123), "123") == 0);
	assert(strcmp(ftk_itoa(str, sizeof(str), -1), "-1") == 0);
	assert(strcmp(ftk_itoa(str, sizeof(str), -12), "-12") == 0);
	assert(strcmp(ftk_itoa(str, sizeof(str), -123), "-123") == 0);
	assert(strcmp(ftk_itoa(str, sizeof(str), 1234567890), "1234567890") == 0);
	assert(strcmp(ftk_itoa(str, sizeof(str), -1234567890), "-1234567890") == 0);


	return;
}

static void test_atof_ftoa()
{
	int i = 0;
	char str[32] = {0};
	const char* strs[] = 
	{
		"1.0",
		"12.12",
		"123.123",
		"+1.1",
		"+12.12",
		"+123.123",
		"-1.1",
		"-12.12",
		"-123.123",
		"123456789.123",
		"0.0",
		NULL
	};

	for(i = 0; strs[i] != NULL; i++)
	{
		float a = ftk_atof(strs[i]);
		float b = atof(strs[i]);
		float r = a - b;
		assert(r < 0.00001 && r > -0.00001);
	}

	assert(strcmp(ftk_ftoa(str, sizeof(str), 0), "0") == 0);
	assert(strcmp(ftk_ftoa(str, sizeof(str), 1), "1") == 0);
	assert(strcmp(ftk_ftoa(str, sizeof(str), 1.1), "1.1") == 0);
	assert(strcmp(ftk_ftoa(str, sizeof(str), 1.10), "1.1") == 0);
	assert(strcmp(ftk_ftoa(str, sizeof(str), -1), "-1") == 0);
	assert(strcmp(ftk_ftoa(str, sizeof(str), -1.1), "-1.1") == 0);
	assert(strcmp(ftk_ftoa(str, sizeof(str), -1.10), "-1.1") == 0);
	assert(strncmp(ftk_ftoa(str, sizeof(str), 1234567.123456), "1234567.12345", 13) == 0);
	assert(strncmp(ftk_ftoa(str, sizeof(str), -1234567.123456), "-1234567.12345", 14) == 0);

}

static void test_ftk_strs_cat()
{
	char path[FTK_MAX_PATH] = {0};
	
	assert(strcmp(ftk_strs_cat(path, FTK_MAX_PATH, NULL), "") == 0);
	assert(strcmp(ftk_strs_cat(path, FTK_MAX_PATH, "/", NULL), "/") == 0);
	assert(strcmp(ftk_strs_cat(path, FTK_MAX_PATH, "/", "a", NULL), "/a") == 0);
	assert(strcmp(ftk_strs_cat(path, FTK_MAX_PATH, "/", NULL, "/", "b", NULL), "/") == 0);
	assert(strcmp(ftk_strs_cat(path, FTK_MAX_PATH, "/", "a", "/", "b", NULL), "/a/b") == 0);

	return;
}

void test_normalize_path()
{
	char path_out[FTK_MAX_PATH + 1] = {0};
	int chdir_ret = 0;

	assert(normalize_path_name(NULL, path_out) == NULL);
	assert(normalize_path_name(NULL, NULL) == NULL);
	assert(strcmp(normalize_path_name("/test", path_out), "/test") == 0);
	assert(strcmp(normalize_path_name("/test/.", path_out), "/test/") == 0);
	assert(strcmp(normalize_path_name("/test/..", path_out), "/") == 0);
	assert(strcmp(normalize_path_name("/test/../..", path_out), "/") == 0);
	assert(strcmp(normalize_path_name("/test/../test/..", path_out), "/") == 0);
	assert(strcmp(normalize_path_name("/test.txt", path_out), "/test.txt") == 0);
	assert(strcmp(normalize_path_name("//////test.txt", path_out), "/test.txt") == 0);
	assert(strcmp(normalize_path_name("/./././././test.txt", path_out), "/test.txt") == 0);

	chdir_ret = chdir("/home");
	assert(strcmp(normalize_path_name("test.txt", path_out), "/home/test.txt") == 0);
	assert(strcmp(normalize_path_name("./test.txt", path_out), "/home/test.txt") == 0);
	assert(strcmp(normalize_path_name("./", path_out), "/home/") == 0);
	assert(strcmp(normalize_path_name(".", path_out), "/home") == 0);

	setenv("HOME", "/home/lixianjing", 1);
	assert(strcmp(normalize_path_name("~/test.txt", path_out), "/home/lixianjing/test.txt") == 0);
	assert(strcmp(normalize_path_name("~/", path_out), "/home/lixianjing/") == 0);
	assert(strcmp(normalize_path_name("~", path_out), "/home/lixianjing") == 0);

	return ;
}

static void test_ftk_parse_color()
{
	FtkColor c = {0};

	c = ftk_parse_color("AABBCCDD");
	assert(c.a == 0xAA && c.r == 0xBB && c.g == 0xCC && c.b == 0xDD);
	
	c = ftk_parse_color("11223344");
	assert(c.a == 0x11 && c.r == 0x22 && c.g == 0x33 && c.b == 0x44);

	return;
}

int main(int argc, char* argv[])
{
	int i = 0;
	unsigned short codes[] = {0x9690, 0x5f0f, 0x58f0, 0x660e, 0x4e0e, 0x5185, 0x5efa, 0x51fd, 0x6570};
	const char* str = "隐式声明与内建函数";
	const char* p = str;

	for(i = 0; *p; i++)
	{
		assert(codes[i] == utf8_get_char(p, &p));	
		assert(codes[i] == utf8_get_prev_char(p, NULL));
	}
	assert(utf8_count_char(str, strlen(str)) == 9);
	str = "abc";
	assert(utf8_count_char(str, strlen(str)) == 3);
	str = "abc中国";
	assert(utf8_count_char(str, strlen(str)) == 5);
	
	str = "abc中国";
	assert(utf8_count_char(str, 3) == 3);

	test_normalize_path();

	test_ftk_strs_cat();

	test_atoi_itoa();
	test_atof_ftoa();
	test_ftk_parse_color();
	test_strtol();

	return 0;
}

#include "ftk.h"
#include "ftk_expr.h"

typedef struct tagTESTCASE_T
{
	double val;
	char* expr;
}TESTCASE_T, *PTESTCASE_T;

TESTCASE_T g_aTestcase[] =
{
	{0.5, "0.5"},
	{8, "010"},
	{9, "011"},
	{1, "0x1"},
	{1, "0x001"},
	{15, "0x0F"},
	{15, "0XF"},
	{5, "05"},
	{15, "05+(5+0x5)"},
	{1, "1.00"},
	{3, "1.5+1.5"},
	{3, "1+1+1"},
	{3, "(1+1+1)*1.00"},
	{7.5, "(1.2+1.8)*2.5"},
	{1, "1"},
	{2, "1+1"},
	{1, "1|1"},
	{3, "2|1"},
	{1, "1&1"},
	{0, "2&1"},
	{3, "1+1+1"},
	{4, "(1+1)*2"},
	{5, "2*2+1"},
	{6, "2+(2*2)"},
	{7, "(1+2)+(2*2)"},
	{8, "64/8)"},
	{9, "(82-1)/9)"},
	{0, "(64/8-8)"},
	{1, "  1  "},
	{2, "  1  +  1  "},
	{3, "  1  +  1  +  1"},
	{4, "  (  1  +  1  )  *  2  "},
	{5, "  2  *  2  +  1  "},
	{6, "  2  +  (  2  *  2  )"},
	{7, "  (  1  +  2  )  +  (  2  *  2  )  "},
	{8, "  64 /  8  ) "},
	{9, "  (  82 -  1  )  /  9  )  "},
	{0, "  (  64 /  8  -  8  )  "},
	{0, NULL}
};

int main(int argc, char* argv[])
{
	int i = 0;
	double val = 0;

	ftk_set_allocator(ftk_allocator_default_create());
	while(g_aTestcase[i].expr != NULL)
	{
		val = ftk_expr_eval(g_aTestcase[i].expr);
		printf("%s=%lf\n", g_aTestcase[i].expr, val);
		assert(g_aTestcase[i].val == val);

		i++;
	}
	
	return 0;
}

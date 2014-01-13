#include "ftk.h"
#include "ftk_params.h"

int main(int argc, char* argv[])
{
	int i = 0;
	int nr = 100;
	char key[FTK_KEY_LEN+1];
	char value[FTK_VALUE_LEN+1];
	FtkParams* thiz = NULL;
	ftk_set_allocator(ftk_allocator_default_create());
	
	thiz = ftk_params_create(nr, nr);

	for(i = 0; i < nr; i++)
	{
		snprintf(key, FTK_KEY_LEN, "key%d", i);
		snprintf(value, FTK_VALUE_LEN, "2*($i%d+%d)-($i%d+%d))", i, i, i, i);
		assert(ftk_params_set_param(thiz, key, value) == RET_OK);

		snprintf(key, FTK_KEY_LEN, "i%d", i);
		snprintf(value, FTK_VALUE_LEN, "%d", i);
		assert(ftk_params_set_var(thiz, key, value) == RET_OK);
		
		snprintf(key, FTK_KEY_LEN, "key%d", i);

		assert(ftk_params_eval_int(thiz, key, 0) == 2 * i);
		ftk_logd("[%d] %d\n", i, ftk_params_eval_int(thiz, key, 0));
		ftk_logd("[%d] %s\n", i, ftk_params_eval_string(thiz, key));
	}

	ftk_params_dump(thiz);
	ftk_params_destroy(thiz);

	return 0;
}

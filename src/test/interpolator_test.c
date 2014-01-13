#include "ftk.h"
#include "ftk_interpolator_bounce.h"
#include "ftk_interpolator_linear.h"
#include "ftk_interpolator_accelerate.h"
#include "ftk_interpolator_decelerate.h"
#include "ftk_interpolator_acc_decelerate.h"

static void test_interpolator(const char* name, FtkInterpolator* interpolator)
{
	float percent = 0;
	
	ftk_logd("=========================%s==========================\n", name);
	for(percent = 0; percent < 1.05; percent += 0.05)
	{
		printf("%f --> %f\n", percent, ftk_interpolator_get(interpolator, percent));
	}

	ftk_interpolator_destroy(interpolator);

	return;
}

int main(int argc, char* argv[])
{
	ftk_set_allocator(ftk_allocator_default_create());
	
	test_interpolator("linear", ftk_interpolator_linear_create());
	test_interpolator("bounce", ftk_interpolator_bounce_create());
	test_interpolator("accelerate", ftk_interpolator_accelerate_create());
	test_interpolator("decelerate", ftk_interpolator_decelerate_create());
	test_interpolator("acc_decelerate", ftk_interpolator_acc_decelerate_create());

	return 0;
}

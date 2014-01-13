#include "ftk.h"
#include "ftk_key_board.h"

int main(int argc, char* argv[])
{
	ftk_init(argc, argv);

	ftk_key_board_test(argv[1]);

	return 0;
}

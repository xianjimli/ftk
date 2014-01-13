#include <stdio.h>

void gen_ascii(FILE* fp)
{
	unsigned short ch = 0x20;
	for(; ch < 127; ch++)
	{
		fwrite(&ch, sizeof(ch), 1, fp);
	}
}

void gen_cjk(FILE* fp)
{
	unsigned short ch = 0;
	
	for(ch = 0x4E00; ch <= 0x9FA5; ch++)
	{
		fwrite(&ch, sizeof(ch), 1, fp);
	}
	return;
}

int main(int argc, char* argv[])
{
	FILE* fp = fopen("ascii.txt", "wb+");
	
	if(fp != NULL)
	{
		gen_ascii(fp);
		fclose(fp);
	}
	
	fp = fopen("unicode.txt", "wb+");
	
	if(fp != NULL)
	{
		gen_ascii(fp);
		gen_cjk(fp);
		fclose(fp);
	}

	return 0;
}

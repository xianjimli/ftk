//Build gcc -g gb.c -o gb.exe

#include <stdio.h>

static void DumpAscii(FILE* fp)
{
	unsigned char c = 0;
	for(c = 32; c < 128; c++)
	{
      fwrite (&c, sizeof (c), 1, fp);
	}

	return;
}

static void DumpRange (FILE * fp, unsigned short n)
{
  unsigned char c = 0;
  unsigned short i = 0;

  for (i = 0; i < (0xFF - 0xA0); i++)
    {
      c = 0xA0 + n;
      fwrite (&c, sizeof (c), 1, fp);
      c = 0xA0 + i;
      fwrite (&c, sizeof (c), 1, fp);
    }

  fflush (fp);

  return;
}

static void DumpGB2312 (void)
{
  unsigned short i = 0;

  FILE *fp = fopen ("gb.txt", "wb+");

  if (fp != NULL)
    {
		DumpAscii(fp);
      for (i = 1; i <= 9; i++)
		DumpRange (fp, i);

      for (i = 16; i <= 55; i++)
		DumpRange (fp, i);

      for (i = 56; i <= 87; i++)
		DumpRange (fp, i);

      fclose (fp);
    }
  else
    {
      perror ("fopen");
    }

  return;
}

int main (int argc, char *argv[])
{
  DumpGB2312 ();

  return 0;
}

#include "ftk.h"
#include "ftk_xml_parser.h"
#include "xml_builder_dump.h"

#define XML "<?xml version=\"1.0\" encoding=\"utf-8\"?> \
<!--comment--> <br/> <p>ppp</p> <br />\
<programmer name=\"lixianjing\" blog=\"http://www.limodev.cn/blog\">text</programmer>"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "xml_builder_dump.h"

char* read_file(const char* file_name)
{
	char* buffer = NULL;
	FILE* fp = fopen(file_name, "r");
	size_t fread_ret = 0;

	if(fp != NULL)
	{
		struct stat st = {0};
		if(stat(file_name, &st) == 0)
		{
			buffer = malloc(st.st_size + 1);
			fread_ret = fread(buffer, st.st_size, 1, fp);
			buffer[st.st_size] = '\0';
		}
	}

	return buffer;
}

int main(int argc, char* argv[])
{
	FtkXmlParser* thiz = NULL;
	FtkXmlBuilder* builder = NULL;

	ftk_set_allocator(ftk_allocator_default_create());
	thiz = ftk_xml_parser_create();
	builder = ftk_xml_builder_dump_create(NULL);
	ftk_xml_parser_set_builder(thiz, builder);
	ftk_xml_parser_parse(thiz, XML, strlen(XML));
	
	if(argc > 1)
	{
		char* buffer = read_file(argv[1]);
		ftk_xml_parser_parse(thiz, buffer, strlen(buffer));
		free(buffer);
	}

	ftk_xml_builder_destroy(builder);
	ftk_xml_parser_destroy(thiz);

	return 0;
}


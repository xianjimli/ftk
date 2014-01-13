#ifndef XML_PARSER_H
#define XML_PARSER_H

#include "ftk_xml_builder.h"

FTK_BEGIN_DECLS

#define MAX_ATTR_NR 48
struct _FtkXmlParser;
typedef struct _FtkXmlParser FtkXmlParser;

FtkXmlParser* ftk_xml_parser_create(void);
void       ftk_xml_parser_set_builder(FtkXmlParser* thiz, FtkXmlBuilder* builder);
void ftk_xml_parser_parse(FtkXmlParser* thiz, const char* xml, int length);
void       ftk_xml_parser_destroy(FtkXmlParser* thiz);

FTK_END_DECLS

#endif/*XML_PARSER_H*/


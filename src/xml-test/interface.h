#ifndef _ARL_INTERFACE_H
#define _ARL_INTERFACE_H

//#include "TopPublic.h"
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


xmlDocPtr getdoc(char *docname);
xmlPathObjectPtr getnodeset(xmlDocPtr doc, xmlChar *xpath);
int findInXml(char *docname, char *keyword);

#endif

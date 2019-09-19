#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

#if 0
int main(int argc, char* argv[])
{
	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlChar *szKey;
	char *szDocName;

	if(argc <= 1)
	{
		printf("Usage:%s docname\n", argv[0]);
		return(0);
	}
	
	szDocName = argv[1];
	doc = xmlReadFile(szDocName, "UTF-8", XML_PARSE_RECOVER);
	
	if(NULL == doc)
	{
		fprintf(stderr, "xmlRarseFile Error in %s %d\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	curNode = xmlDocGetRootElement(doc);
	if(NULL == curNode)
	{
		fprintf(stderr, "xmlDocGetRootElement Error in %s %d\n", __FUNCTION__, __LINE__);
		xmlFreeDoc(doc);
		return -1;
	}
	
	printf("Node name is %s\n", curNode->name);
	
	if(xmlStrcmp(curNode->name, BAD_CAST "root"))
	{
		fprintf(stderr, "document of the wrong type, root node != root\n");
		xmlFreeDoc(doc);
		return -1;
	}
	
	curNode = curNode->xmlChildrenNode;
	xmlNodePtr propNodePtr = curNode;
	while(curNode != NULL)
	{
		if((!xmlStrcmp(curNode->name, (const xmlChar *)"monitor")))
		{
			szKey = xmlNodeGetContent(curNode);
			printf("newNode1:%s\n", szKey);
			xmlFree(szKey);
		}

		if(xmlHasProp(curNode, BAD_CAST "attribute"))
		{
			propNodePtr = curNode;
		}
		curNode = curNode->next;
	}

	xmlAttrPtr attrPtr = propNodePtr->properties;
	while(attrPtr != NULL)
	{
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "attribute"))
		{
			xmlChar* szAttr = xmlGetProp(propNodePtr, BAD_CAST "attribute");
			xmlFree(szAttr);
		}
		attrPtr = attrPtr->next;
	}
	
	xmlFreeDoc(doc);
	return 0;
}
#endif

int main(int argc, const char *argv[])
{
	if(2 != argc)
	{
		fprintf(stdout, "please start this program with %s xmlfilepath!", argv[0]);
		return 1;
	}
	xmlDocPtr pDoc = xmlReadFile(argv[1], "UTF-8", XML_PARSE_RECOVER);
	if(NULL == pDoc)
	{
		fprintf(stderr, "xmlParseFile Error in %s %d\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	xmlNodePtr pRoot = xmlDocGetRootElement(pDoc);
	if(NULL == pRoot)
	{
		fprintf(stderr, "xmlDocGetRootElement Error in %s %d\n", __FUNCTION__, __LINE__);
		xmlFreeDoc(pDoc);
		return -1;
	}

	printf("Node name is %s!\n", pRoot->name);
	xmlNodePtr pFirst = pRoot->xmlChildrenNode;
	while(NULL != pFirst)
	{
		if(!xmlStrcmp(pFirst->name, (const xmlChar *)("monitor")))
		{
			xmlNodePtr pSecond = pFirst->xmlChildrenNode;
			while(NULL != pSecond)
			{
				xmlChar* value = NULL;
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("name")))
				{
					value = xmlNodeGetContent(pSecond);
					printf("\n%s-->%s\n", pSecond->name, value);
					xmlFree(value);
					value = NULL;
				}
				
				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("path")))
				{
					value = xmlNodeGetContent(pSecond);
					printf("\n%s-->%s\n", pSecond->name, value);
					xmlFree(value);
					value = NULL;
				}

				if(!xmlStrcmp(pSecond->name, (const xmlChar *)("log")))
				{
					xmlNodePtr pThird = pSecond->xmlChildrenNode;
					while(NULL != pThird)
					{
						if(!xmlStrcmp(pThird->name, (const xmlChar *)("folderpath")))
						{
							value = xmlNodeGetContent(pThird);
							printf("\n%s-->%s\n", pThird->name, value);
							xmlFree(value);
							value = NULL;
						}
						if(!xmlStrcmp(pThird->name, (const xmlChar *)("savedays")))
						{
							value = xmlNodeGetContent(pThird);
							printf("\n%s-->%s\n", pThird->name, value);
							xmlFree(value);
							value = NULL;
						}
						
						pThird = pThird->next;
					}
				}
				pSecond = pSecond->next;
			}
		}
		
		pFirst = pFirst->next;
	}
	xmlFreeDoc(pDoc);
	return 0;
}


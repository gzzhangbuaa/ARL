#include "interface.h"

xmlDocPtr getdoc(char *docname)
{
	xmlDocPtr doc;
	doc = xmlParseFile(docname);
	
	if(NULL == doc)
	{
		fprintf(stderr, "Document not parsed successfully. \n");
		return NULL;
	}
	return doc;
}

xmlXPathObjectPtr getnodeset(xmlDocPtr doc, xmlChar *xpath)
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;
	
	context = xmlXPathNewContext(doc);
	if(context == NULL)
	{
		printf("Error in xmlXPathNewContext\n");
		return NULL;
	}
	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if(NULL == result)
	{
		printf("Error in xmlXPathEvalExpression\n");
		return NULL;
	}
	if(xmlXPathNodeSetIsEmpty(result->nodesetval))
	{
		xmlXPathFreeObject(result);
		printf("No result\n");
		return NULL;
	}
	return result;
}


int findInXml1(char *docname, char* target)
{
	xmlDocPtr doc;
	//xmlChar *xpath = (xmlChar *) "//keyword";
	int keysize = strlen(target);
	char *keyword = malloc(sizeof(char)*(keysize+1+2));
	memset(keyword, 0, sizeof(keyword));
	strcat(keyword, "//");
	strcat(keyword, target);
	printf("[DEBUG_INFO]:keyword is %s\n", keyword);
	xmlChar *xpath = (xmlChar *)keyword;
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	int i;
	xmlChar *keyvalue;
	
	if((access(docname, 0)) == -1)
	{
		printf("[DEBGU_INFO]:doc %s dosen't exists\n", docname);	
		return(-1);
	}
	doc = getdoc(docname);
	result = getnodeset(doc, xpath);
	if(result)
	{
		nodeset = result->nodesetval;
		for(i = 0; i < nodeset->nodeNr; i++)
		{	
			keyvalue = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
			printf("keyword is %s, key-value:%s\n", keyword, keyvalue);
			xmlFree(keyvalue);
		}
		xmlXPathFreeObject(result);
	}
	if(!keyword)
	{
		free(keyword);
		keyword = NULL;
	}
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return (0);
}

int findInXml(char *docname, char* keyword)
{
	xmlDocPtr doc;
	printf("[DEBUG_INFO]:keyword is %s\n", keyword);

	xmlChar *xpath = (xmlChar *)keyword;
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	int i;
	xmlChar *keyvalue;
	
	if((access(docname, 0)) == -1)
	{
		printf("[DEBGU_INFO]:doc %s dosen't exists\n", docname);	
		return(-1);
	}
	doc = getdoc(docname);
	result = getnodeset(doc, xpath);
	if(result)
	{
		nodeset = result->nodesetval;
		for(i = 0; i < nodeset->nodeNr; i++)
		{	
			keyvalue = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
			printf("keyword is %s, key-value:%s\n", keyword, keyvalue);
			xmlFree(keyvalue);
		}
		xmlXPathFreeObject(result);
	}
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return (0);
}

int getDMS(char *docname, char *keyword)
{
	int button = 0;
        xmlDocPtr doc;
        xmlNodePtr rootNode;
        doc = xmlReadFile(docname, "UTF-8", XML_PARSE_RECOVER);
        if(NULL == doc)
        {
                printf("[DEBUG_INFO]: xmlReadFile error in %s %d\n", __FUNCTION__, __LINE__);
                return -1;
        }
        rootNode = xmlDocGetRootElement(doc);
        if(rootNode == NULL)
        {
                printf("[DEBUG_INFO]: xmlGetRootElement error in %s %d\n", __FUNCTION__, __LINE__);
                xmlFreeDoc(doc);
                return -1;
        }

        printf("[DEBUG_INFO]:##### I am looking for %s in %s\n", keyword, docname);
        xmlNodePtr pFirst = rootNode->xmlChildrenNode;
        int flag = FALSE;

        while(NULL != pFirst)
        {
                printf("[DEBUG_INFO]:pFirst->name is %s\n", pFirst->name);

                if(!strcmp(pFirst->name, (const xmlChar *)keyword))
                {
                        xmlChar *value = xmlNodeGetContent(pFirst);
                        printf("[DEBUG_INFO]:current looking key is %s value is %s\n", pFirst->name, value);
                        if(value != NULL)
                        {
				button = atoi(value);	
                                xmlFree(value);
                        }
                        flag = TRUE;
                        break;
                }
                else
                {
                        xmlNodePtr pSecond = pFirst->xmlChildrenNode;
                        while(NULL != pSecond)
                        {
                                printf("[DEBUG_INFO]:pSecond->name is %s\n", pSecond->name);

                                if(!strcmp(pSecond->name, (const xmlChar *)keyword))
                                {
                                        xmlChar *value = xmlNodeGetContent(pSecond);
                                        printf("[DEBUG_INFO]:current looking key is %s value is %s\n", pSecond->name, value);
                                        if(value != NULL)
                                        {
						button = atoi(value);
                                                xmlFree(value);
                                        }
                                        flag = TRUE;
                                        break;
                                }
                                pSecond = pSecond->next;
                        }
                }
                if(flag)
                {
                        break;
                }
                pFirst = pFirst->next;
        }

        printf("[DEBUG_INFO]:##### switch value  is %d\n", button);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        return button;
}



#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

int main(int argc, char** argv)
{
	
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "root");
	xmlDocSetRootElement(doc, root_node);
	
	xmlNewChild(root_node, NULL, BAD_CAST "node1", BAD_CAST "content of node1");
	node = xmlNewChild(root_node, NULL, BAD_CAST "node3", BAD_CAST "node has attributes");
	xmlNewProp(node, BAD_CAST "attribute", BAD_CAST "yes");

	node = xmlNewNode(NULL, BAD_CAST "node4");
	node1 = xmlNewText(BAD_CAST "other way to create content");
	xmlAddChild(node, node1);
	xmlAddChild(root_node, node);
	//xmlSaveFormatFileEnc(argc > 1 ? argv[1] : "-", doc, "UTF-8", 1);
	xmlSaveFormatFileEnc("jobs-submit.xml", doc, "UTF-8", 1);	

	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();
	return(0);
	
	/*
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	xmlDocSetRootElement(doc, root_node);
	xmlNewTextChild(root_node, NULL, BAD_CAST "newNode1", BAD_CAST "newNode1 content");
	xmlNewTextChild(root_node, NULL, BAD_CAST "newNode2", BAD_CAST "newNode2 content");
	xmlNewTextChild(root_node, NULL, BAD_CAST "newNode3", BAD_CAST "newNode3 content");

	xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "node2");
	xmlNodePtr content = xmlNewText(BAD_CAST "NODE CONTENT");	
	xmlAddChild(root_node, node);
	xmlAddChild(node, content);
	xmlNewProp(node, BAD_CAST "attribute", BAD_CAST "yes");

	node = xmlNewNode(NULL, BAD_CAST "son");
	xmlAddChild(root_node, node);
	xmlNodePtr grandson = xmlNewNode(NULL, BAD_CAST "grandson");
	xmlAddChild(node, grandson);
	xmlAddChild(grandson, xmlNewText(BAD_CAST "This is a grandson node"));
	
	int nRel = xmlSaveFile("CreatedXml.xml", doc);
	if(nRel != -1)
	{
		printf("A XML file has been created, written into %d bytes\n", nRel);
	}
	xmlFreeDoc(doc);
	return 0;
	*/
}






















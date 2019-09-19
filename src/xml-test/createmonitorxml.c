#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define MONITOR_INFO  "monitor.xml"

int main(int argc, char **argv)
{
        createMonitorXml();
}

/**     create a initial xml file for initiation        **/
int createMonitorXml()
{

        xmlKeepBlanksDefault(0);
        xmlIndentTreeOutput = 1;
        xmlDocPtr doc = NULL;
        xmlNodePtr root_node = NULL, jobs_node = NULL, switch_node = NULL;
        doc = xmlNewDoc(BAD_CAST "1.0");
        root_node = xmlNewNode(NULL, BAD_CAST "Monitor");
        xmlDocSetRootElement(doc, root_node);

	xmlNodePtr placeholder_node = NULL;
	placeholder_node = xmlNewChild(root_node, NULL, BAD_CAST "Placeholder", BAD_CAST "0");


	xmlNodePtr setup_node = NULL, parameter_n_node = NULL, parameter_N_node = NULL, partition_node = NULL;
	xmlNodePtr shellpath_node = NULL, master_node = NULL, backup_node = NULL;

	setup_node = xmlNewChild(root_node, NULL, BAD_CAST "Setup", NULL);
	parameter_n_node = xmlNewChild(setup_node, NULL, BAD_CAST "parameter-n", BAD_CAST "1");
	parameter_N_node = xmlNewChild(setup_node, NULL, BAD_CAST "parameter-N", BAD_CAST "1");
	partition_node = xmlNewChild(setup_node, NULL, BAD_CAST "partition", BAD_CAST "paratera");
	shellpath_node = xmlNewChild(setup_node, NULL, BAD_CAST "shellpath", BAD_CAST "./autoReLaunch.sh");

        master_node = xmlNewChild(root_node, NULL, BAD_CAST "Master", NULL); 
        backup_node = xmlNewChild(root_node, NULL, BAD_CAST "Backup", NULL);

	xmlNodePtr mhost_node = NULL, mIP_node = NULL, mlaunchID_node = NULL;
	xmlNodePtr bhost_node = NULL, bIP_node = NULL, blaunchID_node = NULL;
	mhost_node = xmlNewChild(master_node, NULL, BAD_CAST "hostname", NULL);
	mIP_node = xmlNewChild(master_node, NULL, BAD_CAST "IP", NULL);
	mlaunchID_node = xmlNewChild(master_node, NULL, BAD_CAST "launchID", NULL);

	bhost_node = xmlNewChild(backup_node, NULL, BAD_CAST "hostname", NULL);
	bIP_node = xmlNewChild(backup_node, NULL, BAD_CAST "IP", NULL);
	blaunchID_node = xmlNewChild(backup_node, NULL, BAD_CAST "launchID", NULL);

        xmlSaveFormatFileEnc(MONITOR_INFO, doc, "UTF-8", 1);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        return (0);
}

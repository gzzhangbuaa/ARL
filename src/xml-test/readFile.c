#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

typedef struct item_t
{
	char *key;
	char *value;
}ITEM;

char *strtrimr(char *pstr)
{
	int i;	
	i = strlen(pstr) - 1;
	while(isspace(pstr[i]) && (i >= 0))
	{
		pstr[i--] = '\0';
	}
	return pstr;
}

char *strtriml(char *pstr)
{
	int i = 0, j;
	j = strlen(pstr) - 1;
	while(isspace(pstr[i]) && (i <= j))
	{
		i++;
	}
	if(0 < i)
	{
		strcpy(pstr, &pstr[i]);
	}
	return pstr;
}

char *strtrim(char *pstr)
{
	char *p;
	p = strtrimr(pstr);
	return strtriml(p);
}

int get_item_from_line(char *line, struct item_t *item)
{
	char *p = strtrim(line);
	int len = strlen(p);
	if(len <= 0)
	{
		return 1;
	}
	else if(p[0] == '#')
	{
		return 2;
	}
	else
	{
		char *p2 = strchr(p, '=');
		*p2++ = '\0';
		item->key = (char *)malloc(strlen(p) + 1);
		item->value = (char *)malloc(strlen(p2) + 1);
		strcpy(item->key, p);
		strcpy(item->value, p2);
	}
	return 0;
}

int file_to_items(const char*file, struct item_t *items, int *num)
{
	char line[1024];
	FILE *fp;
	fp = fopen(file, "r");
	if(fp == NULL)
	{
		return 1;
	}
	int i = 0;
	while(fgets(line, 1023, fp))
	{
		char *p = strtrim(line);
		int len = strlen(p);
		if(len <= 0)
		{
			continue;
		}
		else if(p[0] == '#')
		{
			continue;
		}
		else
		{
			char *p2 = strchr(p, '=');
			if(p2 == NULL)
			{
				continue;
			}
			*p2++ = '\0';
			items[i].key = (char *)malloc(strlen(p) + 1);
			items[i].value = (char *)malloc(strlen(p2) + 1);
			strcpy(items[i].key, p);
			strcpy(items[i].value, p2);
			i++;
		}
	}
	(*num) = i;
	fclose(fp);
	return 0;
}

int read_conf_value(const char *key, char *value1, const char *file)
{
	char line[1024];
	char *key1, *key3, *key2;
	FILE *fp;
	fp = fopen(file, "r");
	if(fp == NULL)
	{
		return 1;
	}
	while(fgets(line, 1023, fp))
	{
		ITEM item;
		get_item_from_line(line, &item);
		if(!strcmp(item.key, key))
		{
			strcpy(value1, item.value);
			fclose(fp);
			free(item.key);
			free(item.value);
			break;
		}	
	}
	return 0;
}

int write_conf_value(const char *key, char * value, const char *file)
{
	ITEM items[20];
	int num;
	file_to_items(file, items, &num);
	
	int i = 0;
	for(i = 0; i < num; i++)
	{
		if(!strcmp(items[i].key, key))
		{
			items[i].value = value;
			break;
		}
	}
	
	FILE *fp;
	fp = fopen(file, "w");
	if(fp == NULL)
	{
		return 1;
	}
	
	i = 0;
	for(i = 0; i < num; i++)
	{
		fprintf(fp, "%s=%s\n", items[i].key, items[i].value);
	}
	fclose(fp);
	return 0;
}

void main(void)
{
	char *key;
	char *value = NULL, *value1 = NULL;
	char *file;
	file = "/home/wangwei/ww/file/from_file";
	
	key = "IP";
	value = (char *)malloc(sizeof(char)*30);
	value1 = (char *)malloc(sizeof(char)*30);
	read_conf_value(key, value, file);
	printf("IP = %s\n", value);
	
	key = "MASK";
	read_conf_value(key, value, file);
	printf("MASK = %s\n", value);

	key = "GATEWAY";
	read_conf_value(key, value, file);
	printf("GATEWAY = %s\n", value);
	free(value);
	free(value1);
	value = NULL;
	value1 = NULL;
}














































































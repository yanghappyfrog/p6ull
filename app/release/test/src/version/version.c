/*************************************************************************
    > File Name: version.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年06月26日 星期二 17时57分49秒
 ************************************************************************/

#include <stdio.h>
#include "version.h"
#include "../iniparser/iniparser.h"
#include "../iniparser/dictionary.h"

int get_version()
{
	char * ini_name = "/root/pavt/info/version.info";

	FILE *version_filep;
    dictionary * ini ;
	
	printf("*** %s *** \n", __func__);
    ini = iniparser_load(ini_name);
    iniparser_dump(ini, stdout);

	iniparser_set(ini, "dev:id", ID);
	iniparser_set(ini, "dev:uboot", V_UBOOT);
	iniparser_set(ini, "dev:kernel", V_KERNEL);
	iniparser_set(ini, "dev:so", V_SO);
	iniparser_set(ini, "dev:pavt_boot", V_FW);

	version_filep = fopen(ini_name, "w");
	if(version_filep == NULL){
		printf("open file error!\n");
		iniparser_freedict(ini);
		return -1;
	}
    iniparser_dump_ini(ini, version_filep);

	iniparser_freedict(ini);
	fclose(version_filep);
	return 0;
}


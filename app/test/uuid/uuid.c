/*************************************************************************
    > File Name: uuid.c
    > Author: Happyfrog
    > Mail: yanghappyfrog@163.com 
    > Created Time: 2018年11月13日 星期二 14时59分00秒
 ************************************************************************/

#include <stdio.h>
#include "/usr/local/include/uuid/uuid.h"

int main(void)
{
	uuid_t uuid;
	char str[36];

	uuid_generate(uuid);
	uuid_unparse(uuid, str);

	printf("uuid: %s\n", str);

	return 0;
}

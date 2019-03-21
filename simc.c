#include "cachelab.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct result
{
	int miss;
	int hit;
	int evit;
}result;


char *rtrim(char *str)
{
	if (str == NULL || *str == '\0')
	{
		return str;
	}

	int len = strlen(str);
	char *p = str + len - 1;
	while (p >= str && isspace(*p))
	{
		*p = '\0';
		--p;
	}

	return str;
}

int searchAndSolve(int *cache, int E, int set, int tag)
{
	int * tmp = (int *)malloc(sizeof(int)*E);
	int pos = -1;
	int lasPos = E;
	for (size_t i = set * E; i < (set+1)*E; i++)
	{
		tmp[i - set * E] = cache[i];
		if (cache[i]==tag)
		{
			pos = i - set * E;
		}
		if (cache[i]==-1)
		{
			lasPos = i - set * E;
			break;
		}
	}

	if (pos!=-1)
	{
		cache[set*E] = tag;
		for (size_t i = 0; i < pos; i++)
		{
			cache[set*E + i + 1] = tmp[i];
		}
		for (size_t i = pos+1; i < lasPos; i++)
		{
			cache[set*E + i] = tmp[i];
		}
		free(tmp);
		return 1;
	}
	else if (lasPos!=E)
	{
		cache[set*E] = tag;
		for (size_t i = 0; i < lasPos; i++)
		{
			cache[set*E + i + 1] = tmp[i];
		}
		free(tmp);
		return 2;
	}
	else
	{
		cache[set*E] = tag;
		for (size_t i = 0; i < E - 1; i++)
		{
			cache[set*E + i + 1] = tmp[i];
		}
		free(tmp);
		return 3;
	}
}

result solve(char *str, int s, int E, int b, int v,int *cache)
{
	result r = { 0,0,0 };
	str = rtrim(str);
	char type = ' ';
	if (strlen(str)<=0)
	{
		return r;
	}
	if (str[0]!=' ')
	{
		return r;
	}
	else {
		char *Addr = NULL;
		Addr = (char *)malloc(strlen(str)*sizeof(char));
		if (Addr == NULL)
		{
			printf("malloc for Addr error.");
			return r;
		}
		type = str[1];
		int i = 2;
		while (str[i]==' ')
		{
			i++;
		}
		int j = i;
		while (str[j]!=',')
		{
			Addr[j - i] = str[j];
			j++;
		}
		int addrLen = j - i;
		char * bAddr = (char *)malloc(4 * addrLen * sizeof(char));
		for (size_t i = 0; i < addrLen; i++)
		{
			char c = Addr[i];
			int hex = 0;
			if (c>='a'&&c<='f')
			{
				hex = c - 'a' + 10;
			}
			else if (c >= 'A'&&c <= 'F')
			{
				hex = c - 'A' + 10;
			}
			else if (c >= '0'&&c <= '9')
			{
				hex = c - '0';
			}
			else {
				return r;
			}
			bAddr[i * 4] = hex / 8 + '0';
			bAddr[i * 4 + 1] = (hex - (bAddr[i * 4] - '0') * 8) / 4 + '0';
			bAddr[i * 4 + 2] = (hex - (bAddr[i * 4] - '0') * 8 - (bAddr[i * 4 + 1] - '0') * 4) / 2 + '0';
			bAddr[i * 4 + 3] = hex - (bAddr[i * 4] - '0') * 8 - (bAddr[i * 4 + 1] - '0') * 4 - (bAddr[i * 4 + 2] - '0') * 2 + '0';
		}
		int set = 0;
		int tag = 0;
		int length = 4 * addrLen;
		if (s+b <length)
		{
			for (int i = length - b - 1; i >= length - b - s; i--)
			{
				int offset = -i + length - b - 1;
				set += (bAddr[i]-'0')*(1 << offset);
			}
			for (int i = length - b - s - 1; i >= 0; i--)
			{
				int offset = -i + length - b - s - 1;
				tag += (bAddr[i]-'0')*(1 << offset);
			}
		}
		else
		{
			for (int i = length - b - 1; i >= 0; i--)
			{
				int offset = -i + length - b - 1;
				set += (bAddr[i]-'0')*(1 << offset);
			}
		}
		free(bAddr);
		free(Addr);
		bAddr = NULL;
		Addr = NULL;
		int flag = searchAndSolve(cache, E, set, tag);
		if (type=='S')
		{
			if (flag == 1)
			{
				r.hit += 1;
			}
			else if (flag == 2)
			{
				r.miss += 1;
			}
			else
			{
				r.miss += 1;
				r.evit += 1;
			}
		}
		else
		{
			if (type=='L')
			{
				if (flag == 1)
				{
					r.hit += 1;
				}
				else if (flag == 2)
				{
					r.miss += 1;
				}
				else
				{
					r.miss += 1;
					r.evit += 1;
				}
			}
			else
			{
				if (flag == 1)
				{
					r.hit += 2;
				}
				else if (flag == 2)
				{
					r.miss += 1;
					r.hit += 1;
				}
				else
				{
					r.miss += 1;
					r.evit += 1;
					r.hit += 1;
				}
			}
		}

		if (v)
		{
			if (r.hit)
			{
				strcat(str, " hit");
			}
			if (r.miss)
			{
				strcat(str, " miss");
			}
			if (r.evit)
			{
				strcat(str, " eviction");
			}
			printf("%s\n", str);
		}
	}
	return r;
}

int main(int argc,char * argv[])
{
	int hit = 0, miss = 0, evit = 0;
	int E = 2, s = 4, b = 4;
	char * testFile = NULL;
	int v = 0;
	for (size_t i = 1; i < argc - 1; i++)
	{
		if (strcmp("-v", argv[i])==0)
		{
			v = 1;
		}
		if (strcmp("-s", argv[i])==0)
		{
			s = atoi(argv[i + 1]);
		}
		if (strcmp("-E", argv[i])==0)
		{
			E = atoi(argv[i + 1]);
		}
		if (strcmp("-b", argv[i])==0)
		{
			b = atoi(argv[i + 1]);
		}
		if (strcmp("-t", argv[i])==0)
		{
			testFile = malloc(strlen(argv[i + 1])* sizeof(char));
			strcat(testFile, argv[i + 1]);
		}
	}
	if (s==0||b==0||E==0||testFile==NULL)
	{
		printf("usage: ./csim [-v] -s [s num] -E [E num] -b [b num] -t [test file directory]\n");
		printf("example: ./csim -v -s 4 -E 2 -b 2 -t traces/yi.trace\n");
		printf("s:%d, b:%d, E:%d, file:%s ", s, b, E, testFile);
		return -1;
	}
	int *cache = (int *)malloc((1 << s) * sizeof(int) * E);
	if (cache == NULL)
	{
		printf("malloc for cache fail.");
		return -1;
	}
	for (size_t i = 0; i < (1<<s)*E; i++)
	{
		cache[i] = -1;
	}

	FILE *fp;
	char strLine[80];
	if ((fp = fopen(testFile,"r"))==NULL)
	{
		printf("open file %s fail", testFile);
		return -1;
	}
	while (fgets(strLine, 80, fp))
	{
		result r = solve(strLine, s, E, b, v, cache);
		hit += r.hit;
		miss += r.miss;
		evit += r.evit;
	}
	fclose(fp);
	printSummary(hit, miss, evit);
	free(cache);
	cache = NULL;
	if (testFile!=NULL)
	{
		free(testFile);
		testFile = NULL;
	}
	return 0;
}
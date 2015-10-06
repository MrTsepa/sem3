#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TEXT_SIZE 1000
#define AMOUNT_OF_WORDS 100
#define MAX_WORD_SIZE 100

int split(char* str, char* spl, int amount_of_spl, char** res)
{
	int i, j, num_of_word = 0, num_of_char = 0;
	char * temp_str;
	temp_str = (char*)malloc(sizeof(char)*MAX_WORD_SIZE);
	int is_ended = 0;
	for (i = 0; i < strlen(str); i++)
	{
		int is_splitter = 0;
		for (j = 0; j < amount_of_spl; j++)
		{
			if (str[i] == spl[j])
			{
				is_splitter = 1;
				break;
			}
		}
		if (is_splitter && !is_ended)
		{
			is_ended = 1;
			temp_str[num_of_char] = '\0';
			strcpy(res[num_of_word], temp_str);
			num_of_word++;
			num_of_char = 0;
		}
		if (!is_splitter)
		{
			is_ended = 0;
			temp_str[num_of_char] = str[i];
			num_of_char++;
		}
	}
	if(!is_ended)
	{
		strcpy(res[num_of_word], temp_str);
		num_of_word++;
	}
	free(temp_str);
	return num_of_word;
}

int main()
{
	int n, i = 0;
	char* str;
	char* spl;
	str = (char*)malloc(sizeof(char)*TEXT_SIZE);
	fgets(str, TEXT_SIZE, stdin);
	scanf("%d", &n);
	spl = (char*)malloc(sizeof(char)*(n+2));
	spl[0] = ' ';
	spl[1] = '\n';
	scanf("%s", spl+2); //разделители вводятся подряд без пробелов

	char **res_arr;
	res_arr = (char**)malloc(sizeof(char*)*AMOUNT_OF_WORDS);
	for(i = 0; i < AMOUNT_OF_WORDS; i++)
		res_arr[i] = (char*)malloc(sizeof(char)*MAX_WORD_SIZE);

	int am_of_words;
	am_of_words = split(str, spl, n+2, res_arr);

	for(i = 0; i < am_of_words; i++)
		printf("%s\n", res_arr[i]); 

	free(str);
	free(spl);
	free(res_arr);
}

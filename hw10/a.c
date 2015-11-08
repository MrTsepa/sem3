#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

pid_t ppid, chpid;

/*
 * У вас переменная ppid не инициализирована. В debug режиме ей присваивается значение 0.
 * Написав kill(0, ...) вы отправляете сигнал всем процессам в рабочей группе (мы пропустили это на семинаре, сказав, что общаются только родственные процессы).
 * Т.е. с большой вероятностью процесс сам себе что-то отправлял.
 */

char * str = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
char ** str_res_ptr;

char cur_char = 0;
int pos_in_char = 0;
int pos_in_str = 0;

/*
 * Зачем вам эта ф-я?
 * Можно ведь написать 1 << p.
 */
char pwr_of_2(int p)
{
	int i;
	int res = 1;
	for (i = 0; i < p; i++) {
		res *= 2;
	}
	return res;
}

void handler(int nsig) {
	char* str_res = *str_res_ptr;
	int t;
	if (nsig == 30)
		t = 0;
	if (nsig == 31)
		t = 1;
	if (pos_in_char < 8) {
		cur_char += t * pwr_of_2(pos_in_char);
		pos_in_char++;
	}
	if (pos_in_char == 8) {
		str_res[pos_in_str] = cur_char;
		cur_char = 0;
		pos_in_char = 0;
		pos_in_str++;
	}
	if (pos_in_str == strlen(str)) {
		str_res[pos_in_str] = '\0';
		printf("%s\n", str_res);
	}
}

int main()
{
	str_res_ptr = (char**)malloc(sizeof(char*));
	*str_res_ptr = (char*)malloc(sizeof(char)*strlen(str));
	pid_t pid = fork();
/*
 * Зачем разводить магические числа на ровном месте. Даже в задании написано, что сигналы
 * называются SIGUSR1, SIGUSR2. Зачем вам тогда 30 и 31.
 */
	signal(30, handler);
	signal(31, handler);
  
	if (pid != 0) {
		int i;
		for (i = 0; i < strlen(str); i++) {
			int j;
			char c = str[i];
			for (j = 0; j < sizeof(char) * 8; j++) {
				int flag = c % 2;
				if (flag == 0) {
					kill(ppid, 30);
				}
				if (flag == 1) {
					kill(ppid, 31);
				}
				c = c >> 1;
			}
		}
	}
}

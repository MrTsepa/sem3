#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <dirent.h>
#include <string.h>

#define MAX_PATH_LENGTH 1024
#define MAX_NAME_LENGTH 256

char* find_file(char * cur_dir, int depth, const char * name)
{
	printf("%s\n", cur_dir);
	DIR * dirp = opendir(cur_dir);
	struct dirent * dp = (struct dirent *)malloc(sizeof(struct dirent));
	while ((dp = readdir(dirp)) != NULL) {
		if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
			continue;
		
		char * temp_cur_dir = (char*)malloc(sizeof(char) * MAX_PATH_LENGTH);
		strcpy(temp_cur_dir, cur_dir);
		if (!strcmp(dp->d_name, name) && (dp->d_type != DT_DIR)) {
			closedir(dirp);
			return temp_cur_dir;
		}
		if ((dp->d_type == 4) && (depth > 0)) {
			strcat(temp_cur_dir, "/");
			strcat(temp_cur_dir, dp->d_name);
			char * res_dir = find_file(temp_cur_dir, depth - 1, name);
			if (res_dir != NULL) return res_dir;
			free(temp_cur_dir);
		}
	}
	free(dp);
	return NULL;
}

int main(int argc, char* argv[])
{
	char * path = (char*)malloc(sizeof(char) * MAX_PATH_LENGTH);
	path = argv[1];
	
	int depth = atoi(argv[2]);

	const char * name = (char*)malloc(sizeof(char) * MAX_NAME_LENGTH);
	name  = argv[3];

	char * res_dir = find_file(path, depth, name);

	if (res_dir != NULL) {
		printf("%s/%s\n", res_dir, name);
	}
	else printf("Not found\n");
}

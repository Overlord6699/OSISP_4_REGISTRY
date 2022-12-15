#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void search_key(HKEY key, const char* subKey, const char* keyToSearch);



void output_categories()
{
	printf("1 - HKEY_CLASSES_ROOT\n2 - HKEY_CURRENT_USER\n3 - HKEY_LOCAL_MACHINE\n4 - HKEY_USERS\n5 - HKEY_CURRENT_CONFIG\n");
	printf("Your choice: ");
}

int main(int argc, char* argv[])
{
	char* subKey = argv[1];
	char* keyToSearch = argv[2];
	int category = 0;

	output_categories();

	scanf("%d", &category);
	fflush(stdin);

	switch(category)
	{
		case 1:
			search_key(HKEY_CLASSES_ROOT, subKey, keyToSearch);
			break;
		case 2:
			search_key(HKEY_CURRENT_USER, subKey, keyToSearch);
			break;
		case 3:
			search_key(HKEY_LOCAL_MACHINE, subKey, keyToSearch);
			break;
		case 4:
			search_key(HKEY_USERS, subKey, keyToSearch);
			break;
		case 5:
			search_key(HKEY_CURRENT_CONFIG, subKey, keyToSearch);
			break;
		default:
			printf("Invalid input. Please try again\n");
			return -1;
	}

	return 0;
}

void search_key(HKEY category, const char* subKey, const char* keyToSearch)
{
	LSTATUS status;
	HKEY hKey;
	DWORD subKeysCount, maxSubKeyLength;

	status = RegOpenKey(category, subKey, &hKey);

	if (status != ERROR_SUCCESS)
	{
		return;
	}

	status = RegQueryInfoKey(hKey, NULL, NULL, NULL,
		&subKeysCount, &maxSubKeyLength, NULL, NULL,
		NULL, NULL, NULL, NULL);

	if (status != ERROR_SUCCESS)
	{
		return;
	}

	if (subKeysCount <= 0)
	{
		return;
	}

	char cur_sub_key[MAX_PATH];
	for (int i = 0; i < subKeysCount; i++)
	{
		DWORD cur_sub_length = MAX_PATH;
		status == RegEnumKeyEx(hKey, i, cur_sub_key, &cur_sub_length, NULL, NULL, NULL, NULL);
		if (status == ERROR_SUCCESS)
		{
			if (strcmp(cur_sub_key, keyToSearch) == 0)
				printf("%s\\%s\n", subKey, cur_sub_key);
			//increase the length of path str
			char sub_key_path[cur_sub_length + strlen(subKey)];

			sprintf(sub_key_path, "%s\\%s", subKey, cur_sub_key);
			//search key recursivly
			search_key(category, sub_key_path, keyToSearch);
		}
	}
}
#include <windows.h>
#include <strsafe.h>
#include <stdio.h>
#include <stdlib.h>
#include <sddl.h>

#define ID_PREDICATEDKEYCOMBOBOX 1
#define ID_OPERATIONCOMBOBOX 2
#define ID_SUBKEYEDIT 3
#define ID_EXECUTEBUTTON 4
#define ID_CURRENTKEYEDIT 5
#define ID_KEYTOCREATEEDIT 6
#define ID_REGTYPESCOMBOBOX 7
#define ID_PARAMTOADDEDIT 8
#define ID_PARAMVALUEEDIT 9
#define ID_KEYTOSEARCHEDIT 10
#define ID_KEYTODELETEEDIT 11
#define ID_PARAMTODELETEEDIT 12
#define ID_KEYMASKEDIT 13

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
void open_key();
void update_key(LPCSTR currentkey);
void create_key();
void update_file(LPSTR status);
void add_data_to_key();
void add_DWORD();
void add_QWORD();
void add_string();
void add_binary();
void close_current_key();
void delete_param();
void search_for_key();
void delete_key();
void delete_param();
void get_flags_from_mask();

const LPCSTR LOG_FILE_NAME = "logs.txt";

const LPSTR predicated_keys[] = {
	"HKEY_CLASSES_ROOT",
	"HKEY_CURRENT_USER",
	"HKEY_LOCAL_MACHINE",
	"HKEY_USERS",
	"HKEY_CURRENT_CONFIG",
};
const LPSTR operations[] = {
	"Open key",
	"Create key",
	"Close current key",
	"Add data to key",
	"Search for key",
	"Delete key",
	"Delete param",
};
const LPSTR reg_types[] = {
	"REG_DWORD",
	"REG_QWORD",
	"REG_SZ",
	"REG_BINARY"
};

unsigned int selected_predicated_key = 0, selected_operation = 0, selected_param = 0;

HWND hPredicatedKeysComboBox, hOperationComboBox, hSubKeyToOpenEdit,
hExecuteButton, hCurrentKeyEdit, hKeyToCreateEdit, hRegTypes, hParamToAddEdit,
hParamValueEdit, hKeyToSearchEdit, hKeyToDeleteEdit, hParamToDeleteEdit, hKeyMaskEdit, hKeyFlagsEdit;
HINSTANCE hInst;
HANDLE hLogFile;
HKEY hCurrentKey, hCreatedKey;
LSTATUS status;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 0);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "RegEditClass";
	wcex.hIconSm = wcex.hIcon;
	
	RegisterClassEx(&wcex);
	
	hWnd = CreateWindow("RegEditClass", "OSISP LAB5",
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
		300, 300, 700, 500, NULL, NULL, hInstance, NULL);
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		case WM_CREATE:
			hLogFile = CreateFile(LOG_FILE_NAME, FILE_GENERIC_WRITE, 
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			SetFilePointer(hLogFile, 0, NULL, FILE_END);

			CreateWindow("static", "Predicated keys", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 10, 10, 150, 20, hWnd, NULL, hInst, NULL);
			CreateWindow("static", "Operation", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 10, 60, 150, 20, hWnd, NULL, hInst, NULL);
			CreateWindow("static", "Registry type", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 10, 110, 150, 20, hWnd, NULL, hInst, NULL);

			hPredicatedKeysComboBox = CreateWindow("Combobox", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS, 10, 30, 200, 200, hWnd, (HMENU)ID_PREDICATEDKEYCOMBOBOX, hInst, NULL);
			for (int i = 0; i < 5; i++)
			{
				SendMessage(hPredicatedKeysComboBox, CB_ADDSTRING, 1, (LPARAM) predicated_keys[i]);
			}
			SendMessage(hPredicatedKeysComboBox, CB_SETCURSEL, 0, (LPARAM)0);

			hOperationComboBox = CreateWindow("Combobox", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS, 10, 80, 200, 200, hWnd, (HMENU)ID_OPERATIONCOMBOBOX, hInst, NULL);
			for (int i = 0; i < 7; i++)
			{
				SendMessage(hOperationComboBox, CB_ADDSTRING, 1, (LPARAM) operations[i]);
			}
			SendMessage(hOperationComboBox, CB_SETCURSEL, 0, (LPARAM)0);

			hRegTypes = CreateWindow("Combobox", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS, 10, 130, 200, 200, hWnd, (HMENU) ID_REGTYPESCOMBOBOX, hInst, NULL);
			for (int i = 0; i < 4; i++)
			{
				SendMessage	(hRegTypes, CB_ADDSTRING, 1, (LPARAM) reg_types[i]);
			}
			SendMessage(hRegTypes, CB_SETCURSEL, 0, (LPARAM) 0);

			CreateWindow("static", "Key to open",
				WS_CHILD | WS_VISIBLE | WS_TABSTOP, 
				220, 10, 150, 20, hWnd, NULL, hInst, NULL);
			hSubKeyToOpenEdit = CreateWindow("Edit", NULL,
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
				220, 30, 450, 23, hWnd, (HMENU) ID_SUBKEYEDIT, hInst, NULL);
			hExecuteButton = CreateWindow("Button", "Execute", 
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				10, 425, 200, 30, hWnd, (HMENU)ID_EXECUTEBUTTON, hInst, NULL);
			CreateWindow("static", "Current key", 
				WS_CHILD | WS_VISIBLE | WS_TABSTOP,
				220, 60, 150, 20, hWnd, NULL, hInst, NULL);
			hCurrentKeyEdit = CreateWindow("Edit", NULL, 
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
				220, 80, 450, 23, hWnd, (HMENU) ID_CURRENTKEYEDIT, hInst, NULL);
			CreateWindow("static", "Key to create", 
				WS_CHILD | WS_VISIBLE | WS_TABSTOP,
				220, 110, 150, 20, hWnd, NULL, hInst, NULL);
			hKeyToCreateEdit = CreateWindow("Edit", NULL,
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
				220, 130, 450, 23, hWnd, (HMENU) ID_KEYTOCREATEEDIT, hInst, NULL);
			CreateWindow("static", "Param to add", 
				WS_CHILD | WS_VISIBLE | WS_TABSTOP,
				220, 160, 150, 24, hWnd, NULL, hInst, NULL);
			hParamToAddEdit = CreateWindow("Edit", NULL, 
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
				220, 180, 450, 23, hWnd, (HMENU) ID_PARAMTOADDEDIT, hInst, NULL);
			CreateWindow("static", "Param Value", 
				WS_CHILD | WS_VISIBLE | WS_TABSTOP,
				220, 210, 150, 24, hWnd, NULL, hInst, NULL);
			hParamValueEdit = CreateWindow("Edit", NULL,
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
				220, 230, 450, 23, hWnd, (HMENU) ID_PARAMVALUEEDIT, hInst, NULL);
			CreateWindow("static", "Key to search", 
				WS_CHILD | WS_VISIBLE | WS_TABSTOP,
				220, 260, 150, 24, hWnd, NULL, hInst, NULL);
			hKeyToSearchEdit = CreateWindow("Edit", NULL,
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
				220, 280, 450, 23, hWnd, (HMENU) ID_KEYTOSEARCHEDIT, hInst, NULL);
			CreateWindow("static", "Key to delete", 
				WS_CHILD | WS_VISIBLE | WS_TABSTOP,
				220, 310, 150, 24, hWnd, NULL, hInst, NULL);
			hKeyToDeleteEdit = CreateWindow("Edit", NULL,
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
				220, 330, 450, 23, hWnd, (HMENU) ID_KEYTODELETEEDIT, hInst, NULL);
			CreateWindow("static", "Param to delete", 
				WS_CHILD | WS_VISIBLE | WS_TABSTOP,
				220, 360, 150, 24, hWnd, NULL, hInst, NULL);
			hParamToDeleteEdit = CreateWindow("Edit", NULL, 
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
				220, 380, 450, 23, hWnd, (HMENU) ID_PARAMTODELETEEDIT, hInst, NULL);
			break;
		case WM_DESTROY:
			CloseHandle(hLogFile);
			PostQuitMessage(0);
			return 0;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_PREDICATEDKEYCOMBOBOX:
					selected_predicated_key = SendMessage(hPredicatedKeysComboBox, CB_GETCURSEL, 0, 0);
					break;
				case ID_OPERATIONCOMBOBOX:
					selected_operation = SendMessage(hOperationComboBox	, CB_GETCURSEL, 0, 0);
					break;
				case ID_PARAMTOADDEDIT:
					selected_param = SendMessage(hRegTypes, CB_GETCURSEL, 0, 0);
					break;
				case ID_EXECUTEBUTTON:
					switch(selected_operation)
					{
						case 0:
							open_key();
							break;
						case 1:
							create_key();
							break;
						case 2:
							close_current_key();
							break;
						case 3:
							add_data_to_key();
							break;
						case 4:
							search_for_key();
							break;
						case 5:
							delete_key();
							break;
						case 6:
							delete_param();
							break;
					}
					break;
			}
			break;
	}
	return DefWindowProc(hWnd, Message, wParam, lParam);
}

void open_key()
{
	TCHAR subKey[1024];
	TCHAR buffer[1024];

	GetWindowText(hSubKeyToOpenEdit, subKey, 1024);

	switch (selected_predicated_key)
	{
		case 0: 
			status = RegOpenKey(HKEY_CLASSES_ROOT, subKey, &hCurrentKey);
			break;
		case 1:
			status = RegOpenKey(HKEY_CURRENT_USER, subKey, &hCurrentKey);
			break;
		case 2:
			status = RegOpenKey(HKEY_LOCAL_MACHINE, subKey, &hCurrentKey);
			break;
		case 3:
			status = RegOpenKey(HKEY_USERS, subKey, &hCurrentKey);
			break;
		case 4:
			status = RegOpenKey(HKEY_CURRENT_CONFIG, subKey, &hCurrentKey);
			break;
	}
	if (status != ERROR_SUCCESS)
	{
		update_key("");
		MessageBox(NULL, "Failed to OpenKey", NULL, MB_OK);
		update_file("Failed");
	}
	else
	{
		StringCchCopy(buffer, 1024, predicated_keys[selected_predicated_key]);
		StringCchCat(buffer, 1024, "\\");
		StringCchCat(buffer, 1024, subKey);

		update_key(buffer);
		update_file("Success");

		MessageBox(NULL, "Key opened successfully", NULL, MB_OK);
	}
}

void update_key(LPCSTR currentKey)
{
	SetWindowText(hCurrentKeyEdit, currentKey);
}

void create_key()
{
	hCreatedKey = NULL;
	TCHAR keyToCreate[1024];

	GetWindowText(hKeyToCreateEdit, keyToCreate, 1024);
	MessageBox(NULL, keyToCreate, NULL, MB_OK);

	status = RegCreateKey(hCurrentKey, keyToCreate, &hCreatedKey);

	if (status != ERROR_SUCCESS)
	{
		MessageBox(NULL, "Failed to create key", NULL, MB_OK);
		update_file("Failed");
	}
	else
	{
		update_file("Success");
		MessageBox(NULL, "Key created successfully", NULL, MB_OK);
	}
	RegCloseKey(hCreatedKey);
}

void close_current_key()
{
	status = RegCloseKey(hCurrentKey);

	if (status != ERROR_SUCCESS)
	{
		update_file("Failed");
		MessageBox(NULL, "Failed to close current key", NULL, MB_OK);
	}
	else
	{
		update_file("Success");
		MessageBox(NULL, "Current key closed successfully", NULL, MB_OK);
		update_key("");
	}
}

void get_flags_from_mask()
{
	DWORD nb = 0;
	PSECURITY_DESCRIPTOR secDesc;
	SECURITY_INFORMATION information = OWNER_SECURITY_INFORMATION;

	if (RegGetKeySecurity(hCurrentKey, information, NULL, &nb) != ERROR_INSUFFICIENT_BUFFER)
		MessageBox(NULL, NULL, NULL, MB_OK);

	secDesc = malloc(nb);
	status = RegGetKeySecurity(hCurrentKey, information, secDesc, &nb);

	if (status != ERROR_SUCCESS)
		MessageBox(NULL, NULL, NULL, MB_OK);

	LPSTR message = malloc(1024);
	ConvertSecurityDescriptorToStringSecurityDescriptor(secDesc, SDDL_REVISION_1, information, &message, NULL);
	MessageBox(NULL, message, NULL, MB_OK);
}

void add_data_to_key()
{
	switch(selected_param)
	{
		case 0:
			add_DWORD();
			break;
		case 1:
			add_QWORD();
			break;
		case 2:
			add_string();
			break;
		case 3:
			add_binary();
			break;
	}
}

void add_DWORD()
{
	char paramName[1024], paramValue[1024];
	BYTE bytes[4];

	GetWindowText(hParamToAddEdit, paramName, 1024);
	GetWindowText(hParamValueEdit, paramValue, 1024);

	int value = atoi(paramValue);
	for (int i = 0; i < 4; i++)
		bytes[i] = (value >> (8 * (i))) & 0xFF;

	status = RegSetValueEx(hCurrentKey, paramName, 0, REG_DWORD, bytes, 4);
	if (status != ERROR_SUCCESS)
	{
		update_file("Failed");
		MessageBox(NULL, "Failed to add DWORD value", NULL, MB_OK);
	}
	else
	{
		update_file("Success");
		MessageBox(NULL, "Value added", NULL, MB_OK);
	}
}

void add_QWORD()
{
	char paramName[1024], paramValue[1024];

	GetWindowText(hParamToAddEdit, paramName, 1024);
	GetWindowText(hParamValueEdit, paramValue, 1024);

	long long value = atoi(paramValue);
	BYTE bytes[8];
	for (int i = 0; i < 8; i++)
		bytes[i] = (value >> (8 * (i))) & 0xFF;

	status = RegSetValueEx(hCurrentKey, paramName, 0, REG_QWORD, bytes, 8);
	if (status != ERROR_SUCCESS)
	{
		update_file("Failed");
		MessageBox(NULL, "Failed to add DWORD value", NULL, MB_OK);
	}
	else
	{
		update_file("Success");
		MessageBox(NULL, "Value added", NULL, MB_OK);
	}
}

void add_string()
{
	char paramName[1024], paramValue[1024];
	unsigned char unsignedValue[1024];

	GetWindowText(hParamToAddEdit, paramName, 1024);
	GetWindowText(hParamValueEdit, paramValue, 1024);

	for (int i = 0; i < 1024; i++)
		unsignedValue[i] = (unsigned char) paramValue[i];

	status = RegSetValueEx(hCurrentKey, paramName, 0, REG_SZ, unsignedValue, strlen(paramValue));
	if (status != ERROR_SUCCESS)
	{
		update_file("Failed");
		MessageBox(NULL, "Failed to add REG_SZ value", NULL, MB_OK);
	}
	else
	{
		update_file("Success");
		MessageBox(NULL, "Value added successfully", NULL, MB_OK);
	}
}

void add_binary()
{
	char paramName[1024], param_value[1024];

	GetWindowText(hParamToAddEdit, paramName, 1024);
	GetWindowText(hParamValueEdit, param_value, 1024);

	BYTE* byte_repr = (BYTE*) calloc((strlen(param_value) + 1) / 2, sizeof(byte));

	for (int i = 0, j = 0; i < (strlen(param_value) + 1) / 2; i += 2, j++)
	{
		byte_repr[j] = 0;
		if (param_value[i] >= '0' && param_value[i + 1] <= '9')
			byte_repr[j] += (param_value[i] - '0') * 16;
        else if(param_value[i] >= 'A' && param_value[i] <= 'F')
            byte_repr[j] += (param_value[i] - 55) * 16;
        else if(param_value[i] >= 'a' && param_value[i] <= 'f')
            byte_repr[j] += (param_value[i] - 87) * 16;
		if (param_value[i + 1] >= '0' && param_value[i + 1] <= '9')
			byte_repr[j] += (param_value[i + 1] - '0');
        else if(param_value[i + 1] >= 'A' && param_value[i + 1] <= 'F')
            byte_repr[j] += (param_value[i + 1] - 55);
        else if(param_value[i + 1] >= 'a' && param_value[i + 1] <= 'f')
            byte_repr[j] += (param_value[i + 1] - 87);
	}
	status = RegSetValueEx(hCurrentKey, paramName, 0, REG_BINARY, byte_repr, (strlen(param_value) + 1) / 2);
	if (status != ERROR_SUCCESS)
	{
		update_file("Failed");
		MessageBox(NULL, "Failed to add REG_BINARY value", NULL, MB_OK);
	}
	else
	{
		update_file("Success");
		MessageBox(NULL, "Value added successfully", NULL, MB_OK);
	}

	free(byte_repr);
}

void search_for_key()
{
	int isFound = 0;
	char keyToSearch[1024], currentKey[1024];
	DWORD index = 0;

	GetWindowText(hKeyToSearchEdit, keyToSearch, 1024);

	while (RegEnumKey(hCurrentKey, index, currentKey, 1024) == ERROR_SUCCESS)
	{
		index++;
		currentKey[1023] = '\0';
		if (strcmp(keyToSearch, currentKey) == 0)
		{
			isFound = 1;
			MessageBox(NULL, currentKey, NULL, MB_OK);
		}
	}
	if (isFound)
	{
		MessageBox(NULL, "Key found successfully", NULL, MB_OK);
		update_file("Success");
	}
	else
	{
		MessageBox(NULL, "Key not found", NULL, MB_OK);
		update_file("Failed");
	}
}

void delete_key()
{
	char keyToDelete[1024];

	GetWindowText(hKeyToDeleteEdit, keyToDelete, 1024);
	status = RegDeleteKey(hCurrentKey, keyToDelete);
	if (status != ERROR_SUCCESS)
	{
		MessageBox(NULL, "Key not deleted", NULL, MB_OK);
		update_file("Failed");
	}
	else
	{
		MessageBox(NULL, "Key deleted", NULL, MB_OK);
		update_file("Success");
	}
}

void delete_param()
{
	char paramToDelete[1024];
	GetWindowText(hParamToDeleteEdit, paramToDelete, 1024);
	status = RegDeleteValue(hCurrentKey, paramToDelete);

	if (status != ERROR_SUCCESS)
	{
		MessageBox(NULL, "Param not deleted", NULL, MB_OK);
		update_file("Failed");
	}
	else
	{
		MessageBox(NULL, "Param deleted", NULL, MB_OK);
		update_file("Success");
	}
}

void update_file(LPSTR status)
{
	SYSTEMTIME st;
	char username[1024], dataBuffer[1024], logMessage[2048];
	DWORD usernameLength = 1024;

	GetUserName(username, &usernameLength);
	GetLocalTime(&st);

	StringCbPrintf(dataBuffer, 2048, "[%02i.%02i.%0000i %02i:%02i:%02i]", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
	StringCchCopy(logMessage, 2048, dataBuffer);
	StringCchCat(logMessage, 2048, " User ");
	StringCchCat(logMessage, 2048, username);
	StringCchCat(logMessage, 2048, " execute operation ");
	StringCchCat(logMessage, 2048, operations[selected_operation]);
	StringCchCat(logMessage, 2048, ". Status: ");
	StringCchCat(logMessage, 2048, status);
	StringCchCat(logMessage, 2048, ";\n");

	WriteFile(hLogFile, logMessage, (DWORD)strlen(logMessage), NULL, NULL);
}


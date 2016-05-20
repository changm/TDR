#include <stdio.h>
#include <Windows.h>
#include <initguid.h>
#include <Ntddvdeo.h>
#include <setupapi.h>

#pragma comment (lib, "Setupapi.lib")

int main() {
	printf("Hello World\n");

	HDEVINFO deviceInfo;
	deviceInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISPLAY_ADAPTER, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (deviceInfo == INVALID_HANDLE_VALUE) {
		printf("Blue up , could not get class dev\n");
	}

	SP_DEVINFO_DATA deviceInstance;
	memset(&deviceInstance, 0, sizeof(SP_DEVINFO_DATA));
	deviceInstance.cbSize = sizeof(SP_DEVINFO_DATA);

	int deviceIndex = 0;
	while (SetupDiEnumDeviceInfo(deviceInfo, deviceIndex, &deviceInstance)) {
		if (deviceIndex > 0) {
			return 1;
		}

		deviceIndex++;

		DWORD dataT;
		DWORD bufferSize = 1024;
		LPTSTR buffer = (LPTSTR)LocalAlloc(LPTR, bufferSize);
		// Grab the name of the device
		BOOL results = SetupDiGetDeviceRegistryProperty(deviceInfo, &deviceInstance, SPDRP_DEVICEDESC, &dataT, (PBYTE)buffer, bufferSize, &bufferSize);
		if (!results) {
			printf("bad things died\n");
			continue;
		}

		printf("Device is: %s\n", buffer);

		// Now let's disable it?
		SP_PROPCHANGE_PARAMS changeParams;
		memset(&changeParams, 0, sizeof(SP_PROPCHANGE_PARAMS));
		changeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
		changeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
		changeParams.Scope = DICS_FLAG_GLOBAL;
		changeParams.StateChange = DICS_DISABLE;

		results = SetupDiSetClassInstallParams(deviceInfo, &deviceInstance, (SP_CLASSINSTALL_HEADER*)&changeParams, sizeof(changeParams));
		if (!results) {
			printf("Couldn't setup class install params\n");
			return 1;
		}

		results = SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, deviceInfo, &deviceInstance);
		if (!results) {
			DWORD error = GetLastError();
			printf("Error code is: %llu \n", error);
			HRESULT resultError = HRESULT_FROM_WIN32(error);
			printf("HRESULT error is: %lu\n", resultError);
			//LPSTR errorString = nullptr;
			LPVOID errorString = nullptr;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						  NULL,
						  error,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
				(LPSTR) errorString, 0, NULL);
			//printf("Couldn't call class installer: error: %lx\n", GetLastError());
			wprintf(L"Error is: %s\n", errorString);
		}

		printf("Try to re-enable\n");
		Sleep(1000 * 20);
		memset(&changeParams, 0, sizeof(SP_PROPCHANGE_PARAMS));
		changeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
		changeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
		changeParams.Scope = DICS_FLAG_GLOBAL;
		changeParams.StateChange = DICS_ENABLE;
		results = SetupDiSetClassInstallParams(deviceInfo, &deviceInstance, (SP_CLASSINSTALL_HEADER*)&changeParams, sizeof(changeParams));
		if (!results) {
			printf("Couldn't setup class install params\n");
			return 1;
		}

		results = SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, deviceInfo, &deviceInstance);
		if (!results) {
			DWORD error = GetLastError();
			printf("Error code is: %llu \n", error);
			HRESULT resultError = HRESULT_FROM_WIN32(error);
			printf("HRESULT error is: %lu\n", resultError);
			//LPSTR errorString = nullptr;
			LPVOID errorString = nullptr;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						  NULL,
						  error,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
				(LPSTR) errorString, 0, NULL);
			//printf("Couldn't call class installer: error: %lx\n", GetLastError());
			wprintf(L"Error is: %s\n", errorString);
		}

		printf("Re-enabled I hope\n");
	}
}
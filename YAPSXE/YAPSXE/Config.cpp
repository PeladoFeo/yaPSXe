#include "Config.h"
#include "resource.h"
#include "Bios.h"
#include "Psx.h"
#include "Memory.h"
#include "Cpu.h"
#include "Console.h"


std::string Config::mBiosDirectoryPath = "BIOS";
const std::string Config::regions[] = {"NTSC:J", "NTSC:U/C", "PAL", "NTSC:J"};

Config::Config() {
	psx = CPsx::GetInstance();

	if (!OpenRegistryKeys()) {
		MessageBox(NULL, "Config: failed to open registry keys\n", "Error", MB_ICONERROR);
	}

	/* try to load the bios if it's configured */
	std::string strCurBios = GetCurBiosRegKey();
	if (strCurBios != DEFAULT_REG_STR_KEY_VAL) {
		std::string strBiosPath = mBiosDirectoryPath + '/' + strCurBios;
		if (IsValidBiosFile(strBiosPath, NULL)) {
			/* load the BIOS */
			if (psx->mem->LoadBiosRom(strBiosPath)) {
				bBiosLoaded = TRUE;
			}
		} else {
			bBiosLoaded = FALSE;
		}
	} else {
		bBiosLoaded = FALSE;
	}
}

Config::~Config() {
}

/* does basic checks to see if the file is a valid BIOS image.
   also (if info is not null) tries to identity the BIOS using a 
   checksum. if it's recognised, the pointer passed in "info" is 
   modified to point to the relevant "struct BiosInfo" entry */
BOOL Config::IsValidBiosFile(std::string path, const BiosInfo **info) {
	std::ifstream bios;

	bios.open(path, std::ios::in | std::ios::binary);

	if (!bios.is_open()) {
		return FALSE;
	}

	bios.seekg(0, std::ios::end);
	u32 size = (u32)bios.tellg();
	if (size != BIOS_SIZE) {
		return FALSE;
	}

	if (info) {
		u8 buf[BIOS_SIZE];

		bios.seekg(0, std::ios::beg);
		bios.read((char*)buf, size);

		*info = NULL;

		u32 checksum = CheckSum(buf, BIOS_SIZE);
		for (u32 i = 0; gBiosData[i].checksum != terminator; i++) {
			if (gBiosData[i].checksum == checksum) {
				*info = &gBiosData[i];
				break;
			}
		}
	}

	bios.close();
	return TRUE;
}

/* 
	returns a vector containing info about all valid BIOS ROMs in a directory 
		NOTE: To return BIOS files in "C:\BIOS", use a directory string of 
			  'C:\BIOS\*'.
*/
std::vector<ConfigBiosFiles> Config::GetValidBiosFileNamesInDir(std::string directory) {
	ConfigBiosFiles tmp;
	std::vector<ConfigBiosFiles> files;
	WIN32_FIND_DATA fileData; 
	HANDLE hFind = INVALID_HANDLE_VALUE; 

	if ((hFind = FindFirstFile(directory.c_str(), &fileData)) == INVALID_HANDLE_VALUE) {
		return (std::vector<ConfigBiosFiles>)NULL;
	}

	if (Config::IsValidBiosFile(std::string(fileData.cFileName), &tmp.info)) {
		strcpy(tmp.fileName, fileData.cFileName);
		files.push_back(tmp);
	}

	while(FindNextFile(hFind, &fileData)) {
		if (Config::IsValidBiosFile(mBiosDirectoryPath + '/' + fileData.cFileName, &tmp.info)) {
			strcpy(tmp.fileName, fileData.cFileName);
			files.push_back(tmp);
		}
	}

	FindClose(hFind);
	return files;
}

LRESULT CALLBACK ConfigDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	static std::vector<ConfigBiosFiles> vBiosFiles;
	static CPsx *psx = CPsx::GetInstance();

	switch(msg) {
		case WM_INITDIALOG: {
			psx->PauseEmulation(TRUE);

			std::string biosStr;
			std::string strCurConfigBios = psx->conf->GetCurBiosRegKey();
			int nCurConfigBiosIndex = 0;
			HWND hwndBiosCombo = GetDlgItem(hDlg, IDC_CONF_BIOS_LIST_COMBO);
			vBiosFiles = Config::GetValidBiosFileNamesInDir(Config::mBiosDirectoryPath + "/*");

			for (int i = 0; i < vBiosFiles.size(); i++) {
				biosStr = std::string(vBiosFiles[i].fileName);

				/* if the bios is loaded, catch the index of the currently
				   loaded bios */
				if (psx->conf->bBiosLoaded) {
					if (biosStr == strCurConfigBios) {
						nCurConfigBiosIndex = i;
					}
				}

				/* is this BIOS known (has a valid info pointer)? if so, 
				   display that info in the combobox */
				if (vBiosFiles[i].info) {
					biosStr +=  "  (" + 
						std::string(vBiosFiles[i].info->name) + " " +
						Config::regions[vBiosFiles[i].info->region] + ')';
				} else {
					biosStr += "  (Unknown)";
				}

				ComboBox_AddString(hwndBiosCombo, biosStr.c_str());
			}

			ComboBox_SetCurSel(hwndBiosCombo, nCurConfigBiosIndex);
		} break;

		case WM_COMMAND: 
			switch(LOWORD(wParam)) {
				case ID_OK: {
					HWND hwndBiosCombo = GetDlgItem(hDlg, IDC_CONF_BIOS_LIST_COMBO);
					int index = ComboBox_GetCurSel(hwndBiosCombo);

					/* check that there's bios files listed and the selected bios isn't already loaded */
					if (ComboBox_GetCount(hwndBiosCombo) != 0 && vBiosFiles[index].info != psx->mCurBios) {
						/* load the selected bios */
						if (psx->mem->LoadBiosRom(Config::mBiosDirectoryPath + '/' + vBiosFiles[index].fileName)) {
							psx->conf->SetCurBiosRegKey(vBiosFiles[index].fileName);
							psx->conf->bBiosLoaded = TRUE;
						} else {
							break;
						}
					}

					EndDialog(hDlg, LOWORD(wParam));
				} break;

				case ID_CANCEL:
					EndDialog(hDlg, LOWORD(wParam));
					break;
			}
			break;

		case WM_CLOSE:		
			EndDialog(hDlg, LOWORD(wParam));
			break;	
	}
	return 0;
}

BOOL Config::OpenRegistryKeys() {
	/* 
		open all the keys we want. if they don't exist, create them. 
	*/
	/* parent key */
	if (RegOpenKeyEx( HKEY_CURRENT_USER, "Software\\YAPSXE", 0, 
					  KEY_ALL_ACCESS, &hParentKey) != ERROR_SUCCESS) {
		if (RegCreateKeyEx( HKEY_CURRENT_USER, "Software\\YAPSXE", 0, 0, 
							REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, 
							&hParentKey, 0) != ERROR_SUCCESS) {
			return FALSE;
		}
	}

	/* current bios file name key */
	if (RegOpenKeyEx( hParentKey, "CurBiosFileName", 0, 
					  KEY_ALL_ACCESS, &hCurBiosFileKey) != ERROR_SUCCESS) {
		if (RegCreateKeyEx( hParentKey, "CurBiosFileName", 0, 0, REG_OPTION_NON_VOLATILE, 
						    KEY_ALL_ACCESS, NULL, &hCurBiosFileKey, 0) != ERROR_SUCCESS) {
			return FALSE;
		}
		SetCurBiosRegKey(DEFAULT_REG_STR_KEY_VAL);
	}

	return TRUE;
}

void Config::SetCurBiosRegKey(char *fileName) {
	RegSetValueEx( hCurBiosFileKey, NULL, 0, REG_SZ, 
				  (const BYTE*)fileName, strlen(fileName));
}

std::string Config::GetCurBiosRegKey() {
	char str[MAX_PATH];
	DWORD size = MAX_PATH;
	RegQueryValueEx( hCurBiosFileKey, NULL, 0, NULL, 
				  (LPBYTE)str, (LPDWORD)&size);
	return std::string(str);
}
/*  yaPSXe - PSX emulator
 *  Copyright (C) 2011-2012 Ryan Hackett
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "Config.h"
#include "resource.h"
#include "Bios.h"
#include "Psx.h"
#include "Memory.h"
#include "Cpu.h"
#include "Console.h"
#include "Windows.h"
#include "Psx.h"


/* 
	configuration settings are stored in the registry under
	'HKEY_CURRENT_USER\\Software\\YAPSXE'.
*/


const std::string PsxConfig::regions[] = {"NTSC:J", "NTSC:U/C", "PAL", "NTSC:J"};

PsxConfig::PsxConfig() {
	psx = Psx::GetInstance();

	if (!OpenRegistryKeys()) {
		MessageBox(0, "PsxConfig: failed to open registry keys\n", "Error", MB_ICONERROR);
	}

	mBiosDirectoryPath = GetBiosDirKey();
	std::string strCurBios = GetCurBiosRegKey();
	bLimitFps = GetLimitFpsKey();

	/* try to load the bios if it's configured */
	if (strCurBios != DEFAULT_REG_STR_KEY_VAL) {
		std::string strBiosPath = mBiosDirectoryPath + '/' + strCurBios;
		if (IsValidBiosFile(strBiosPath, 0)) {
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

PsxConfig::~PsxConfig() {
}

/* does basic checks to see if the file is a valid BIOS image.
   also (if info is not null) tries to identity the BIOS using a 
   checksum. if it's recognised, the pointer passed in "info" is 
   modified to point to the relevant "struct BiosInfo" entry */
BOOL PsxConfig::IsValidBiosFile(std::string path, const BiosInfo **info) {
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

		*info = 0;

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
std::vector<ConfigBiosFiles> PsxConfig::GetValidBiosFileNamesInDir(std::string directory) {
	ConfigBiosFiles tmp;
	std::vector<ConfigBiosFiles> files;
	WIN32_FIND_DATA fileData; 
	HANDLE hFind = INVALID_HANDLE_VALUE; 

	if ((hFind = FindFirstFile(directory.c_str(), &fileData)) == INVALID_HANDLE_VALUE) {
		return (std::vector<ConfigBiosFiles>)0;
	}

	if (PsxConfig::IsValidBiosFile(std::string(fileData.cFileName), &tmp.info)) {
		strcpy(tmp.fileName, fileData.cFileName);
		files.push_back(tmp);
	}

	while(FindNextFile(hFind, &fileData)) {
		if (PsxConfig::IsValidBiosFile(Psx::GetInstance()->conf->mBiosDirectoryPath + '/' + fileData.cFileName, &tmp.info)) {
			strcpy(tmp.fileName, fileData.cFileName);
			files.push_back(tmp);
		}
	}

	FindClose(hFind);
	return files;
}

/* populates the bios combo box with valid bios roms from the current bios directory */
void PsxConfig::RefreshBiosCombo(HWND hDlg, std::vector<ConfigBiosFiles> &vBiosFiles, Psx *psx) {
	psx->PauseEmulation(TRUE);

	std::string biosStr;
	std::string strCurConfigBios = psx->conf->GetCurBiosRegKey();
	int nCurConfigBiosIndex = 0;
	HWND hwndBiosCombo = GetDlgItem(hDlg, IDC_CONF_BIOS_LIST_COMBO);
	vBiosFiles = PsxConfig::GetValidBiosFileNamesInDir(psx->conf->mBiosDirectoryPath + "/*");

	ComboBox_ResetContent(hwndBiosCombo);

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
				PsxConfig::regions[vBiosFiles[i].info->region] + ')';
		} else {
			biosStr += "  (Unknown)";
		}

		ComboBox_AddString(hwndBiosCombo, biosStr.c_str());
	}

	ComboBox_SetCurSel(hwndBiosCombo, nCurConfigBiosIndex);
}

LRESULT CALLBACK ConfigDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	static std::vector<ConfigBiosFiles> vBiosFiles;
	static Psx *psx = Psx::GetInstance();

	switch(msg) {
		case WM_INITDIALOG: {
			PsxConfig::RefreshBiosCombo(hDlg, vBiosFiles, psx);

			HWND hBiosDirEdit = GetDlgItem(hDlg, IDC_BIOS_DIR_EDIT);
			SetWindowText(hBiosDirEdit, psx->conf->mBiosDirectoryPath.c_str());

			// set the limitfps button 
			CheckDlgButton(hDlg, IDC_LIMIT_FPS_CHECK, psx->conf->GetLimitFpsKey()); 
		} break;

		case WM_COMMAND: 
			switch(LOWORD(wParam)) {
				case ID_OK: {
					long res = Button_GetCheck(GetDlgItem(hDlg, IDC_LIMIT_FPS_CHECK));
					if(res == BST_CHECKED) {
						psx->conf->bLimitFps = TRUE;
					} else if(res == BST_UNCHECKED) {
						psx->conf->bLimitFps = FALSE;
					}
					psx->conf->SetLimitFpsKey(psx->conf->bLimitFps);

					/* 
						handle bios related stuff 
					*/
					HWND hwndBiosCombo = GetDlgItem(hDlg, IDC_CONF_BIOS_LIST_COMBO);
					int index = ComboBox_GetCurSel(hwndBiosCombo);

					/* check that there's bios files listed and the selected bios isn't already loaded */
					if (ComboBox_GetCount(hwndBiosCombo) != 0 && vBiosFiles[index].info != psx->mCurBios) {
						/* load the selected bios */
						if (psx->mem->LoadBiosRom(psx->conf->mBiosDirectoryPath + '/' + vBiosFiles[index].fileName)) {
							psx->conf->SetCurBiosRegKey(vBiosFiles[index].fileName);
							psx->conf->bBiosLoaded = TRUE;
						} else {
							break;
						}
					}
					/* ********************* */

					EndDialog(hDlg, LOWORD(wParam));
				} break;

				case ID_CANCEL:
					EndDialog(hDlg, LOWORD(wParam));
					break;

				case ID_BROWSE_BIOS_FOLDER: {
					TCHAR szFolder[MAX_PATH];
					if (Window::GetFolderSelection(0, szFolder, "Select BIOS Directory")) {
						psx->conf->mBiosDirectoryPath = szFolder;
						psx->conf->SetBiosDirKey(szFolder);
						PsxConfig::RefreshBiosCombo(hDlg, vBiosFiles, psx);
						HWND hBiosDirEdit = GetDlgItem(hDlg, IDC_BIOS_DIR_EDIT);
						SetWindowText(hBiosDirEdit, psx->conf->mBiosDirectoryPath.c_str());
					}
				} break;

				case IDC_RESET_CONF: {
					psx->conf->mBiosDirectoryPath = "BIOS";
					psx->conf->SetBiosDirKey((char*)psx->conf->mBiosDirectoryPath.c_str());
					PsxConfig::RefreshBiosCombo(hDlg, vBiosFiles, psx);
					HWND hBiosDirEdit = GetDlgItem(hDlg, IDC_BIOS_DIR_EDIT);
					SetWindowText(hBiosDirEdit, psx->conf->mBiosDirectoryPath.c_str());
				} break;

				case IDC_LIMIT_FPS_CHECK: {
				} break;
			}
			break;

		case WM_CLOSE:		
			EndDialog(hDlg, LOWORD(wParam));
			break;	
	}
	return 0;
}

BOOL PsxConfig::OpenRegistryKeys() {
	/* 
		open all the keys we want. if they don't exist, create them. 
	*/
	/* parent key */
	if (RegOpenKeyEx( HKEY_CURRENT_USER, "Software\\YAPSXE", 0, 
					  KEY_ALL_ACCESS, &hParentKey) != ERROR_SUCCESS) {
		if (RegCreateKeyEx( HKEY_CURRENT_USER, "Software\\YAPSXE", 0, 0, 
							REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, 
							&hParentKey, 0) != ERROR_SUCCESS) {
			return FALSE;
		}
	}

	/* current bios file name key */
	if (RegOpenKeyEx( hParentKey, "CurBiosFileName", 0, 
					  KEY_ALL_ACCESS, &hCurBiosFileKey) != ERROR_SUCCESS) {
		if (RegCreateKeyEx( hParentKey, "CurBiosFileName", 0, 0, REG_OPTION_NON_VOLATILE, 
						    KEY_ALL_ACCESS, 0, &hCurBiosFileKey, 0) != ERROR_SUCCESS) {
			return FALSE;
		}
		SetCurBiosRegKey(DEFAULT_REG_STR_KEY_VAL);
	}

	/* current bios directory key */
	if (RegOpenKeyEx( hParentKey, "BiosDirectory", 0, 
					  KEY_ALL_ACCESS, &hBiosDirectory) != ERROR_SUCCESS) {
		if (RegCreateKeyEx( hParentKey, "BiosDirectory", 0, 0, REG_OPTION_NON_VOLATILE, 
						    KEY_ALL_ACCESS, 0, &hBiosDirectory, 0) != ERROR_SUCCESS) {
			return FALSE;
		}
		SetBiosDirKey("BIOS"); // default BIOS directory path
	}

	/* LimitFPS directory key */
	if (RegOpenKeyEx( hParentKey, "LimitFPS", 0, 
					  KEY_ALL_ACCESS, &hLimitFpsKey) != ERROR_SUCCESS) {
		if (RegCreateKeyEx( hParentKey, "LimitFps", 0, 0, REG_OPTION_NON_VOLATILE, 
						    KEY_ALL_ACCESS, 0, &hLimitFpsKey, 0) != ERROR_SUCCESS) {
			return FALSE;
		}
		SetLimitFpsKey(TRUE);
	}
	
	return TRUE;
}

void PsxConfig::SetCurBiosRegKey(char *fileName) {
	RegSetValueEx( hCurBiosFileKey, 0, 0, REG_SZ, 
				  (const BYTE*)fileName, strlen(fileName));
}

std::string PsxConfig::GetCurBiosRegKey() {
	char str[MAX_PATH];
	DWORD size = MAX_PATH;
	RegQueryValueEx( hCurBiosFileKey, 0, 0, 0, 
				  (LPBYTE)str, (LPDWORD)&size);
	return std::string(str);
}

void PsxConfig::SetBiosDirKey(char *path) {
	RegSetValueEx( hBiosDirectory, 0, 0, REG_SZ, 
				  (const BYTE*)path, strlen(path));
}

std::string PsxConfig::GetBiosDirKey() {
	char str[MAX_PATH];
	DWORD size = MAX_PATH;
	RegQueryValueEx( hBiosDirectory, 0, 0, 0, 
				  (LPBYTE)str, (LPDWORD)&size);
	return std::string(str);
}

void PsxConfig::SetLimitFpsKey(BOOL val) {
	std::string str;
	str = (val ? "true" : "false");
	RegSetValueEx( hLimitFpsKey, 0, 0, REG_SZ, 
		(const BYTE*)str.c_str(), str.length());
}

BOOL PsxConfig::GetLimitFpsKey() {
	char str[MAX_PATH];
	DWORD size = MAX_PATH;
	RegQueryValueEx( hLimitFpsKey, 0, 0, 0, 
				  (LPBYTE)str, (LPDWORD)&size);
	return (std::string(str) == "true" ? true : false);
}
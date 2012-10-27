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

#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include "Common.h"

#define DEFAULT_REG_STR_KEY_VAL	"NULL"

LRESULT CALLBACK ConfigDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

struct ConfigBiosFiles {
	const BiosInfo *info;
	char fileName[MAX_PATH];
};

class Config {
public:
	Config();
	~Config();

	BOOL OpenRegistryKeys();

	static BOOL IsValidBiosFile(std::string path, const BiosInfo **info);
	static std::vector<ConfigBiosFiles> GetValidBiosFileNamesInDir(std::string directory);

	static void RefreshBiosCombo(HWND hDlg, std::vector<ConfigBiosFiles> &vBiosFiles, CPsx *psx);

	static const std::string regions[];
	std::string mBiosDirectoryPath;

	void SetCurBiosRegKey(char *fileName);
	std::string GetCurBiosRegKey();
	void SetBiosDirKey(char *fileName);
	std::string GetBiosDirKey();
	void SetLimitFpsKey(BOOL val);
	BOOL GetLimitFpsKey();

	HKEY hParentKey;
	HKEY hCurBiosFileKey;
	HKEY hBiosDirectory;
	HKEY hLimitFpsKey;

	BOOL bLimitFps;

	BOOL bBiosLoaded;
	CPsx *psx;
};

#endif /* CONFIG_H */
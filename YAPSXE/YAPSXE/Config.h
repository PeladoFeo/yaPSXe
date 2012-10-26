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
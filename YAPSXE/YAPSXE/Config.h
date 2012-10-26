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

	static const std::string regions[];
	static std::string mBiosDirectoryPath;

	void SetCurBiosRegKey(char *fileName);
	std::string GetCurBiosRegKey();

	HKEY hParentKey;
	HKEY hCurBiosFileKey;

	BOOL bBiosLoaded;
	CPsx *psx;
};

#endif /* CONFIG_H */
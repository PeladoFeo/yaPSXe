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

#include "Memory.h"
#include "Common.h"
#include "Console.h"
#include "Bios.h"
#include "Psx.h"


const BiosInfo gBiosData[] = {
	{ "SCPH-1000", 0xc9a3264f, NTSC_JAP, REV_C },
	{ "SCPH-1001", 0xf67ecb99, NTSC_USA, REV_B },
	{ "SCPH-1002", 0x63042cd8, PAL_EURO, REV_B },
	{ "SCPH-3000", 0x41b9617f, NTSC_JAP, REV_B },
	{ "SCPH-3500", 0x766eb1c7, NTSC_JAP, REV_B },
	{ "SCPH-5000", 0xb6a91b55, NTSC_JAP, REV_C },
	{ "SCPH-5500", 0x9a20506e, NTSC_JAP, REV_C },
	{ "SCPH-5502", 0x8372e64c, PAL_EURO, REV_C },
	{ "SCPH-5552", 0xb4497827, PAL_EURO, REV_C },	/* Men in Black promotional model */
	{ "SCPH-7000", 0xb625ca50, NTSC_JAP, REV_C },
	{ "SCPH-7001", 0xcddf93e2, NTSC_USA, REV_C },
	{ "SCPH-7003", 0x7c506633, NTSC_JAP, REV_C },
	{ "SCPH-7502", 0xcde393e2, PAL_EURO, REV_C },
	{ "SCPH-100", 0x9ee84bd1, NTSC_JAP, REV_C },	/* White PSone models */
	{ "SCPH-101", 0x6897a2ea, NTSC_USA, REV_C },
	{ "", terminator, terminator, terminator },
};

const BiosCalls bioscalls[] = {
	{ 0xa0, 0x00, "int open(const char *name, int mode)" },
	{ 0xa0, 0x01, "int lseek(int fd, int offset, int whence)" },
	{ 0xa0, 0x02, "int read(int fd, void *buf, int nbytes)" },
	{ 0xa0, 0x03, "int write(int fd, void *buf, int nbytes)" },
	{ 0xa0, 0x04, "int close(int fd)" },
	{ 0xa0, 0x05, "int ioctl(int fd, int cmd, int arg)" },
	{ 0xa0, 0x06, "void exit(int code)" },
	{ 0xa0, 0x07, "sys_a0_07()" },
	{ 0xa0, 0x08, "char getc(int fd)" },
	{ 0xa0, 0x09, "void putc(char c, int fd)" },
	{ 0xa0, 0x0a, "todigit()" },
	{ 0xa0, 0x0b, "double atof(const char *s)" },
	{ 0xa0, 0x0c, "long strtoul(const char *s, char **ptr, int base)" },
	{ 0xa0, 0x0d, "unsigned long strtol(const char *s, char **ptr, int base)" },
	{ 0xa0, 0x0e, "int abs(int val)" },
	{ 0xa0, 0x0f, "long labs(long lval)" },
	{ 0xa0, 0x10, "long atoi(const char *s)" },
	{ 0xa0, 0x11, "int atol(const char *s)" },
	{ 0xa0, 0x12, "atob()" },
	{ 0xa0, 0x13, "int setjmp(jmp_buf *ctx)" },
	{ 0xa0, 0x14, "void longjmp(jmp_buf *ctx, int value)" },
	{ 0xa0, 0x15, "char *strcat(char *dst, const char *src)" },
	{ 0xa0, 0x16, "char *strncat(char *dst, const char *src, size_t n)" },
	{ 0xa0, 0x17, "int strcmp(const char *dst, const char *src)" },
	{ 0xa0, 0x18, "int strncmp(const char *dst, const char *src, size_t n)" },
	{ 0xa0, 0x19, "char *strcpy(char *dst, const char *src)" },
	{ 0xa0, 0x1a, "char *strncpy(char *dst, const char *src, size_t n)" },
	{ 0xa0, 0x1b, "size_t strlen(const char *s)" },
	{ 0xa0, 0x1c, "int index(const char *s, int c)" },
	{ 0xa0, 0x1d, "int rindex(const char *s, int c)" },
	{ 0xa0, 0x1e, "char *strchr(const char *s, int c)" },
	{ 0xa0, 0x1f, "char *strrchr(const char *s, int c)" },
	{ 0xa0, 0x20, "char *strpbrk(const char *dst, const char *src)" },
	{ 0xa0, 0x21, "size_t strspn(const char *s, const char *set)" },
	{ 0xa0, 0x22, "size_t strcspn(const char *s, const char *set)" },
	{ 0xa0, 0x23, "char *strtok(char *s, const char *set)" },
	{ 0xa0, 0x24, "char *strstr(const char *s, const char *set)" },
	{ 0xa0, 0x25, "int toupper(int c)" },
	{ 0xa0, 0x26, "int tolower(int c)" },
	{ 0xa0, 0x27, "void bcopy(const void *src, void *dst, size_t len)" },
	{ 0xa0, 0x28, "void bzero(void *ptr, size_t len)" },
	{ 0xa0, 0x29, "int bcmp(const void *ptr1, const void *ptr2, int len)" },
	{ 0xa0, 0x2a, "void *memcpy(void *dst, const void *src, size_t n)" },
	{ 0xa0, 0x2b, "void *memset(void *dst, char c, size_t n)" },
	{ 0xa0, 0x2c, "void *memmove(void *dst, const void *src, size_t n)" },
	{ 0xa0, 0x2d, "int memcmp(const void *dst, const void *src, size_t n)" },
	{ 0xa0, 0x2e, "void *memchr(const void *s, int c, size_t n)" },
	{ 0xa0, 0x2f, "int rand()" },
	{ 0xa0, 0x30, "void srand(unsigned int seed)" },
	{ 0xa0, 0x31, "void qsort(void *base, int nel, int width, int (*cmp)(void *, void *))" },
	{ 0xa0, 0x32, "double strtod(const char *s, char **endptr)" },
	{ 0xa0, 0x33, "void *malloc(int size)" },
	{ 0xa0, 0x34, "void free(void *buf)" },
	{ 0xa0, 0x35, "void *lsearch(void *key, void *base, int belp, int width, int (*cmp)(void *, void *))" },
	{ 0xa0, 0x36, "void *bsearch(void *key, void *base, int nel, int size, int (*cmp)(void *, void *))" },
	{ 0xa0, 0x37, "void *calloc(int size, int n)" },
	{ 0xa0, 0x38, "void *realloc(void *buf, int n)" },
	{ 0xa0, 0x39, "InitHeap(void *block, int size)" },
	{ 0xa0, 0x3a, "void _exit(int code)" },
	{ 0xa0, 0x3b, "char getchar(void)" },
	{ 0xa0, 0x3c, "void putchar(char c)" },
	{ 0xa0, 0x3d, "char *gets(char *s)" },
	{ 0xa0, 0x3e, "void puts(const char *s)" },
	{ 0xa0, 0x3f, "int printf(const char *fmt, ...)" },
	{ 0xa0, 0x40, "sys_a0_40()" },
	{ 0xa0, 0x41, "int LoadTest(const char *name, struct EXEC *header)" },
	{ 0xa0, 0x42, "int Load(const char *name, struct EXEC *header)" },
	{ 0xa0, 0x43, "int Exec(struct EXEC *header, int argc, char **argv)" },
	{ 0xa0, 0x44, "void FlushCache()" },
	{ 0xa0, 0x45, "void InstallInterruptHandler()" },
	{ 0xa0, 0x46, "GPU_dw(int x, int y, int w, int h, long *data)" },
	{ 0xa0, 0x47, "mem2vram(int x, int y, int w, int h, long *data)" },
	{ 0xa0, 0x48, "SendGPU(int status)" },
	{ 0xa0, 0x49, "GPU_cw(long cw)" },
	{ 0xa0, 0x4a, "GPU_cwb(long *pkt, int len)" },
	{ 0xa0, 0x4b, "SendPackets(void *ptr)" },
	{ 0xa0, 0x4c, "sys_a0_4c()" },
	{ 0xa0, 0x4d, "int GetGPUStatus()" },
	{ 0xa0, 0x4e, "GPU_sync()" },
	{ 0xa0, 0x4f, "sys_a0_4f()" },
	{ 0xa0, 0x50, "sys_a0_50()" },
	{ 0xa0, 0x51, "int LoadExec(const char *name, int, int)" },
	{ 0xa0, 0x52, "GetSysSp()" },
	{ 0xa0, 0x53, "sys_a0_53()" },
	{ 0xa0, 0x54, "_96_init()" },
	{ 0xa0, 0x55, "_bu_init()" },
	{ 0xa0, 0x56, "_96_remove()" },
	{ 0xa0, 0x57, "sys_a0_57()" },
	{ 0xa0, 0x58, "sys_a0_58()" },
	{ 0xa0, 0x59, "sys_a0_59()" },
	{ 0xa0, 0x5a, "sys_a0_5a()" },
	{ 0xa0, 0x5b, "dev_tty_init()" },
	{ 0xa0, 0x5c, "dev_tty_open()" },
	{ 0xa0, 0x5d, "dev_tty_5d()" },
	{ 0xa0, 0x5e, "dev_tty_ioctl()" },
	{ 0xa0, 0x5f, "dev_cd_open()" },
	{ 0xa0, 0x60, "dev_cd_read()" },
	{ 0xa0, 0x61, "dev_cd_close()" },
	{ 0xa0, 0x62, "dev_cd_firstfile()" },
	{ 0xa0, 0x63, "dev_cd_nextfile()" },
	{ 0xa0, 0x64, "dev_cd_chdir()" },
	{ 0xa0, 0x65, "dev_card_open()" },
	{ 0xa0, 0x66, "dev_card_read()" },
	{ 0xa0, 0x67, "dev_card_write()" },
	{ 0xa0, 0x68, "dev_card_close()" },
	{ 0xa0, 0x69, "dev_card_firstfile()" },
	{ 0xa0, 0x6a, "dev_card_nextfile()" },
	{ 0xa0, 0x6b, "dev_card_erase()" },
	{ 0xa0, 0x6c, "dev_card_undelete()" },
	{ 0xa0, 0x6d, "dev_card_format()" },
	{ 0xa0, 0x6e, "dev_card_rename()" },
	{ 0xa0, 0x6f, "dev_card_6f()" },
	{ 0xa0, 0x70, "_bu_init()" },
	{ 0xa0, 0x71, "_96_init()" },
	{ 0xa0, 0x72, "_96_remove()" },
	{ 0xa0, 0x73, "sys_a0_73()" },
	{ 0xa0, 0x74, "sys_a0_74()" },
	{ 0xa0, 0x75, "sys_a0_75()" },
	{ 0xa0, 0x76, "sys_a0_76()" },
	{ 0xa0, 0x77, "sys_a0_77()" },
	{ 0xa0, 0x78, "_96_CdSeekL()" },
	{ 0xa0, 0x79, "sys_a0_79()" },
	{ 0xa0, 0x7a, "sys_a0_7a()" },
	{ 0xa0, 0x7b, "sys_a0_7b()" },
	{ 0xa0, 0x7c, "_96_CdGetStatus()" },
	{ 0xa0, 0x7d, "sys_a0_7d()" },
	{ 0xa0, 0x7e, "_96_CdRead()" },
	{ 0xa0, 0x7f, "sys_a0_7f()" },
	{ 0xa0, 0x80, "sys_a0_80()" },
	{ 0xa0, 0x81, "sys_a0_81()" },
	{ 0xa0, 0x82, "sys_a0_82()" },
	{ 0xa0, 0x83, "sys_a0_83()" },
	{ 0xa0, 0x84, "sys_a0_84()" },
	{ 0xa0, 0x85, "_96_CdStop()" },
	{ 0xa0, 0x84, "sys_a0_84()" },
	{ 0xa0, 0x85, "sys_a0_85()" },
	{ 0xa0, 0x86, "sys_a0_86()" },
	{ 0xa0, 0x87, "sys_a0_87()" },
	{ 0xa0, 0x88, "sys_a0_88()" },
	{ 0xa0, 0x89, "sys_a0_89()" },
	{ 0xa0, 0x8a, "sys_a0_8a()" },
	{ 0xa0, 0x8b, "sys_a0_8b()" },
	{ 0xa0, 0x8c, "sys_a0_8c()" },
	{ 0xa0, 0x8d, "sys_a0_8d()" },
	{ 0xa0, 0x8e, "sys_a0_8e()" },
	{ 0xa0, 0x8f, "sys_a0_8f()" },
	{ 0xa0, 0x90, "sys_a0_90()" },
	{ 0xa0, 0x91, "sys_a0_91()" },
	{ 0xa0, 0x92, "sys_a0_92()" },
	{ 0xa0, 0x93, "sys_a0_93()" },
	{ 0xa0, 0x94, "sys_a0_94()" },
	{ 0xa0, 0x95, "sys_a0_95()" },
	{ 0xa0, 0x96, "AddCDROMDevice()" },
	{ 0xa0, 0x97, "AddMemCardDevice()" },
	{ 0xa0, 0x98, "DisableKernelIORedirection()" },
	{ 0xa0, 0x99, "EnableKernelIORedirection()" },
	{ 0xa0, 0x9a, "sys_a0_9a()" },
	{ 0xa0, 0x9b, "sys_a0_9b()" },
	{ 0xa0, 0x9c, "void SetConf(int Event, int TCB, int Stack)" },
	{ 0xa0, 0x9d, "void GetConf(int *Event, int *TCB, int *Stack)" },
	{ 0xa0, 0x9e, "sys_a0_9e()" },
	{ 0xa0, 0x9f, "void SetMem(int size)" },
	{ 0xa0, 0xa0, "_boot()" },
	{ 0xa0, 0xa1, "SystemError()" },
	{ 0xa0, 0xa2, "EnqueueCdIntr()" },
	{ 0xa0, 0xa3, "DequeueCdIntr()" },
	{ 0xa0, 0xa4, "sys_a0_a4()" },
	{ 0xa0, 0xa5, "ReadSector(int count, int sector, void *buffer)" },
	{ 0xa0, 0xa6, "get_cd_status()" },
	{ 0xa0, 0xa7, "bufs_cb_0()" },
	{ 0xa0, 0xa8, "bufs_cb_1()" },
	{ 0xa0, 0xa9, "bufs_cb_2()" },
	{ 0xa0, 0xaa, "bufs_cb_3()" },
	{ 0xa0, 0xab, "_card_info()" },
	{ 0xa0, 0xac, "_card_load()" },
	{ 0xa0, 0xad, "_card_auto()" },
	{ 0xa0, 0xae, "bufs_cb_4()" },
	{ 0xa0, 0xaf, "sys_a0_af()" },
	{ 0xa0, 0xb0, "sys_a0_b0()" },
	{ 0xa0, 0xb1, "sys_a0_b1()" },
	{ 0xa0, 0xb2, "do_a_long_jmp()" },
	{ 0xa0, 0xb3, "sys_a0_b3()" },
	{ 0xa0, 0xb4, "GetKernelInfo(int sub_function)" },
	{ 0xb0, 0x00, "SysMalloc()" },
	{ 0xb0, 0x01, "sys_b0_01()" },
	{ 0xb0, 0x02, "sys_b0_02()" },
	{ 0xb0, 0x03, "sys_b0_03()" },
	{ 0xb0, 0x04, "sys_b0_04()" },
	{ 0xb0, 0x05, "sys_b0_05()" },
	{ 0xb0, 0x06, "sys_b0_06()" },
	{ 0xb0, 0x07, "void DeliverEvent(u_long class, u_long event)" },
	{ 0xb0, 0x08, "long OpenEvent(u_long class, long spec, long mode, long (*func)())" },
	{ 0xb0, 0x09, "long CloseEvent(long event)" },
	{ 0xb0, 0x0a, "long WaitEvent(long event)" },
	{ 0xb0, 0x0b, "long TestEvent(long event)" },
	{ 0xb0, 0x0c, "long EnableEvent(long event)" },
	{ 0xb0, 0x0d, "long DisableEvent(long event)" },
	{ 0xb0, 0x0e, "OpenTh()" },
	{ 0xb0, 0x0f, "CloseTh()" },
	{ 0xb0, 0x10, "ChangeTh()" },
	{ 0xb0, 0x11, "sys_b0_11()" },
	{ 0xb0, 0x12, "int InitPAD(char *buf1, int len1, char *buf2, int len2)" },
	{ 0xb0, 0x13, "int StartPAD(void)" },
	{ 0xb0, 0x14, "int StopPAD(void)" },
	{ 0xb0, 0x15, "PAD_init(u_long nazo, u_long *pad_buf)" },
	{ 0xb0, 0x16, "u_long PAD_dr()" },
	{ 0xb0, 0x17, "void ReturnFromException(void)" },
	{ 0xb0, 0x18, "ResetEntryInt()" },
	{ 0xb0, 0x19, "HookEntryInt()" },
	{ 0xb0, 0x1a, "sys_b0_1a()" },
	{ 0xb0, 0x1b, "sys_b0_1b()" },
	{ 0xb0, 0x1c, "sys_b0_1c()" },
	{ 0xb0, 0x1d, "sys_b0_1d()" },
	{ 0xb0, 0x1e, "sys_b0_1e()" },
	{ 0xb0, 0x1f, "sys_b0_1f()" },
	{ 0xb0, 0x20, "UnDeliverEvent(int class, int event)" },
	{ 0xb0, 0x21, "sys_b0_21()" },
	{ 0xb0, 0x22, "sys_b0_22()" },
	{ 0xb0, 0x23, "sys_b0_23()" },
	{ 0xb0, 0x24, "sys_b0_24()" },
	{ 0xb0, 0x25, "sys_b0_25()" },
	{ 0xb0, 0x26, "sys_b0_26()" },
	{ 0xb0, 0x27, "sys_b0_27()" },
	{ 0xb0, 0x28, "sys_b0_28()" },
	{ 0xb0, 0x29, "sys_b0_29()" },
	{ 0xb0, 0x2a, "sys_b0_2a()" },
	{ 0xb0, 0x2b, "sys_b0_2b()" },
	{ 0xb0, 0x2c, "sys_b0_2c()" },
	{ 0xb0, 0x2d, "sys_b0_2d()" },
	{ 0xb0, 0x2e, "sys_b0_2e()" },
	{ 0xb0, 0x2f, "sys_b0_2f()" },
	{ 0xb0, 0x2f, "sys_b0_30()" },
	{ 0xb0, 0x31, "sys_b0_31()" },
	{ 0xb0, 0x32, "int open(const char *name, int access)" },
	{ 0xb0, 0x33, "int lseek(int fd, long pos, int seektype)" },
	{ 0xb0, 0x34, "int read(int fd, void *buf, int nbytes)" },
	{ 0xb0, 0x35, "int write(int fd, void *buf, int nbytes)" },
	{ 0xb0, 0x36, "close(int fd)" },
	{ 0xb0, 0x37, "int ioctl(int fd, int cmd, int arg)" },
	{ 0xb0, 0x38, "exit(int exitcode)" },
	{ 0xb0, 0x39, "sys_b0_39()" },
	{ 0xb0, 0x3a, "char getc(int fd)" },
	{ 0xb0, 0x3b, "putc(int fd, char ch)" },
	{ 0xb0, 0x3c, "char getchar(void)" },
	{ 0xb0, 0x3d, "putchar(char ch)" },
	{ 0xb0, 0x3e, "char *gets(char *s)" },
	{ 0xb0, 0x3f, "puts(const char *s)" },
	{ 0xb0, 0x40, "int cd(const char *path)" },
	{ 0xb0, 0x41, "int format(const char *fs)" },
	{ 0xb0, 0x42, "struct DIRENTRY* firstfile(const char *name, struct DIRENTRY *dir)" },
	{ 0xb0, 0x43, "struct DIRENTRY* nextfile(struct DIRENTRY *dir)" },
	{ 0xb0, 0x44, "int rename(const char *oldname, const char *newname)" },
	{ 0xb0, 0x45, "int delete(const char *name)" },
	{ 0xb0, 0x46, "undelete()" },
	{ 0xb0, 0x47, "AddDevice()" },
	{ 0xb0, 0x48, "RemoveDevice()" },
	{ 0xb0, 0x49, "PrintInstalledDevices()" },
	{ 0xb0, 0x4a, "InitCARD()" },
	{ 0xb0, 0x4b, "StartCARD()" },
	{ 0xb0, 0x4c, "StopCARD()" },
	{ 0xb0, 0x4d, "sys_b0_4d()" },
	{ 0xb0, 0x4e, "_card_write()" },
	{ 0xb0, 0x4f, "_card_read()" },
	{ 0xb0, 0x50, "_new_card()" },
	{ 0xb0, 0x51, "void *Krom2RawAdd(int code)" },
	{ 0xb0, 0x52, "sys_b0_52()" },
	{ 0xb0, 0x53, "sys_b0_53()" },
	{ 0xb0, 0x54, "long _get_errno(void)" },
	{ 0xb0, 0x55, "long _get_error(long fd)" },
	{ 0xb0, 0x56, "GetC0Table()" },
	{ 0xb0, 0x57, "GetB0Table()" },
	{ 0xb0, 0x58, "_card_chan()" },
	{ 0xb0, 0x59, "sys_b0_59()" },
	{ 0xb0, 0x5a, "sys_b0_5a()" },
	{ 0xb0, 0x5b, "ChangeClearPAD(int, int)" },
	{ 0xb0, 0x5c, "_card_status()" },
	{ 0xb0, 0x5d, "_card_wait()" },
	{ 0xc0, 0x00, "InitRCnt()" },
	{ 0xc0, 0x01, "InitException()" },
	{ 0xc0, 0x02, "SysEnqIntRP(int index, long *queue)" },
	{ 0xc0, 0x03, "SysDeqIntRP(int index, long *queue)" },
	{ 0xc0, 0x04, "int get_free_EvCB_slot(void)" },
	{ 0xc0, 0x05, "get_free_TCB_slot()" },
	{ 0xc0, 0x06, "ExceptionHandler()" },
	{ 0xc0, 0x07, "InstallExceptionHandlers()" },
	{ 0xc0, 0x08, "SysResetMemory()" },
	{ 0xc0, 0x09, "SysInitKMem()" },
	{ 0xc0, 0x0a, "ChangeClearRCnt()" },
	{ 0xc0, 0x0b, "SystemError()" },
	{ 0xc0, 0x0c, "InitDefInt()" },
	{ 0xc0, 0x0d, "sys_c0_0d()" },
	{ 0xc0, 0x0e, "sys_c0_0e()" },
	{ 0xc0, 0x0f, "sys_c0_0f()" },
	{ 0xc0, 0x10, "sys_c0_10()" },
	{ 0xc0, 0x11, "sys_c0_11()" },
	{ 0xc0, 0x12, "InstallDevices()" },
	{ 0xc0, 0x13, "FlushStdInOutPut()" },
	{ 0xc0, 0x14, "sys_c0_14()" },
	{ 0xc0, 0x15, "_cdevinput()" },
	{ 0xc0, 0x16, "_cdevscan()" },
	{ 0xc0, 0x17, "char _circgetc(struct device_buf *circ)" },
	{ 0xc0, 0x18, "_circputc(char c, struct device_buf *circ)" },
	{ 0xc0, 0x19, "ioabort(const char *str)" },
	{ 0xc0, 0x1a, "sys_c0_1a()" },
	{ 0xc0, 0x1b, "KernelRedirect(int flag)" },
	{ 0xc0, 0x1c, "PatchA0Table()" },
	{ 0x00, 0x00, 0 }
};

u32 CheckSum(u8 *data, u32 size) {
	u32 checksum = 0;
	for (u32 i = 0; i < size; i += 4) {
		checksum += *(u32*)(data + i);
	}
	return checksum;
}

BOOL PsxMemory::LoadBiosRom(std::string path) {
	std::ifstream biosrom;
	static Psx *psx = Psx::GetInstance();
	
	biosrom.open(path.c_str(), std::ios::in | std::ios::binary);
	if (!biosrom.is_open()) {
		MessageBox(0, "Failed to open BIOS ROM", "Error", MB_ICONERROR);
		biosrom.close();
		return FALSE;
	}

	biosrom.seekg(0, std::ios::end);
	u32 size = (u32)biosrom.tellg();
	if (size != BIOS_SIZE) {
		MessageBox(0, "BIOS is incorrect size", "Error", MB_ICONERROR);
		biosrom.close();
		return FALSE;
	}

	biosrom.seekg(0, std::ios::beg);
	biosrom.read((char*)bios, size);

	biosrom.close();
	
	u32 checksum = CheckSum(bios, BIOS_SIZE);

	for (u32 i = 0; gBiosData[i].checksum != terminator; i++) {
		if (gBiosData[i].checksum == checksum) {
			psx->mCurBios = &gBiosData[i];
			csl->out(CWHITE, "BIOS %s detected (checksum=0x%x)\n", 
				gBiosData[i].name, checksum);

			// no ascii info in SCPH-1000
			if (gBiosData[i].checksum != 0xc9a3264f) {
				char strBiosInfo[0x60];
				memcpy(strBiosInfo, &bios[0x7ff32], 0x60);

				// remove null characters
				for (int i = 0; i < 0x5e; i++) {
					if (strBiosInfo[i] == '\0') 
						strBiosInfo[i] = ' ';
				}

				strBiosInfo[0x5e] = '\0';
				csl->out(CWHITE, "%s\n\n", strBiosInfo);
			}

			return TRUE;
		}
	}

	csl->out("Unknown BIOS loaded (checksum=0x%x)\n", checksum);
	psx->mCurBios = 0;
	return TRUE;
}


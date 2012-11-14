

Rom Information

Updates:

04 Apr 1997 	- Replaced some sys_xx_yy by their names.
		- Added some copy protection comments doExec (bfc0d570).
		- Corrected some addresses (some a001xxxx are in fact a000xxxx).
		- Change some numeric variables (a000b9xx) to names (event_cd_xxxxx).
08 Jul 1997	- Changed Rectangle()
		- 0xbfc03d80 -> GPU_dw
		- Added 0xbfc04610|InitIntr()
		- Added 0xbfc04678|InitEvent()
		- Changed ClearWorkArea to SysInitKMem
		- Changed OpenAllEvents to OpenCDEvents
		- Added 0xbfc0d4cc|EnableIORedirection()
		- Added black cd check related routines: bfc0d600/d640/d664/d72c
16 Oct 1997	- Changed SetGPU_status to SendGPU
		- Changed *GPU_status to *gp1
		- Changed H0x1f801810 to gp0
		- Changed H0x1f80180[0123] to reg[0123]
		- Changed _0x1f801014 to *spu_delay
		- Changed _0x1f801018 to *dv5_delay
		- Changed _0x1f801020 to *com_delay
		- Changed _0x1f801060, *H0x1f801060 to *ram_size
		- Changed H0x1f801070 to *i_reg
22 Oct 1997	- Added some nice comments on ColorBars()



    _0xXXXXXXXX means the value at address XXXXXXXX.
    When _xXXXXXXXX=(YYYY) with YYYY={char,short,etc..}, it means that address XXXXXXXX is of type YYYY
    'Buffer' is a multipurpose buffer, located at 0xa000b080
    _stk_ stands for register $sp, _fp_ for $fp, _gp_ for $gp, etc...
    Some (maybe lots) of variable names, functions are invented...Cause i still dont know what they do, or what is the meaning of what they do
    sys_XX_YY means a system call, the same as li $t1,YY , jr XX

    _0xXXXXXXXXÇÕÉAÉhÉåÉXXXXXXXXXÇÃílÇà”ñ°ÇµÇ‹Ç∑ÅB
    YYYY={char,short,ìô}Ç≈_xXXXXXXXX=(YYYY)ÇÃéû, ÉAÉhÉåÉXXXXXXXXXÇ™YYYYå^Ç≈Ç ÇÈÇ±Ç∆Çà”ñ°ÇµÇ‹Ç∑ÅB
    'Buffer'ÇÕ0xa000b080Ç…à íuÇµÇƒÇ¢ÇÈëΩñ⁄ìIÇÃÉoÉbÉtÉ@Ç≈Ç∑ÅB
    _stk_ stands for register $sp, _fp_ for $fp, _gp_ for $gp, etc...
    Ç¢Ç≠Ç¬Ç©(ÇΩÇ‘ÇÒëΩÇ≠)ÇÃïœêîñºÅAä÷êîÇÕÇ≈Ç¡Çøè„Ç∞Ç≈Ç∑... ÇªÇÍÇÁÇ™çsÇ§Ç±Ç∆Ç‚ÅAçsÇ§Ç±Ç∆ÇÃà”ñ°ÇéÑÇ™Ç‹ÇæímÇÁÇ»Ç¢ÇΩÇﬂÇ≈Ç∑ÅB
    sys_XX_YYÇÕÉVÉXÉeÉÄÉRÅ[ÉãÇà”ñ°ÇµÅAli $t1,YY , jr XXÇ∆ìØÇ∂Ç≈Ç∑ÅB



/* 0xbfc00000 */
start()
{
	_0x1f801010 = 0x0013243f;
	*ram_size = 0x00000b88;
	jmp_part0();
}

/* 0xbfc00150 */
part0()
{
	*com_delay = 0x00031125;
	_0x1f801000 = 0x1f000000;
	_0x1f801004 = 0x1f002000;
	_0x1f801008 = 0x0013243f;
	*spu_delay = 0x200931e1;
	*dv5_delay = 0x00020843;
	_0x1f80100c = 0x00003022;
	_0x1f80101c = 0x00070777;
	clear_all_registers(); /* inline */
	_0xfffe0130 = 0x00000804;
	/* Flushing Cache begin */
		/* details deleted */
	/* Flushing Cache end */
	tmp = _0xa0000000;
	tmp = _0x00000000;
	tmp = _0x00000000;
	tmp = _0x00000000;
	tmp = _0x00000000;
	tmp = _0x00000000;
	tmp = _0x00000000;
	tmp = _0x00000000;
	_0xfffe0130 = 0x0001e988;
	C0.DCIC = 0;
	C0.BPC = 0;
	C0.BDA = 0;
	C0.TAR = 0;
	C0.BDAM = 0;
	C0.BPCM = 0;
	C0.SR = 0;
	C0.CAUSE = 0;
	for (tmp = 0xa0009000 ; tmp != 0xa000c160 ; tmp++)
		*tmp=0;
	_stk = 0x801fff00;
	_gp_ = 0xa0010ff0;
	_fp_ = _stk;
	*ram_size = 0x00000b88;
	_0x00000060=2;
	_0x00000064=0;
	_0x00000068=0xff;
	zero_stuff();
	jump_part2();
}


SysInitKMem() /* 0xbfc00420 */
{
	long count , *src , *dst;

	src = 0xbfc10000;
	dst = 0xa0000500;
	for (count = 0x8fb0 ; count != 0 ; count -= 4)
		*dst++ = *src++;
	jmp_0xa0000500();
}


/* 0xbfc00460 */
ColorBars()
{
	SendGPU(0);		/* Some kind of reset ? */
	Clear_();
	SendGPU(0x03000000);	/* Display Mask (enable display) */
	Clear_();
	SendGPU(0x06c60260);	/* Screen horizontal start/end */
	Clear_();
	SendGPU(0x07040010);	/* Screen vertical start/end */
	Clear_();
	GPU_cw(0xe1000400);	/* Draw on display area */
	Clear_();
	GPU_cw(0xe3000000);	/* Draw Area x, y */
	Clear_();
	GPU_cw(0xe407ffff);	/* Draw Area w, h */
	Clear_();
	GPU_cw(0xe5000000);	/* Draw Offset */
	Clear_();
	SendGPU(0x08000000);	/* Display mode, 256x240/NTSC/noninterlaced */
	Clear_();
	SendGPU(0x08000000);	/* Display mode, 256x240/NTSC/noninterlaced */
	Clear_();
	DrawBackground();
	SendGPU(0x08000000);	/* Display mode, 256x240/NTSC/noninterlaced */
	Clear_();
	DrawBackground();
	Rectangle(  0,   0,  32, 241, 255, 255, 255);
	Rectangle( 32,   0,  32, 241, 255, 255,   0);
	Rectangle( 64,   0,  32, 241,   0, 255, 255);
	Rectangle( 96,   0,  32, 241,   0, 255,   0);
	Rectangle(128,   0,  32, 241, 255,   0, 255);
	Rectangle(160,   0,  32, 241, 255,   0,   0);
	Rectangle(192,   0,  32, 241,   0,   0, 255);
	Rectangle(224,   0,  32, 241,   0,   0,   0);

	return 0;
}

/* 0xbfc00778 */
DrawBackground()
{
	Rectangle(  0 ,   0 , 513 , 257 ,   0 ,   0 ,   0);
	Rectangle(  0 , 256 , 513 , 257 ,   0 ,   0 ,   0);
	Rectangle(512 ,   0 , 513 , 257 ,   0 ,   0 ,   0);
	Rectangle(512 , 256 , 513 , 257 ,   0 ,   0 ,   0);
}


/* 0xbfc00810 */
Rectangle(int x , int y , int w , int h , int R , int G , int B)
{
	long local[5];

	local[0] = ((B << 16) | 0x28000000) | (G << 8) | (R);
	local[1] = (y << 16) | (x);
	local[2] = (y << 16) | (x + w);
	local[3] = ((y + h) << 16) | (x);
	local[4] = ((y + h) << 16) | (x + w);
	GPU_cwb(local , 5);
}

ParseSetupFile(char *buf,TCBEV *a,char *boot)
{
	long tmp,*z;

	z=a;
	for (tmp=0 ; tmp !=3 ; ++tmp) {
		*z++=0;
	}
	*boot="\0";
	_0x00000180=(char)0;
	GetValue(buf , &a->Tcb , "TCB");
	GetValue(buf , &a->Event , "EVENT");
	GetValue(buf , &a->Stack , "STACK");
	GetString(buf , boot , 384 , "BOOT");
}
	
__trace(char value)
{
	_1f802041 = value;
	Clear0XA001B068();
}


/* sys_a0_42 0xbfc03a18 */
int Load(char *name,struct EXEC *header)
{
	int fd;
	
	if ((fd = open(name,1)) <0) {
		return 0;
	}
	if (ReadExeHeader(fd , header)="=" 0) {
		close(fd); return 0;
	}
	read(fd , header->t_addr , header->t_size);
	close(fd);
	FlushCache();
	
	return 1;
}


/* 0xbfc03aa4 */
LoadExec(char *name , int argc , char **argv)
{
	char *src,*dst;
	char tmp[32];

	src = name;
	dst = tmp;
	while (*src && *src != ':') {
		*dst++ = *src++;
	}
	while (*dst++ = toupper(*src++)) ;
	dst = tmp;
	while (*dst && *dst != ';') {
		dst++;
	}
	if (!*dst) {
		strcat(tmp,";1");
	}
	_0xa0009144 = _0xa000b890;
	_0xa0009148 = _0xa000b894;
	ExitCriticalSection();
	if (Load(tmp , ExeHeader) == 1) {
		_0xa000b890 = argc;
		_0xa000b894 = argv;
		Exec(ExeHeader , tmp , 0);
	} else {
		printf("No EXE-file !\n");
	}
	printf("Execute the boot file %s.\n" , BootFile);
	ExitCriticalSection();
	if (Load(BootFile,ExeHeader) == 1) {
		_0xa000b890 = _0xa0009144;
		_0xa000b894 = _0xa0009148;
		Exec(ExeHeader,1,0);
	}
	printf("No boot file !\n");
	while (1) ;
}
	
/* 0xbfc03c90 */
int ReadExeHeader(int fd,void *buf)
{
	if (read(fd , Buffer , 2048) <2048) {
		return 0;
	}
	memcpy(buf , &Buffer[0x10] , 0x3c);
	return 1;
}


/* sys_a0_43 0xbfc03cf0 */
int Exec(struct EXEC *exe , int argc , char **argv)
{
	int i;
	long *p;

	exe->SavedS0 = _s0_;
	exe->SavedRA = _ra_;
	exe->SavedSP = _sp_;
	exe->SavedS8 = _s8_;
	exe->SavedGP = _gp_;
	if (exe->b_size) {
		p = exe->b_addr;
		for (i = exe->b_size ; i > 0 ; i -= 4) {
			*p++ = 0;
		}
	}
	if (exe->s_addr) {
		_stk_ = exe->s_addr + exe->s_size;
		_fp_ = _stk_;
	}
	_gp_ = exe->gp0;
	(exe->pc0)(argc , argv);
	_ra_ = exe->SavedRA;
	_sp_ = exe->SavedSP;
	_s8_ = exe->SavedS8;
	_gp_ = exe->SavedGP;
	_s0_ = exe->SavedS0;

	return 1;
}


/* 0xbfc03d80 */
GPU_dw(x,y,w,h,*data)
{
	int count;

	count = 16;
	while (!(*gp1 & 0x04000000)) {
		if (! count--) {
			printGPUtimeout("GPU_dw");
		}
	}
	*gp0 = 0xa0000000;
	*gp0 = (y<<16)|(x&0xffff);
	*gp0 = (h<<16)|(y&0xffff);

	count = (w*h)/2-1;
	if (count==-1) return;
	do {
		*gp0 = *data++;
	} while(count--);
}

printGPUtimeout(char* str) // bfc04260
{
	printf("%s timeout: gp1=%08x",str,*gp1);
}


GPU_cw(u_long c)
{
	GPU_sync();
	*gp0=c;
}


bfunc40ec()
{
	*d2_chcr=0x0401;
	*gp1=0x04000000;
	*gp1=0x02000000;
	*gp1=0x01000000;
}

// download from gp0
bfunc3fe0(u_long a0, u_long a1)
{
	GPU_sync();
}


SendPrimitives(void* ptr) // bfc0403c
{
	printf("0x01(%08x)\n",ptr);
	GPU_sync();

	*gp1=0x04000002;

	*d_icr=0;
	*d_pcr|=0x0800;

	*d2_madr=ptr;
	*d2_bcr=0;
	printf("0x02\n");

	*d2_chcr=0x01000401;
	printf("0x03\n");
}

/* 0xbfc04610 */
InitIntr(int count)
{
	u_char *p;
	u_long size;
	size=count * 8;
	if ((p=_SysMalloc(size))==NULL) {
		return 0;
	}
	bclear(p, size);
	*(u_long *)0xa0000100=p;
	*(u_long *)0xa0000104=size;
	return size;
}

/* 0xbfc04678 */
InitEvent(int n) {
	int i;
	unsigned long *h, *p;
	printf("\nConfiguration : EvCB\0100x%02x\010\010", n);
	i=n;
	n *=28;
	if ((h=_SysMalloc(n))== NULL) {
		return 0;
	}
	*(u_long *)0xa0000124=n;
	*(u_long *)0xa0000120=h;
	for (p=h ; p < (EvCB *)(h + n) ; p++) {
		*p=0;
	}
	return n;
}


/* sys_a0_9f 0xbfc06680 */
SetMem(size)
{
	int *mem,new;
	mem=*ram_size;
	new=*mem & 0xfffff8ff;
	switch(size) {
	case 2:
		*mem=new;
		*(long *)(0x00000060)=size;
		printf("Change effective memory : %d MBytes\n",size);
		break;
	case 8:
		*mem=new | 0x300;
		*(long *)(0x00000060)=size;
		printf("Change effective memory : %d MBytes\n",size);
	default:
		printf("Effective memory must be 2/8 MBytes\n");
		break;
	}
}


/* 0xbfc06700 */
do_a_long_jump(int value) {
	longjmp(&context,value);
}


/* sys_a0_9d 0xbfc06728 */
GetConf(long *Event,long *TCB,long *Stack)
{
	*Stack=sys_conf.Stack;
	*Event=sys_conf.Event;
	*TCB=sys_conf.TCB;
}


/* sys_a0_9c 0xbfc06750 */
SetConf(long Event,long TCB,long Stack)
{
	sys_conf.Stack=Stack;
	sys_conf.Event=Event;
	sys_conf.TCB=TCB;
	_kernel_setup();
}

/* 0xbfc06784 */
part3() {
	char s1[80],s2[80];

	strcpy(s2,"cdrom:");
	strcat(s2,"SYSTEM.CNF;1");
	strcpy(s1,"cdrom:");
	strcat(s1,"PSX.EXE;1");
	bootstrap(s2,s1);
}

/* 0xbfc067e8 */
bootstrap(char *config,char *executable)
{
	int fd,readen;

	__trace(1);
	setSTATUS(getSTATUS() & 0xfffffbfe);
	zero_stuff();
	__trace(2);
	SysInitKMem();
	__trace(3);
	InstallInterruptTables();
	InstallInterruptDispatch();
	PatchA0Table();
	InstallExceptionHandler();
	ResetEntryInt();
	__trace(4);
	zero_stuff();
	IntrMask=0;
	*i_reg=0;
	InstallDevices(_0xa000b9b0);
	__trace(5);
	printf("\nPS-X Realtime Kernel Ver.2.5\n \ Copyright 1993,1994 (C) Sony Computer Entertainment Inc. \n");
	__trace(6);
	zero_stuff();
	memcpy(&sys_conf , &default_sys_conf , 12);
	printf("KERNEL SETUP!\n");
	SysInitMemory(0xa000e000 , 0x2000);
	InitIntr(4);
	InitException(0);
	InitDefInt(3);
	InitEvent(sys_conf.Event);
	InitSchr(1,sys_conf.TCB);
	InitRCnt(1);
	zero_stuff();
	if (setjmp(&context))
		sys_err(0x0385);
	__trace(7);
	ExecuteShell();
	/* this is where the country lockout is located */
	__trace(8);
	IntrMask=0;
	*i_reg=0;
	_96_init();
	if (setjmp(&context))
		sys_err(0x0399);
	if (CheckForDevice1()== 1)
		InitDevice1();
	printf("\nBOOTSTRAP LOADER Type C Ver 2.1 03-JUL-1994\n\ Copyright 1993,1994 (C) Sony Computer Entertainment Inc.\n");
	if (setjmp(&context))
		sys_err(0x0386);
	__trace(9);
	if (setjmp(&context))
		sys_err(0x387);
	if ((fd=open(config , 1))>= 0) {
		printf("setup file    : %s\n",config);
		if (setjmp(&context))
			sys_err(0x038f);
		if ( (readen = read(fd , Buffer , 2048)) == 0) {
			memcpy(&sys_conf , &default_sys_conf , 12);
			strcpy(BootFile , executable);
		} else {
		    	Buffer[readen] = (char) 0;
			close(fd);
			if (setjmp(&context))
				sys_err(0x0390);
			ParseSetupFile(Buffer , sys_conf , BootFile);
		}
	} else {
		if (setjmp(&context))
			sys_err(0x0391);
		_0x00000180 = (char)0;
		memcpy(&sys_conf , &default_sys_conf , 12);
		strcpy(BootFile , executable);
	}
	if (setjmp(&context))
		sys_err(0x0388);
	kernel_setup();
	printf("boot file     : %s\n",BootFile);
	if (setjmp(&context))
		sys_err(0x0389);
	ClearRam();
	if (Load(BootFile,ExeHeader))
		sys_err(0x038a);
	printf("EXEC:PC0(%08x)  T_ADDR(%08x)  T_SIZE(%08x)\n",
		ExeHeader.pc0 , ExeHeader.t_addr , ExeHeader.t_size);
	printf("boot address  : %08x %08x\nExecute !\n\n",
		ExeHeader.pc0 , sys_conf.Stack);
	_0xa000b890 = sys_conf.Stack;
	_0xa000b894 = 0;
	printf("                S_ADDR(%08x)  S_SIZE(%08x)\n",
		sys_conf.Stack , 0);
	EnterCriticalSection();
	if (setjmp(&context))
		sys_err(0x038b);
	doExec(ExeHeader,1,0);
	printf("End of Main\n");
	sys_err(0x038c);
}

/* 0xbfc06f28 */
kernel_setup()
{
	printf("KERNEL SETUP!\n");
	SysInitMemory(0xa000e000 , 0x2000);
	InitIntr(4);
	InitException(0);
	InitDefInt(3);
	InitEvent(sys_conf.Event);
	InitSchr(1,sys_conf.TCB);
	InitRCnt(1);
	_96_init();
}

part2()
{
	__trace(0x0f);
	zero_stuff();
	if (CheckForDevice2() == 1)
		InitDevice2();
	__trace(0x0e);
	_0xa000b9b0 = 0;
	part3();
}

/* 0xbfc06ff0 */
ExecuteShell(long arg)
{
	memcpy(0x80030000 , 0xbfc18000 , 0x67ff0);
	FlushCache();
	(char *)(0x80030000)(arg);
}

/* 0xbfc0703c */
CheckForDevice2()
{
	char *src,*dst;

	src = License;
	dst = 0x1f000084;
	
	if (License[0]) {
		while (src) {
			if (*src++ != *dst++) break;
		}
	}
	if (*src)
		return 0;
	else
		return 1;
}

/* 0xbfc070ac */
CheckForDevice1()
{
	char *src,*dst;

	src = License;
	dst = 0x1f000004;
	
	if (License[0]) {
		while (src) {
			if (*src++ != *dst++) break;
		}
	}
	if (*src)
		return 0;
	else
		return 1;
}

/* 0xbfc0711c */
InitDevice2()
{
	(char *)(0x1f000080)();
}

0xbfc07148() /* */
{
	printf("PIO SHELL for PlayStation(tm)\n");
	printf("%s\n",0x1f000004);
	(char *)(0x1f000000)();
}

/* 0xbfc071a0 */
OpenCDEvents()
{
	sys_a0_a2();
	event_cd_ack   = OpenEvent(0xf0000003 , 0x10 , 0x2000 , 0);
	event_cd_comp  = OpenEvent(0xf0000003 , 0x20 , 0x2000 , 0);
	event_cd_dr    = OpenEvent(0xf0000003 , 0x40 , 0x2000 , 0);
	event_cd_de    = OpenEvent(0xf0000003 , 0x80 , 0x2000 , 0);
	event_cd_error = OpenEvent(0xf0000003 , 0x8000 , 0x2000 , 0);
	EnableEvent(event_cd_ack);
	EnableEvent(event_cd_comp);
	EnableEvent(event_cd_dr);
	EnableEvent(event_cd_de);
	EnableEvent(event_cd_error);
	ExitCriticalSection();
	_0xa0009d80 = (char)0;
}

0xbfc07330()
{
	OpenCDEvents();
	while (!sys_a0_95()) ;
}


/* 0xbfc073a0 */
_96_init()
{
	long tmp;

	0xbfc07330();
	tmp = 0;
	while (tmp <50000) {	/* this loop might be stupid, but its */
		tmp=tmp;	/* what it does */
		tmp++;
	}
	dev_cd_initfs();
}

/* 0xbfc0c720 */
Test6ButtonPad()
{
	/* Would u believe me if i told u that */
	u_long tmp; /* this and a lot more routines are */
	u_long paddata; /* NEVER called ? */
	printf("test 6 button PAD (NEW)\n");
	if (!PAD_init(0x20000001 , &paddata)) return;
	while (1) {
		tmp=0x000186a0;
		for (i=0 ; i < tmp; i++ ) {
			i=i; /* burn some cycles */
		}
		tmp=PAD_dr();
		EnterCriticalSection();
		printf("ret:%08x stock:",tmp); printf(" %08x",paddata);
		printf("\n");
		ExitCriticalSection();
	}
}

/* 0xbfc0d4cc */
EnableIORedirection()
{
	AddDrv(&tty_redir);	/* 0xbfc0e59c */
}

/* 0xbfc0d570 */
doExec(struct EXEC *exeheader,long arg1,long arg2)
{
	ExitCriticalSection();
	if (_0xa000dffc) {
		/* THIS LOOP IS COPYPROTECTION RELATED */
		if (0xbfc0d72c() < 0) {
			/* This routine and the next one, */
			SystemError(0x44 , 0x038b);
		}
		/* does some kind of black or country */
		if (0xbfc0d7bc() < 0) {
			/* check. These are used in the shell */
			SystemError(0x44 , 0x038b);
		} /* also, if they fail, byebye */
	} /* Noping them here or in the shell, wont solve a thing */
	EnterCriticalSection();		/* I hope to have the time someday */
	Exec(exeheader , arg1 , arg2);	/* to write about protections, since */
	/* i know some stuff about. I guided a friend with a DDX, in tracing */
	/* executing some code. He came to the conclusion that these routines */
	/* return different values whether a CD is audio, psx original, psx */
	/* foreign, or gold/HK. Hope to write about it soon */
}

/* 0xbfc0d600 */
0xbfc0d600()
{
	*reg0=1;
	*reg3=0x1f;
	for (i=0 ; i !=4 ; i++) {
		*(long *)(0)=i & 3;
	}
}

/* 0xbfc0d640 */
0xbfc0d640()
{
	*reg0=1;
	*reg2=0x1f;
}

/* 0xbfc0d664 */
0xbfc0d664()
{
	*reg0=1;
	*reg2=0x18;
}

/* 0xbfc0d72c */
0xbfc0d72c()
{
	0xbfc0d600();
	0xbfc0d664();
	*reg0=0;
	*reg1=0x1e;
	tmp=0xbfc0d688();
	if (tmp < 0) {
		0xbfc0d640();
		return 1;
	}
	if (tmp & 0x1d) {
		0xbfc0d640();
		return 1;
	}
	0xbfc0d600();
	0xbfc0d640();
	return 0;
}

/* 0xbfc0d850 */
ClearRam() {
	u_long *p,top;

	top=_stk_ | 0xa0000000;
	for (p=0xa0010000 ; p !=top ; p++) {
		*p=0;
	}
}

static char License[]="Licensed by Sony Computer Entertainment Inc." ;
static struct default_sys_conf={ 4, 16, 0x801fff00 }; /* 0xbfc0e14c */
static struct EXEC ExeHeader; /* 0xa000b870 */
static char *BootFile; /* 0xa000b8b0 */
static struct sys_conf { /* 0xa000b940 */
	u_long TCB;
	u_long Event;
	u_long Stack;
};
static jmp_buf context; /* 0xa000b980 */

A lot more soon, got a bus to catch, i need the week-end break...


si17911@ci.uminho.pt
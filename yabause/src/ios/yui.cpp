
extern "C"{
#include "config.h"
#include "yabause.h"
#include "scsp.h"
#include "vidsoft.h"
#include "vidogl.h"
#include "peripheral.h"
#include "m68kcore.h"
#include "sh2core.h"
#include "sh2int.h"
#include "cdbase.h"
#include "cs2.h"
#include "debug.h"
#include "osdcore_ios.h"
#include "sndal.h"
}

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

#define YUI_LOG printf
#define MAKE_PAD(a,b) ((a<<24)|(b))

// Setting Infomation From
static char mpegpath[256] = "\0";
static char cartpath[256] = "\0";
static char screenShotFilename[256] = "\0";
const char * s_biospath = NULL;
const char * s_cdpath = NULL;
const char * s_buppath = NULL;
const char * s_cartpath = NULL;
int s_carttype;
char s_savepath[256] ="\0";
int s_vidcoretype = VIDCORE_OGL;
int s_player2Enable = -1;
int g_EnagleFPS = 1;
int g_CpuType = 2;
yabause_filter_type g_VideoFilter = FILTER_NONE;
VideoInterface_struct *VIDCoreList[] = {
    &VIDDummy,
    &VIDSoft,
    &VIDOGL,
    NULL
};

M68K_struct * M68KCoreList[] = {
    &M68KDummy,
#ifdef HAVE_C68K
    &M68KC68K,
#endif
#ifdef HAVE_Q68
    &M68KQ68,
#endif
    NULL
};

SH2Interface_struct *SH2CoreList[] = {  
    &SH2Interpreter,
    &SH2DebugInterpreter,
#ifdef SH2_DYNAREC
    &SH2Dynarec,
#endif
    NULL
};

PerInterface_struct *PERCoreList[] = {
    &PERDummy,
    NULL
};

CDInterface *CDCoreList[] = {
    &DummyCD,
    &ISOCD,
    NULL
};

SoundInterface_struct *SNDCoreList[] = {
    &SNDDummy,
    &SNDAL,
    NULL
};


extern "C" {
    
    const char * GetBiosPath();
    const char * GetGamePath();
    const char * GetMemoryPath();
    int GetCartridgeType();
    int GetVideoInterface();
    const char * GetCartridgePath();
    int GetPlayer2Device();
    
int swapAglBuffer ();
    
int start_emulation( int width, int height ){
	int i;
    int res;
    yabauseinit_struct yinit;
    void * padbits;

    s_biospath = GetBiosPath();
    s_cdpath = GetGamePath();
    s_buppath = GetMemoryPath();
    s_cartpath = GetCartridgePath();
    s_vidcoretype = GetVideoInterface();
    s_carttype =  GetCartridgeType();
    //s_player2Enable = GetPlayer2Device();

    YUI_LOG("%s",glGetString(GL_VENDOR));
    YUI_LOG("%s",glGetString(GL_RENDERER));
    YUI_LOG("%s",glGetString(GL_VERSION));
    YUI_LOG("%s",glGetString(GL_EXTENSIONS));
    //YUI_LOG("%s",eglQueryString(g_Display,EGL_EXTENSIONS));

 
    glViewport(0,0,width,height);

    glClearColor( 0.0f, 0.0f,0.0f,1.0f);
    glClear( GL_COLOR_BUFFER_BIT );

    yinit.m68kcoretype = M68KCORE_C68K;
    yinit.percoretype = PERCORE_DUMMY;
    yinit.sh2coretype = SH2CORE_DEFAULT;
    yinit.vidcoretype = VIDCORE_OGL;
    yinit.sndcoretype = SNDCORE_AL; //SNDCORE_DEFAULT;
    yinit.cdcoretype = CDCORE_ISO;
    yinit.carttype = 0;//GetCartridgeType();
    yinit.regionid = 0;

    yinit.biospath = s_biospath;
    yinit.cdpath = s_cdpath;
    yinit.buppath = s_buppath;
    yinit.carttype = s_carttype;
    yinit.cartpath = s_cartpath;
    
    printf("bios %s¥n",s_biospath);

    yinit.mpegpath = mpegpath;
    yinit.videoformattype = VIDEOFORMATTYPE_NTSC;
    yinit.frameskip = 0;
    yinit.usethreads = 0;
    yinit.skip_load = 0;
    yinit.video_filter_type = g_VideoFilter;
    s_vidcoretype = VIDCORE_OGL;
     
    res = YabauseInit(&yinit);
    if (res != 0) {
      YUI_LOG("Fail to YabauseInit %d", res);
      return -1;
    }

    PerPortReset();
    padbits = PerPadAdd(&PORTDATA1);
    PerSetKey(MAKE_PAD(0,PERPAD_UP), PERPAD_UP, padbits);
    PerSetKey(MAKE_PAD(0,PERPAD_RIGHT), PERPAD_RIGHT, padbits);
    PerSetKey(MAKE_PAD(0,PERPAD_DOWN), PERPAD_DOWN, padbits);
    PerSetKey(MAKE_PAD(0,PERPAD_LEFT), PERPAD_LEFT, padbits);
    PerSetKey(MAKE_PAD(0,PERPAD_START), PERPAD_START, padbits);
    PerSetKey(MAKE_PAD(0,PERPAD_A), PERPAD_A, padbits);
    PerSetKey(MAKE_PAD(0,PERPAD_B), PERPAD_B, padbits);
    PerSetKey(MAKE_PAD(0,PERPAD_C), PERPAD_C, padbits);
    PerSetKey(MAKE_PAD(0,PERPAD_X), PERPAD_X, padbits);
    PerSetKey(MAKE_PAD(0,PERPAD_Y), PERPAD_Y, padbits);
    PerSetKey(MAKE_PAD(0,PERPAD_Z), PERPAD_Z, padbits);
    PerSetKey(MAKE_PAD(0,PERPAD_RIGHT_TRIGGER),PERPAD_RIGHT_TRIGGER,padbits);
    PerSetKey(MAKE_PAD(0,PERPAD_LEFT_TRIGGER),PERPAD_LEFT_TRIGGER,padbits);
	
	if( s_player2Enable != -1 ) {
		padbits = PerPadAdd(&PORTDATA2);
		PerSetKey(MAKE_PAD(1,PERPAD_UP), PERPAD_UP, padbits);
		PerSetKey(MAKE_PAD(1,PERPAD_RIGHT), PERPAD_RIGHT, padbits);
		PerSetKey(MAKE_PAD(1,PERPAD_DOWN), PERPAD_DOWN, padbits);
		PerSetKey(MAKE_PAD(1,PERPAD_LEFT), PERPAD_LEFT, padbits);
		PerSetKey(MAKE_PAD(1,PERPAD_START), PERPAD_START, padbits);
		PerSetKey(MAKE_PAD(1,PERPAD_A), PERPAD_A, padbits);
		PerSetKey(MAKE_PAD(1,PERPAD_B), PERPAD_B, padbits);
		PerSetKey(MAKE_PAD(1,PERPAD_C), PERPAD_C, padbits);
		PerSetKey(MAKE_PAD(1,PERPAD_X), PERPAD_X, padbits);
		PerSetKey(MAKE_PAD(1,PERPAD_Y), PERPAD_Y, padbits);
		PerSetKey(MAKE_PAD(1,PERPAD_Z), PERPAD_Z, padbits);
		PerSetKey(MAKE_PAD(1,PERPAD_RIGHT_TRIGGER),PERPAD_RIGHT_TRIGGER,padbits);
		PerSetKey(MAKE_PAD(1,PERPAD_LEFT_TRIGGER),PERPAD_LEFT_TRIGGER,padbits);
	}

    ScspSetFrameAccurate(1);
    ScspUnMuteAudio(SCSP_MUTE_SYSTEM);
    ScspSetVolume(100);

    OSDInit(0);
    OSDChangeCore(OSDCORE_NANOVG);
    
    if( s_vidcoretype == VIDCORE_OGL ){
        
	   for (i = 0; VIDCoreList[i] != NULL; i++)
	   {
		  if (VIDCoreList[i]->id == s_vidcoretype)
		  {
			 VIDCoreList[i]->Resize(width,height,0);
			 break;
		  }
	   }
    }else{
        //OSDChangeCore(OSDCORE_SOFT);
        //if( YuiInitProgramForSoftwareRendering() != GL_TRUE ){
        //    YUI_LOG("Fail to YuiInitProgramForSoftwareRendering");
        //    return -1;
        //}
    }

    return 0;
}
    void YuiErrorMsg(const char *string)
    {
        printf("%s",string);
    }
    void YuiSwapBuffers(void)
    {
        SetOSDToggle(g_EnagleFPS);
        OSDDisplayMessages(NULL,0,0);
        swapAglBuffer();
        
    }
    int YuiRevokeOGLOnThisThread(){
    }
    int YuiUseOGLOnThisThread(){
    }
    
    int emulation_step(){
        YabauseExec();
    }
    
}
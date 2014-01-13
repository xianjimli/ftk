#include "ftk_log.h"
#include "ftk_globals.h"
#include "ftk_wnd_manager.h"
#include "ftk_display_sigma.h"

#define ALLOW_OS_CODE 1
#include "dcc/include/dcc.h"
#include "common.h"
#include "osdlib.h"
#include "rmcore/include/rmcore.h"

#define DEB(f) (f)

#define DEFAULT_OSD_WIDTH 640
#define DEFAULT_OSD_HEIGHT 480
#define DEFAULT_OSD_BPP 32
#define DEFAULT_DRAM 0
#define DEFAULT_OSD_FORMAT OSD_FORMAT_TC
#define DEFAULT_OSD_SUBFORMAT OSD_SUBFORMAT_32BPP
#define DEFAULT_CHIP 0

/* Global */
static char create_osd = 0;
static char bg_R = 0xff, bg_G, bg_B, bg_A=0xff;
static char fg_R, fg_G, fg_B, fg_A;

static RMuint8 *osd_base_addr;          //Base address where OSD is mapped
static struct osd_descriptor p_osd;

static void init_tc(struct osd_descriptor *m_osd, RMuint8 *base_addr)
{
	RMuint32 bg_color = 0 , fg_color = 0;

	bg_color = 0;
	fg_color = 0;
	switch(m_osd->profile.ColorFormat){
		case EMhwlibColorFormat_24BPP_565 :
		case EMhwlibColorFormat_24BPP :
		case EMhwlibColorFormat_32BPP_4444:
		case EMhwlibColorFormat_32BPP :
			/* for 24BPP, alpha will not be taken */
			RMinsShiftBits(&bg_color, bg_A, 8, 24);
			RMinsShiftBits(&bg_color, bg_R, 8, 16);
			RMinsShiftBits(&bg_color, bg_G, 8,  8);
			RMinsShiftBits(&bg_color, bg_B, 8,  0);
			RMinsShiftBits(&fg_color, fg_A, 8, 24);
			RMinsShiftBits(&fg_color, fg_R, 8, 16);
			RMinsShiftBits(&fg_color, fg_G, 8,  8);
			RMinsShiftBits(&fg_color, fg_B, 8,  0);
			break;
		case EMhwlibColorFormat_16BPP_565:
			RMinsShiftBits(&bg_color, bg_R, 5, 11);
			RMinsShiftBits(&bg_color, bg_G, 6,  5);
			RMinsShiftBits(&bg_color, bg_B, 5,  0);
			RMinsShiftBits(&fg_color, fg_R, 5, 11);
			RMinsShiftBits(&fg_color, fg_G, 6,  5);
			RMinsShiftBits(&fg_color, fg_B, 5,  0);
			break;
		case EMhwlibColorFormat_16BPP_1555:
			RMinsShiftBits(&bg_color, bg_A, 1, 15);
			RMinsShiftBits(&bg_color, bg_R, 5, 10);
			RMinsShiftBits(&bg_color, bg_G, 5,  5);
			RMinsShiftBits(&bg_color, bg_B, 5,  0);
			RMinsShiftBits(&fg_color, fg_A, 1, 15);
			RMinsShiftBits(&fg_color, fg_R, 5, 10);
			RMinsShiftBits(&fg_color, fg_G, 5,  5);
			RMinsShiftBits(&fg_color, fg_B, 5,  0);
			break;
		case EMhwlibColorFormat_16BPP_4444:
			RMinsShiftBits(&bg_color, bg_A, 4, 12);
			RMinsShiftBits(&bg_color, bg_R, 4,  8);
			RMinsShiftBits(&bg_color, bg_G, 4,  4);
			RMinsShiftBits(&bg_color, bg_B, 4,  0);
			RMinsShiftBits(&fg_color, fg_A, 4, 12);
			RMinsShiftBits(&fg_color, fg_R, 4,  8);
			RMinsShiftBits(&fg_color, fg_G, 4,  4);
			RMinsShiftBits(&fg_color, fg_B, 4,  0);
			break;
	}

	DEB(fprintf(stderr,"bg color = 0x%08lx, fg color = 0x%08lx\n", bg_color, fg_color));
	{ 
		int x,y;
		int width = m_osd->profile.Width;
		int height = m_osd->profile.Height;

		RMuint32 *addr= (RMuint32 *)base_addr;
		for (x=0;x<(int)m_osd->profile.Width;x++)
		{
			for (y=0;y<(int)m_osd->profile.Height;y++)
			{
		        *addr = bg_color;
		        addr++;
			}
		}
	}
}

/** Cleanup
 */
static void sigma_deinit(void *param)
{
	RMstatus err;
	struct osd_descriptor *my_osd=(struct osd_descriptor *)param;
	
	DEB(fprintf(stderr,"begin sigma_deinit\n"));
	
	if (my_osd == NULL)
		return;
	
	if(my_osd->dcc_info.pRUA) {
		(void)RUAUnMap(my_osd->dcc_info.pRUA, osd_base_addr, p_osd.LumaSize);
		(void)RUAUnLock(my_osd->dcc_info.pRUA, p_osd.LumaAddr, p_osd.LumaSize);
	}
	
	if (my_osd->dcc_info.pDCC){
		err = DCCClose(my_osd->dcc_info.pDCC);
		if (RMFAILED(err))
			fprintf(stderr, "Cannot close DCC %d\n", err); 
	}

	if (my_osd->dcc_info.pRUA){
		err = RUADestroyInstance(my_osd->dcc_info.pRUA);
		if (RMFAILED(err))
			fprintf(stderr, "Cannot destroy RUA instance %d\n", err);
	}
        DEB(fprintf(stderr,"end sigma_deinit\n"));
}

static Ret sigma_init(int argc, char* argv[])
{
	RMstatus status;
	struct display_context disp_info;
	p_osd.dcc_info.disp_info = &disp_info;

	RMSignalInit(sigma_deinit, &p_osd);
	
	p_osd.chip_num = 0;
	status = RUACreateInstance(&(p_osd.dcc_info.pRUA),p_osd.chip_num);
	if(RMFAILED(status)) {
		printf("Error creating intance : %d\n",status);
		goto wayout;
	}

	DEB(fprintf(stderr,"  DCCOpen\n"));
	status = DCCOpen(p_osd.dcc_info.pRUA, &(p_osd.dcc_info.pDCC));
	if(RMFAILED(status)) {
		printf("Error opening DCC : %d\n",status);
		goto wayout;
	}
	
	status = get_osd_infos(&p_osd); 
	if (RMFAILED(status)) {
		fprintf(stderr,"Error getting OSD infos : %s\n", RMstatusToString(status)); 
		goto wayout;
	}

	if ( p_osd.LumaAddr == 0 || p_osd.LumaSize == 0){
		fprintf(stderr,"No OSD buffer currently opened, you should create one with osdbuf_control.\n");
		goto wayout;
	}

	DEB(fprintf(stderr,"Locking %d bytes of RUA memory ...\n", (int)p_osd.LumaSize));
	status = RUALock(p_osd.dcc_info.pRUA, p_osd.LumaAddr, p_osd.LumaSize);
	if (RMFAILED(status)){
		fprintf(stderr,"Error locking OSD buffer at 0x%08lx (%ld bytes): %s\n",
		        p_osd.LumaAddr, p_osd.LumaSize, RMstatusToString(status));
		goto wayout;
	}

	osd_base_addr = RUAMap(p_osd.dcc_info.pRUA, p_osd.LumaAddr, p_osd.LumaSize);
	if (osd_base_addr == NULL){
		status= RM_ERROR;
		fprintf(stderr,"Error mapping OSD buffer.\n");
		goto wayout;
	}

	if (p_osd.profile.ColorMode <= EMhwlibColorMode_LUT_8BPP)
	{
		assert("not supported");
	} else {
		DEB(fprintf(stderr,"test_display_tc\n"));
		init_tc(&p_osd, osd_base_addr);
	}

	DEB(fprintf(stderr, "osd_base_addr=%p width=%d height=%d bpp=%d\n", 
		osd_base_addr,
		(int)p_osd.profile.Width, (int)p_osd.profile.Height, (int)p_osd.bpp));
	ftk_set_display(ftk_display_sigma_create(osd_base_addr, 
		p_osd.profile.Width, p_osd.profile.Height, p_osd.bpp, &p_osd));
	
	DEB(fprintf(stderr,"ftk_set_display %p\n", ftk_default_display()));
	return RET_OK;

wayout:
	sigma_deinit(&p_osd);
	return RET_OK;
}

Ret ftk_backend_init(int argc, char* argv[])
{
	sigma_init(argc, argv);
	fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);
	if(ftk_default_display() == NULL)
	{
		ftk_loge("open display failed.\n");
		sigma_deinit(&p_osd);
		exit(0);
	}
	fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);

	return RET_OK;
}


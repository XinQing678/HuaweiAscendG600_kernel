/* Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio_event.h>
#include <linux/memblock.h>
#include <asm/mach-types.h>
#include <linux/memblock.h>
#include <asm/mach/arch.h>
#include <asm/hardware/gic.h>
#include <mach/board.h>
#include <mach/msm_iomap.h>
#include <mach/msm_hsusb.h>
#include <mach/rpc_hsusb.h>
#include <mach/rpc_pmapp.h>
#include <mach/usbdiag.h>
#include <mach/msm_memtypes.h>
#include <mach/msm_serial_hs.h>
#include <linux/usb/android.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <mach/vreg.h>
#include <mach/pmic.h>
#include <mach/socinfo.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <asm/mach/mmc.h>
#include <linux/i2c.h>
#include <linux/i2c/sx150x.h>
#include <linux/gpio.h>
#include <linux/android_pmem.h>
#include <linux/bootmem.h>
#include <linux/mfd/marimba.h>
#include <mach/vreg.h>
#include <linux/power_supply.h>
#include <linux/regulator/consumer.h>
#include <mach/rpc_pmapp.h>
#include <mach/msm_battery.h>
#include <linux/smsc911x.h>
#include <linux/atmel_maxtouch.h>
/* update Qcomm original  base line , delete 1 line for fmem disable and avoid deadlock*/
#include <linux/msm_adc.h>
#include "devices.h"
#include "timer.h"
#include "board-msm7x27a-regulator.h"
#include "devices-msm7x2xa.h"
#include "pm.h"
#include <mach/rpc_server_handset.h>
#include <mach/socinfo.h>
#include "pm-boot.h"
#include "board-msm7627a.h"
#ifdef CONFIG_HUAWEI_MTK6252_MODEM
#include <mach/msm_smsm.h>
#include <linux/mtk6252_dev.h>
#endif

#ifdef CONFIG_HUAWEI_KERNEL
#include <linux/hardware_self_adapt.h>
/*added for virtualkeys*/
static char buf_virtualkey[500];
static ssize_t  buf_vkey_size=0;
#endif

#ifdef CONFIG_HUAWEI_KERNEL
#include <asm-arm/huawei/smem_vendor_huawei.h>
#include <asm-arm/huawei/usb_switch_huawei.h>
#endif

#define PMEM_KERNEL_EBI1_SIZE	0x3A000
#define MSM_PMEM_AUDIO_SIZE	0x1F4000

#if defined(CONFIG_GPIO_SX150X)
enum {
	SX150X_CORE,
};

/* -------------------- huawei devices -------------------- */
/* add leds,button-backlight,pmic-leds device */
#ifdef CONFIG_HUAWEI_KERNEL
static struct platform_device rgb_leds_device = {
    .name   = "rgb-leds",
    .id     = 0,
};

static struct platform_device keyboard_backlight_device = {
    .name       = "button-backlight",
    .id     = 1,
}; 

static struct platform_device msm_device_pmic_leds = {
    .name   = "pmic-leds",
    .id = -1,
};
#endif

/* driver for hw device detect */
#ifdef CONFIG_HUAWEI_HW_DEV_DCT
static struct platform_device huawei_device_detect = {
	.name = "hw-dev-detect",
	.id   =-1,
};
#endif
#ifdef CONFIG_HUAWEI_KERNEL
static struct resource hw_extern_sdcard_resources[] = {
    {
        .flags  = IORESOURCE_MEM,
    },
};

/* 
 * Define the 'hw_extern_sdcard' device node for MMI sdcard test to  
 * judge if the sd card inserted.
 */
static struct platform_device hw_extern_sdcard_device = {
    .name           = "hw_extern_sdcard",
    .id             = -1,
    .num_resources  = ARRAY_SIZE(hw_extern_sdcard_resources),
    .resource       = hw_extern_sdcard_resources,
};

static struct resource hw_extern_sdcardMounted_resources[] = {
    {
        .flags  = IORESOURCE_MEM,
    },
};

/* 
 * Define the 'hw_extern_sdcardMounted' device node for MMI sdcard test to  
 * judge if the sd card mounted.
 */
static struct platform_device hw_extern_sdcardMounted_device = {
    .name           = "hw_extern_sdcardMounted",
    .id             = -1,
    .num_resources  = ARRAY_SIZE(hw_extern_sdcardMounted_resources),
    .resource       = hw_extern_sdcardMounted_resources,
};

/* 
 * Add the device nodes 'hw_extern_sdcard' and 'hw_extern_sdcardMounted' in /dev. 
 * It is used by MMI sdcard test.
 */
int __init hw_extern_sdcard_add_device(void)
{
    platform_device_register(&hw_extern_sdcard_device);
    platform_device_register(&hw_extern_sdcardMounted_device);
    return 0;
}
#endif

static struct sx150x_platform_data sx150x_data[] __initdata = {
	[SX150X_CORE]	= {
		.gpio_base		= GPIO_CORE_EXPANDER_BASE,
		.oscio_is_gpo		= false,
		.io_pullup_ena		= 0,
		.io_pulldn_ena		= 0x02,
		.io_open_drain_ena	= 0xfef8,
		.irq_summary		= -1,
	},
};
#endif


static struct platform_device msm_wlan_ar6000_pm_device = {
	.name           = "wlan_ar6000_pm_dev",
	.id             = -1,
};

#if defined(CONFIG_I2C) && defined(CONFIG_GPIO_SX150X)
static struct i2c_board_info core_exp_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("sx1509q", 0x3e),
	},
};

static void __init register_i2c_devices(void)
{
	if (machine_is_msm7x27a_surf() || machine_is_msm7625a_surf() ||
			machine_is_msm8625_surf())
		sx150x_data[SX150X_CORE].io_open_drain_ena = 0xe0f0;

	core_exp_i2c_info[0].platform_data =
			&sx150x_data[SX150X_CORE];

	i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID,
				core_exp_i2c_info,
				ARRAY_SIZE(core_exp_i2c_info));
}
#endif

static struct msm_gpio qup_i2c_gpios_io[] = {
	{ GPIO_CFG(60, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_scl" },
	{ GPIO_CFG(61, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_sda" },
	{ GPIO_CFG(131, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_scl" },
	{ GPIO_CFG(132, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_sda" },
};

static struct msm_gpio qup_i2c_gpios_hw[] = {
	{ GPIO_CFG(60, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_scl" },
	{ GPIO_CFG(61, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_sda" },
	{ GPIO_CFG(131, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_scl" },
	{ GPIO_CFG(132, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_sda" },
};

static void gsbi_qup_i2c_gpio_config(int adap_id, int config_type)
{
	int rc;

	if (adap_id < 0 || adap_id > 1)
		return;

	/* Each adapter gets 2 lines from the table */
	if (config_type)
		rc = msm_gpios_request_enable(&qup_i2c_gpios_hw[adap_id*2], 2);
	else
		rc = msm_gpios_request_enable(&qup_i2c_gpios_io[adap_id*2], 2);
	if (rc < 0)
		pr_err("QUP GPIO request/enable failed: %d\n", rc);
}

static struct msm_i2c_platform_data msm_gsbi0_qup_i2c_pdata = {
	.clk_freq		= 100000,
	.msm_i2c_config_gpio	= gsbi_qup_i2c_gpio_config,
};

static struct msm_i2c_platform_data msm_gsbi1_qup_i2c_pdata = {
	.clk_freq		= 100000,
	.msm_i2c_config_gpio	= gsbi_qup_i2c_gpio_config,
};

#ifdef CONFIG_ARCH_MSM7X27A
#define MSM_PMEM_MDP_SIZE       0x2300000
#define MSM7x25A_MSM_PMEM_MDP_SIZE       0x1500000

#define MSM_PMEM_ADSP_SIZE      0x1200000
#define MSM_PMEM_ADSP_BIG_SIZE      0x1E00000
#define MSM7x25A_MSM_PMEM_ADSP_SIZE      0xB91000
/*   enlarge the pmem space for HDR on 8950s
 */
static unsigned int get_pmem_adsp_size(void)
{
	if( machine_is_msm8x25_C8950D()
	|| machine_is_msm8x25_U8950D()
	||machine_is_msm8x25_U8950()){
			return MSM_PMEM_ADSP_BIG_SIZE;		
		}
	else
		return MSM_PMEM_ADSP_SIZE;

}
#endif

static struct android_usb_platform_data android_usb_pdata = {
	.update_pid_and_serial_num = usb_diag_update_pid_and_serial_num,
};

static struct platform_device android_usb_device = {
	.name	= "android_usb",
	.id	= -1,
	.dev	= {
		.platform_data = &android_usb_pdata,
	},
};

#ifdef CONFIG_USB_EHCI_MSM_72K
static void msm_hsusb_vbus_power(unsigned phy_info, int on)
{
	int rc = 0;
	unsigned gpio;

	gpio = GPIO_HOST_VBUS_EN;

	rc = gpio_request(gpio, "i2c_host_vbus_en");
	if (rc < 0) {
		pr_err("failed to request %d GPIO\n", gpio);
		return;
	}
	gpio_direction_output(gpio, !!on);
	gpio_set_value_cansleep(gpio, !!on);
	gpio_free(gpio);
}

static struct msm_usb_host_platform_data msm_usb_host_pdata = {
	.phy_info       = (USB_PHY_INTEGRATED | USB_PHY_MODEL_45NM),
};

static void __init msm7x2x_init_host(void)
{
	msm_add_host(0, &msm_usb_host_pdata);
}
#endif

#ifdef CONFIG_USB_MSM_OTG_72K
static int hsusb_rpc_connect(int connect)
{
	if (connect)
		return msm_hsusb_rpc_connect();
	else
		return msm_hsusb_rpc_close();
}

static struct regulator *reg_hsusb;
static int msm_hsusb_ldo_init(int init)
{
	int rc = 0;

	if (init) {
		reg_hsusb = regulator_get(NULL, "usb");
		if (IS_ERR(reg_hsusb)) {
			rc = PTR_ERR(reg_hsusb);
			pr_err("%s: could not get regulator: %d\n",
					__func__, rc);
			goto out;
		}

		rc = regulator_set_voltage(reg_hsusb, 3300000, 3300000);
		if (rc) {
			pr_err("%s: could not set voltage: %d\n",
					__func__, rc);
			goto reg_free;
		}

		return 0;
	}
	/* else fall through */
reg_free:
	regulator_put(reg_hsusb);
out:
	reg_hsusb = NULL;
	return rc;
}

static int msm_hsusb_ldo_enable(int enable)
{
	static int ldo_status;

	if (IS_ERR_OR_NULL(reg_hsusb))
		return reg_hsusb ? PTR_ERR(reg_hsusb) : -ENODEV;

	if (ldo_status == enable)
		return 0;

	ldo_status = enable;

	return enable ?
		regulator_enable(reg_hsusb) :
		regulator_disable(reg_hsusb);
}

#ifndef CONFIG_USB_EHCI_MSM_72K
static int msm_hsusb_pmic_notif_init(void (*callback)(int online), int init)
{
	int ret = 0;

	if (init)
		ret = msm_pm_app_rpc_init(callback);
	else
		msm_pm_app_rpc_deinit(callback);

	return ret;
}
#endif

static struct msm_otg_platform_data msm_otg_pdata = {
#ifndef CONFIG_USB_EHCI_MSM_72K
	.pmic_vbus_notif_init	 = msm_hsusb_pmic_notif_init,
#else
	.vbus_power		 = msm_hsusb_vbus_power,
#endif
	.rpc_connect		 = hsusb_rpc_connect,
	.pemp_level		 = PRE_EMPHASIS_WITH_20_PERCENT,
	.cdr_autoreset		 = CDR_AUTO_RESET_DISABLE,
	.drv_ampl		 = HS_DRV_AMPLITUDE_DEFAULT,
	.se1_gating		 = SE1_GATING_DISABLE,
	.ldo_init		 = msm_hsusb_ldo_init,
	.ldo_enable		 = msm_hsusb_ldo_enable,
	.chg_init		 = hsusb_chg_init,
	.chg_connected		 = hsusb_chg_connected,
	.chg_vbus_draw		 = hsusb_chg_vbus_draw,
};
#endif

static struct msm_hsusb_gadget_platform_data msm_gadget_pdata = {
	.is_phy_status_timer_on = 1,
};

static struct resource smc91x_resources[] = {
	[0] = {
		.start = 0x90000300,
		.end   = 0x900003ff,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = MSM_GPIO_TO_INT(4),
		.end   = MSM_GPIO_TO_INT(4),
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device smc91x_device = {
	.name           = "smc91x",
	.id             = 0,
	.num_resources  = ARRAY_SIZE(smc91x_resources),
	.resource       = smc91x_resources,
};

/* back to qcomm orignal 1025 baseline */
static struct msm_serial_hs_platform_data msm_uart_dm1_pdata = {
	.inject_rx_on_wakeup	= 1,
	.rx_to_inject		= 0xFD,
};


static struct msm_pm_platform_data msm7x27a_pm_data[MSM_PM_SLEEP_MODE_NR] = {
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 1,
					.suspend_enabled = 1,
					.latency = 16000,
					.residency = 20000,
	},
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 1,
					.suspend_enabled = 1,
					.latency = 12000,
					.residency = 20000,
	},
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 0,
					.suspend_enabled = 1,
					.latency = 2000,
					.residency = 0,
	},
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 1,
					.suspend_enabled = 1,
					.latency = 2,
					.residency = 0,
	},
};

static struct msm_pm_boot_platform_data msm_pm_boot_pdata __initdata = {
	.mode = MSM_PM_BOOT_CONFIG_RESET_VECTOR_PHYS,
	.p_addr = 0,
};

/* 8625 PM platform data */
static struct msm_pm_platform_data msm8625_pm_data[MSM_PM_SLEEP_MODE_NR * 2] = {
	/* CORE0 entries */
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 0,
					.suspend_enabled = 0,
					.latency = 16000,
					.residency = 20000,
	},

	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 0,
					.suspend_enabled = 0,
					.latency = 12000,
					.residency = 20000,
	},

	/* picked latency & redisdency values from 7x30 */
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 0,
					.suspend_enabled = 0,
					.latency = 500,
					.residency = 6000,
	},

	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 1,
					.suspend_enabled = 1,
					.latency = 2,
					.residency = 10,
	},

	/* picked latency & redisdency values from 7x30 */
	[MSM_PM_MODE(1, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 0,
					.suspend_enabled = 0,
					.latency = 500,
					.residency = 6000,
	},

	[MSM_PM_MODE(1, MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 1,
					.suspend_enabled = 1,
					.latency = 2,
					.residency = 10,
	},

};

static struct msm_pm_boot_platform_data msm_pm_8625_boot_pdata __initdata = {
	.mode = MSM_PM_BOOT_CONFIG_REMAP_BOOT_ADDR,
	.v_addr = MSM_CFG_CTL_BASE,
};

static struct android_pmem_platform_data android_pmem_adsp_pdata = {
	.name = "pmem_adsp",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 1,
	.memory_type = MEMTYPE_EBI1,
	/*  update Qcomm original  base line , delete 3 lines for fmem disable and avoid deadlock*/
};

static struct platform_device android_pmem_adsp_device = {
	.name = "android_pmem",
	.id = 1,
	.dev = { .platform_data = &android_pmem_adsp_pdata },
};

static unsigned pmem_mdp_size = MSM_PMEM_MDP_SIZE;
static int __init pmem_mdp_size_setup(char *p)
{
	pmem_mdp_size = memparse(p, NULL);
	return 0;
}

early_param("pmem_mdp_size", pmem_mdp_size_setup);

static unsigned pmem_adsp_size = MSM_PMEM_ADSP_SIZE;
static int __init pmem_adsp_size_setup(char *p)
{
	pmem_adsp_size = memparse(p, NULL);
	return 0;
}

early_param("pmem_adsp_size", pmem_adsp_size_setup);

#define SND(desc, num) { .name = #desc, .id = num }
static struct snd_endpoint snd_endpoints_list[] = {
	SND(HANDSET, 0),
	SND(MONO_HEADSET, 2),
	SND(HEADSET, 3),
	SND(SPEAKER, 6),
	SND(TTY_HEADSET, 8),
	SND(TTY_VCO, 9),
	SND(TTY_HCO, 10),
	SND(BT, 12),
	SND(IN_S_SADC_OUT_HANDSET, 16),
	SND(IN_S_SADC_OUT_SPEAKER_PHONE, 25),
	SND(FM_DIGITAL_STEREO_HEADSET, 26),
	SND(FM_DIGITAL_SPEAKER_PHONE, 27),
	SND(FM_DIGITAL_BT_A2DP_HEADSET, 28),
	SND(STEREO_HEADSET_AND_SPEAKER, 31),
	SND(CURRENT, 0x7FFFFFFE),
	/* add new device for FM AUX_PGA path */
	SND(FM_RADIO_STEREO_HEADSET, 29),
	SND(FM_RADIO_SPEAKER_PHONE, 30),	
	SND(FM_ANALOG_STEREO_HEADSET, 35),
	SND(FM_ANALOG_STEREO_HEADSET_CODEC, 36),
	/* add new device for 2nd mic MMI test*/
	SND(HANDSET_2NDMIC, 37),
	/* add hac device for handset and handset_dualmic*/
	SND(HANDSET_HAC, 38),
	SND(IN_S_SADC_OUT_HANDSET_HAC, 39),
	SND(STEREO_SPEAKER, 40),
	SND(FM_RADIO_STEREO_SPEAKER_PHONE, 41),
	SND(STEREO_HEADSET_AND_STEREO_SPEAKER,42),
	SND(MTK_HANDSET, 43),
	SND(MTK_HEADSET, 44),
	SND(MTK_SPEAKER, 45),
	SND(MTK_BT, 46),	
    SND(MMI_BTLOOP_MTK, 47),
	SND(MMI_BTLOOP_QCOM, 48),
	SND(MTK_HANDSET_MUSIC, 49),
	SND(MTK_HEADSET_MUSIC, 50),
	SND(MTK_SPEAKER_MUSIC, 51),
};
#undef SND

static struct msm_snd_endpoints msm_device_snd_endpoints = {
	.endpoints = snd_endpoints_list,
	.num = sizeof(snd_endpoints_list) / sizeof(struct snd_endpoint)
};

static struct platform_device msm_device_snd = {
	.name = "msm_snd",
	.id = -1,
	.dev    = {
		.platform_data = &msm_device_snd_endpoints
	},
};

#define DEC0_FORMAT ((1<<MSM_ADSP_CODEC_MP3)| \
	(1<<MSM_ADSP_CODEC_AAC)|(1<<MSM_ADSP_CODEC_WMA)| \
	(1<<MSM_ADSP_CODEC_WMAPRO)|(1<<MSM_ADSP_CODEC_AMRWB)| \
	(1<<MSM_ADSP_CODEC_AMRNB)|(1<<MSM_ADSP_CODEC_WAV)| \
	(1<<MSM_ADSP_CODEC_ADPCM)|(1<<MSM_ADSP_CODEC_YADPCM)| \
	(1<<MSM_ADSP_CODEC_EVRC)|(1<<MSM_ADSP_CODEC_QCELP))
#define DEC1_FORMAT ((1<<MSM_ADSP_CODEC_MP3)| \
	(1<<MSM_ADSP_CODEC_AAC)|(1<<MSM_ADSP_CODEC_WMA)| \
	(1<<MSM_ADSP_CODEC_WMAPRO)|(1<<MSM_ADSP_CODEC_AMRWB)| \
	(1<<MSM_ADSP_CODEC_AMRNB)|(1<<MSM_ADSP_CODEC_WAV)| \
	(1<<MSM_ADSP_CODEC_ADPCM)|(1<<MSM_ADSP_CODEC_YADPCM)| \
	(1<<MSM_ADSP_CODEC_EVRC)|(1<<MSM_ADSP_CODEC_QCELP))
#define DEC2_FORMAT ((1<<MSM_ADSP_CODEC_MP3)| \
	(1<<MSM_ADSP_CODEC_AAC)|(1<<MSM_ADSP_CODEC_WMA)| \
	(1<<MSM_ADSP_CODEC_WMAPRO)|(1<<MSM_ADSP_CODEC_AMRWB)| \
	(1<<MSM_ADSP_CODEC_AMRNB)|(1<<MSM_ADSP_CODEC_WAV)| \
	(1<<MSM_ADSP_CODEC_ADPCM)|(1<<MSM_ADSP_CODEC_YADPCM)| \
	(1<<MSM_ADSP_CODEC_EVRC)|(1<<MSM_ADSP_CODEC_QCELP))
#define DEC3_FORMAT ((1<<MSM_ADSP_CODEC_MP3)| \
	(1<<MSM_ADSP_CODEC_AAC)|(1<<MSM_ADSP_CODEC_WMA)| \
	(1<<MSM_ADSP_CODEC_WMAPRO)|(1<<MSM_ADSP_CODEC_AMRWB)| \
	(1<<MSM_ADSP_CODEC_AMRNB)|(1<<MSM_ADSP_CODEC_WAV)| \
	(1<<MSM_ADSP_CODEC_ADPCM)|(1<<MSM_ADSP_CODEC_YADPCM)| \
	(1<<MSM_ADSP_CODEC_EVRC)|(1<<MSM_ADSP_CODEC_QCELP))
#define DEC4_FORMAT (1<<MSM_ADSP_CODEC_MIDI)

static unsigned int dec_concurrency_table[] = {
	/* Audio LP */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DMA)), 0,
	0, 0, 0,

	/* Concurrency 1 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	 /* Concurrency 2 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 3 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 4 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 5 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 6 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|
			(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	0, 0, 0, 0,

	/* Concurrency 7 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),
};

#define DEC_INFO(name, queueid, decid, nr_codec) { .module_name = name, \
	.module_queueid = queueid, .module_decid = decid, \
	.nr_codec_support = nr_codec}

static struct msm_adspdec_info dec_info_list[] = {
	DEC_INFO("AUDPLAY0TASK", 13, 0, 11), /* AudPlay0BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY1TASK", 14, 1, 11),  /* AudPlay1BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY2TASK", 15, 2, 11),  /* AudPlay2BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY3TASK", 16, 3, 11),  /* AudPlay3BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY4TASK", 17, 4, 1),  /* AudPlay4BitStreamCtrlQueue */
};

static struct msm_adspdec_database msm_device_adspdec_database = {
	.num_dec = ARRAY_SIZE(dec_info_list),
	.num_concurrency_support = (ARRAY_SIZE(dec_concurrency_table) / \
					ARRAY_SIZE(dec_info_list)),
	.dec_concurrency_table = dec_concurrency_table,
	.dec_info_list = dec_info_list,
};

static struct platform_device msm_device_adspdec = {
	.name = "msm_adspdec",
	.id = -1,
	.dev    = {
		.platform_data = &msm_device_adspdec_database
	},
};

static struct android_pmem_platform_data android_pmem_audio_pdata = {
	.name = "pmem_audio",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
	.memory_type = MEMTYPE_EBI1,
};

static struct platform_device android_pmem_audio_device = {
	.name = "android_pmem",
	.id = 2,
	.dev = { .platform_data = &android_pmem_audio_pdata },
};

static struct android_pmem_platform_data android_pmem_pdata = {
	.name = "pmem",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 1,
	.memory_type = MEMTYPE_EBI1,
};
static struct platform_device android_pmem_device = {
	.name = "android_pmem",
	.id = 0,
	.dev = { .platform_data = &android_pmem_pdata },
};

static u32 msm_calculate_batt_capacity(u32 current_voltage);

static struct msm_psy_batt_pdata msm_psy_batt_data = {
	.voltage_min_design     = 3200,
	.voltage_max_design     = 4200,
	.voltage_fail_safe      = 3340,
	.avail_chg_sources      = AC_CHG | USB_CHG ,
	.batt_technology        = POWER_SUPPLY_TECHNOLOGY_LION,
	.calculate_capacity     = &msm_calculate_batt_capacity,
};

static u32 msm_calculate_batt_capacity(u32 current_voltage)
{
	u32 low_voltage	 = msm_psy_batt_data.voltage_min_design;
	u32 high_voltage = msm_psy_batt_data.voltage_max_design;

	if (current_voltage <= low_voltage)
		return 0;
	else if (current_voltage >= high_voltage)
		return 100;
	else
		return (current_voltage - low_voltage) * 100
			/ (high_voltage - low_voltage);
}

static struct platform_device msm_batt_device = {
	.name               = "msm-battery",
	.id                 = -1,
	.dev.platform_data  = &msm_psy_batt_data,
};

static struct smsc911x_platform_config smsc911x_config = {
	.irq_polarity	= SMSC911X_IRQ_POLARITY_ACTIVE_HIGH,
	.irq_type	= SMSC911X_IRQ_TYPE_PUSH_PULL,
	.flags		= SMSC911X_USE_16BIT,
};

static struct resource smsc911x_resources[] = {
	[0] = {
		.start	= 0x90000000,
		.end	= 0x90007fff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= MSM_GPIO_TO_INT(48),
		.end	= MSM_GPIO_TO_INT(48),
		.flags	= IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL,
	},
};

static struct platform_device smsc911x_device = {
	.name		= "smsc911x",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(smsc911x_resources),
	.resource	= smsc911x_resources,
	.dev		= {
		.platform_data	= &smsc911x_config,
	},
};

static char *msm_adc_surf_device_names[] = {
	"XO_ADC",
};

static struct msm_adc_platform_data msm_adc_pdata = {
	.dev_names = msm_adc_surf_device_names,
	.num_adc = ARRAY_SIZE(msm_adc_surf_device_names),
	.target_hw = MSM_8x25,
};

static struct platform_device msm_adc_device = {
	.name   = "msm_adc",
	.id = -1,
	.dev = {
		.platform_data = &msm_adc_pdata,
	},
};

#if defined(CONFIG_SERIAL_MSM_HSL_CONSOLE) \
		&& defined(CONFIG_MSM_SHARED_GPIO_FOR_UART2DM)
static struct msm_gpio uart2dm_gpios[] = {
	{GPIO_CFG(19, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
							"uart2dm_rfr_n" },
	{GPIO_CFG(20, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
							"uart2dm_cts_n" },
	{GPIO_CFG(21, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
							"uart2dm_rx"    },
	{GPIO_CFG(108, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
							"uart2dm_tx"    },
};

static void msm7x27a_cfg_uart2dm_serial(void)
{
	int ret;
	ret = msm_gpios_request_enable(uart2dm_gpios,
					ARRAY_SIZE(uart2dm_gpios));
	if (ret)
		pr_err("%s: unable to enable gpios for uart2dm\n", __func__);
}
#else
static void msm7x27a_cfg_uart2dm_serial(void) { }
#endif

/*  update Qcomm original  base line , delete 6 lines for fmem disable and avoid deadlock*/


/* delete uart1 serial port */
#ifdef CONFIG_HUAWEI_KERNEL
static struct platform_device *rumi_sim_devices[] __initdata = {
	&msm_device_dmov,
	&msm_device_smd,
	&smc91x_device,
	&msm_device_nand,
	&msm_device_uart_dm1,
	&msm_gsbi0_qup_i2c_device,
	&msm_gsbi1_qup_i2c_device,
};
#else
static struct platform_device *rumi_sim_devices[] __initdata = {
	&msm_device_dmov,
	&msm_device_smd,
	&smc91x_device,
	&msm_device_uart1,
	&msm_device_nand,
	&msm_device_uart_dm1,
	&msm_gsbi0_qup_i2c_device,
	&msm_gsbi1_qup_i2c_device,
};
#endif

static struct platform_device *msm8625_rumi3_devices[] __initdata = {
	&msm8625_device_dmov,
	&msm8625_device_smd,
	&msm8625_device_uart1,
	&msm8625_gsbi0_qup_i2c_device,
};

static struct platform_device *msm7627a_surf_ffa_devices[] __initdata = {
	&msm_device_dmov,
	&msm_device_smd,
	&msm_device_uart1,
	&msm_device_uart_dm1,
	&msm_device_uart_dm2,
	&msm_gsbi0_qup_i2c_device,
	&msm_gsbi1_qup_i2c_device,
	&msm_device_otg,
	&msm_device_gadget_peripheral,
	&smsc911x_device,
	&msm_kgsl_3d0,
};

static struct platform_device *common_devices[] __initdata = {
	&android_usb_device,
	&android_pmem_device,
	&android_pmem_adsp_device,
	&android_pmem_audio_device,
    /*  update Qcomm original  base line , delete 1 line for fmem disable and avoid deadlock*/
	&msm_device_nand,
	&msm_device_snd,
	&msm_device_adspdec,
	&asoc_msm_pcm,
	&asoc_msm_dai0,
	&asoc_msm_dai1,
	&msm_batt_device,
	
/* delete all bt devices */
#ifdef CONFIG_HUAWEI_KERNEL
	
	/* Registration device */
	&rgb_leds_device,
	&keyboard_backlight_device,
	&msm_device_pmic_leds,
    /* Registration device */
#ifdef CONFIG_HUAWEI_HW_DEV_DCT
	&huawei_device_detect,
#endif
#endif
	&msm_adc_device,
};

static struct platform_device *msm8625_surf_devices[] __initdata = {
	&msm8625_device_dmov,
	&msm8625_device_uart1,
	&msm8625_device_uart_dm1,
	&msm8625_device_uart_dm2,
	&msm8625_gsbi0_qup_i2c_device,
	&msm8625_gsbi1_qup_i2c_device,
	&msm8625_device_smd,
	&msm8625_device_otg,
	&msm8625_device_gadget_peripheral,
	&msm8625_kgsl_3d0,
};

static unsigned pmem_kernel_ebi1_size = PMEM_KERNEL_EBI1_SIZE;
static int __init pmem_kernel_ebi1_size_setup(char *p)
{
	pmem_kernel_ebi1_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_kernel_ebi1_size", pmem_kernel_ebi1_size_setup);

static unsigned pmem_audio_size = MSM_PMEM_AUDIO_SIZE;
static int __init pmem_audio_size_setup(char *p)
{
	pmem_audio_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_audio_size", pmem_audio_size_setup);

static struct memtype_reserve msm7x27a_reserve_table[] __initdata = {
	[MEMTYPE_SMI] = {
	},
	[MEMTYPE_EBI0] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
	[MEMTYPE_EBI1] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
};

/*  update Qcomm original  base line , delete 7 lines for fmem disable and avoid deadlock*/



static void __init size_pmem_devices(void)
{
#ifdef CONFIG_ANDROID_PMEM

    /*  update Qcomm original  base line , delete 2 lines for fmem disable and avoid deadlock*/

	if (machine_is_msm7625a_surf() || machine_is_msm7625a_ffa()) {
		pmem_mdp_size = MSM7x25A_MSM_PMEM_MDP_SIZE;
		pmem_adsp_size = MSM7x25A_MSM_PMEM_ADSP_SIZE;
	} else {
		pmem_mdp_size = MSM_PMEM_MDP_SIZE;
		pmem_adsp_size = get_pmem_adsp_size();
		printk("pmem_adsp_size=%08x\n",pmem_adsp_size);
		
	}

	android_pmem_adsp_pdata.size = pmem_adsp_size;
	android_pmem_pdata.size = pmem_mdp_size;
	android_pmem_audio_pdata.size = pmem_audio_size;

    /*  update Qcomm original  base line , delete 19 lines for fmem disable and avoid deadlock*/


#endif

}

static void __init reserve_memory_for(struct android_pmem_platform_data *p)
{
	msm7x27a_reserve_table[p->memory_type].size += p->size;
}

static void __init reserve_pmem_memory(void)
{
#ifdef CONFIG_ANDROID_PMEM


    /*  update Qcomm original  base line , delete 3 lines and add 3 lines for fmem disable and avoid deadlock*/	
	reserve_memory_for(&android_pmem_adsp_pdata);
	reserve_memory_for(&android_pmem_pdata);
	reserve_memory_for(&android_pmem_audio_pdata);


	msm7x27a_reserve_table[MEMTYPE_EBI1].size += pmem_kernel_ebi1_size;
#endif
}

static void __init msm7x27a_calculate_reserve_sizes(void)
{
	size_pmem_devices();
	reserve_pmem_memory();
}

static int msm7x27a_paddr_to_memtype(unsigned int paddr)
{
	return MEMTYPE_EBI1;
}

static struct reserve_info msm7x27a_reserve_info __initdata = {
	.memtype_reserve_table = msm7x27a_reserve_table,
	.calculate_reserve_sizes = msm7x27a_calculate_reserve_sizes,
	.paddr_to_memtype = msm7x27a_paddr_to_memtype,
};

static void __init msm7x27a_reserve(void)
{
	reserve_info = &msm7x27a_reserve_info;
	msm_reserve();
}


/* define size of reserve memory is 1M */
#ifdef CONFIG_SRECORDER_MSM
#define SRECORDER_RESERVED_MEM_SIZE (SZ_1M)

/* define start address of reserve memory */
static unsigned long s_srecorder_reserved_mem_phys_start_addr = 0x0;

/*
 * Function:       unsigned long get_srecorder_reserved_mem_size(void)
 * Description:    get size of reserve memory
 * Calls:          No
 * Called By:      setup_arch
 * Table Accessed: No
 * Table Updated:  No
 * Input:          No
 * Output:         No
 * Return:         SRECORDER_RESERVED_MEM_SIZE: size of reserve memory
 * Others:         No
 */
unsigned long get_srecorder_reserved_mem_size(void)
{
    return SRECORDER_RESERVED_MEM_SIZE;
}

/*
 * Function:       unsigned long get_mempools_pstart_addr(void)
 * Description:    get start address of reserve memory
 * Calls:          No
 * Called By:      msm8625_reserve
 * Table Accessed: No
 * Table Updated:  No
 * Input:          No
 * Output:         No
 * Return:         s_srecorder_reserved_mem_phys_start_addr:start address of reserve memory
 * Others:         No
 */
unsigned long get_srecorder_reserved_mem_phys_start_addr(void)
{
    return s_srecorder_reserved_mem_phys_start_addr;
}

extern unsigned long get_mempools_pstart_addr(void);
#endif /* CONFIG_SRECORDER_MSM */

static void __init msm8625_reserve(void)
{
	msm7x27a_reserve();

#ifdef CONFIG_SRECORDER_MSM
    if (0x0 != get_mempools_pstart_addr())
    {
        s_srecorder_reserved_mem_phys_start_addr = get_mempools_pstart_addr() - SRECORDER_RESERVED_MEM_SIZE;
        if (0 == memblock_remove(s_srecorder_reserved_mem_phys_start_addr, SRECORDER_RESERVED_MEM_SIZE))
        {
            printk("<<<< Allocate 0x%08lx bytes starting form 0x%08lx for S-Recorder\n", (unsigned long)SRECORDER_RESERVED_MEM_SIZE, 
                s_srecorder_reserved_mem_phys_start_addr);
        }
        else
        {
            s_srecorder_reserved_mem_phys_start_addr = 0x0;
            printk(">>>> No sufficient mem for S-Recorder!\n");
        }
    }
    else
    {
        printk(">>>> Can't know the start address for S-Recorder's reserved memory!\n");
    }
#endif /* CONFIG_SRECORDER_MSM */

	memblock_remove(MSM8625_SECONDARY_PHYS, SZ_8);
	memblock_remove(MSM8625_WARM_BOOT_PHYS, SZ_32);
}

static void __init msm7x27a_device_i2c_init(void)
{
	msm_gsbi0_qup_i2c_device.dev.platform_data = &msm_gsbi0_qup_i2c_pdata;
	msm_gsbi1_qup_i2c_device.dev.platform_data = &msm_gsbi1_qup_i2c_pdata;
}

static void __init msm8625_device_i2c_init(void)
{
	msm8625_gsbi0_qup_i2c_device.dev.platform_data =
		&msm_gsbi0_qup_i2c_pdata;
	msm8625_gsbi1_qup_i2c_device.dev.platform_data =
		&msm_gsbi1_qup_i2c_pdata;
}

#define MSM_EBI2_PHYS			0xa0d00000
#define MSM_EBI2_XMEM_CS2_CFG1		0xa0d10030

static void __init msm7x27a_init_ebi2(void)
{
	uint32_t ebi2_cfg;
	void __iomem *ebi2_cfg_ptr;

	ebi2_cfg_ptr = ioremap_nocache(MSM_EBI2_PHYS, sizeof(uint32_t));
	if (!ebi2_cfg_ptr)
		return;

	ebi2_cfg = readl(ebi2_cfg_ptr);
	if (machine_is_msm7x27a_rumi3() || machine_is_msm7x27a_surf() ||
		machine_is_msm7625a_surf() || machine_is_msm8625_surf())
		ebi2_cfg |= (1 << 4); /* CS2 */

	writel(ebi2_cfg, ebi2_cfg_ptr);
	iounmap(ebi2_cfg_ptr);

	/* Enable A/D MUX[bit 31] from EBI2_XMEM_CS2_CFG1 */
	ebi2_cfg_ptr = ioremap_nocache(MSM_EBI2_XMEM_CS2_CFG1,
							 sizeof(uint32_t));
	if (!ebi2_cfg_ptr)
		return;

	ebi2_cfg = readl(ebi2_cfg_ptr);
	if (machine_is_msm7x27a_surf() || machine_is_msm7625a_surf())
		ebi2_cfg |= (1 << 31);

	writel(ebi2_cfg, ebi2_cfg_ptr);
	iounmap(ebi2_cfg_ptr);
}

static struct platform_device msm_proccomm_regulator_dev = {
	.name   = PROCCOMM_REGULATOR_DEV_NAME,
	.id     = -1,
	.dev    = {
		.platform_data = &msm7x27a_proccomm_regulator_data
	}
};

static void msm_adsp_add_pdev(void)
{
	int rc = 0;
	struct rpc_board_dev *rpc_adsp_pdev;

	rpc_adsp_pdev = kzalloc(sizeof(struct rpc_board_dev), GFP_KERNEL);
	if (rpc_adsp_pdev == NULL) {
		pr_err("%s: Memory Allocation failure\n", __func__);
		return;
	}
	rpc_adsp_pdev->prog = ADSP_RPC_PROG;

	if (cpu_is_msm8625())
		rpc_adsp_pdev->pdev = msm8625_device_adsp;
	else
		rpc_adsp_pdev->pdev = msm_adsp_device;
	rc = msm_rpc_add_board_dev(rpc_adsp_pdev, 1);
	if (rc < 0) {
		pr_err("%s: return val: %d\n",	__func__, rc);
		kfree(rpc_adsp_pdev);
	}
}

static void __init msm7627a_rumi3_init(void)
{
	msm7x27a_init_ebi2();
	platform_add_devices(rumi_sim_devices,
			ARRAY_SIZE(rumi_sim_devices));
}

static void __init msm8625_rumi3_init(void)
{
	msm7x2x_misc_init();
	msm_adsp_add_pdev();
	msm8625_device_i2c_init();
	platform_add_devices(msm8625_rumi3_devices,
			ARRAY_SIZE(msm8625_rumi3_devices));

	msm_pm_set_platform_data(msm8625_pm_data,
			 ARRAY_SIZE(msm8625_pm_data));
	BUG_ON(msm_pm_boot_init(&msm_pm_8625_boot_pdata));
	msm8x25_spm_device_init();
}

#define UART1DM_RX_GPIO		45

#if defined(CONFIG_BT) && defined(CONFIG_MARIMBA_CORE)
static int __init msm7x27a_init_ar6000pm(void)
{
	msm_wlan_ar6000_pm_device.dev.platform_data = &ar600x_wlan_power;
	return platform_device_register(&msm_wlan_ar6000_pm_device);
}
#else
static int __init msm7x27a_init_ar6000pm(void) { return 0; }
#endif

static void __init msm7x27a_init_regulators(void)
{
	int rc = platform_device_register(&msm_proccomm_regulator_dev);
	if (rc)
		pr_err("%s: could not register regulator device: %d\n",
				__func__, rc);
}
/* add virtual keys fucntion */
/* modify virtualkey function name */
static ssize_t virtualkey_show(struct kobject *kobj,
			       struct kobj_attribute *attr, char *buf)
{
        memcpy( buf, buf_virtualkey, buf_vkey_size );
		return buf_vkey_size; 
}

static struct kobj_attribute synaptics_virtual_keys_attr = {
	.attr = {
		.name = "virtualkeys.synaptics",
		.mode = S_IRUGO,
	},
	.show = &virtualkey_show,
};

#ifdef CONFIG_HUAWEI_MELFAS_TOUCHSCREEN
/* add melfas virtual key node */
static struct kobj_attribute melfas_virtual_keys_attr = {
	.attr = {
		.name = "virtualkeys.melfas-touchscreen",
		.mode = S_IRUGO,
	},
	.show = &virtualkey_show,
};
#endif

static struct attribute *virtualkey_properties_attrs[] = {
	&synaptics_virtual_keys_attr.attr,
	#ifdef CONFIG_HUAWEI_MELFAS_TOUCHSCREEN
	&melfas_virtual_keys_attr.attr,
	#endif
	NULL
};

static struct attribute_group virtualkey_properties_attr_group = {
	.attrs = virtualkey_properties_attrs,
};

static void __init virtualkeys_init(void)
{
    struct kobject *properties_kobj;
    int ret;
    /*Modify the virtualkeys of touchsreen*/
    if(machine_is_msm7x27a_U8815()
        || machine_is_msm8x25_U8825()
        || machine_is_msm8x25_U8825D()
        || machine_is_msm8x25_C8825D()
        || machine_is_msm8x25_C8833D()
        || machine_is_msm8x25_U8833D()
        || machine_is_msm8x25_U8833()
		|| machine_is_msm7x27a_C8820()
        || machine_is_msm8x25_H881C()
        || machine_is_msm8x25_C8812P())
    {
    	buf_vkey_size = sprintf(buf_virtualkey,
        			__stringify(EV_KEY) ":" __stringify(KEY_BACK)  ":57:850:100:80"
        		   ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":240:850:100:80"
        		   ":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":423:850:100:80"
        		   "\n"); 
    }
    else if(machine_is_msm8x25_C8950D()
        || machine_is_msm8x25_U8950D()
        || machine_is_msm8x25_U8950())
    {
        buf_vkey_size = sprintf(buf_virtualkey,
        			__stringify(EV_KEY) ":" __stringify(KEY_BACK)  ":80:1035:160:80"
        		   ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":270:1035:160:80"
        		   ":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":460:1035:160:80"
        		   "\n"); 
    }
	/*New add FWVGA virtual keys */
    else if (machine_is_msm8x25_U8951D()
        || machine_is_msm8x25_U8951()
        || machine_is_msm8x25_C8951())
	/* modify the area of virtualkeys */
    {
    	buf_vkey_size = sprintf(buf_virtualkey,
        			__stringify(EV_KEY) ":" __stringify(KEY_BACK)  ":71:900:142:80"
        		   ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":240:900:142:80"
        		   ":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":409:900:142:80"
        		   "\n"); 
    }
    else if (machine_is_msm7x27a_H867G()
           ||machine_is_msm7x27a_H868C()
           )
    {
	    /* 3 key configuration for 3.5" TP */
        buf_vkey_size = sprintf(buf_virtualkey,
                  __stringify(EV_KEY) ":" __stringify(KEY_BACK)  ":30:510:60:50"
                  ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":165:510:100:50"
                  ":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":290:510:60:50"
                  "\n"); 
    }
    else if (machine_is_msm7x27a_U8655_EMMC())
    {
    	/*4 virtual keys for att */
    	if (HW_VER_SUB_VE <= get_hw_sub_board_id())
    	{
    	    buf_vkey_size = sprintf(buf_virtualkey,
        		          __stringify(EV_KEY) ":" __stringify(KEY_MENU)  ":30:510:60:50"
        		          ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":115:510:80:50"
        		          ":" __stringify(EV_KEY) ":" __stringify(KEY_BACK) ":205:510:80:50"
        		          ":" __stringify(EV_KEY) ":" __stringify(KEY_SEARCH) ":290:510:60:50"
        		          "\n"); 
    	}
    	else
    	{
            buf_vkey_size = sprintf(buf_virtualkey,
        		          __stringify(EV_KEY) ":" __stringify(KEY_MENU)  ":30:510:60:50"
        		          ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":165:510:100:50"
        		          ":" __stringify(EV_KEY) ":" __stringify(KEY_BACK) ":290:510:60:50"
        		          "\n"); 
    	}
    }
    else
    {
    	buf_vkey_size = sprintf(buf_virtualkey,
        			__stringify(EV_KEY) ":" __stringify(KEY_MENU)  ":57:850:100:80"
        		   ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":240:850:100:80"
        		   ":" __stringify(EV_KEY) ":" __stringify(KEY_BACK) ":423:850:100:80"
        		   "\n"); 
    }

    properties_kobj = kobject_create_and_add("board_properties", NULL);
	if (properties_kobj)
		ret = sysfs_create_group(properties_kobj,
					 &virtualkey_properties_attr_group);
	if (!properties_kobj || ret)
		pr_err("failed to create board_properties\n");
}

static void __init msm7x27a_add_footswitch_devices(void)
{
	platform_add_devices(msm_footswitch_devices,
			msm_num_footswitch_devices);
}

static void __init msm7x27a_add_platform_devices(void)
{
#ifdef CONFIG_HUAWEI_KERNEL
    if (machine_is_msm8625_surf() || machine_is_msm8625_ffa()
        || cpu_is_msm8625())
#else
    if (machine_is_msm8625_surf() || machine_is_msm8625_ffa())
#endif
    {
		platform_add_devices(msm8625_surf_devices,
			ARRAY_SIZE(msm8625_surf_devices));
	} else {
		platform_add_devices(msm7627a_surf_ffa_devices,
			ARRAY_SIZE(msm7627a_surf_ffa_devices));
	}

	platform_add_devices(common_devices,
			ARRAY_SIZE(common_devices));
}

static void __init msm7x27a_uartdm_config(void)
{
	msm7x27a_cfg_uart2dm_serial();

    /* set RX intterupt for WCN2243 */
    if(BT_WCN2243 == get_hw_bt_device_model()) 
    {
        msm_uart_dm1_pdata.wakeup_irq = gpio_to_irq(UART1DM_RX_GPIO);
        if (cpu_is_msm8625())
                msm8625_device_uart_dm1.dev.platform_data =
                        &msm_uart_dm1_pdata;
        else
                msm_device_uart_dm1.dev.platform_data = &msm_uart_dm1_pdata;
    }
}

static void __init msm7x27a_otg_gadget(void)
{
	if (cpu_is_msm8625()) {
		msm_otg_pdata.swfi_latency =
		msm8625_pm_data[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT].latency;
		msm8625_device_otg.dev.platform_data = &msm_otg_pdata;
		msm8625_device_gadget_peripheral.dev.platform_data =
			&msm_gadget_pdata;
	} else {
		msm_otg_pdata.swfi_latency =
		msm7x27a_pm_data[
		MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].latency;
		msm_device_otg.dev.platform_data = &msm_otg_pdata;
		msm_device_gadget_peripheral.dev.platform_data =
			&msm_gadget_pdata;
	}
}

static void __init msm7x27a_pm_init(void)
{
#ifdef CONFIG_HUAWEI_KERNEL
    if (machine_is_msm8625_surf() || machine_is_msm8625_ffa()
        || cpu_is_msm8625())
#else
    if (machine_is_msm8625_surf() || machine_is_msm8625_ffa())
#endif
    {
		msm_pm_set_platform_data(msm8625_pm_data,
				ARRAY_SIZE(msm8625_pm_data));
		BUG_ON(msm_pm_boot_init(&msm_pm_8625_boot_pdata));
		msm8x25_spm_device_init();
	} else {
		msm_pm_set_platform_data(msm7x27a_pm_data,
				ARRAY_SIZE(msm7x27a_pm_data));
		BUG_ON(msm_pm_boot_init(&msm_pm_boot_pdata));
	}

	msm_pm_register_irqs();
}


static void __init msm7x2x_init(void)
{
	msm7x2x_misc_init();

	/* Initialize regulators first so that other devices can use them */
	msm7x27a_init_regulators();
	msm_adsp_add_pdev();
	if (cpu_is_msm8625())
		msm8625_device_i2c_init();
	else
		msm7x27a_device_i2c_init();
	msm7x27a_init_ebi2();
	msm7x27a_uartdm_config();

#ifdef CONFIG_HUAWEI_KERNEL
    import_kernel_cmdline();
#endif

	msm7x27a_otg_gadget();
#ifndef CONFIG_HUAWEI_CAMERA
    msm7x27a_cfg_smsc911x();
#endif
#if (defined(HUAWEI_BT_BTLA_VER30) && defined(CONFIG_HUAWEI_KERNEL))
    /*before bt probe, config the bt_wake_msm gpio*/
    bt_wake_msm_config();
#endif
	msm7x27a_add_footswitch_devices();
	msm7x27a_add_platform_devices();
	/* Ensure ar6000pm device is registered before MMC/SDC */
	msm7x27a_init_ar6000pm();
	msm7627a_init_mmc();
	msm_fb_add_devices();
	msm7x2x_init_host();
	msm7x27a_pm_init();
	register_i2c_devices();
	msm7627a_bt_power_init();
	msm7627a_camera_init();
	msm7627a_add_io_devices();
	/*7x25a kgsl initializations*/
	msm7x25a_kgsl_3d0_init();
	
#ifdef CONFIG_HUAWEI_FEATURE_OEMINFO
    rmt_oeminfo_add_device();
#endif

#ifdef CONFIG_HUAWEI_KERNEL
	virtualkeys_init();
#endif

#ifdef CONFIG_HUAWEI_KERNEL
    hw_extern_sdcard_add_device();
#endif
	/*8x25 kgsl initializations*/
	msm8x25_kgsl_3d0_init();

	
#ifdef CONFIG_HUAWEI_MTK6252_MODEM
	{
		unsigned smem_size;
		boot_reason = *(unsigned int *)
			(smem_get_entry(SMEM_POWER_ON_STATUS_INFO, &smem_size));
		printk(KERN_NOTICE "Boot Reason = 0x%02x\n", boot_reason);
		mtk6252_dev_init();
	}
#endif
}

static void __init msm7x2x_init_early(void)
{
	msm_msm7627a_allocate_memory_regions();
}

MACHINE_START(MSM7X27A_RUMI3, "QCT MSM7x27a RUMI3")
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm_common_io_init,
	.reserve	= msm7x27a_reserve,
	.init_irq	= msm_init_irq,
	.init_machine	= msm7627a_rumi3_init,
	.timer		= &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= vic_handle_irq,
MACHINE_END
MACHINE_START(MSM7X27A_SURF, "QCT MSM7x27a SURF")
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm_common_io_init,
	.reserve	= msm7x27a_reserve,
	.init_irq	= msm_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= vic_handle_irq,
MACHINE_END
MACHINE_START(MSM7X27A_FFA, "QCT MSM7x27a FFA")
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm_common_io_init,
	.reserve	= msm7x27a_reserve,
	.init_irq	= msm_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= vic_handle_irq,
MACHINE_END
MACHINE_START(MSM7625A_SURF, "QCT MSM7625a SURF")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm_common_io_init,
	.reserve        = msm7x27a_reserve,
	.init_irq       = msm_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= vic_handle_irq,
MACHINE_END
MACHINE_START(MSM7625A_FFA, "QCT MSM7625a FFA")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm_common_io_init,
	.reserve        = msm7x27a_reserve,
	.init_irq       = msm_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= vic_handle_irq,
MACHINE_END
MACHINE_START(MSM8625_RUMI3, "QCT MSM8625 RUMI3")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm8625_rumi3_init,
	.timer          = &msm_timer,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM8625_SURF, "QCT MSM8625 SURF")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM7X27A_U8655_EMMC, "MSM7x27a U8655Pro BOARD")
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm_common_io_init,
	.reserve	= msm7x27a_reserve,
	.init_irq	= msm_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= vic_handle_irq,
MACHINE_END
MACHINE_START(MSM7X27A_M660, "MSM7x27a M660 BOARD")
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm_common_io_init,
	.reserve	= msm7x27a_reserve,
	.init_irq	= msm_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= vic_handle_irq,
MACHINE_END

MACHINE_START(MSM7X27A_C8820, "MSM7x27a C8820 BOARD")
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm_common_io_init,
	.reserve	= msm7x27a_reserve,
	.init_irq	= msm_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= vic_handle_irq,
MACHINE_END

MACHINE_START(MSM7X27A_H867G, "MSM7x27a H867G BOARD")
    .boot_params    = PHYS_OFFSET + 0x100,
    .map_io         = msm_common_io_init,
    .reserve        = msm7x27a_reserve,
    .init_irq       = msm_init_irq,
    .init_machine   = msm7x2x_init,
    .timer          = &msm_timer,
    .init_early     = msm7x2x_init_early,
    .handle_irq     = vic_handle_irq,
MACHINE_END

MACHINE_START(MSM7X27A_U8815, "MSM7x27a U8815 BOARD")
    .boot_params    = PHYS_OFFSET + 0x100,
    .map_io         = msm_common_io_init,
    .reserve        = msm7x27a_reserve,
    .init_irq       = msm_init_irq,
    .init_machine   = msm7x2x_init,
    .timer          = &msm_timer,
    .init_early     = msm7x2x_init_early,
    .handle_irq     = vic_handle_irq,
MACHINE_END
MACHINE_START(MSM7X27A_H868C, "MSM7x27a H868C BOARD")
    .boot_params    = PHYS_OFFSET + 0x100,
    .map_io         = msm_common_io_init,
    .reserve        = msm7x27a_reserve,
    .init_irq       = msm_init_irq,
    .init_machine   = msm7x2x_init,
    .timer          = &msm_timer,
    .init_early     = msm7x2x_init_early,
    .handle_irq     = vic_handle_irq,
MACHINE_END
MACHINE_START(MSM8X25_U8825, "MSM8x25 U8825 BOARD")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM8X25_U8825D, "MSM8x25 U8825D BOARD")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM8X25_U8951D, "MSM8x25 U8951D BOARD")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM8X25_U8951, "MSM8x25 U8951 BOARD")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM8X25_C8825D, "MSM8x25 C8825D BOARD")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM8X25_U8950D, "MSM8x25 U8950D BOARD")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM8X25_C8950D, "MSM8x25 C8950D BOARD")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM8625_FFA, "QCT MSM8625 FFA")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM8X25_U8950, "MSM8x25 U8950D BOARD")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM8X25_C8812P, "MSM8x25 C8812P BOARD")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM8X25_C8833D, "MSM8x25 C8833D BOARD")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM8X25_U8833D, "MSM8x25 U8833D BOARD")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM8X25_U8833, "MSM8x25 U8833 BOARD")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM8X25_C8951, "MSM8x25 C8951 BOARD")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
MACHINE_START(MSM8X25_H881C, "MSM8x25 H881C BOARD")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm8625_map_io,
	.reserve        = msm8625_reserve,
	.init_irq       = msm8625_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END
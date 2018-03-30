/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2017 Tom Jones <tj@enoti.me>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

/*
 * Copyright (c) 2016 Mark Kettenis
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */


#define CHVGPIO_INTERRUPT_STATUS		0x0300
#define CHVGPIO_INTERRUPT_MASK			0x0380
#define CHVGPIO_PAD_CFG0			0x4400
#define CHVGPIO_PAD_CFG1			0x4404

#define CHVGPIO_PAD_CFG0_GPIORXSTATE		0x00000001
#define CHVGPIO_PAD_CFG0_GPIOTXSTATE		0x00000002
#define CHVGPIO_PAD_CFG0_INTSEL_MASK		0xf0000000
#define CHVGPIO_PAD_CFG0_INTSEL_SHIFT		28

#define CHVGPIO_PAD_CFG0_GPIOCFG_SHIFT		8
#define CHVGPIO_PAD_CFG0_GPIOCFG_MASK		(7 << CHVGPIO_PAD_CFG0_GPIOCFG_SHIFT)
#define CHVGPIO_PAD_CFG0_GPIOCFG_GPIO		0
#define CHVGPIO_PAD_CFG0_GPIOCFG_GPO		1
#define CHVGPIO_PAD_CFG0_GPIOCFG_GPI		2
#define CHVGPIO_PAD_CFG0_GPIOCFG_HIZ		3

#define CHVGPIO_PAD_CFG1_INTWAKECFG_MASK	0x00000007
#define CHVGPIO_PAD_CFG1_INTWAKECFG_FALLING	0x00000001
#define CHVGPIO_PAD_CFG1_INTWAKECFG_RISING	0x00000002
#define CHVGPIO_PAD_CFG1_INTWAKECFG_BOTH	0x00000003
#define CHVGPIO_PAD_CFG1_INTWAKECFG_LEVEL	0x00000004
#define CHVGPIO_PAD_CFG1_INVRXTX_MASK		0x000000f0
#define CHVGPIO_PAD_CFG1_INVRXTX_RXDATA		0x00000040

/*
 * The pads for the pins are arranged in groups of maximal 15 pins.
 * The arrays below give the number of pins per group, such that we
 * can validate the (untrusted) pin numbers from ACPI.
 */
#define	E_UID		3
#define	E_BANK_PREFIX	"eastbank"

const int chv_east_pins[] = {
	12, 12, -1
};

const char *chv_east_pin_names[] = {
		"PMU_SLP_S3_B",
		"PMU_BATLOW_B",
		"SUS_STAT_B",
		"PMU_SLP_S0IX_B",
		"PMU_AC_PRESENT",
		"PMU_PLTRST_B",
		"PMU_SUSCLK",
		"PMU_SLP_LAN_B",
		"PMU_PWRBTN_B",
		"PMU_SLP_S4_B",
		"PMU_WAKE_B",
		"PMU_WAKE_LAN_B"

		"MF_ISH_GPIO_3",
		"MF_ISH_GPIO_7",
		"MF_ISH_I2C1_SCL",
		"MF_ISH_GPIO_1",
		"MF_ISH_GPIO_5",
		"MF_ISH_GPIO_9",
		"MF_ISH_GPIO_0",
		"MF_ISH_GPIO_4",
		"MF_ISH_GPIO_8",
		"MF_ISH_GPIO_2",
		"MF_ISH_GPIO_6",
		"MF_ISH_I2C1_SDA"
};

#define	N_UID		2
#define	N_BANK_PREFIX	"northbank"

const int chv_north_pins[] = {
	9, 13, 12, 12, 13, -1
};

const char *chv_north_pin_names[] = {
	"GPIO_DFX0_PAD",
	"GPIO_DFX3_PAD",
	"GPIO_DFX7_PAD",
	"GPIO_DFX1_PAD",
	"GPIO_DFX5_PAD",
	"GPIO_DFX4_PAD",
	"GPIO_DFX8_PAD",
	"GPIO_DFX2_PAD",
	"GPIO_DFX6_PAD",

	"GPIO_SUS0_PAD",
	"SEC_GPIO_SUS10_PAD",
	"GPIO_SUS3_PAD",
	"GPIO_SUS7_PAD",
	"GPIO_SUS1_PAD",
	"GPIO_SUS5_PAD",
	"SEC_GPIO_SUS11_PAD",
	"GPIO_SUS4_PAD",
	"SEC_GPIO_SUS8_PAD",
	"GPIO_SUS2_PAD",
	"GPIO_SUS6_PAD",
	"CX_PREQ_B_PAD",
	"SEC_GPIO_SUS9_PAD",

	"TRST_B_PAD",
	"TCK_PAD",
	"PROCHOT_B_PAD",
	"SVID0_DATA_PAD",
	"TMS_PAD",
	"CX_PRDY_B_2_PAD",
	"TDO_2_PAD",
	"CX_PRDY_B_PAD",
	"SVID0_ALERT_B_PAD",
	"TDO_PAD",
	"SVID0_CLK_PAD",
	"TDI_PAD",

	"GP_CAMERASB05_PAD",
	"GP_CAMERASB02_PAD",
	"GP_CAMERASB08_PAD",
	"GP_CAMERASB00_PAD",
	"GP_CAMERASB06_PAD",
	"GP_CAMERASB10_PAD",
	"GP_CAMERASB03_PAD",
	"GP_CAMERASB09_PAD",
	"GP_CAMERASB01_PAD",
	"GP_CAMERASB07_PAD",
	"GP_CAMERASB11_PAD",
	"GP_CAMERASB04_PAD",

	"PANEL0_BKLTEN_PAD",
	"HV_DDI0_HPD_PAD",
	"HV_DDI2_DDC_SDA_PAD",
	"PANEL1_BKLTCTL_PAD",
	"HV_DDI1_HPD_PAD",
	"PANEL0_BKLTCTL_PAD",
	"HV_DDI0_DDC_SDA_PAD",
	"HV_DDI2_DDC_SCL_PAD",
	"HV_DDI2_HPD_PAD",
	"PANEL1_VDDEN_PAD",
	"PANEL1_BKLTEN_PAD",
	"HV_DDI0_DDC_SCL_PAD",
	"PANEL0_VDDEN_PAD",
};


#define	SE_UID		4
#define	SE_BANK_PREFIX	"southeastbank"

const int chv_southeast_pins[] = {
	8, 12, 6, 8, 10, 11, -1
};

const char *chv_southeast_pin_names[] = {
	"MF_PLT_CLK0_PAD",
	"PWM1_PAD",
	"MF_PLT_CLK1_PAD",
	"MF_PLT_CLK4_PAD",
	"MF_PLT_CLK3_PAD",
	"PWM0_PAD",
	"MF_PLT_CLK5_PAD",
	"MF_PLT_CLK2_PAD",

	"SDMMC2_D3_CD_B_PAD",
	"SDMMC1_CLK_PAD",
	"SDMMC1_D0_PAD",
	"SDMMC2_D1_PAD",
	"SDMMC2_CLK_PAD",
	"SDMMC1_D2_PAD",
	"SDMMC2_D2_PAD",
	"SDMMC2_CMD_PAD",
	"SDMMC1_CMD_PAD",
	"SDMMC1_D1_PAD",
	"SDMMC2_D0_PAD",
	"SDMMC1_D3_CD_B_PAD",

	"SDMMC3_D1_PAD",
	"SDMMC3_CLK_PAD",
	"SDMMC3_D3_PAD",
	"SDMMC3_D2_PAD",
	"SDMMC3_CMD_PAD",
	"SDMMC3_D0_PAD",

	"MF_LPC_AD2_PAD",
	"LPC_CLKRUNB_PAD",
	"MF_LPC_AD0_PAD",
	"LPC_FRAMEB_PAD",
	"MF_LPC_CLKOUT1_PAD",
	"MF_LPC_AD3_PAD",
	"MF_LPC_CLKOUT0_PAD",
	"MF_LPC_AD1_PAD",

	"SPI1_MISO_PAD",
	"SPI1_CS0_B_PAD",
	"SPI1_CLK_PAD",
	"MMC1_D6_PAD",
	"SPI1_MOSI_PAD",
	"MMC1_D5_PAD",
	"SPI1_CS1_B_PAD",
	"MMC1_D4_SD_WE_PAD",
	"MMC1_D7_PAD",
	"MMC1_RCLK_PAD",

	"USB_OC1_B_PAD",
	"PMU_RESETBUTTON_B_PAD",
	"GPIO_ALERT_PAD",
	"SDMMC3_PWR_EN_B_PAD",
	"ILB_SERIRQ_PAD",
	"USB_OC0_B_PAD",
	"SDMMC3_CD_B_PAD",
	"SPKR_PAD",
	"SUSPWRDNACK_PAD",
	"SPARE_PIN_PAD",
	"SDMMC3_1P8_EN_PAD",
};

#define	SW_UID		1
#define	SW_BANK_PREFIX	"southwestbank"

const int chv_southwest_pins[] = {
	8, 8, 8, 8, 8, 8, 8, -1
};

const char *chv_southwest_pin_names[] = {
	"FST_SPI_D2_PAD",
	"FST_SPI_D0_PAD",
	"FST_SPI_CLK_PAD",
	"FST_SPI_D3_PAD",
	"FST_SPI_CS1_B_PAD",
	"FST_SPI_D1_PAD",
	"FST_SPI_CS0_B_PAD",
	"FST_SPI_CS2_B_PAD",

	"UART1_RTS_B_PAD",
	"UART1_RXD_PAD",
	"UART2_RXD_PAD",
	"UART1_CTS_B_PAD",
	"UART2_RTS_B_PAD",
	"UART1_TXD_PAD",
	"UART2_TXD_PAD",
	"UART2_CTS_B_PAD",

	"MF_HDA_CLK"
	"MF_HDA_RSTB",
	"MF_HDA_SDIO",
	"MF_HDA_SDO",
	"MF_HDA_DOCKRSTB",
	"MF_HDA_SYNC",
	"MF_HDA_SDI1",
	"MF_HDA_DOCKENB",

	"I2C5_SDA_PAD",
	"I2C4_SDA_PAD",
	"I2C6_SDA_PAD",
	"I2C5_SCL_PAD",
	"I2C_NFC_SDA_PAD",
	"I2C4_SCL_PAD",
	"I2C6_SCL_PAD",
	"I2C_NFC_SCL_PAD",

	"I2C1_SDA_PAD",
	"I2C0_SDA_PAD",
	"I2C2_SDA_PAD",
	"I2C1_SCL_PAD",
	"I2C3_SDA_PAD",
	"I2C0_SCL_PAD",
	"I2C2_SCL_PAD",
	"I2C3_SCL_PAD",

	"SATA_GP0",
	"SATA_GP1",
	"SATA_LEDN",
	"SATA_GP2",
	"MF_SMB_ALERTB",
	"SATA_GP3",
	"MF_SMB_CLK",
	"MF_SMB_DATA",

	"PCIE_CLKREQ0B_PAD",
	"PCIE_CLKREQ1B_PAD",
	"GP_SSP_2_CLK_PAD",
	"PCIE_CLKREQ2B_PAD",
	"GP_SSP_2_RXD_PAD",
	"PCIE_CLKREQ3B_PAD",
	"GP_SSP_2_FS_PAD",
	"GP_SSP_2_TXD_PAD",
};

const char *virtualgpio[] = {
	"VIRTUAL0_PAD",
	"VIRTUAL1_PAD",
	"VIRTUAL2_PAD",
	"VIRTUAL3_PAD",
	"VIRTUAL4_PAD",
	"VIRTUAL5_PAD",
	"VIRTUAL6_PAD",
	"VIRTUAL7_PAD",
};

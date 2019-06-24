/*
 *  Sailsky BL270Q (from Atheros AP147) reference board support
 *
 *  Copyright (C) 2014 Matthias Schiffer <mschiffer@universe-factory.net>
 *  Copyright (C) 2015 Sven Eckelmann <sven@open-mesh.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/phy.h>
#include <linux/gpio.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "pci.h"

#define BL270Q_GPIO_LED_SYSTEM 1

#define BL270Q_GPIO_LED_GREEN 12
#define BL270Q_GPIO_LED_RED 13

#define BL270Q_GPIO_BTN_RESET 17

#define BL270Q_KEYS_POLL_INTERVAL 20 /* msecs */
#define BL270Q_KEYS_DEBOUNCE_INTERVAL (3 * BL270Q_KEYS_POLL_INTERVAL)

#define BL270Q_WMAC0_OFFSET 0x1000

static struct gpio_led bl270q_leds_gpio[] __initdata = {
		{
				.name = "bl270q:green:system",
				.gpio = BL270Q_GPIO_LED_SYSTEM,
				.active_low = 1,
		}};

static struct gpio_keys_button bl270q_gpio_keys[] __initdata = {
		{
				.desc = "reset",
				.type = EV_KEY,
				.code = KEY_RESTART,
				.debounce_interval = BL270Q_KEYS_DEBOUNCE_INTERVAL,
				.gpio = BL270Q_GPIO_BTN_RESET,
				.active_low = 1,
		}};

static void __init bl270q_setup(void)
{
	u8 *art = (u8 *)KSEG1ADDR(0x1fff0000);
	u8 *mac = (u8 *)KSEG1ADDR(0x1f046100);

	ath79_register_m25p80(NULL);
	ath79_register_leds_gpio(-1, ARRAY_SIZE(bl270q_leds_gpio),
													 bl270q_leds_gpio);
	ath79_register_gpio_keys_polled(-1, BL270Q_KEYS_POLL_INTERVAL,
																	ARRAY_SIZE(bl270q_gpio_keys),
																	bl270q_gpio_keys);

	ath79_register_pci();

	ath79_register_wmac(art + BL270Q_WMAC0_OFFSET, NULL);

	ath79_setup_ar933x_phy4_switch(false, false);

	ath79_register_mdio(0, 0x0);

	/* WAN */
	ath79_switch_data.phy4_mii_en = 1;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.speed = SPEED_1000;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 0);
	ath79_register_eth(0);

	/* LAN */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_init_mac(ath79_eth1_data.mac_addr, mac, 1);
	ath79_register_eth(1);
}

MIPS_MACHINE(ATH79_MACH_BL270Q, "BL270Q", "Sailsky BL270Q", bl270q_setup);

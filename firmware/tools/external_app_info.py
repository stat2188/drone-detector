#!/usr/bin/env python3

#
# Copyright (C) 2024 Mark Thompson
#
# This file is part of PortaPack.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

# external app address ranges below must match those in linker file "external.ld"
maximum_application_size = 48*1024
external_apps_address_start = 0xADB00000
external_apps_address_end = 0xADF00000

# Map application names to their region sizes for proper address validation
def get_region_size(app_name):
    """
    Get the region size for a specific external application.
    This is used for proper address validation and patching.
    """
    region_sizes = {
        # 48KB regions
        'enhanced_drone_analyzer': 48 * 1024,
        
        # 8KB regions  
        'enhanced_drone_analyzer_settings': 8 * 1024,
        
        # 32KB regions (default)
        'afsk_rx': 32 * 1024,
        'calculator': 32 * 1024,
        'font_viewer': 32 * 1024,
        'blespam': 32 * 1024,
        'analogtv': 32 * 1024,
        'nrf_rx': 32 * 1024,
        'coasterp': 32 * 1024,
        'lge': 32 * 1024,
        'lcr': 32 * 1024,
        'jammer': 32 * 1024,
        'gpssim': 32 * 1024,
        'spainter': 32 * 1024,
        'keyfob': 32 * 1024,
        'tetris': 32 * 1024,
        'extsensors': 32 * 1024,
        'foxhunt_rx': 32 * 1024,
        'audio_test': 32 * 1024,
        'wardrivemap': 32 * 1024,
        'tpmsrx': 32 * 1024,
        'protoview': 32 * 1024,
        'adsbtx': 32 * 1024,
        'morse_tx': 32 * 1024,
        'sstvtx': 32 * 1024,
        'random_password': 32 * 1024,
        'acars_rx': 32 * 1024,
        'shoppingcart_lock': 32 * 1024,
        'cvs_spam': 32 * 1024,
        'ookbrute': 32 * 1024,
        'ook_editor': 32 * 1024,
        'flippertx': 32 * 1024,
        'remote': 32 * 1024,
        'mcu_temperature': 32 * 1024,
        'fmradio': 32 * 1024,
        'tuner': 32 * 1024,
        'metronome': 32 * 1024,
        'app_manager': 32 * 1024,
        'hopper': 32 * 1024,
        'antenna_length': 32 * 1024,
        'view_wav': 32 * 1024,
        'sd_wipe': 32 * 1024,
        'playlist_editor': 32 * 1024,
        'snake': 32 * 1024,
        'stopwatch': 32 * 1024,
        'wefax_rx': 32 * 1024,
        'breakout': 32 * 1024,
        'doom': 32 * 1024,
        'debug_pmem': 32 * 1024,
        'scanner': 32 * 1024,
        'level': 32 * 1024,
        'gfxeq': 32 * 1024,
        'noaaapt_rx': 32 * 1024,
        'detector_rx': 32 * 1024,
        'dinogame': 32 * 1024,
        'spaceinv': 32 * 1024,
        'blackjack': 32 * 1024,
        'battleship': 32 * 1024,
        'ert': 32 * 1024,
        'epirb_rx': 32 * 1024,
        'soundboard': 32 * 1024,
        'game2048': 32 * 1024,
        'bht_tx': 32 * 1024,
        'morse_practice': 32 * 1024,
        'adult_toys_controller': 32 * 1024,
    }
    
    return region_sizes.get(app_name, 32 * 1024)  # default to 32KB

def get_app_region_info(app_name):
    """
    Get complete region information for an application.
    Returns dict with address, size, and validation info.
    """
    # This would need to be implemented to parse the linker script
    # For now, return basic info
    return {
        'app_name': app_name,
        'size': get_region_size(app_name),
        'address_range': f"0xADxxxxxx - 0xADxxxxxx"  # placeholder
    }

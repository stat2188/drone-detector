/*
    Copyright (C) 2023 Bernd Herzog
    Copyright (C) 2024 Mark Thompson
    Copyright (C) 2025 PortaPack Mayhem Team

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

MEMORY
{
    /*
     * External apps: regions can't overlap so addresses are corrected after build.
     * Picking uncommon address values for search & replace in binaries (no false positives) - 0xADB00000-0xADEF0000 seems to be good.
     * Also need to consider processor memory map - reading 0xADxxxxxx generates a fault which may be better than unexpected behavior.
     * External app address ranges below must match those in python file "external_app_info.py".
     */
    ram_external_app_afsk_rx (rwx) : org = 0xadb10000, len = 32k
    ram_external_app_calculator (rwx) : org = 0xadb20000, len = 32k
    ram_external_app_font_viewer (rwx) : org = 0xadb30000, len = 32k
    ram_external_app_nrf_rx (rwx) : org = 0xadb40000, len = 32k
    ram_external_app_coasterp (rwx) : org = 0xadb50000, len = 32k
    ram_external_app_lge (rwx) : org = 0xadb60000, len = 32k
    ram_external_app_jammer (rwx) : org = 0xadb70000, len = 32k
    ram_external_app_gpssim (rwx) : org = 0xadb80000, len = 32k
    ram_external_app_spainter (rwx) : org = 0xadb90000, len = 32k
    ram_external_app_extsensors (rwx) : org = 0xadba0000, len = 32k
    ram_external_app_audio_test (rwx) : org = 0xadbb0000, len = 32k
    ram_external_app_wardrivemap (rwx) : org = 0xadbc0000, len = 32k
    ram_external_app_morse_tx (rwx) : org = 0xadbd0000, len = 32k
    ram_external_app_random_password (rwx) : org = 0xadbe0000, len = 32k
    ram_external_app_shoppingcart_lock (rwx) : org = 0xadbf0000, len = 32k
    ram_external_app_flippertx (rwx) : org = 0xadc00000, len = 32k
    ram_external_app_mcu_temperature (rwx) : org = 0xadc10000, len = 32k
    ram_external_app_fmradio (rwx) : org = 0xadc20000, len = 32k
    ram_external_app_tuner (rwx) : org = 0xadc30000, len = 32k
    ram_external_app_metronome (rwx) : org = 0xadc40000, len = 32k
    ram_external_app_app_manager (rwx) : org = 0xadc50000, len = 32k
    ram_external_app_hopper (rwx) : org = 0xadc60000, len = 32k
    ram_external_app_antenna_length (rwx) : org = 0xadc70000, len = 32k
    ram_external_app_view_wav (rwx) : org = 0xadc80000, len = 32k
    ram_external_app_sd_wipe (rwx) : org = 0xadc90000, len = 32k
    ram_external_app_playlist_editor (rwx) : org = 0xadca0000, len = 32k
    ram_external_app_debug_pmem (rwx) : org = 0xadcb0000, len = 32k
    ram_external_app_scanner (rwx) : org = 0xadcc0000, len = 32k
    ram_external_app_level (rwx) : org = 0xadcd0000, len = 32k
    ram_external_app_gfxeq (rwx) : org = 0xadce0000, len = 32k
    ram_external_app_detector_rx (rwx) : org = 0xadcf0000, len = 32k
    ram_external_app_soundboard (rwx) : org = 0xadd00000, len = 32k
    ram_external_app_morse_practice (rwx) : org = 0xadd10000, len = 32k
    ram_external_app_flex_rx (rwx) : org = 0xadd20000, len = 32k
    ram_external_app_subcarrx (rwx) : org = 0xadd30000, len = 32k
}

SECTIONS
{
    .external_app_afsk_rx : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_afsk_rx.application_information));
        *(*ui*external_app*afsk_rx*);
    } > ram_external_app_afsk_rx

    .external_app_calculator : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_calculator.application_information));
        *(*ui*external_app*calculator*);
    } > ram_external_app_calculator

    .external_app_font_viewer : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_font_viewer.application_information));
        *(*ui*external_app*font_viewer*);
    } > ram_external_app_font_viewer

    .external_app_nrf_rx : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_nrf_rx.application_information));
        *(*ui*external_app*nrf_rx*);
    } > ram_external_app_nrf_rx

    .external_app_coasterp : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_coasterp.application_information));
        *(*ui*external_app*coasterp*);
    } > ram_external_app_coasterp

    .external_app_lge : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_lge.application_information));
        *(*ui*external_app*lge*);
    } > ram_external_app_lge

    .external_app_jammer : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_jammer.application_information));
        *(*ui*external_app*jammer*);
    } > ram_external_app_jammer

    .external_app_gpssim : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_gpssim.application_information));
        *(*ui*external_app*gpssim*);
    } > ram_external_app_gpssim

    .external_app_spainter : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_spainter.application_information));
        *(*ui*external_app*spainter*);
    } > ram_external_app_spainter

    .external_app_extsensors : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_extsensors.application_information));
        *(*ui*external_app*extsensors*);
    } > ram_external_app_extsensors

    .external_app_audio_test : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_audio_test.application_information));
        *(*ui*external_app*audio_test*);
    } > ram_external_app_audio_test

    .external_app_wardrivemap : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_wardrivemap.application_information));
        *(*ui*external_app*wardrivemap*);
    } > ram_external_app_wardrivemap

    .external_app_morse_tx : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_morse_tx.application_information));
        *(*ui*external_app*morse_tx*);
    } > ram_external_app_morse_tx

    .external_app_random_password : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_random_password.application_information));
        *(*ui*external_app*random_password*);
    } > ram_external_app_random_password

    .external_app_shoppingcart_lock : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_shoppingcart_lock.application_information));
        *(*ui*external_app*shoppingcart_lock*);
    } > ram_external_app_shoppingcart_lock

    .external_app_flippertx : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_flippertx.application_information));
        *(*ui*external_app*flippertx*);
    } > ram_external_app_flippertx

    .external_app_mcu_temperature : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_mcu_temperature.application_information));
        *(*ui*external_app*mcu_temperature*);
    } > ram_external_app_mcu_temperature

    .external_app_fmradio : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_fmradio.application_information));
        *(*ui*external_app*fmradio*);
    } > ram_external_app_fmradio

    .external_app_tuner : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_tuner.application_information));
        *(*ui*external_app*tuner*);
    } > ram_external_app_tuner

    .external_app_metronome : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_metronome.application_information));
        *(*ui*external_app*metronome*);
    } > ram_external_app_metronome

    .external_app_app_manager : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_app_manager.application_information));
        *(*ui*external_app*app_manager*);
    } > ram_external_app_app_manager

    .external_app_hopper : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_hopper.application_information));
        *(*ui*external_app*hopper*);
    } > ram_external_app_hopper

    .external_app_antenna_length : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_antenna_length.application_information));
        *(*ui*external_app*antenna_length*);
    } > ram_external_app_antenna_length

    .external_app_view_wav : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_view_wav.application_information));
        *(*ui*external_app*view_wav*);
    } > ram_external_app_view_wav

    .external_app_sd_wipe : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_sd_wipe.application_information));
        *(*ui*external_app*sd_wipe*);
    } > ram_external_app_sd_wipe

    .external_app_playlist_editor : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_playlist_editor.application_information));
        *(*ui*external_app*playlist_editor*);
    } > ram_external_app_playlist_editor

    .external_app_debug_pmem : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_debug_pmem.application_information));
        *(*ui*external_app*debug_pmem*);
    } > ram_external_app_debug_pmem

    .external_app_scanner : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_scanner.application_information));
        *(*ui*external_app*scanner*);
    } > ram_external_app_scanner

    .external_app_level : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_level.application_information));
        *(*ui*external_app*level*);
    } > ram_external_app_level

    .external_app_gfxeq : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_gfxeq.application_information));
        *(*ui*external_app*gfxeq*);
    } > ram_external_app_gfxeq

    .external_app_detector_rx : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_detector_rx.application_information));
        *(*ui*external_app*detector_rx*);
    } > ram_external_app_detector_rx

    .external_app_soundboard : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_soundboard.application_information));
        *(*ui*external_app*soundboard*);
    } > ram_external_app_soundboard

    .external_app_morse_practice : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_morse_practice.application_information));
        *(*ui*external_app*morse_practice*);
    } > ram_external_app_morse_practice

    .external_app_flex_rx : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_flex_rx.application_information));
        *(*ui*external_app*flex_rx*);
    } > ram_external_app_flex_rx

    .external_app_subcarrx : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_subcarrx.application_information));
        *(*ui*external_app*subcarrx*);
    } > ram_external_app_subcarrx

}

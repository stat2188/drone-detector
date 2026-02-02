 set(EXTCPPSRC
	#afsk_rx   16 byte (uses baseband)
	#external/afsk_rx/main.cpp
	#external/afsk_rx/ui_afsk_rx.cpp

	#calculator  632 bytes
	external/calculator/main.cpp
	external/calculator/ui_calculator.cpp

	#font_viewer 8 byte?!
	external/font_viewer/main.cpp
	external/font_viewer/ui_font_viewer.cpp

	#blespam 336 bytes - array initializers? (uses baseband)
	#external/blespam/main.cpp
	#external/blespam/ui_blespam.cpp

	#analogtv 552 bytes (uses baseband)
	#external/analogtv/main.cpp
	#external/analogtv/analog_tv_app.cpp
	#external/analogtv/ui_tv.cpp
	#external/analogtv/tv_signal_detector.cpp

	#nrf_rx  40 byte (uses baseband)
	#external/nrf_rx/main.cpp
	#external/nrf_rx/ui_nrf_rx.cpp

	#coasterp  0 byte (uses baseband)
	#external/coasterp/main.cpp
	#external/coasterp/ui_coasterp.cpp

	#lge  120 byte (uses baseband)
	#external/lge/main.cpp
	#external/lge/lge_app.cpp
	#external/lge/rfm69.cpp

	#lcr - 460 byte flash (uses baseband)
	#external/lcr/main.cpp
	#external/lcr/ui_lcr.cpp

	#jammer 144 byte (uses baseband)
	#external/jammer/main.cpp
	#external/jammer/ui_jammer.cpp

	#gpssim  160 byte (uses baseband)
	#external/gpssim/main.cpp
	#external/gpssim/gps_sim_app.cpp

	#spainter   464 byte (uses baseband)
	#external/spainter/main.cpp
	#external/spainter/ui_spectrum_painter.cpp
	#external/spainter/ui_spectrum_painter_text.cpp
	#external/spainter/ui_spectrum_painter_image.cpp

	#keyfob 216 byte (uses baseband)
	#external/keyfob/main.cpp
	#external/keyfob/ui_keyfob.cpp
	#external/keyfob/ui_keyfob.hpp

	#tetris 88 byte
	external/tetris/main.cpp
	external/tetris/ui_tetris.cpp


	#extsensors 192 byte
	external/extsensors/main.cpp
	external/extsensors/ui_extsensors.cpp
	external/extsensors/ui_extsensors.hpp

	#foxhunt 0 (uses baseband)
	#external/foxhunt/main.cpp
	#external/foxhunt/ui_foxhunt_rx.cpp
	#external/foxhunt/ui_foxhunt_rx.hpp

	#audio_test 192 byte (uses baseband)
	#external/audio_test/main.cpp
	#external/audio_test/ui_audio_test.cpp

	#wardrivemap 64 byte
	external/wardrivemap/main.cpp
	external/wardrivemap/ui_wardrivemap.cpp

	#tpmsrx 920 byte- possible shared part with baseband (uses baseband)
	#external/tpmsrx/main.cpp
	#external/tpmsrx/tpms_app.cpp

	#protoview 8 byte (uses baseband)
	#external/protoview/main.cpp
	#external/protoview/ui_protoview.cpp

	#adsbtx  3544 byte - adsb shared part (uses baseband)
	#external/adsbtx/main.cpp
	#external/adsbtx/ui_adsb_tx.cpp

	#morse_tx 768 bytes (uses baseband)
	#external/morse_tx/main.cpp
	#external/morse_tx/ui_morse.cpp

	#sstvtx 456 bytes (uses baseband)
	#external/sstvtx/main.cpp
	#external/sstvtx/ui_sstvtx.cpp

	#sstvrx (uses baseband)
	#external/sstvrx/main.cpp
	#external/sstvrx/ui_sstvrx.cpp

	#random 464  bytes. (uses baseband)
	#external/random_password/main.cpp
	#external/random_password/ui_random_password.cpp
	#external/random_password/sha512.cpp

	#acars (uses baseband)
	#external/acars_rx/main.cpp
	#external/acars_rx/acars_app.cpp

	#wefax_rx 192 bytes (uses baseband)
	#external/wefax_rx/main.cpp
	#external/wefax_rx/ui_wefax_rx.cpp


	#noaaapt_rx  72 bytes (uses baseband)
	#external/noaaapt_rx/main.cpp
	#external/noaaapt_rx/ui_noaaapt_rx.cpp

	#shoppingcart_lock 272 bytes (uses baseband)
	#external/shoppingcart_lock/main.cpp
	#external/shoppingcart_lock/shoppingcart_lock.cpp


	#ookbrute  80 byte (uses baseband)
	#external/ookbrute/main.cpp
	#external/ookbrute/ui_ookbrute.cpp

	#ook_editor  1808 bytes (uses baseband)
	#external/ook_editor/main.cpp
	#external/ook_editor/ui_ook_editor.cpp

	#cvs_spam 24 byte (uses baseband)
	#external/cvs_spam/main.cpp
	#external/cvs_spam/cvs_spam.cpp

	#flippertx  712 bytes (uses baseband)
	#external/flippertx/main.cpp
	#external/flippertx/ui_flippertx.cpp

	#remote 1664 bytes (uses baseband)
	#external/remote/main.cpp
	#external/remote/ui_remote.cpp

	#mcu_temperature    112
	external/mcu_temperature/main.cpp
	external/mcu_temperature/mcu_temperature.cpp

	#fmradio  640 (uses baseband)
	#external/fmradio/main.cpp
	#external/fmradio/ui_fmradio.cpp

	#tuner 384 (uses baseband)
	#external/tuner/main.cpp
	#external/tuner/ui_tuner.cpp

	#metronome 696 bytes (uses baseband)
	#external/metronome/main.cpp
	#external/metronome/ui_metronome.cpp

	#app_manager 40 bytes
	external/app_manager/main.cpp
	external/app_manager/ui_app_manager.cpp

	#hopper 472 bytes (uses baseband)
	#external/hopper/main.cpp
	#external/hopper/ui_hopper.cpp

	# whip calculator  48 bytes
	external/antenna_length/main.cpp
	external/antenna_length/ui_whipcalc.cpp

	# wav viewer 1232 bytes (uses baseband)
	#external/wav_view/main.cpp
	#external/wav_view/ui_view_wav.cpp


	# wipe sdcard 16 byte
	external/sd_wipe/main.cpp
	external/sd_wipe/ui_sd_wipe.cpp

	# playlist editor 232 bytes
	external/playlist_editor/main.cpp
	external/playlist_editor/ui_playlist_editor.cpp

	#snake 240 bytes
	external/snake/main.cpp
	external/snake/ui_snake.cpp


	#stopwatch 0
	external/stopwatch/main.cpp
	external/stopwatch/ui_stopwatch.cpp

	#breakout 1144 bytes (uses baseband)
	#external/breakout/main.cpp
	#external/breakout/ui_breakout.cpp

	#dinogame 0 
	external/dinogame/main.cpp
	external/dinogame/ui_dinogame.cpp

	#doom 224
	external/doom/main.cpp
	external/doom/ui_doom.cpp

	#debug_pmem  944 byte
	external/debug_pmem/main.cpp
	external/debug_pmem/ui_debug_pmem.cpp

	#scanner 520 byte (uses baseband)
	#external/scanner/main.cpp
	#external/scanner/ui_scanner.cpp

	#level  24 byte (uses baseband)
	#external/level/main.cpp
	#external/level/ui_level.cpp

	#gfxEQ 80 byte (uses baseband)
	#external/gfxeq/main.cpp
	#external/gfxeq/ui_gfxeq.cpp

	#detector_rx  168 byte (uses baseband)
	#external/detector_rx/main.cpp
	#external/detector_rx/ui_detector_rx.cpp

	#space_invaders  0 byte
	external/spaceinv/main.cpp
	external/spaceinv/ui_spaceinv.cpp

	#blackjack 24 byte
	external/blackjack/main.cpp
	external/blackjack/ui_blackjack.cpp

	#battleship  256 byte (uses baseband)
	#external/battleship/main.cpp
	#external/battleship/ui_battleship.cpp

	#ert 3040 bytes - has common with baseband, could be renamed the namespace, so both could have it, but not kept in fw. (uses baseband)
	#external/ert/main.cpp
	#external/ert/ert_app.cpp

	#epirb_rx 168 byte flash (uses baseband)
	#external/epirb_rx/main.cpp
	#external/epirb_rx/ui_epirb_rx.cpp

	#soundboard  272byte  - 1236 bytes (uses baseband)
	#external/soundboard/main.cpp
	#external/soundboard/soundboard_app.cpp

	#game2048   - 168 byte flash (uses baseband)
	#external/game2048/main.cpp
	#external/game2048/ui_game2048.cpp

	#bht_tx - 3920 byte flash, unknown (uses baseband)
	#external/bht_tx/main.cpp
	#external/bht_tx/ui_bht_tx.cpp
	#external/bht_tx/bht.cpp

	#morse_practice - 80 byte flash - bc of array initializers (uses baseband)
	#external/morse_practice/main.cpp
	#external/morse_practice/ui_morse_practice.cpp

	#adult_toys_controller  144 bytes (uses baseband)
	#external/adult_toys_controller/main.cpp
	#external/adult_toys_controller/ui_adult_toys_controller.cpp

	#flex_rx (uses baseband)
	#external/flex_rx/main.cpp
	#external/flex_rx/ui_flex_rx.cpp	

	#subcarrx (uses baseband)
	#external/subcarrx/main.cpp
	#external/subcarrx/ui_subcar.cpp	


)

set(EXTAPPLIST
	#afsk_rx (uses baseband)
	calculator
	font_viewer
	#blespam (uses baseband)
	#analogtv (uses baseband)
	#nrf_rx (uses baseband)
	#coasterp (uses baseband)
	#lge (uses baseband)
	#lcr (uses baseband)
	#jammer (uses baseband)
	#gpssim (uses baseband)
	#spainter (uses baseband)
	#keyfob (uses baseband)
	tetris
	extsensors
	#foxhunt_rx (uses baseband)
	#audio_test (uses baseband)
	wardrivemap
	#tpmsrx (uses baseband)
	#protoview (uses baseband)
	#adsbtx (uses baseband)
	#morse_tx (uses baseband)
	#sstvtx (uses baseband)
	#sstvrx (uses baseband)
	#random_password (uses baseband)
	# acars_rx --not working (uses baseband)
	#wefax_rx (uses baseband)
	#noaaapt_rx (uses baseband)
	#shoppingcart_lock (uses baseband)
	#ookbrute (uses baseband)
	#ook_editor (uses baseband)
	#cvs_spam (uses baseband)
	#flippertx (uses baseband)
	#remote (uses baseband)
	mcu_temperature
	#fmradio (uses baseband)
	#tuner (uses baseband)
	#metronome (uses baseband)
	app_manager
	#hopper (uses baseband)
	antenna_length
	#view_wav (uses baseband)
	sd_wipe
	playlist_editor
	snake
	stopwatch
	#breakout (uses baseband)
	dinogame
	doom
	debug_pmem
	#scanner (uses baseband)
	#level (uses baseband)
	#gfxeq (uses baseband)
	#detector_rx (uses baseband)
	spaceinv
	blackjack
	#battleship (uses baseband)
	#ert (uses baseband)
	#epirb_rx (uses baseband)
	#soundboard (uses baseband)
	#game2048 (uses baseband)
	#bht_tx (uses baseband)
	#morse_practice (uses baseband)
	#adult_toys_controller (uses baseband)
	#flex_rx (uses baseband)
	#subcarrx (uses baseband)
)

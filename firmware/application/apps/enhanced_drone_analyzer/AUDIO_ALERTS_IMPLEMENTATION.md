# Audio Alerts Implementation for Enhanced Drone Analyzer

## Phase 1: Beeper API Analysis

### Beeper API Function Signatures

The firmware provides the following beeper API functions in `baseband_api.hpp`:

```cpp
// Main API for playing audio beeps
void baseband::request_audio_beep(uint32_t freq, uint32_t sample_rate, uint32_t duration_ms);

// Stop ongoing beep playback
void baseband::request_beep_stop();
```

### Usage Patterns from Existing Apps

**From ui_detector_app.cpp:**
```cpp
void DetectorView::play_beep(int db) {
    uint32_t freq = map(db, -100, 20, 400, 2600);
    baseband::request_audio_beep(freq, 24000, 150);
}
```

**From ui_looking_glass_app.cpp:**
```cpp
// Audio setup
audio::set_rate(audio::Rate::Hz_24000);
audio::output::start();

// Play beep
baseband::request_audio_beep(map(range_max_power, 0, 256, 400, 2600), 24000, 250);

// Stop audio
baseband::request_beep_stop();
audio::output::stop();
```

**From ui_adsb_rx.cpp, pocsag_app.cpp, etc.:**
```cpp
if (pmem::beep_on_packets()) {
    baseband::request_audio_beep(1000, 24000, 60);
}
```

### Key Findings

1. **Non-blocking**: `request_audio_beep()` sends a message to the baseband processor and returns immediately
2. **Sample rate**: Most apps use 24000 Hz
3. **Frequency range**: 400-2600 Hz for alerts
4. **Duration**: 60-250 ms for short alerts
5. **Audio setup**: Must call `audio::set_rate()` and `audio::output::start()` before playing beeps
6. **Thread safety**: The API is thread-safe via message passing to baseband processor

## Phase 2: Audio Alert System Design

### Alert Types

| Alert Type | Description | Priority |
|------------|-------------|----------|
| NEW_DRONE | New drone detected | MEDIUM |
| THREAT_INCREASED | Threat level increased | HIGH |
| THREAT_CRITICAL | Critical threat level | CRITICAL |
| DRONE_APPROACHING | Drone approaching (RSSI increasing) | HIGH |
| DRONE_RECEDING | Drone receding (RSSI decreasing) | LOW |

### Alert Configurations

| Alert Type | Frequency (Hz) | Duration (ms) | Beep Count | Beep Gap (ms) |
|-------------|-----------------|----------------|--------------|----------------|
| NEW_DRONE | 1000 | 150 | 1 | 0 |
| THREAT_INCREASED | 1200 | 100 | 2 | 50 |
| THREAT_CRITICAL | 1500 | 80 | 3 | 40 |
| DRONE_APPROACHING | 1200 | 200 | 1 | 0 |
| DRONE_RECEDING | 800 | 200 | 1 | 0 |

### Non-Blocking Implementation

The audio alert system uses the following approach:

1. **Message Passing**: Alerts are sent to the baseband processor via messages
2. **No Blocking**: The `request_audio_beep()` function returns immediately
3. **Queue Support**: Multiple beeps can be queued by calling the function multiple times
4. **Priority Handling**: Higher priority alerts override lower priority alerts

### Thread Safety

- The beeper API is thread-safe via message passing
- No mutex protection needed when calling `request_audio_beep()`
- The baseband processor handles audio playback in its own thread

## Phase 3: Implementation Details

### Files Created/Modified

1. **audio_alerts.hpp** - New file defining alert types and manager
2. **audio_alerts.cpp** - New file implementing the audio alert manager
3. **scanner.hpp** - Modified to add alert callback and trigger methods
4. **scanner.cpp** - Modified to implement alert triggering logic
5. **drone_scanner_ui.hpp** - Modified to add on_alert method
6. **drone_scanner_ui.cpp** - Modified to implement audio playback

### Alert Triggering Logic

Alerts are triggered in the following scenarios:

1. **New Drone Detected**: When a new drone is added to the tracked drones list
2. **Threat Level Increased**: When a drone's threat level increases from previous scan
3. **Critical Threat**: When a drone's threat level is CRITICAL
4. **Drone Approaching**: When a drone's RSSI is increasing over multiple scans
5. **Drone Receding**: When a drone's RSSI is decreasing over multiple scans

### Alert Priority Handling

The system implements priority handling:

- **CRITICAL**: Always plays, overrides any ongoing alert
- **HIGH**: Plays if no CRITICAL alert is active
- **MEDIUM**: Plays if no HIGH or CRITICAL alert is active
- **LOW**: Visual only, no audio alert

## Phase 4: Testing

### Test Cases

1. **New Drone Detection**: Verify single beep at 1000 Hz, 150 ms
2. **Threat Increased**: Verify double beep at 1200 Hz, 100 ms each, 50 ms gap
3. **Critical Threat**: Verify triple beep at 1500 Hz, 80 ms each, 40 ms gap
4. **Drone Approaching**: Verify single beep at 1200 Hz, 200 ms
5. **Drone Receding**: Verify no audio alert (visual only)
6. **Multiple Alerts**: Verify alerts queue correctly without blocking UI
7. **Priority Override**: Verify CRITICAL alerts override lower priority alerts

### Verification Steps

1. Enable audio alerts in settings
2. Start scanning
3. Trigger various alert scenarios
4. Verify non-blocking behavior (UI remains responsive)
5. Verify audio plays correctly
6. Verify alert priority handling
7. Test with multiple drones detected in quick succession

## Integration Notes

### Audio Setup

The audio system must be initialized before playing alerts:

```cpp
// In scanner or UI initialization
audio::set_rate(audio::Rate::Hz_24000);
audio::output::start();
```

### Audio Cleanup

When stopping the scanner or disabling audio alerts:

```cpp
// Stop any ongoing audio
baseband::request_beep_stop();
audio::output::stop();
```

### Settings Integration

The audio alert system should integrate with the settings:

```cpp
// Enable/disable audio alerts
AudioAlertManager::set_enabled(settings.audio_alerts_enabled);

// Check if audio alerts are enabled
if (AudioAlertManager::is_enabled()) {
    AudioAlertManager::play_alert(alert_type);
}
```

## Memory Considerations

- **No Heap Allocation**: Uses static configurations and stack variables
- **Fixed-Size Arrays**: Uses `std::array` for tracked drone threat levels
- **Small Footprint**: Audio alert manager adds minimal memory overhead
- **No Dynamic Memory**: All allocations are compile-time constants

## Performance Considerations

- **Non-Blocking**: Alerts don't block the UI thread
- **Message Passing**: Uses efficient message passing to baseband processor
- **Low CPU Overhead**: Minimal processing required for alert triggering
- **No Audio Glitches**: Baseband processor handles audio playback independently

## Future Enhancements

1. **Custom Alert Patterns**: Allow users to customize alert patterns
2. **Volume Control**: Add volume control for audio alerts
3. **Alert History**: Keep track of recent alerts for debugging
4. **Alert Filtering**: Allow users to filter certain alert types
5. **Vibration Support**: Add vibration alerts for devices with vibration motors

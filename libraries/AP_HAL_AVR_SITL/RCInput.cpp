
#include "RCInput.h"

using namespace AVR_SITL;

extern const AP_HAL::HAL& hal;

void SITLRCInput::init(void* machtnichts)
{
    clear_overrides();
}

uint8_t SITLRCInput::valid() {
    return 0;
}

uint16_t SITLRCInput::read(uint8_t ch) {
    return _override[ch]? _override[ch] : _sitlState->pwm_input[ch];
}

uint8_t SITLRCInput::read(uint16_t* periods, uint8_t len) {
    for (uint8_t i=0; i<len; i++) {
	periods[i] = _override[i]? _override[i] : _sitlState->pwm_input[i];
    }
    return len;
}

bool SITLRCInput::set_overrides(int16_t *overrides, uint8_t len) {
    bool res = false;
    for (uint8_t i = 0; i < len; i++) {
        res |= set_override(i, overrides[i]);
    }
    return res;
}

bool SITLRCInput::set_override(uint8_t channel, int16_t override) {
    if (override < 0) return false; /* -1: no change. */
    if (channel < 8) {
        _override[channel] = override;
        if (override != 0) {
            return true;
        }
    }
    return false;
}

void SITLRCInput::clear_overrides()
{
    for (uint8_t i = 0; i < 8; i++) {
	_override[i] = 0;
    }
}

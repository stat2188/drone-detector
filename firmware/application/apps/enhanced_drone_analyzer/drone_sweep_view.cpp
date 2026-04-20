#include <cstdint>
#include <cstring>
#include <new>

#include "drone_sweep_view.hpp"
#include "drone_settings.hpp"
#include "settings_manager.hpp"
#include "scanner.hpp"
#include "constants.hpp"
#include "ui_receiver.hpp"
#include "file.hpp"
#include "file_path.hpp"

namespace drone_analyzer {

// ============================================================================
// SweepWindowGroup1View — Tab 1: Windows 1-2
// ============================================================================

SweepWindowGroup1View::SweepWindowGroup1View(NavigationView& nav, const Rect parent_rect) noexcept
    : ui::View() {
    set_parent_rect(parent_rect);
    add_children({
        &labels_,
        &field_sw1_start_,
        &field_sw1_end_,
        &field_sw1_step_,
        &labels_sw2_,
        &check_sw2_enabled_,
        &field_sw2_start_,
        &field_sw2_end_,
        &field_sw2_step_,
        &labels_exc1_,
        &field_sw1_exc0_,
        &field_sw1_exc1_,
        &field_sw1_exc2_,
        &field_sw1_exc3_,
        &field_sw1_exc4_,
        &labels_exc2_,
        &field_sw2_exc0_,
        &field_sw2_exc1_,
        &field_sw2_exc2_,
        &field_sw2_exc3_,
        &field_sw2_exc4_,
    });

    // Keyboard callbacks for frequency fields (MHz → keypad → MHz)
    field_sw1_start_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw1_start_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw1_start_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw1_end_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw1_end_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw1_end_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw2_start_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw2_start_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw2_start_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw2_end_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw2_end_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw2_end_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    // Keyboard callbacks for exception fields (MHz → keypad → MHz)
    field_sw1_exc0_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw1_exc0_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw1_exc0_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw1_exc1_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw1_exc1_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw1_exc1_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw1_exc2_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw1_exc2_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw1_exc2_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw2_exc0_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw2_exc0_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw2_exc0_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw2_exc1_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw2_exc1_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw2_exc1_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw2_exc2_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw2_exc2_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw2_exc2_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw1_exc3_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw1_exc3_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw1_exc3_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw1_exc4_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw1_exc4_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw1_exc4_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw2_exc3_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw2_exc3_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw2_exc3_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw2_exc4_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw2_exc4_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw2_exc4_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };
}

void SweepWindowGroup1View::focus() {
    field_sw1_start_.focus();
}

// ============================================================================
// SweepWindowGroup2View — Tab 2: Windows 3-4
// ============================================================================

SweepWindowGroup2View::SweepWindowGroup2View(NavigationView& nav, const Rect parent_rect) noexcept
    : ui::View() {
    set_parent_rect(parent_rect);
    add_children({
        &labels_sw3_,
        &check_sw3_enabled_,
        &field_sw3_start_,
        &field_sw3_end_,
        &field_sw3_step_,
        &labels_sw4_,
        &check_sw4_enabled_,
        &field_sw4_start_,
        &field_sw4_end_,
        &field_sw4_step_,
        &labels_exc3_,
        &field_sw3_exc0_,
        &field_sw3_exc1_,
        &field_sw3_exc2_,
        &field_sw3_exc3_,
        &field_sw3_exc4_,
        &labels_exc4_,
        &field_sw4_exc0_,
        &field_sw4_exc1_,
        &field_sw4_exc2_,
        &field_sw4_exc3_,
        &field_sw4_exc4_,
    });

    // Keyboard callbacks for frequency fields (MHz → keypad → MHz)
    field_sw3_start_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw3_start_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw3_start_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw3_end_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw3_end_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw3_end_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw4_start_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw4_start_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw4_start_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw4_end_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw4_end_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw4_end_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    // Keyboard callbacks for exception fields (MHz → keypad → MHz)
    field_sw3_exc0_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw3_exc0_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw3_exc0_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw3_exc1_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw3_exc1_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw3_exc1_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw3_exc2_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw3_exc2_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw3_exc2_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw4_exc0_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw4_exc0_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw4_exc0_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw4_exc1_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw4_exc1_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw4_exc1_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw4_exc2_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw4_exc2_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw4_exc2_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw3_exc3_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw3_exc3_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw3_exc3_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw3_exc4_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw3_exc4_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw3_exc4_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw4_exc3_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw4_exc3_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw4_exc3_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw4_exc4_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw4_exc4_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw4_exc4_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };
}

void SweepWindowGroup2View::focus() {
    check_sw3_enabled_.focus();
}

// ============================================================================
// DroneSweepView — Main sweep settings view with TabView
// ============================================================================

void DroneSweepView::construct_view(const uint8_t index) noexcept {
    const Rect content_rect{0, TAB_BAR_H, screen_width, screen_height - TAB_BAR_H};

    if (index == 0) {
        auto* v = new (&view_storage_) SweepWindowGroup1View(nav_, content_rect);
        active_view_ = v;

        v->field_sw1_start_.set_value(static_cast<int32_t>(state_buffer_.sw1_start / 1000000ULL));
        v->field_sw1_end_.set_value(static_cast<int32_t>(state_buffer_.sw1_end / 1000000ULL));
        v->field_sw1_step_.set_value(static_cast<int32_t>(state_buffer_.sw1_step / 1000ULL));

        v->check_sw2_enabled_.set_value(state_buffer_.sw2_enabled);
        v->field_sw2_start_.set_value(static_cast<int32_t>(state_buffer_.sw2_start / 1000000ULL));
        v->field_sw2_end_.set_value(static_cast<int32_t>(state_buffer_.sw2_end / 1000000ULL));
        v->field_sw2_step_.set_value(static_cast<int32_t>(state_buffer_.sw2_step / 1000ULL));

        ui::NumberField* exc1_fields[] = {
            &v->field_sw1_exc0_, &v->field_sw1_exc1_,
            &v->field_sw1_exc2_, &v->field_sw1_exc3_,
            &v->field_sw1_exc4_};
        ui::NumberField* exc2_fields[] = {
            &v->field_sw2_exc0_, &v->field_sw2_exc1_,
            &v->field_sw2_exc2_, &v->field_sw2_exc3_,
            &v->field_sw2_exc4_};

        for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
            exc1_fields[i]->set_value(static_cast<int32_t>(state_buffer_.sw1_exc[i] / 1000000ULL));
            exc2_fields[i]->set_value(static_cast<int32_t>(state_buffer_.sw2_exc[i] / 1000000ULL));
        }
    } else {
        auto* v = new (&view_storage_) SweepWindowGroup2View(nav_, content_rect);
        active_view_ = v;

        v->check_sw3_enabled_.set_value(state_buffer_.sw3_enabled);
        v->field_sw3_start_.set_value(static_cast<int32_t>(state_buffer_.sw3_start / 1000000ULL));
        v->field_sw3_end_.set_value(static_cast<int32_t>(state_buffer_.sw3_end / 1000000ULL));
        v->field_sw3_step_.set_value(static_cast<int32_t>(state_buffer_.sw3_step / 1000ULL));

        v->check_sw4_enabled_.set_value(state_buffer_.sw4_enabled);
        v->field_sw4_start_.set_value(static_cast<int32_t>(state_buffer_.sw4_start / 1000000ULL));
        v->field_sw4_end_.set_value(static_cast<int32_t>(state_buffer_.sw4_end / 1000000ULL));
        v->field_sw4_step_.set_value(static_cast<int32_t>(state_buffer_.sw4_step / 1000ULL));

        ui::NumberField* exc3_fields[] = {
            &v->field_sw3_exc0_, &v->field_sw3_exc1_,
            &v->field_sw3_exc2_, &v->field_sw3_exc3_,
            &v->field_sw3_exc4_};
        ui::NumberField* exc4_fields[] = {
            &v->field_sw4_exc0_, &v->field_sw4_exc1_,
            &v->field_sw4_exc2_, &v->field_sw4_exc3_,
            &v->field_sw4_exc4_};

        for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
            exc3_fields[i]->set_value(static_cast<int32_t>(state_buffer_.sw3_exc[i] / 1000000ULL));
            exc4_fields[i]->set_value(static_cast<int32_t>(state_buffer_.sw4_exc[i] / 1000000ULL));
        }
    }

    tab_states_[index].state = TabState::CONSTRUCTED;
    add_child(active_view_);
}

void DroneSweepView::destroy_active_view() noexcept {
    if (active_view_ != nullptr) {
        remove_child(active_view_);
        active_view_->~View();
        active_view_ = nullptr;
    }
    tab_states_[active_tab_index_].state = TabState::INACTIVE;
}

void DroneSweepView::serialize_active_view() noexcept {
    if (active_view_ == nullptr) return;

    if (active_tab_index_ == 0) {
        auto* v = static_cast<SweepWindowGroup1View*>(active_view_);
        state_buffer_.sw1_start = static_cast<uint32_t>(v->field_sw1_start_.value()) * 1000000ULL;
        state_buffer_.sw1_end = static_cast<uint32_t>(v->field_sw1_end_.value()) * 1000000ULL;
        state_buffer_.sw1_step = static_cast<uint32_t>(v->field_sw1_step_.value()) * 1000ULL;

        state_buffer_.sw2_enabled = v->check_sw2_enabled_.value();
        state_buffer_.sw2_start = static_cast<uint32_t>(v->field_sw2_start_.value()) * 1000000ULL;
        state_buffer_.sw2_end = static_cast<uint32_t>(v->field_sw2_end_.value()) * 1000000ULL;
        state_buffer_.sw2_step = static_cast<uint32_t>(v->field_sw2_step_.value()) * 1000ULL;

        ui::NumberField* exc1_fields[] = {
            &v->field_sw1_exc0_, &v->field_sw1_exc1_,
            &v->field_sw1_exc2_, &v->field_sw1_exc3_,
            &v->field_sw1_exc4_};
        ui::NumberField* exc2_fields[] = {
            &v->field_sw2_exc0_, &v->field_sw2_exc1_,
            &v->field_sw2_exc2_, &v->field_sw2_exc3_,
            &v->field_sw2_exc4_};

        for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
            state_buffer_.sw1_exc[i] = static_cast<uint32_t>(exc1_fields[i]->value()) * 1000000ULL;
            state_buffer_.sw2_exc[i] = static_cast<uint32_t>(exc2_fields[i]->value()) * 1000000ULL;
        }
    } else {
        auto* v = static_cast<SweepWindowGroup2View*>(active_view_);
        state_buffer_.sw3_enabled = v->check_sw3_enabled_.value();
        state_buffer_.sw3_start = static_cast<uint32_t>(v->field_sw3_start_.value()) * 1000000ULL;
        state_buffer_.sw3_end = static_cast<uint32_t>(v->field_sw3_end_.value()) * 1000000ULL;
        state_buffer_.sw3_step = static_cast<uint32_t>(v->field_sw3_step_.value()) * 1000ULL;

        state_buffer_.sw4_enabled = v->check_sw4_enabled_.value();
        state_buffer_.sw4_start = static_cast<uint32_t>(v->field_sw4_start_.value()) * 1000000ULL;
        state_buffer_.sw4_end = static_cast<uint32_t>(v->field_sw4_end_.value()) * 1000000ULL;
        state_buffer_.sw4_step = static_cast<uint32_t>(v->field_sw4_step_.value()) * 1000ULL;

        ui::NumberField* exc3_fields[] = {
            &v->field_sw3_exc0_, &v->field_sw3_exc1_,
            &v->field_sw3_exc2_, &v->field_sw3_exc3_,
            &v->field_sw3_exc4_};
        ui::NumberField* exc4_fields[] = {
            &v->field_sw4_exc0_, &v->field_sw4_exc1_,
            &v->field_sw4_exc2_, &v->field_sw4_exc3_,
            &v->field_sw4_exc4_};

        for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
            state_buffer_.sw3_exc[i] = static_cast<uint32_t>(exc3_fields[i]->value()) * 1000000ULL;
            state_buffer_.sw4_exc[i] = static_cast<uint32_t>(exc4_fields[i]->value()) * 1000000ULL;
        }
    }
}

void DroneSweepView::switch_tab(const uint8_t new_index) noexcept {
    if (new_index == active_tab_index_) return;

    serialize_active_view();
    destroy_active_view();
    active_tab_index_ = new_index;
    construct_view(new_index);

    button_tab1_.set_style(active_tab_index_ == 0 ? Theme::getInstance()->bg_darkest : Theme::getInstance()->bg_darkest);
    button_tab2_.set_style(active_tab_index_ == 1 ? Theme::getInstance()->bg_darkest : Theme::getInstance()->bg_darkest);
}

DroneSweepView::DroneSweepView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr) noexcept
    : ui::View()
    , nav_(nav)
    , scanner_ptr_(scanner_ptr)
    , original_config_(config)
    , button_tab1_{{0, 0, screen_width / 2 - 1, TAB_BAR_H - 1}, "Win 1-2"}
    , button_tab2_{{screen_width / 2 + 1, 0, screen_width / 2 - 1, TAB_BAR_H - 1}, "Win 3-4"} {
    state_buffer_.sw1_start = config.sweep_start_freq;
    state_buffer_.sw1_end = config.sweep_end_freq;
    state_buffer_.sw1_step = config.sweep_step_freq;
    state_buffer_.sw2_enabled = config.sweep2_enabled;
    state_buffer_.sw2_start = config.sweep2_start_freq;
    state_buffer_.sw2_end = config.sweep2_end_freq;
    state_buffer_.sw2_step = config.sweep2_step_freq;
    state_buffer_.sw3_enabled = config.sweep3_enabled;
    state_buffer_.sw3_start = config.sweep3_start_freq;
    state_buffer_.sw3_end = config.sweep3_end_freq;
    state_buffer_.sw3_step = config.sweep3_step_freq;
    state_buffer_.sw4_enabled = config.sweep4_enabled;
    state_buffer_.sw4_start = config.sweep4_start_freq;
    state_buffer_.sw4_end = config.sweep4_end_freq;
    state_buffer_.sw4_step = config.sweep4_step_freq;

    for (uint8_t w = 0; w < 4; ++w) {
        for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
            state_buffer_.sw1_exc[i] = config.sweep_exceptions[w < 2 ? w : w - 2][i];
        }
    }

    add_children({
        &button_tab1_,
        &button_tab2_,
        &labels_exc_radius_,
        &field_exc_radius_,
        &button_defaults_,
        &button_save_,
    });

    button_tab1_.on_select = [this](ui::Button&) { switch_tab(0); };
    button_tab2_.on_select = [this](ui::Button&) { switch_tab(1); };

    button_save_.on_select = [this](ui::Button&) {
        save_settings();
        nav_.pop();
    };

    button_defaults_.on_select = [this](ui::Button&) {
        apply_defaults();
    };

    field_exc_radius_.set_value(static_cast<int32_t>(config.exception_radius_mhz));

    button_tab1_.set_style(Theme::getInstance()->bg_darkest);
    button_tab2_.set_style(Theme::getInstance()->bg_darkest);

    active_tab_index_ = 0;
    tab_states_[0].state = TabState::CONSTRUCTED;
    construct_view(0);
}

DroneSweepView::~DroneSweepView() noexcept {
    if (active_view_ != nullptr) {
        remove_child(active_view_);
        active_view_->~View();
        active_view_ = nullptr;
    }
    chThdSleepMilliseconds(1);
}

void DroneSweepView::focus() {
    if (active_view_ != nullptr) {
        active_view_->focus();
    }
}

void DroneSweepView::save_settings() noexcept {
    serialize_active_view();

    FreqHz sw1_start = state_buffer_.sw1_start;
    FreqHz sw1_end = state_buffer_.sw1_end;
    FreqHz sw1_step = state_buffer_.sw1_step;
    bool sw2_enabled = state_buffer_.sw2_enabled;
    FreqHz sw2_start = state_buffer_.sw2_start;
    FreqHz sw2_end = state_buffer_.sw2_end;
    FreqHz sw2_step = state_buffer_.sw2_step;
    bool sw3_enabled = state_buffer_.sw3_enabled;
    FreqHz sw3_start = state_buffer_.sw3_start;
    FreqHz sw3_end = state_buffer_.sw3_end;
    FreqHz sw3_step = state_buffer_.sw3_step;
    bool sw4_enabled = state_buffer_.sw4_enabled;
    FreqHz sw4_start = state_buffer_.sw4_start;
    FreqHz sw4_end = state_buffer_.sw4_end;
    FreqHz sw4_step = state_buffer_.sw4_step;

    FreqHz exc[4][EXCEPTIONS_PER_WINDOW];
    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
        exc[0][i] = state_buffer_.sw1_exc[i];
        exc[1][i] = state_buffer_.sw2_exc[i];
        exc[2][i] = state_buffer_.sw3_exc[i];
        exc[3][i] = state_buffer_.sw4_exc[i];
    }

    // Read exception radius (1-100 MHz)
    const uint8_t exc_radius = static_cast<uint8_t>(field_exc_radius_.value());

    // Validate: start must be < end AND within hardware limits
    // HackRF One RFFC5072 mixer practical limit: 6 GHz
    if (sw1_start < HARDWARE_MIN_FREQ_HZ) sw1_start = HARDWARE_MIN_FREQ_HZ;
    if (sw1_end > HARDWARE_MAX_FREQ_HZ) sw1_end = HARDWARE_MAX_FREQ_HZ;
    if (sw1_start >= sw1_end) sw1_end = sw1_start + 20000000;
    if (sw2_enabled) {
        if (sw2_start < HARDWARE_MIN_FREQ_HZ) sw2_start = HARDWARE_MIN_FREQ_HZ;
        if (sw2_end > HARDWARE_MAX_FREQ_HZ) sw2_end = HARDWARE_MAX_FREQ_HZ;
        if (sw2_start >= sw2_end) sw2_end = sw2_start + 20000000;
    }
    if (sw3_enabled) {
        if (sw3_start < HARDWARE_MIN_FREQ_HZ) sw3_start = HARDWARE_MIN_FREQ_HZ;
        if (sw3_end > HARDWARE_MAX_FREQ_HZ) sw3_end = HARDWARE_MAX_FREQ_HZ;
        if (sw3_start >= sw3_end) sw3_end = sw3_start + 20000000;
    }
    if (sw4_enabled) {
        if (sw4_start < HARDWARE_MIN_FREQ_HZ) sw4_start = HARDWARE_MIN_FREQ_HZ;
        if (sw4_end > HARDWARE_MAX_FREQ_HZ) sw4_end = HARDWARE_MAX_FREQ_HZ;
        if (sw4_start >= sw4_end) sw4_end = sw4_start + 20000000;
    }

    // Update scanner config in memory
    if (scanner_ptr_ != nullptr) {
        ScanConfig updated_config = original_config_;
        updated_config.sweep_start_freq = sw1_start;
        updated_config.sweep_end_freq = sw1_end;
        updated_config.sweep_step_freq = sw1_step;
        updated_config.sweep2_start_freq = sw2_start;
        updated_config.sweep2_end_freq = sw2_end;
        updated_config.sweep2_step_freq = sw2_step;
        updated_config.sweep2_enabled = sw2_enabled;
        updated_config.sweep3_start_freq = sw3_start;
        updated_config.sweep3_end_freq = sw3_end;
        updated_config.sweep3_step_freq = sw3_step;
        updated_config.sweep3_enabled = sw3_enabled;
        updated_config.sweep4_start_freq = sw4_start;
        updated_config.sweep4_end_freq = sw4_end;
        updated_config.sweep4_step_freq = sw4_step;
        updated_config.sweep4_enabled = sw4_enabled;
        for (uint8_t w = 0; w < 4; ++w) {
            for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
                updated_config.sweep_exceptions[w][i] = exc[w][i];
            }
        }
        updated_config.exception_radius_mhz = exc_radius;
        (void)scanner_ptr_->set_config(updated_config);
    }

    // Save to SD card via centralized settings manager
    // Read current settings first (preserves non-sweep settings)
    SettingsStruct current;
    (void)SettingsFileManager::load(current);

    // Update sweep fields from UI
    current.sweep_start_freq = sw1_start;
    current.sweep_end_freq = sw1_end;
    current.sweep_step_freq = sw1_step;
    current.sweep2_start_freq = sw2_start;
    current.sweep2_end_freq = sw2_end;
    current.sweep2_step_freq = sw2_step;
    current.sweep2_enabled = sw2_enabled;
    current.sweep3_start_freq = sw3_start;
    current.sweep3_end_freq = sw3_end;
    current.sweep3_step_freq = sw3_step;
    current.sweep3_enabled = sw3_enabled;
    current.sweep4_start_freq = sw4_start;
    current.sweep4_end_freq = sw4_end;
    current.sweep4_step_freq = sw4_step;
    current.sweep4_enabled = sw4_enabled;
    for (uint8_t w = 0; w < 4; ++w) {
        for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
            current.sweep_exceptions[w][i] = exc[w][i];
        }
    }
    current.exception_radius_mhz = exc_radius;

    (void)SettingsFileManager::save(scanner_ptr_, current);
}

void DroneSweepView::apply_defaults() noexcept {
    serialize_active_view();

    SettingsStruct defaults;

    state_buffer_.sw1_start = defaults.sweep_start_freq;
    state_buffer_.sw1_end = defaults.sweep_end_freq;
    state_buffer_.sw1_step = defaults.sweep_step_freq;
    state_buffer_.sw2_enabled = defaults.sweep2_enabled;
    state_buffer_.sw2_start = defaults.sweep2_start_freq;
    state_buffer_.sw2_end = defaults.sweep2_end_freq;
    state_buffer_.sw2_step = defaults.sweep2_step_freq;
    state_buffer_.sw3_enabled = defaults.sweep3_enabled;
    state_buffer_.sw3_start = defaults.sweep3_start_freq;
    state_buffer_.sw3_end = defaults.sweep3_end_freq;
    state_buffer_.sw3_step = defaults.sweep3_step_freq;
    state_buffer_.sw4_enabled = defaults.sweep4_enabled;
    state_buffer_.sw4_start = defaults.sweep4_start_freq;
    state_buffer_.sw4_end = defaults.sweep4_end_freq;
    state_buffer_.sw4_step = defaults.sweep4_step_freq;

    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
        state_buffer_.sw1_exc[i] = 0;
        state_buffer_.sw2_exc[i] = 0;
        state_buffer_.sw3_exc[i] = 0;
        state_buffer_.sw4_exc[i] = 0;
    }

    field_exc_radius_.set_value(static_cast<int32_t>(DEFAULT_EXCEPTION_RADIUS_MHZ));

    if (active_view_ != nullptr) {
        remove_child(active_view_);
        active_view_->~View();
        active_view_ = nullptr;
    }
    tab_states_[active_tab_index_].state = TabState::INACTIVE;
    construct_view(active_tab_index_);
}

} // namespace drone_analyzer

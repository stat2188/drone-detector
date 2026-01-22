#include <iostream>
#include <cassert>

struct FastMedianFilter {
private:
    static constexpr size_t WINDOW_SIZE = 11;
    unsigned char window_[WINDOW_SIZE] = {};
    size_t head_ = 0;
    bool full_ = false;

public:
    void add_sample(unsigned char value) {
        window_[head_] = value;
        head_ = (head_ + 1) % WINDOW_SIZE;
        if (head_ == 0) full_ = true;
    }

    unsigned char get_median() const {
        if (!full_) return 0;

        unsigned char temp[WINDOW_SIZE];
        for (size_t i = 0; i < WINDOW_SIZE; i++) {
            temp[i] = window_[i];
        }

        size_t k = WINDOW_SIZE / 2;
        for (size_t i = 0; i <= k; ++i) {
            size_t min_idx = i;
            for (size_t j = i + 1; j < WINDOW_SIZE; ++j) {
                if (temp[j] < temp[min_idx]) {
                    min_idx = j;
                }
            }
            if (min_idx != i) {
                unsigned char tmp = temp[i];
                temp[i] = temp[min_idx];
                temp[min_idx] = tmp;
            }
        }
        return temp[k];
    }

    void reset() {
        full_ = false;
        head_ = 0;
        for (size_t i = 0; i < WINDOW_SIZE; i++) {
            window_[i] = 0;
        }
    }
};

void test_empty_filter_returns_zero() {
    FastMedianFilter filter;
    assert(filter.get_median() == 0);
    std::cout << "PASS: Empty filter returns zero" << std::endl;
}

void test_full_window_calculates_correct_median() {
    FastMedianFilter filter;
    
    for (unsigned char i = 0; i < 11; i++) {
        filter.add_sample(i);
    }
    
    assert(filter.get_median() == 5);
    std::cout << "PASS: Full window calculates correct median (5)" << std::endl;
}

void test_odd_number_of_samples() {
    FastMedianFilter filter;
    
    for (unsigned char i = 0; i < 11; i++) {
        filter.add_sample(i * 2);
    }
    
    assert(filter.get_median() == 10);
    std::cout << "PASS: Odd number of samples, median = 10" << std::endl;
}

void test_reset_clears_window() {
    FastMedianFilter filter;
    
    for (unsigned char i = 0; i < 11; i++) {
        filter.add_sample(100);
    }
    
    assert(filter.get_median() == 100);
    
    filter.reset();
    
    assert(filter.get_median() == 0);
    std::cout << "PASS: Reset clears window" << std::endl;
}

void test_circular_buffer_behavior() {
    FastMedianFilter filter;
    
    for (unsigned char i = 0; i < 22; i++) {
        filter.add_sample(i % 11);
    }
    
    assert(filter.get_median() == 5);
    std::cout << "PASS: Circular buffer behavior" << std::endl;
}

int main() {
    std::cout << "=== Running FastMedianFilter Tests ===" << std::endl;
    
    test_empty_filter_returns_zero();
    test_full_window_calculates_correct_median();
    test_odd_number_of_samples();
    test_reset_clears_window();
    test_circular_buffer_behavior();
    
    std::cout << "=== All tests passed! ===" << std::endl;
    return 0;
}

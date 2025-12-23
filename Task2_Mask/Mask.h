#pragma once
#include <array>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm> 
#include <iterator>  

template <size_t N>
class Mask {
private:
    std::array<int, N> _mask;

public:
    // Конструктор с Variadic Templates.
    // Позволяет добиться ОШИБКИ КОМПИЛЯЦИИ, если количество аргументов не равно N.
    // Поддерживает синтаксис: Mask<4> m = {1, 1, 0, 1};
    template <typename... Args>
    Mask(Args... args) : _mask{static_cast<int>(args)...} {
        static_assert(sizeof...(Args) == N, "Number of arguments must match Mask size N");
        
        // Дополнительная проверка: маска может содержать только 0 и 1
        for (int val : _mask) {
            if (val != 0 && val != 1) {
                throw std::invalid_argument("Mask can only contain 1 and 0");
            }
        }
    }

    // Метод size
    size_t size() const {
        return N;
    }

    // Метод at с проверкой границ
    int at(size_t index) const {
        if (index >= N) {
            throw std::out_of_range("Index out of range in Mask");
        }
        return _mask[index];
    }

    // Метод SLICE
    // Видоизменяет переданный контейнер, удаляя элементы, где маска = 0.
    // Использует метод erase(), поэтому подходит для vector, deque, list и т.д.
    template <typename Container>
    void slice(Container& c) {
        if (c.empty()) return;

        // Используем итераторный подход для изменения "на лету"
        auto it = c.begin();
        size_t idx = 0;

        while (it != c.end()) {
            // Если маска закончилась, начинаем сначала (idx % N)
            // Если маска длиннее, просто берем текущий индекс (idx % N все равно сработает корректно)
            int mask_val = _mask[idx % N];

            if (mask_val == 0) {
                // Удаляем элемент
                it = c.erase(it);
            } else {
                // Оставляем элемент, идем дальше
                ++it;
            }
            idx++;
        }
    }

    // Метод TRANSFORM
    // Возвращает Новый контейнер того же размера
    // Элементы под маской 1 изменены функцией func, остальные без изменений
    template <typename Container, typename Func>
    Container transform(const Container& c, Func func) {
        Container result = c; // Копируем контейнер (того же размера)
        
        size_t idx = 0;
        for (auto& item : result) {
            if (_mask[idx % N] == 1) {
                item = func(item);
            }
            idx++;
        }
        return result;
    }

    // Метод SLICE_AND_TRANSFORM
    // Возвращает НОВЫЙ контейнер, содержащий только элементы под маской 1 и к ним применена функция func.
    template <typename Container, typename Func>
    Container slice_and_transform(const Container& c, Func func) {
        Container result;
        // Резервируем память, если контейнер это поддерживает (оптимизация)
        // result.reserve(c.size()); 
        
        // Для универсальности используем back_inserter, но тогда нужен алгоритм.
        // Проще через цикл:
        size_t idx = 0;
        for (const auto& item : c) {
            if (_mask[idx % N] == 1) {
                // Вставляем видоизмененный элемент
                // Предполагается, что контейнер поддерживает push_back (vector, list, deque)
                result.push_back(func(item));
            }
            idx++;
        }
        
        return result;
    }
};
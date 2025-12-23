#include <iostream>
#include <vector>
#include <cmath>
#include <iterator>
#include <algorithm>

// Класс генератора псевдослучайных чисел
class SimpleRNG {
private:
    double m, a, c; // Параметры формулы
    double x0;      // Начальное состояние
    double current; // Текущее состояние

public:
    // Конструктор: задает параметры m, a, c
    SimpleRNG(double m, double a, double c) : m(m), a(a), c(c), x0(0), current(0) {}

    // Установка начального состояния
    void reset(double start_val) {
        x0 = start_val;
        current = start_val;
    }

    // Сброс к ранее заданному начальному состоянию
    void reset() {
        current = x0;
    }

    // Вложенный класс итератора
    class Iterator {
    public:
        // Трейты итератора
        using iterator_category = std::input_iterator_tag;
        using value_type        = double;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const double*;
        using reference         = const double&;

    private:
        double val;             // Текущее значение числа
        double m, a, c;         // Копии параметров
        double target_start;    // Значение, с которого начался цикл (для проверки end)
        double eps;             // Точность сравнения
        bool is_sentinel;       // Флаг итератор конца
        bool started;           // Флаг был ли сделан шаг

    public:
        // Конструктор для begin
        Iterator(double v, double m, double a, double c) 
            : val(v), m(m), a(a), c(c), target_start(v), eps(0), is_sentinel(false), started(false) {}

        // Конструктор для end
        Iterator(double target, double eps) 
            : val(0), m(0), a(0), c(0), target_start(target), eps(eps), is_sentinel(true), started(false) {}

        // Оператор разыменования: возвращает текущее число
        double operator*() const {
            return val;
        }

        // Переход к следующему числу
        Iterator& operator++() {
            // Формула: X[N+1] = ( a * X[N] + c ) % m
            val = std::fmod(a * val + c, m);
            started = true;
            return *this;
        }

        // Шаг вперед
        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        // Оператор неравенства
        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

        // Оператор равенства
        bool operator==(const Iterator& other) const {
            if (other.is_sentinel) {
                // Если сравниваем текущий итератор с end
                // Останавливаемся, если:
                // 1. Мы уже начали двигаться (started == true)
                // 2. Текущее значение вернулось к начальному с точностью eps
                if (started) {
                    double diff = std::abs(val - other.target_start);
                    if (diff < other.eps) {
                        return true; // Цикл замкнулся
                    }
                }
                return false;
            } else {
                // Сравнение двух обычных итераторов
                return val == other.val;
            }
        }
    };

    // begin возвращает итератор на текущее состояние
    Iterator begin() {
        return Iterator(current, m, a, c);
    }

    // end возвращает итератор-часовой с заданной точностью
    Iterator end(double eps = 0.05) {
        return Iterator(current, eps);
    }
};

int main() {
    // Создаем генератор с параметрами из задания
    SimpleRNG generator(5, 0.2, 1);
    
    // Вектор
    std::cout << "Part 1: Vector" << std::endl;
    generator.reset(0.4);
    
    std::vector<double> vec;
    auto it = generator.begin();
    auto end_it = generator.end(0.5); // Точность 0.5
    
    int safety_limit = 20; // Ограничиваем вывод 20 числами
    
    while(it != end_it && safety_limit > 0) {
        vec.push_back(*it);
        ++it;
        --safety_limit;
    }

    std::cout << "Generated vector: ";
    for(const auto& v : vec) {
        std::cout << v << " ";
    }
    std::cout << std::endl << std::endl;

    //Цикл for
    std::cout << "Part 2: Loop" << std::endl;
    generator.reset(0); // Сброс в 0
    
    std::cout << "Loop sequence: ";
    
    int counter = 0;
    // Range-based for использует begin и end (по умолчанию eps=0.05)
    for(auto x : generator) {
        std::cout << x << " ";
        
        // Тдобавляем защиту от бесконечного цикла
        counter++;
        if (counter >= 20) {
            std::cout << "[Stopped manually]";
            break;
        }
    }
    std::cout << std::endl;
    std::cout << "\nPress Enter to exit";
    std::cin.get();
    return 0;

}

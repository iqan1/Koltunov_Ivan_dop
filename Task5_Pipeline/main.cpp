#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include "Pipeline.h"

// Создаем объект который выглядит как функция
struct {
    template<typename T>
    auto operator()(const T& t) const { return t.size(); }
} size_fn; 

int main() {
    std::cout << "Example 1: Lazy Execution (saved to variable)" << std::endl;
    {
        std::string str = "Hello World!";
        
        // str | size_fn создает Node
        // Node | lambda1 создает Node
        // Node | lambda2 создает Node
        auto pipeline = str | size_fn 
                            | [](auto x){ return x * 2; } 
                            | [](auto x){ std::cout << "Result inside pipeline: " << x << std::endl; };
        
        std::cout << "Pipeline created. Executing now" << std::endl;
        pipeline(); // Явный запуск. Выведет 24 (12 * 2)
    } 
    // Здесь сработает деструктор pipeline, но так как executed = true, повторного вывода не будет

    std::cout << "\nExample 2: Immediate Execution (temporary object)" << std::endl;
    {
        std::string str = "Hello World!";
        
        // Здесь создается временный объект PipelineNode
        // Мы не вызываем (), но пайплайн заканчивается точкой с запятой
        // В конце строки временный объект уничтожается -> вызывается деструктор -> вызывается chain()
        str | size_fn 
            | [](auto x){ return x * 2; } 
            | [](auto x){ std::cout << "Immediate result: " << x << std::endl; };
            
        // Ожидается вывод 24 (Hello World! = 12 chars * 2)
    }

    std::cout << "\nExample 3: Mixed Types" << std::endl;
    {
        int start_val = 5;
        // int -> +5 -> string -> print
        start_val | [](int x) { return x + 5; } // 10
                  | [](int x) { return std::to_string(x) + " apples"; } // "10 apples"
                  | [](const std::string& s) { std::cout << "String processing: " << s << std::endl; };
    }

    // Ожидание ввода для завершенния программы
    std::cout << "\nPress Enter to exit";
    std::cin.get();

    return 0;
}
#include <iostream>
#include <vector>
#include "Mask.h"

int main() {
    try {
        // 1. Создание маски
        std::cout << "Test 1: Creation and slice" << std::endl;
        Mask<3> mask = {1, 0, 0}; // Маска размера 3: [1, 0, 0]
        
        // Тестовый вектор: [1 2 3 4 5 6 7]
        std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7};
        
        std::cout << "Original vector: ";
        for(int x : vec) std::cout << x << " ";
        std::cout << std::endl;

        // 2. Применение метода slice
        // Маска [1 0 0] применяется циклично:
        // 1(Keep), 2(Drop), 3(Drop), 4(Keep), 5(Drop), 6(Drop), 7(Keep) -> [1, 4, 7]
        mask.slice(vec);

        std::cout << "Sliced vector:   ";
        for(int x : vec) std::cout << x << " ";
        std::cout << std::endl; // Ожидается: 1 4 7

        
        // 3. Тест transform
        std::cout << "\nTest 2: Transform" << std::endl;
        Mask<3> mask2 = {1, 1, 0};
        std::vector<int> vec2 = {10, 20, 30, 40, 50, 60};
        // Маска: 1(mod), 1(mod), 0(skip), 1(mod), 1(mod), 0(skip)
        
        // Функция умножения на 2
        auto res_trans = mask2.transform(vec2, [](int x){ return x * 2; });
        
        std::cout << "Original:    ";
        for(int x : vec2) std::cout << x << " ";
        std::cout << std::endl;
        
        std::cout << "Transformed: ";
        for(int x : res_trans) std::cout << x << " "; 
        std::cout << std::endl;
        // Ожидается: 20 40 30 80 100 60


        // 4. Тест slice_and_transform
        std::cout << "\nTest 3: Slice and Transform" << std::endl;
        // Берем только те, где 1, и умножаем их на 10
        auto res_slice_trans = mask2.slice_and_transform(vec2, [](int x){ return x * 10; });
        
        std::cout << "Slice&Trans: ";
        for(int x : res_slice_trans) std::cout << x << " ";
        std::cout << std::endl;
        // Ожидается (из 10 20 30 40 50 60):
        // 10*10, 20*10, skip, 40*10, 50*10, skip -> 100 200 400 500

        // 5. Тест на ошибку компиляции
        // Mask<4> wrong_size = {1, 1, 0}; // ОШИБКА КОМПИЛЯЦИИ: static assertion failed

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    std::cout << "\nPress Enter to exit";
    std::cin.get();
    return 0;
}
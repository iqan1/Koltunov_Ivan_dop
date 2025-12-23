#include <iostream>
#include "MemReserver.h"

// Тестовый класс, чтобы видеть, когда вызываются конструкторы и деструкторы
class SomeClass {
public:
    int id;
    SomeClass(int a = 0, int b = 0, int c = 0) {
        id = a + b + c;
        std::cout << " [SomeClass constructed] ID:" << id << std::endl;
    }
    ~SomeClass() {
        std::cout << " [SomeClass destroyed] ID:" << id << std::endl;
    }
};

int main() {
    std::cout << "Start of Memory Test \n";

    // Создаем резервуар на 2 элемента
    MemReserver<SomeClass, 2> reserver;

    try {
        // 1. Создаем объекты
        std::cout << "Creating obj1...\n";
        auto& obj1 = reserver.create();       // Конструктор по умолчанию

        std::cout << "Creating obj2...\n";
        auto& obj2 = reserver.create(1, 2, 3); // Конструктор с параметрами (ID = 6)

        std::cout << "Current count: " << reserver.count() << "\n";

        // 2. Пытаемся создать третий (должна быть ошибка)
        std::cout << "Creating obj3 (should fail)\n";
        reserver.create();

    } catch (const NotEnoughSlotsError& e) {
        std::cout << "EXCEPTION CAUGHT: " << e.what() << "\n";
    }

    // 3. Тест position и удаление
    try {
        // Получаем ссылку на первый объект
        auto& objRef = reserver.get(0);
        size_t pos = reserver.position(objRef);
        std::cout << "Position of obj1 is: " << pos << "\n";

        std::cout << "Deleting object at position " << pos << "...\n";
        reserver._delete(pos); // Метод назван _delete в классе

    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }

    // 4. Проверка ошибки доступа к удаленному
    try {
        std::cout << "Trying to access deleted object at index 0...\n";
        auto& temp = reserver.get(0);
        (void)temp;
    } catch (const EmptySlotError& e) {
        std::cout << "EXCEPTION CAUGHT: " << e.what() << "\n";
    }

    std::cout << "End of main (Remaining objects will be destroyed automatically)\n";
    
    std::cout << "\nPress Enter to exit";
    std::cin.get();
    
    return 0;
}
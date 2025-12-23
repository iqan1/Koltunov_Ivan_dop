#pragma once
#include <cstddef>
#include <new>         
#include <utility>      
#include <stdexcept>
#include <string>
#include <iostream>

// Классы исключений

class NotEnoughSlotsError : public std::exception {
    std::string msg;
public:
    explicit NotEnoughSlotsError(size_t current_count) {
        msg = "Not enough slots. Already created: " + std::to_string(current_count);
    }
    const char* what() const noexcept override { return msg.c_str(); }
};

class EmptySlotError : public std::exception {
    std::string msg;
public:
    explicit EmptySlotError(size_t index) {
        msg = "Slot is empty or index out of range: " + std::to_string(index);
    }
    const char* what() const noexcept override { return msg.c_str(); }
};

class ObjectNotFoundError : public std::exception {
public:
    const char* what() const noexcept override { return "Object not found in reserver"; }
};

//Шаблонный класс MemReserver

template <typename T, size_t N>
class MemReserver {
private:
    // Структура для хранения одного элемента
    struct Slot {
        //Сырая память, выровненная под тип T
        alignas(T) unsigned char data[sizeof(T)];
        bool is_active = false;
    };

    //Статический массив слотов (на стеке или внутри объекта)
    Slot slots[N];
    size_t active_count = 0;

public:
    MemReserver() = default;

    //Деструктор: должен удалить все оставшиеся объекты
    ~MemReserver() {
        for (size_t i = 0; i < N; ++i) {
            if (slots[i].is_active) {
                //Приводим сырую память к указателю на T и вызываем деструктор
                T* ptr = reinterpret_cast<T*>(slots[i].data);
                ptr->~T();
                slots[i].is_active = false;
            }
        }
    }

    // Метод create: создает объект in-place
    template <typename... Args>
    T& create(Args&&... args) {
        // Ищем первый свободный слот
        for (size_t i = 0; i < N; ++i) {
            if (!slots[i].is_active) {
                // Используем placement new для создания объекта в памяти slot.data
                // std::forward позволяет идеально передать аргументы конструктору T
                new (slots[i].data) T(std::forward<Args>(args)...);
                
                slots[i].is_active = true;
                active_count++;
                
                return *reinterpret_cast<T*>(slots[i].data);
            }
        }
        // Если свободного места нет
        throw NotEnoughSlotsError(active_count);
    }

    // Метод delete: удаляет объект по индексу
    void _delete(size_t index) { // Назвал _delete, так как delete - ключевое слово
        if (index >= N || !slots[index].is_active) {
            throw EmptySlotError(index);
        }

        T* ptr = reinterpret_cast<T*>(slots[index].data);
        ptr->~T(); // Явный вызов деструктора
        
        slots[index].is_active = false;
        active_count--;
    }

    // Метод count
    size_t count() const {
        return active_count;
    }

    // Метод get: получение объекта по индексу
    T& get(size_t index) {
        if (index >= N || !slots[index].is_active) {
            throw EmptySlotError(index);
        }
        return *reinterpret_cast<T*>(slots[index].data);
    }

    // Метод position: поиск индекса по ссылке на объект
    size_t position(const T& obj) {
        // Получаем адрес переданного объекта
        const T* target_ptr = &obj;

        // Проверяем, принадлежит ли этот адрес нашему хранилищу
        for (size_t i = 0; i < N; ++i) {
            if (slots[i].is_active) {
                const T* current_ptr = reinterpret_cast<const T*>(slots[i].data);
                if (current_ptr == target_ptr) {
                    return i;
                }
            }
        }
        throw ObjectNotFoundError();
    }
};
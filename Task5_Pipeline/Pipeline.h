#pragma once
#include <iostream>
#include <utility>
#include <type_traits>

// Проверка, является ли тип PipelineNode
template <typename T>
struct is_pipeline_node : std::false_type {};

template <typename P, typename O>
class PipelineNode; // Forward declaration

template <typename P, typename O>
struct is_pipeline_node<PipelineNode<P, O>> : std::true_type {};

//Класс узла пайплайна
template <typename Previous, typename Operation>
class PipelineNode {
private:
    Previous prev;       
    Operation op;        
    bool executed;       
    bool is_moved_from;  

public:
    // Шаблонный конструктор позволяющий принимать и l-value, и r-value
    template <typename P_Arg, typename O_Arg>
    PipelineNode(P_Arg&& p, O_Arg&& o) 
        : prev(std::forward<P_Arg>(p)), op(std::forward<O_Arg>(o)), 
          executed(false), is_moved_from(false) {}

    // Move конструктор
    PipelineNode(PipelineNode&& other) noexcept 
        : prev(std::move(other.prev)), op(std::move(other.op)), 
          executed(other.executed), is_moved_from(false) {
        other.is_moved_from = true;
    }

    PipelineNode(const PipelineNode&) = delete;
    PipelineNode& operator=(const PipelineNode&) = delete;

    ~PipelineNode() {
        if (!executed && !is_moved_from) {
            (*this)(); // Запуск при уничтожении
        }
    }

    // Логика выполнения
    // Если prev - это тоже узел пайплайна
    template <typename T = Previous>
    auto run_prev(typename std::enable_if<is_pipeline_node<T>::value>::type* = 0) 
    -> decltype(std::declval<T>()()) {
        return prev();
    }

    //Если prev - это просто значение
    template <typename T = Previous>
    auto run_prev(typename std::enable_if<!is_pipeline_node<T>::value>::type* = 0) 
    -> T& {
        return prev;
    }

    // Оператор вызова ()
    auto operator()() {
        executed = true;
        return op(run_prev());
    }
};

//Глобальный оператор

//Продолжение цепочки: Узел | Функция
template <typename P, typename O, typename Func>
auto operator|(PipelineNode<P, O>&& node, Func&& func) {
    using NodeT = PipelineNode<P, O>;
    using FuncT = typename std::decay<Func>::type;
    
    return PipelineNode<NodeT, FuncT>(
        std::move(node), std::forward<Func>(func)
    );
}

//Начало цепочки: Значение | Функция
template <typename T, typename Func>
auto operator|(T&& val, Func&& func) 
-> typename std::enable_if<
    !is_pipeline_node<typename std::decay<T>::type>::value, 
    PipelineNode<typename std::decay<T>::type, typename std::decay<Func>::type>
   >::type 
{
    using ValT = typename std::decay<T>::type;
    using FuncT = typename std::decay<Func>::type;
    return PipelineNode<ValT, FuncT>(
        std::forward<T>(val), std::forward<Func>(func)
    );
}
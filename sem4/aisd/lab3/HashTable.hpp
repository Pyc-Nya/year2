// HashTable.hpp
#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <vector>      // std::vector для хранения списка списков бакетов
#include <list>        // std::list для хранения цепочек и списка ключей
#include <functional>  // std::hash и std::equal_to для дефолтных функторов
#include <algorithm>   // std::find, std::sort, std::merge
#include <iterator>    // std::back_inserter
#include <stdexcept>   // std::out_of_range
#include <iostream>    // std::cout для методов вывода

// Шаблонный класс хеш‑таблицы (множество ключей без значений)
template<typename T,
         typename Hash = std::hash<T>,        // функтор хеширования по умолчанию
         typename KeyEqual = std::equal_to<T> // функтор сравнения ключей
>
class HashTable {
public:
    using value_type     = T;                    // тип хранимого ключа
    using hasher         = Hash;                 // тип хеш‑функции
    using key_equal      = KeyEqual;             // тип функции сравнения ключей
    using const_iterator = typename std::list<T>::const_iterator; // итератор по ключам

    // Конструктор: задать число бакетов и максимальную мощность множества
    explicit HashTable(size_t buckets = 16, size_t max_size = 10)
        : buckets_(buckets)    // инициализация числа бакетов
        , size_(0)             // инициализация текущего размера
        , max_size_(max_size)  // инициализация лимита мощности
        , table_(buckets)      // инициализация вектора пустых цепочек
    {}

    // Основные методы:
    bool insert(const T& key);     // вставить ключ, если нет и не превышен лимит
    bool contains(const T& key) const; // проверить наличие ключа
    bool erase(const T& key);      // удалить ключ, если найден
    void clear() noexcept;         // очистить все ключи
    void print() const;            // вывести содержимое
    size_t size() const noexcept { return size_; } // вернуть число ключей

    // Итераторы по множеству
    const_iterator begin() const noexcept { return entries_.begin(); }
    const_iterator end()   const noexcept { return entries_.end();   }

    // Операции над последовательностями (упорядоченными):
    HashTable MERGE(const HashTable& other) const;  // слияние двух отсортированных списков
    HashTable CONCAT(const HashTable& other) const; // конкатенация списков
    HashTable ERASE(size_t p1, size_t p2) const;    // удалить диапазон по индексам

    // Двуместные операции над множествами:
    HashTable unionSet(const HashTable& other) const;      // объединение A ∪ B
    HashTable intersection(const HashTable& other) const;  // пересечение A ∩ B
    HashTable difference(const HashTable& other) const;    // разность A \ B
    HashTable symDifference(const HashTable& other) const; // симметрическая разность A ⊕ B

private:
    size_t buckets_;   // число бакетов (списки коллизий)
    size_t size_;      // текущее число элементов
    size_t max_size_;  // максимальная мощность (лимит)
    std::vector<std::list<T>> table_; // вектор цепочек для коллизий
    std::list<T> entries_;            // список всех ключей для итератора и вывода
    Hash hasher_;                     // функтор хеширования
    KeyEqual equal_;                  // функтор сравнения ключей

    void rehash(size_t new_buckets);  // перераспределение по новому числу бакетов
    typename std::list<T>::iterator findEntry(const T& key);       // поиск в entries_
    typename std::list<T>::const_iterator findEntry(const T& key) const; // константная версия
};

// Вспомогательная функция: извлечь и отсортировать все ключи
template<typename T, typename H, typename E>
static void ensure_sorted(const HashTable<T,H,E>& ht, std::vector<T>& out) {
    out.assign(ht.begin(), ht.end());     // скопировать все ключи
    std::sort(out.begin(), out.end());    // отсортировать
}

// Реализация методов:
// insert — вставить ключ, если нет и если размер меньше max_size_
template<typename T, typename H, typename E>
bool HashTable<T,H,E>::insert(const T& key) {
    if (contains(key))        // если ключ уже есть
        return false;
    if (size_ >= max_size_) { // если достигнут лимит мощности
        std::cout << "Ошибка: множество переполнено (макс. мощность = "
                  << max_size_ << ")\n";
        return false;
    }
    entries_.push_back(key);           // добавить к общему списку ключей
    size_t h = hasher_(key) % buckets_; // вычислить индекс бакета
    table_[h].push_back(key);          // вставить в цепочку коллизий
    ++size_;                           // увеличить счётчик
    if (size_ > buckets_)              // если load factor > 1
        rehash(buckets_ * 2);         // — увеличить число бакетов вдвое
    return true;
}

// erase — удалить ключ
template<typename T, typename H, typename E>
bool HashTable<T,H,E>::erase(const T& key) {
    auto it = findEntry(key);          // найти в списке entries_
    if (it == entries_.end())          // если не найден
        return false;
    entries_.erase(it);                // удалить из entries_
    size_t h = hasher_(key) % buckets_;
    table_[h].remove(key);             // удалить из цепочки коллизий
    --size_;                           // уменьшить счётчик
    return true;
}

// contains — проверить наличие ключа
template<typename T, typename H, typename E>
bool HashTable<T,H,E>::contains(const T& key) const {
    return findEntry(key) != entries_.end();
}

// clear — очистить все ключи
template<typename T, typename H, typename E>
void HashTable<T,H,E>::clear() noexcept {
    size_ = 0;
    entries_.clear();                   // очистить список ключей
    for (auto& chain : table_)         // очистить каждую цепочку
        chain.clear();
}

// findEntry — линейный поиск в entries_
template<typename T, typename H, typename E>
typename std::list<T>::iterator
HashTable<T,H,E>::findEntry(const T& key) {
    return std::find(entries_.begin(), entries_.end(), key);
}

template<typename T, typename H, typename E>
typename std::list<T>::const_iterator
HashTable<T,H,E>::findEntry(const T& key) const {
    return std::find(entries_.begin(), entries_.end(), key);
}

// rehash — перераспределить ключи по новому числу бакетов
template<typename T, typename H, typename E>
void HashTable<T,H,E>::rehash(size_t new_buckets) {
    buckets_ = new_buckets;
    table_.clear();
    table_.resize(buckets_);            // создать новые пустые цепочки
    for (const T& key : entries_) {     // для каждого ключа
        size_t h = hasher_(key) % buckets_;
        table_[h].push_back(key);       // вставить в новую цепочку
    }
}

// MERGE — слияние двух отсортированных списков (с дубликатами)
template<typename T, typename H, typename E>
HashTable<T,H,E> HashTable<T,H,E>::MERGE(const HashTable& other) const {
    std::vector<T> v1, v2, result;
    ensure_sorted(*this, v1);          // получить и отсортировать текущие ключи
    ensure_sorted(other, v2);          // получить и отсортировать ключи other
    result.reserve(v1.size() + v2.size());
    std::merge(v1.begin(), v1.end(),   // стандартная функция слияния
               v2.begin(), v2.end(),
               std::back_inserter(result));
    HashTable ht(v1.size() + v2.size(), max_size_);
    for (const T& x : result) ht.insert(x);
    return ht;
}

// CONCAT — конкатенация списков (дубли сохраняются)
template<typename T, typename H, typename E>
HashTable<T,H,E> HashTable<T,H,E>::CONCAT(const HashTable& other) const {
    std::vector<T> v1, v2, result;
    ensure_sorted(*this, v1);
    ensure_sorted(other, v2);
    result.reserve(v1.size() + v2.size());
    result.insert(result.end(), v1.begin(), v1.end()); // добавить все из v1
    result.insert(result.end(), v2.begin(), v2.end()); // затем все из v2
    HashTable ht(v1.size() + v2.size(), max_size_);
    for (const T& x : result) ht.insert(x);
    return ht;
}

// ERASE — удалить диапазон элементов по индексам из упорядоченного списка
template<typename T, typename H, typename E>
HashTable<T,H,E> HashTable<T,H,E>::ERASE(size_t p1, size_t p2) const {
    if (p1 > p2 || p2 > size_)
        throw std::out_of_range("Invalid ERASE range");
    std::vector<T> v;
    ensure_sorted(*this, v);
    HashTable ht(size_ - (p2 - p1), max_size_);
    for (size_t i = 0; i < v.size(); ++i) {
        if (i < p1 || i >= p2)
            ht.insert(v[i]);             // вставлять только вне диапазона
    }
    return ht;
}

// unionSet — объединение множеств A ∪ B
template<typename T, typename H, typename E>
HashTable<T,H,E> HashTable<T,H,E>::unionSet(const HashTable& other) const {
    HashTable ht(size_ + other.size(), max_size_);
    for (auto& x : *this) ht.insert(x);               // добавить все из A
    for (auto& x : other) ht.insert(x);               // добавить все из B (повторы игнорируются)
    return ht;
}

// intersection — пересечение множеств A ∩ B
template<typename T, typename H, typename E>
HashTable<T,H,E> HashTable<T,H,E>::intersection(const HashTable& other) const {
    const HashTable *small = (size_ < other.size_) ? this : &other; // меньшая таблица
    const HashTable *big   = (size_ < other.size_) ? &other : this;
    HashTable ht(small->size_, max_size_);
    for (auto it = small->begin(); it != small->end(); ++it) {
        if (big->contains(*it))                          // если элемент есть в обоих
            ht.insert(*it);
    }
    return ht;
}

// difference — разность множеств A \ B
template<typename T, typename H, typename E>
HashTable<T,H,E> HashTable<T,H,E>::difference(const HashTable& other) const {
    HashTable ht(size_, max_size_);
    for (auto& x : *this) {
        if (!other.contains(x))                            // добавить только если нет в B
            ht.insert(x);
    }
    return ht;
}

// symDifference — симметрическая разность A ⊕ B
template<typename T, typename H, typename E>
HashTable<T,H,E> HashTable<T,H,E>::symDifference(const HashTable& other) const {
    return this->difference(other)                        // A \ B
           .unionSet(other.difference(*this));          // ∪ (B \ A)
}

// print — вывести список ключей в консоль в формате { k1, k2, ... }
template<typename T, typename H, typename E>
void HashTable<T,H,E>::print() const {
    std::cout << "{";
    for (auto it = entries_.begin(); it != entries_.end(); ++it) {
        std::cout << " " << *it;                           // вывести ключ
        if (std::next(it) != entries_.end())
            std::cout << ",";                              // разделитель
    }
    std::cout << " }\n";                                     // закрывающая скобка
}

#endif // HASHTABLE_HPP

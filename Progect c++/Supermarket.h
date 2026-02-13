#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <stdexcept>
using namespace std;

struct TOC {
    int day, month, year;

    bool isLeapYear() const {
        return (year % 4 == 0 and (year % 100 != 0 or year % 400 == 0));
    }

    bool isValidDate() const {
        if (month < 1 or month > 12) return false;
        int daysInMonth[] = { 31, 28 + isLeapYear(), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        return day > 0 and day <= daysInMonth[month - 1];
    }

    void printTOC() const {
        if (isValidDate()) {
            cout << (day < 10 ? "0" : "") << day << "."
                << (month < 10 ? "0" : "") << month << "."
                << year;
        }
        else {
            throw invalid_argument("Invalid date");
        }
    }
};

class Goods {
protected:
    static const int SIZE = 50;
    char name[SIZE]{};
    char category[SIZE]{};
    int price = 0;
    int quantity = 0;

public:
    void setName(const char* name) {
        if (!name or strlen(name) < 2 or strlen(name) >= SIZE) {
            throw invalid_argument("Error! Invalid name");
        }
        strncpy_s(this->name, name, SIZE - 1);
        this->name[SIZE - 1] = '\0';
    }

    void setCategory(const char* category) {
        if (!category or strlen(category) < 2 or strlen(category) >= SIZE) {
            throw invalid_argument("Error! Invalid category");
        }
        strncpy_s(this->category, category, SIZE - 1);
        this->category[SIZE - 1] = '\0';
    }

    void setPrice(int price) {
        if (price <= 0) {
            throw invalid_argument("Error! Price must be positive");
        }
        this->price = price;
    }

    void setQuantity(int quantity) {
        if (quantity < 0) {
            throw invalid_argument("Error! Quantity cannot be negative");
        }
        this->quantity = quantity;
    }

    const char* getName() const { return this->name; }
    const char* getCategory() const { return this->category; }
    int getQuantity() const { return this->quantity; }
    int getPrice() const { return this->price; }

    Goods() = default;

    Goods(const char* name, const char* category, int price, int quantity) {
        setName(name);
        setCategory(category);
        setPrice(price);
        setQuantity(quantity);
    }

    virtual void print() const {
        cout << "\t\t Item information" << endl;
        cout << "Name              :: " << this->name << endl;
        cout << "Category          :: " << this->category << endl;
        cout << "Price             :: " << this->price << " UAH" << endl;
        cout << "Quantity          :: " << this->quantity << endl;
    }

    virtual void serialize(ofstream& file) const {
        file.write(this->name, SIZE);
        file.write(this->category, SIZE);
        file.write(reinterpret_cast<const char*>(&price), sizeof(price));
        file.write(reinterpret_cast<const char*>(&quantity), sizeof(quantity));
    }

    virtual void deserialize(ifstream& file) {
        file.read(this->name, SIZE);
        file.read(this->category, SIZE);
        file.read(reinterpret_cast<char*>(&price), sizeof(price));
        file.read(reinterpret_cast<char*>(&quantity), sizeof(quantity));
        if (price <= 0 or quantity < 0) {
            throw runtime_error("Invalid data in file");
        }
    }

    virtual ~Goods() = default;
};

class Product : public Goods {
private:
    TOC data;

public:
    void setData(const TOC& toc) {
        if (!toc.isValidDate()) {
            throw invalid_argument("Invalid date");
        }
        this->data = toc;
    }

    void print() const override {
        cout << "\t\t Product information" << endl;
        cout << "Name              :: " << this->name << endl;
        cout << "Expiration date   :: ";
        this->data.printTOC();
        cout << endl;
        cout << "Category          :: " << this->category << endl;
        cout << "Price             :: " << this->price << " UAH" << endl;
        cout << "Quantity          :: " << this->quantity << endl;
    }

    Product() = default;

    Product(const char* name, const TOC& data, const char* category, int price, int quantity)
        : Goods(name, category, price, quantity) {
        setData(data);
    }

    void serialize(ofstream& file) const override {
        Goods::serialize(file);
        file.write(reinterpret_cast<const char*>(&data.day), sizeof(data.day));
        file.write(reinterpret_cast<const char*>(&data.month), sizeof(data.month));
        file.write(reinterpret_cast<const char*>(&data.year), sizeof(data.year));
    }

    void deserialize(ifstream& file) override {
        Goods::deserialize(file);
        file.read(reinterpret_cast<char*>(&data.day), sizeof(data.day));
        file.read(reinterpret_cast<char*>(&data.month), sizeof(data.month));
        file.read(reinterpret_cast<char*>(&data.year), sizeof(data.year));
        if (!data.isValidDate()) {
            throw runtime_error("Invalid date in file");
        }
    }
};

class Supermarket {
private:
    vector<Goods*> supermarketInventory;

public:
    void addGoods(Goods* goods) {
        auto it = find_if(supermarketInventory.begin(), supermarketInventory.end(),
            [&goods](Goods* p) {
                return strcmp(p->getName(), goods->getName()) == 0;
            });

        if (it != supermarketInventory.end()) {
            (*it)->setQuantity((*it)->getQuantity() + goods->getQuantity());
        }
        else {
            supermarketInventory.push_back(goods);
        }
    }

    void delete_(const char* name) {
        auto it = remove_if(supermarketInventory.begin(), supermarketInventory.end(),
            [name](Goods* goods) { return strcmp(goods->getName(), name) == 0; });
        if (it != supermarketInventory.end()) {
            delete* it;
            supermarketInventory.erase(it, supermarketInventory.end());
        }
        else {
            cout << "Item with name \"" << name << "\" not found." << endl;
        }
    }

    Goods* search(const char* name) const {
        auto it = find_if(supermarketInventory.begin(), supermarketInventory.end(),
            [name](Goods* goods) { return strcmp(goods->getName(), name) == 0; });
        if (it != supermarketInventory.end()) {
            return *it;
        }
        cout << "Item with name \"" << name << "\" not found." << endl;
        return nullptr;
    }

    void printSortByCategory() const {
        vector<Goods*> sortedInventory = supermarketInventory;
        sort(sortedInventory.begin(), sortedInventory.end(),
            [](Goods* a, Goods* b) { return strcmp(a->getCategory(), b->getCategory()) < 0; });

        for (const auto& goods : sortedInventory) {
            goods->print();
            cout << endl;
        }
    }

    void selling(const char* name, int amount) {
        auto it = find_if(supermarketInventory.begin(), supermarketInventory.end(),
            [name](Goods* goods) { return strcmp(goods->getName(), name) == 0; });
        if (it != supermarketInventory.end()) {
            Goods* goods = *it;
            if (goods->getQuantity() >= amount) {
                goods->setQuantity(goods->getQuantity() - amount);
                cout << "Sale of item \"" << name << "\" successful!" << endl;
            }
            else {
                cout << "Not enough \"" << name << "\" in stock." << endl;
            }
        }
        else {
            cout << "Item \"" << name << "\" not found." << endl;
        }
    }

    void save(ofstream& file) const {
        if (!file) {
            throw runtime_error("Failed to open file for writing");
        }

        size_t size = supermarketInventory.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));

        for (const auto& goods : supermarketInventory) {
            goods->serialize(file);
        }
    }

    void download(ifstream& file) {
        if (!file) {
            throw runtime_error("Failed to open file for reading");
        }

        file.seekg(0, ios::end);
        if (file.tellg() == 0) {
            cout << "File is empty. Loading skipped." << endl;
            return;
        }
        file.seekg(0, ios::beg);

        size_t size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
        if (!file) {
            throw runtime_error("Invalid file format or file is corrupted");
        }

        for (auto& goods : supermarketInventory) {
            delete goods;
        }
        supermarketInventory.clear();

        for (size_t i = 0; i < size; ++i) {
            streampos startPos = file.tellg();

            Goods* tempGoods = new Goods();
            tempGoods->deserialize(file);

            int day, month, year;
            file.read(reinterpret_cast<char*>(&day), sizeof(day));
            file.read(reinterpret_cast<char*>(&month), sizeof(month));
            file.read(reinterpret_cast<char*>(&year), sizeof(year));

            if (file) {
                TOC toc{ day, month, year };
                if (toc.isValidDate()) {
                    Product* product = new Product();
                    file.seekg(startPos);
                    product->deserialize(file);
                    supermarketInventory.push_back(product);
                    delete tempGoods;
                    continue;
                }
            }

            supermarketInventory.push_back(tempGoods);
        }

        cout << "Loading completed successfully." << endl;
    }

    ~Supermarket() {
        for (auto& goods : supermarketInventory) {
            delete goods;
        }
    }
};
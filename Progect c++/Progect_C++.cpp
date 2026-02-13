#include <iostream>
#include <fstream>
#include <Windows.h>
#include <exception>
#include <string>
#include "Supermarket.h"

using namespace std;

static const string fname = "Supermarket.txt";

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int choice;
    Supermarket supermarket;

    while (true) {
        cout << "\t [1] - Add a product or item; \n\t [2] - View products (items) sorted by categories; \n\t [3] - Search for a specific item (product) with full information; \n\t [4] - Delete a specific item (product) from the database; \n\t [5] - Sell an item (with receipt); \n\t [6] - Save the product database (to file); \n\t [7] - Load the product database from file; \n\t [0] - Exit; \n\t\t Enter :: ";
        cin >> choice;
        cin.ignore();

        if (choice == 0) {
            cout << "\tThe program has finished its work!!!";
            break;
        }

        switch (choice) {
        case 1: {
            string name, category;
            int price = 0, quantity = 0, n;
            TOC data{};

            cout << "\n\t [1] - Add product; \n\t [2] - Add item \n\t\t Enter :: ";
            cin >> n;
            cin.ignore();

            if (n == 1) {
                cout << "\t Enter data: " << endl;
                cout << "\t Enter product name :: ";
                getline(cin, name);
                cout << "\t Enter product category :: ";
                getline(cin, category);
                cout << "\t Enter expiration date (day month year) :: ";
                cin >> data.day >> data.month >> data.year;
                cout << "\t Enter price :: ";
                cin >> price;

                if (price <= 0) {
                    cout << "Error: Price must be positive!" << endl;
                    break;
                }

                cout << "\t Enter quantity :: ";
                cin >> quantity;

                try {
                    supermarket.addGoods(new Product(name.c_str(), data, category.c_str(), price, quantity));
                    cout << "\t Product added successfully!" << endl;
                }
                catch (const invalid_argument& ex) {
                    cout << "\t Error: " << ex.what() << endl;
                }
            }
            else if (n == 2) {
                cout << "\t Enter item name :: ";
                getline(cin, name);
                cout << "\t Enter item category :: ";
                getline(cin, category);
                cout << "\t Enter price :: ";
                cin >> price;

                if (price <= 0) {
                    cout << "Error: Price must be positive!" << endl;
                    break;
                }

                cout << "\t Enter quantity :: ";
                cin >> quantity;

                try {
                    supermarket.addGoods(new Product(name.c_str(), data, category.c_str(), price, quantity));
                    cout << "\t Item added successfully!" << endl;
                }
                catch (const invalid_argument& ex) {
                    cout << "\t Error: " << ex.what() << endl;
                }
            }
            else {
                cout << "\t Error: Invalid option!" << endl;
            }
            break;
        }
        case 2:
            supermarket.printSortByCategory();
            break;

        case 3: {
            string name;
            cout << "\t Enter the name of the product (item) you want to find \n\t\t Enter :: ";
            getline(cin, name);

            try {
                supermarket.search(name.c_str());
            }
            catch (const invalid_argument& ex) {
                cout << "\t Error: " << ex.what() << endl;
            }
            break;
        }
        case 4: {
            string name;
            cout << "\t Enter the name of the product (item) you want to delete \n\t\t Enter :: ";
            getline(cin, name);

            try {
                supermarket.delete_(name.c_str());
            }
            catch (const invalid_argument& ex) {
                cout << "\t Error: " << ex.what() << endl;
            }
            break;
        }
        case 5: {
            string name;
            int n = 0;
            cout << "\t Enter the product name and quantity you want to sell (separated by space)\n\t\t Enter :: ";
            cin >> name >> n;

            try {
                supermarket.selling(name.c_str(), n);
            }
            catch (const invalid_argument& ex) {
                cout << "\t Error: " << ex.what() << endl;
            }
            break;
        }
        case 6: {
            ofstream file(fname, ios::binary);
            if (!file) {
                cout << "\t Failed to open file for saving!" << endl;
                break;
            }
            supermarket.save(file);
            file.close();
            cout << "\t Database saved successfully!" << endl;
            break;
        }
        case 7: {
            ifstream file(fname, ios::binary);
            if (!file) {
                cout << "\t Failed to open file for loading!" << endl;
                break;
            }
            supermarket.download(file);
            file.close();
            cout << "\t Database loaded successfully!" << endl;
            break;
        }
        default:
            cout << "\t Error: Invalid choice! Try again." << endl;
            break;
        }
    }

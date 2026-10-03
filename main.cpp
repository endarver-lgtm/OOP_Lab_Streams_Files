#define _CRT_SECURE_NO_WARNINGS
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include "Customer.h"
#include "Input.h"
#include "Order.h"
#include "OrderFile.h"

using namespace std;

static string toLower(string s) {
    for (char& c : s) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return s;
}

static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void pauseScreen() {
    cout << "\nНажмите Enter, чтобы продолжить...";
    cin.ignore(10000, '\n');
    cin.get();
}

static void addItems() {
    do {
        OrderItem item;
        cin >> item;
        ItemRecord rec;
        item.toRecord(rec);
        if (OrderFile::append(rec))
            cout << "Позиция добавлена. Всего позиций: " << OrderFile::count() << "\n";
    } while (askYesNo(cin, "Добавить ещё одну позицию? (y/n): "));
}

static void viewOrder(const Customer& customer, int number) {
    Order order(number, &customer);
    if (!OrderFile::load(order)) return;
    cout << "\n" << order << "\n";
}

static void editItem() {
    int n = OrderFile::count();
    if (n == 0) { cout << "Заказ пуст, редактировать нечего.\n"; return; }
    int num = readInt(cin, "Номер позиции для редактирования (1-" + to_string(n) + "): ", 1, n);

    ItemRecord rec;
    if (!OrderFile::readAt(num - 1, rec)) return;
    cout << "Текущие данные:\n";
    OrderItem::printHeader(cout);
    cout << setw(4) << num << ' ' << OrderItem::fromRecord(rec) << "\n";

    cout << "Введите новые данные позиции:\n";
    OrderItem item;
    cin >> item;
    item.toRecord(rec);
    if (OrderFile::writeAt(num - 1, rec)) cout << "Позиция изменена.\n";
}

static void deleteItem() {
    int n = OrderFile::count();
    if (n == 0) { cout << "Заказ пуст, удалять нечего.\n"; return; }
    int num = readInt(cin, "Номер позиции для удаления (1-" + to_string(n) + "): ", 1, n);
    if (!askYesNo(cin, "Точно удалить позицию " + to_string(num) + "? (y/n): ")) {
        cout << "Удаление отменено.\n";
        return;
    }
    if (OrderFile::removeAt(num - 1)) cout << "Позиция удалена.\n";
}

static void searchItems(const Customer& customer, int number) {
    Order order(number, &customer);
    if (!OrderFile::load(order)) return;
    if (order.items().empty()) { cout << "Заказ пуст.\n"; return; }

    cout << "Поиск:\n"
        "  1 - по названию или бренду (часть слова)\n"
        "  2 - по диапазону цены\n"
        "  3 - по типу товара\n";
    int mode = readInt(cin, "Ваш выбор: ", 1, 3);

    string query;
    double lo = 0, hi = 0;
    int type = 0;
    if (mode == 1) query = toLower(readLine(cin, "Введите часть названия или бренда: ", 25));
    else if (mode == 2) {
        lo = readDouble(cin, "Цена от: ", 0, 99999);
        hi = readDouble(cin, "Цена до: ", lo, 99999);
    }
    else type = readInt(cin, "Тип (1 - парфюм, 2 - дезодорант, 3 - крем): ", 1, 3);

    int found = 0;
    const vector<OrderItem>& items = order.items();
    for (size_t i = 0; i < items.size(); ++i) {
        const Product& p = items[i].product();
        bool match = false;
        if (mode == 1)
            match = toLower(p.name()).find(query) != string::npos || toLower(p.brand()).find(query) != string::npos;
        else if (mode == 2) match = p.price() >= lo && p.price() <= hi;
        else match = static_cast<int>(p.type()) == type;

        if (match) {
            if (found == 0) { cout << "\n"; OrderItem::printHeader(cout); }
            cout << setw(4) << i + 1 << ' ' << items[i] << "\n";
            ++found;
        }
    }
    if (found == 0) cout << "Ничего не найдено.\n";
    else cout << "\nНайдено позиций: " << found << "\n";
}

static void saveReceipt(const Customer& customer, int number) {
    Order order(number, &customer);
    if (!OrderFile::load(order)) return;
    ofstream f;
    f.open("receipt.txt", ios::trunc);
    if (!f.is_open()) { cerr << "Не удалось создать receipt.txt\n"; return; }
    f << order;
    if (f.bad() || f.fail()) cerr << "Ошибка записи чека\n";
    else cout << "Чек сохранён в receipt.txt\n";
    f.close();
}

int main() {

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    cout << "=== Магазин парфюмерной продукции: сборка заказа ===\n";
    cout << "Данные заказа хранятся в файле " << OrderFile::fileName()
        << " (сейчас позиций: " << OrderFile::count() << ")\n\n";

    Customer customer;
    cin >> customer;
    int number = readInt(cin, "Номер заказа (1-99999): ", 1, 99999);

    while (true) {
        clearScreen();
        cout << "=== Магазин парфюмерной продукции: сборка заказа ===\n";
        cout << "Номер заказа: " << number
            << " | Позиций в заказе: " << OrderFile::count() << "\n";
        cout << "\n--- МЕНЮ ---\n"
            "1 - Добавить позицию\n"
            "2 - Просмотреть заказ\n"
            "3 - Редактировать позицию\n"
            "4 - Удалить позицию\n"
            "5 - Поиск\n"
            "6 - Сохранить чек в receipt.txt\n"
            "0 - Выход\n";
        int choice = readInt(cin, "Ваш выбор: ", 0, 6);

        clearScreen();

        switch (choice) {
        case 1: addItems(); break;
        case 2: viewOrder(customer, number); break;
        case 3: editItem(); break;
        case 4: deleteItem(); break;
        case 5: searchItems(customer, number); break;
        case 6: saveReceipt(customer, number); break;
        case 0:
            cout << "До свидания!\n";
            return 0;
        }

        pauseScreen();
    }
}
#include "OrderFile.h"
#include <cstdio>
#include <fstream>
#include <iostream>

using namespace std;

static const char* FILE_NAME = "order.dat";
static const char* TMP_NAME = "order.tmp";

static bool streamOk(const ios& s, const char* action) {
    if (s.good()) return true;
    if (s.bad())       cerr << "Критический сбой потока: " << action << "\n";
    else if (s.fail()) cerr << "Ошибка операции: " << action << "\n";
    else if (s.eof())  cerr << "Неожиданный конец файла: " << action << "\n";
    return false;
}

const char* OrderFile::fileName() { return FILE_NAME; }

int OrderFile::count() {
    ifstream f;
    f.open(FILE_NAME, ios::binary);
    if (!f.is_open()) return 0;
    f.seekg(0, ios::end);
    streamoff end = f.tellg();
    bool ok = streamOk(f, "определение размера файла");
    f.close();
    if (!ok || end < 0) return 0;
    return static_cast<int>(end / static_cast<streamoff>(sizeof(ItemRecord)));
}

bool OrderFile::append(const ItemRecord& rec) {
    ofstream f;
    f.open(FILE_NAME, ios::app | ios::binary);
    if (!f.is_open()) {
        cerr << "Не удалось открыть файл " << FILE_NAME << " для записи\n";
        return false;
    }
    f.write(reinterpret_cast<const char*>(&rec), sizeof(rec));
    bool ok = streamOk(f, "запись в файл");
    f.close();
    return ok;
}

bool OrderFile::readAt(int index, ItemRecord& rec) {
    ifstream f;
    f.open(FILE_NAME, ios::binary);
    if (!f.is_open()) {
        cerr << "Не удалось открыть файл " << FILE_NAME << " для чтения\n";
        return false;
    }
    f.seekg(static_cast<streamoff>(index) * sizeof(ItemRecord), ios::beg);
    f.read(reinterpret_cast<char*>(&rec), sizeof(rec));
    bool ok = streamOk(f, "чтение записи");
    f.close();
    return ok;
}

bool OrderFile::writeAt(int index, const ItemRecord& rec) {
    fstream f;
    f.open(FILE_NAME, ios::in | ios::out | ios::binary);
    if (!f.is_open()) {
        cerr << "Не удалось открыть файл " << FILE_NAME << " для изменения\n";
        return false;
    }
    f.seekp(static_cast<streamoff>(index) * sizeof(ItemRecord), ios::beg);
    f.write(reinterpret_cast<const char*>(&rec), sizeof(rec));
    bool ok = streamOk(f, "перезапись записи");
    f.close();
    return ok;
}

bool OrderFile::loadAll(vector<ItemRecord>& out) {
    out.clear();
    ifstream f;
    f.open(FILE_NAME, ios::binary);
    if (!f.is_open()) return true;
    ItemRecord rec;
    while (true) {
        f.read(reinterpret_cast<char*>(&rec), sizeof(rec));
        if (f.eof()) {
            if (f.gcount() != 0) cerr << "Предупреждение: в конце файла неполная запись, она пропущена\n";
            break;
        }
        if (!streamOk(f, "чтение файла")) { f.close(); return false; }
        out.push_back(rec);
    }
    f.close();
    return true;
}

bool OrderFile::removeAt(int index) {
    vector<ItemRecord> all;
    if (!loadAll(all)) return false;
    if (index < 0 || index >= static_cast<int>(all.size())) return false;

    ofstream tmp;
    tmp.open(TMP_NAME, ios::trunc | ios::binary);
    if (!tmp.is_open()) {
        cerr << "Не удалось создать временный файл\n";
        return false;
    }
    for (int i = 0; i < static_cast<int>(all.size()); ++i) {
        if (i == index) continue;
        tmp.write(reinterpret_cast<const char*>(&all[i]), sizeof(ItemRecord));
    }
    bool ok = streamOk(tmp, "запись временного файла");
    tmp.close();
    if (!ok) { remove(TMP_NAME); return false; }

    if (remove(FILE_NAME) != 0 || rename(TMP_NAME, FILE_NAME) != 0) {
        cerr << "Не удалось заменить файл заказа\n";
        return false;
    }
    return true;
}

bool OrderFile::load(Order& order) {
    vector<ItemRecord> recs;
    if (!loadAll(recs)) return false;
    order.clear();
    for (const ItemRecord& r : recs) order.add(OrderItem::fromRecord(r));
    return true;
}

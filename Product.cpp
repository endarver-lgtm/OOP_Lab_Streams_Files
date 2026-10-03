#define _CRT_SECURE_NO_WARNINGS
#include "Product.h"
#include <cstring>
#include <iomanip>
#include <iostream>
#include "Input.h"

static std::string fromChars(const char* a, size_t n) {
    size_t len = 0;
    while (len < n && a[len] != '\0') ++len;
    return std::string(a, len);
}

static void toChars(char* dst, size_t n, const std::string& s) {
    std::memset(dst, 0, n);
    std::strncpy(dst, s.c_str(), n - 1);
}

void Product::read(std::istream& in) {
    id_ = readInt(in, "Артикул (1-99999): ", 1, 99999);
    name_ = readLine(in, "Название (до 25 симв.): ", 25);
    brand_ = readLine(in, "Бренд (до 20 симв.): ", 20);
    price_ = readDouble(in, "Цена, руб. (0.01-99999): ", 0.01, 99999.0);
    volume_ = readInt(in, "Объём, мл (1-1000): ", 1, 1000);
    setExtra(readLine(in, extraLabel() + " (до 12 симв.): ", 12));
}

void Product::printHeader(std::ostream& os) {
    std::ios::fmtflags old = os.flags();
    os.setf(std::ios::right, std::ios::adjustfield);
    os << std::setw(6) << "Артик." << ' ';
    os.setf(std::ios::left, std::ios::adjustfield);
    os << std::setw(11) << "Тип" << ' '
        << std::setw(25) << "Название" << ' '
        << std::setw(20) << "Бренд" << ' ';
    os.setf(std::ios::right, std::ios::adjustfield);
    os << std::setw(9) << "Цена" << ' ' << std::setw(5) << "Мл" << ' ';
    os.setf(std::ios::left, std::ios::adjustfield);
    os << std::setw(12) << "Особенность";
    os.flags(old);
}

void Product::print(std::ostream& os) const {
    std::ios::fmtflags oldFlags = os.flags();
    std::streamsize oldPrec = os.precision();
    os.setf(std::ios::right, std::ios::adjustfield);
    os << std::setw(6) << id_ << ' ';
    os.setf(std::ios::left, std::ios::adjustfield);
    os << std::setw(11) << typeName() << ' '
        << std::setw(25) << name_ << ' '
        << std::setw(20) << brand_ << ' ';
    os.setf(std::ios::right, std::ios::adjustfield);
    os.setf(std::ios::fixed, std::ios::floatfield);
    os << std::setw(9) << std::setprecision(2) << price_ << ' '
        << std::setw(5) << volume_ << ' ';
    os.setf(std::ios::left, std::ios::adjustfield);
    os << std::setw(12) << extra();
    os.flags(oldFlags);
    os.precision(oldPrec);
}

void Product::toRecord(ItemRecord& rec) const {
    std::memset(&rec, 0, sizeof(rec));
    rec.type = static_cast<int>(type());
    rec.id = id_;
    toChars(rec.name, sizeof(rec.name), name_);
    toChars(rec.brand, sizeof(rec.brand), brand_);
    rec.price = price_;
    rec.volume = volume_;
    toChars(rec.extra, sizeof(rec.extra), extra());
}

void Product::fromRecord(const ItemRecord& rec) {
    id_ = rec.id;
    name_ = fromChars(rec.name, sizeof(rec.name));
    brand_ = fromChars(rec.brand, sizeof(rec.brand));
    price_ = rec.price;
    volume_ = rec.volume;
    setExtra(fromChars(rec.extra, sizeof(rec.extra)));
}

std::unique_ptr<Product> createProduct(ProductType t) {
    switch (t) {
    case PT_PERFUME:   return std::unique_ptr<Product>(new Perfume);
    case PT_DEODORANT: return std::unique_ptr<Product>(new Deodorant);
    case PT_CREAM:     return std::unique_ptr<Product>(new Cream);
    }
    return nullptr;
}

std::istream& operator>>(std::istream& in, Product& p) {
    p.read(in);
    return in;
}

std::ostream& operator<<(std::ostream& os, const Product& p) {
    p.print(os);
    return os;
}

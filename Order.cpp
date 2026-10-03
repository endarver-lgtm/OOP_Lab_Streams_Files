#include "Order.h"
#include <iomanip>
#include <iostream>
#include <string>
#include "Input.h"

static const int LINE_WIDTH = 115;

OrderItem::OrderItem() : product_(createProduct(PT_PERFUME)), quantity_(1) {}

OrderItem::OrderItem(std::unique_ptr<Product> p, int quantity)
    : product_(std::move(p)), quantity_(quantity) {
    if (!product_) product_ = createProduct(PT_PERFUME);
}

void OrderItem::toRecord(ItemRecord& rec) const {
    product_->toRecord(rec);
    rec.quantity = quantity_;
}

OrderItem OrderItem::fromRecord(const ItemRecord& rec) {
    std::unique_ptr<Product> p = createProduct(static_cast<ProductType>(rec.type));
    if (!p) p = createProduct(PT_PERFUME);
    p->fromRecord(rec);
    return OrderItem(std::move(p), rec.quantity);
}

void OrderItem::printHeader(std::ostream& os) {
    std::ios::fmtflags old = os.flags();
    os.setf(std::ios::right, std::ios::adjustfield);
    os << std::setw(4) << "№" << ' ';
    Product::printHeader(os);
    os.setf(std::ios::right, std::ios::adjustfield);
    os << ' ' << std::setw(4) << "Кол." << ' ' << std::setw(10) << "Сумма" << '\n';
    os << std::setfill('-') << std::setw(LINE_WIDTH) << "" << std::setfill(' ') << '\n';
    os.flags(old);
}

std::istream& operator>>(std::istream& in, OrderItem& item) {
    int t = readInt(in, "Тип товара (1 - парфюм, 2 - дезодорант, 3 - крем): ", 1, 3);
    std::unique_ptr<Product> p = createProduct(static_cast<ProductType>(t));
    in >> *p;
    int q = readInt(in, "Количество (1-99): ", 1, 99);
    item = OrderItem(std::move(p), q);
    return in;
}

std::ostream& operator<<(std::ostream& os, const OrderItem& item) {
    std::ios::fmtflags oldFlags = os.flags();
    std::streamsize oldPrec = os.precision();
    item.product_->print(os);
    os.setf(std::ios::right, std::ios::adjustfield);
    os.setf(std::ios::fixed, std::ios::floatfield);
    os << ' ' << std::setw(4) << item.quantity_
        << ' ' << std::setw(10) << std::setprecision(2) << item.sum();
    os.flags(oldFlags);
    os.precision(oldPrec);
    return os;
}

double Order::total() const {
    double s = 0.0;
    for (const OrderItem& it : items_) s += it.sum();
    return s;
}

std::ostream& operator<<(std::ostream& os, const Order& order) {
    os << "Заказ № " << order.number_ << '\n';
    if (order.customer_) os << *order.customer_ << '\n';
    OrderItem::printHeader(os);
    if (order.items_.empty()) {
        os << "(в заказе нет позиций)\n";
    }
    else {
        int n = 0;
        for (const OrderItem& it : order.items_)
            os << std::setw(4) << ++n << ' ' << it << '\n';
    }
    std::ios::fmtflags oldFlags = os.flags();
    std::streamsize oldPrec = os.precision();
    os << std::setfill('-') << std::setw(LINE_WIDTH) << "" << std::setfill(' ') << '\n';
    os << "ИТОГО: " << std::fixed << std::setprecision(2) << order.total() << " руб.\n";
    os.flags(oldFlags);
    os.precision(oldPrec);
    return os;
}

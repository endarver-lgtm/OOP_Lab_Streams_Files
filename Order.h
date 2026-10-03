#pragma once
#include <iosfwd>
#include <memory>
#include <vector>
#include "Customer.h"
#include "Product.h"

class OrderItem {
    std::unique_ptr<Product> product_;
    int quantity_;
public:
    OrderItem();
    OrderItem(std::unique_ptr<Product> p, int quantity);

    const Product& product() const { return *product_; }
    int quantity() const { return quantity_; }
    double sum() const { return product_->price() * quantity_; }

    void toRecord(ItemRecord& rec) const;
    static OrderItem fromRecord(const ItemRecord& rec);
    static void printHeader(std::ostream& os);

    friend std::istream& operator>>(std::istream& in, OrderItem& item);
    friend std::ostream& operator<<(std::ostream& os, const OrderItem& item);
};

class Order {
    int number_;
    const Customer* customer_;
    std::vector<OrderItem> items_;
public:
    Order(int number, const Customer* customer) : number_(number), customer_(customer) {}

    void add(OrderItem&& item) { items_.push_back(std::move(item)); }
    void clear() { items_.clear(); }
    const std::vector<OrderItem>& items() const { return items_; }
    double total() const;

    friend std::ostream& operator<<(std::ostream& os, const Order& order);
};

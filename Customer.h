#pragma once
#include <iostream>
#include <string>
#include "Input.h"

class Customer {
    std::string name_;
    std::string phone_;
public:
    Customer() : name_("не указан"), phone_("-") {}
    Customer(const std::string& name, const std::string& phone) : name_(name), phone_(phone) {}

    const std::string& name() const { return name_; }
    const std::string& phone() const { return phone_; }

    friend std::istream& operator>>(std::istream& in, Customer& c) {
        c.name_ = readLine(in, "ФИО клиента (до 30 симв.): ", 30);
        c.phone_ = readLine(in, "Телефон (до 20 симв.): ", 20);
        return in;
    }
    friend std::ostream& operator<<(std::ostream& os, const Customer& c) {
        os << "Клиент: " << c.name_ << ", тел. " << c.phone_;
        return os;
    }
};

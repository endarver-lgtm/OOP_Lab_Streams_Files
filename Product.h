#pragma once
#include <iosfwd>
#include <memory>
#include <string>

enum ProductType { PT_PERFUME = 1, PT_DEODORANT = 2, PT_CREAM = 3 };

struct ItemRecord {
    int    type;
    int    id;
    char   name[26];
    char   brand[21];
    double price;
    int    volume;
    char   extra[13];
    int    quantity;
};

class Product {
protected:
    int         id_;
    std::string name_;
    std::string brand_;
    double      price_;
    int         volume_;
public:
    Product() : id_(0), price_(0.0), volume_(0) {}
    virtual ~Product() {}

    virtual ProductType type() const = 0;
    virtual std::string typeName() const = 0;
    virtual std::string extraLabel() const = 0;
    virtual std::string extra() const = 0;
    virtual void setExtra(const std::string& s) = 0;

    virtual void read(std::istream& in);
    virtual void print(std::ostream& os) const;
    static void printHeader(std::ostream& os);

    void toRecord(ItemRecord& rec) const;
    void fromRecord(const ItemRecord& rec);

    int id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& brand() const { return brand_; }
    double price() const { return price_; }
    int volume() const { return volume_; }
};

class Perfume : public Product {
    std::string concentration_;
public:
    ProductType type() const override { return PT_PERFUME; }
    std::string typeName() const override { return "Парфюм"; }
    std::string extraLabel() const override { return "Концентрация (EDT/EDP/Parfum)"; }
    std::string extra() const override { return concentration_; }
    void setExtra(const std::string& s) override { concentration_ = s; }
};

class Deodorant : public Product {
    std::string form_;
public:
    ProductType type() const override { return PT_DEODORANT; }
    std::string typeName() const override { return "Дезодорант"; }
    std::string extraLabel() const override { return "Форма (спрей/стик/ролик)"; }
    std::string extra() const override { return form_; }
    void setExtra(const std::string& s) override { form_ = s; }
};

class Cream : public Product {
    std::string skinType_;
public:
    ProductType type() const override { return PT_CREAM; }
    std::string typeName() const override { return "Крем"; }
    std::string extraLabel() const override { return "Тип кожи"; }
    std::string extra() const override { return skinType_; }
    void setExtra(const std::string& s) override { skinType_ = s; }
};

std::unique_ptr<Product> createProduct(ProductType t);

std::istream& operator>>(std::istream& in, Product& p);
std::ostream& operator<<(std::ostream& os, const Product& p);

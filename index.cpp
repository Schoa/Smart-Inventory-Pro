#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <limits>

// Declare pause() before Warehouse class
void pause()
{
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// Product class
class Product
{
    int id;
    std::string name;
    int stock;
    double price;
    int supplierId;

public:
    Product() : id(0), name(""), stock(0), price(0.0), supplierId(0) {} // Default constructor
    Product(int id, const std::string &name, int stock, double price, int supplierId)
        : id(id), name(name), stock(stock), price(price), supplierId(supplierId) {}

    int getId() const { return id; }
    std::string getName() const { return name; }
    int getStock() const { return stock; }
    double getPrice() const { return price; }
    int getSupplierId() const { return supplierId; }

    void updateStock(int amount) { stock += amount; }
    void setName(const std::string &newName) { name = newName; }
    void setStock(int newStock) { stock = newStock; }
    void setPrice(double newPrice) { price = newPrice; }
    void setSupplierId(int newSupplierId) { supplierId = newSupplierId; }
};

// Supplier class
class Supplier
{
    int id;
    std::string name;
    std::string contact;

public:
    Supplier() : id(0), name(""), contact("") {} // Default constructor
    Supplier(int id, const std::string &name, const std::string &contact)
        : id(id), name(name), contact(contact) {}

    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getContact() const { return contact; }

    void setName(const std::string &newName) { name = newName; }
    void setContact(const std::string &newContact) { contact = newContact; }
};

// Member class (for employees/customers)
class Member
{
    int id;
    std::string name;
    std::string role;
    std::string password;

public:
    Member() : id(0), name(""), role(""), password("") {} // Default constructor
    Member(int id, const std::string &name, const std::string &role, const std::string &password)
        : id(id), name(name), role(role), password(password) {}

    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getRole() const { return role; }
    bool authenticate(const std::string &pwd) const { return pwd == password; }

    void setName(const std::string &newName) { name = newName; }
    void setRole(const std::string &newRole) { role = newRole; }
    void setPassword(const std::string &newPassword) { password = newPassword; }
};

// OrderItem class
class OrderItem
{
    int productId;
    int quantity;

public:
    OrderItem(int productId, int quantity)
        : productId(productId), quantity(quantity) {}

    int getProductId() const { return productId; }
    int getQuantity() const { return quantity; }
};

// Order class
class Order
{
    int id;
    int memberId;
    std::vector<OrderItem> items;
    std::chrono::system_clock::time_point date;

public:
    Order(int id, int memberId)
        : id(id), memberId(memberId), date(std::chrono::system_clock::now()) {}

    void addItem(const OrderItem &item) { items.push_back(item); }
    int getId() const { return id; }
    int getMemberId() const { return memberId; }
    const std::vector<OrderItem> &getItems() const { return items; }
    std::chrono::system_clock::time_point getDate() const { return date; }
};

// Inventory class
class Inventory
{
    std::map<int, Product> products;

public:
    void addProduct(const Product &product)
    {
        products[product.getId()] = product;
    }
    void updateStock(int productId, int amount)
    {
        if (products.find(productId) != products.end())
        {
            products[productId].updateStock(amount);
        }
    }
    Product *getProduct(int productId)
    {
        auto it = products.find(productId);
        if (it != products.end())
            return &(it->second);
        return nullptr;
    }
    std::vector<Product> getLowStockProducts(int threshold)
    {
        std::vector<Product> lowStock;
        for (auto &[id, product] : products)
        {
            if (product.getStock() < threshold)
                lowStock.push_back(product);
        }
        return lowStock;
    }
    std::vector<Product> getAllProducts() const
    {
        std::vector<Product> all;
        for (const auto &[id, product] : products)
        {
            all.push_back(product);
        }
        return all;
    }
};

// Warehouse class
class Warehouse
{
    Inventory inventory;
    std::map<int, Supplier> suppliers;
    std::map<int, Member> members;
    std::vector<Order> orders;

public:
    void addSupplier(const Supplier &supplier)
    {
        suppliers[supplier.getId()] = supplier;
    }
    void addMember(const Member &member)
    {
        members[member.getId()] = member;
    }
    void addProduct(const Product &product)
    {
        inventory.addProduct(product);
    }
    void processOrder(const Order &order)
    {
        // First, check if all items are available in sufficient quantity
        for (const auto &item : order.getItems())
        {
            Product *product = inventory.getProduct(item.getProductId());
            if (!product)
            {
                std::cout << "Product ID " << item.getProductId() << " not found. Order not processed.\n";
                pause();
                return;
            }
            if (product->getStock() < item.getQuantity())
            {
                std::cout << "Not enough stock for product '" << product->getName()
                          << "' (ID: " << product->getId() << "). Available: "
                          << product->getStock() << ", Requested: " << item.getQuantity() << ".\n";
                std::cout << "Order not processed.\n";
                pause();
                return;
            }
        }
        // If all checks pass, process the order
        for (const auto &item : order.getItems())
        {
            inventory.updateStock(item.getProductId(), -item.getQuantity());
        }
        orders.push_back(order);
        std::cout << "Order processed!\n";
        pause();
    }
    void showLowStock(int threshold)
    {
        auto lowStock = inventory.getLowStockProducts(threshold);
        for (const auto &product : lowStock)
        {
            std::cout << "Low stock: " << product.getName() << " (ID: " << product.getId() << "), Stock: " << product.getStock() << std::endl;
        }
    }
    void showAllStock() const
    {
        auto all = inventory.getAllProducts();
        std::cout << "\n--- Current Stock ---\n";
        if (all.empty())
        {
            std::cout << "No products in stock.\n";
        }
        else
        {
            std::cout << "ID\tName\tStock\tPrice\tSupplierID\n";
            for (const auto &product : all)
            {
                std::cout << product.getId() << "\t" << product.getName() << "\t"
                          << product.getStock() << "\t" << product.getPrice() << "\t"
                          << product.getSupplierId() << "\n";
            }
        }
        pause();
    }
    void showSupplierList() const
    {
        std::cout << "\n--- Supplier List ---\n";
        if (suppliers.empty())
        {
            std::cout << "No suppliers available.\n";
        }
        else
        {
            std::cout << "ID\tName\tContact\n";
            for (const auto &[id, supplier] : suppliers)
            {
                std::cout << supplier.getId() << "\t" << supplier.getName() << "\t" << supplier.getContact() << "\n";
            }
        }
        pause();
    }

    void showMemberList() const
    {
        std::cout << "\n--- Member List ---\n";
        if (members.empty())
        {
            std::cout << "No members available.\n";
        }
        else
        {
            std::cout << "ID\tName\tRole\n";
            for (const auto &[id, member] : members)
            {
                std::cout << member.getId() << "\t" << member.getName() << "\t" << member.getRole() << "\n";
            }
        }
        pause();
    }

    // Add function to count orders by member
    int countOrdersByMember(int memberId) const {
        int count = 0;
        for (const auto& order : orders) {
            if (order.getMemberId() == memberId)
                ++count;
        }
        return count;
    }

    void showMemberOrderCounts() const {
        std::cout << "\n--- Member Order Counts ---\n";
        if (members.empty()) {
            std::cout << "No members available.\n";
        } else {
            std::cout << "ID\tName\tRole\tOrder Count\n";
            for (const auto& [id, member] : members) {
                int orderCount = 0;
                for (const auto& order : orders) {
                    if (order.getMemberId() == id)
                        ++orderCount;
                }
                std::cout << member.getId() << "\t" << member.getName() << "\t"
                          << member.getRole() << "\t" << orderCount << "\n";
            }
        }
        pause();
    }

    Product *getProductById(int id)
    {
        return inventory.getProduct(id);
    }
    Supplier *getSupplierById(int id)
    {
        auto it = suppliers.find(id);
        if (it != suppliers.end())
            return &(it->second);
        return nullptr;
    }
    Member *getMemberById(int id)
    {
        auto it = members.find(id);
        if (it != members.end())
            return &(it->second);
        return nullptr;
    }
};

// Helper functions for UI
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int inputInt(const std::string &prompt)
{
    int value;
    std::string line;
    while (true)
    {
        std::cout << prompt;
        std::getline(std::cin, line);
        try
        {
            value = std::stoi(line);
            break;
        }
        catch (...)
        {
            std::cout << "Invalid input. Please enter a number.\n";
        }
    }
    return value;
}

// Helper for double input
double inputDouble(const std::string &prompt)
{
    double value;
    std::string line;
    while (true)
    {
        std::cout << prompt;
        std::getline(std::cin, line);
        try
        {
            value = std::stod(line);
            break;
        }
        catch (...)
        {
            std::cout << "Invalid input. Please enter a number.\n";
        }
    }
    return value;
}

// Helper for ID input with guidance and validation
int inputProductId(const std::string &prompt)
{
    int value;
    while (true)
    {
        std::cout << prompt << " (numbers only, max 8 digits): ";
        std::string input;
        std::getline(std::cin, input);
        if (input.length() > 8)
        {
            std::cout << "ID must be under 8 characters.\n";
            continue;
        }
        if (!std::all_of(input.begin(), input.end(), ::isdigit))
        {
            std::cout << "ID must only include numbers.\n";
            continue;
        }
        try
        {
            value = std::stoi(input);
            return value;
        }
        catch (...)
        {
            std::cout << "Invalid input. Please enter a valid number.\n";
        }
    }
}

// UI Functions
void showMainMenu()
{
    std::cout << "=============================\n";
    std::cout << "   SmartInventoryPro System  \n";
    std::cout << "=============================\n";
    std::cout << "1. Add Product\n";
    std::cout << "2. Add Supplier\n";
    std::cout << "3. Add Member\n";
    std::cout << "4. Process Order\n";
    std::cout << "5. Show All Stock\n";
    std::cout << "6. Edit Product\n";
    std::cout << "7. Edit Supplier\n";
    std::cout << "8. Edit Member\n";
    std::cout << "9. Show Supplier List\n";
    std::cout << "10. Show Member List\n";
    std::cout << "11. Show Member Order Counts\n";
    std::cout << "12. Exit\n";
    std::cout << "Select an option: ";
}

void addProductUI(Warehouse &warehouse)
{
    int id = inputProductId("Product ID");
    std::string name;
    std::cout << "Name: ";
    std::getline(std::cin, name);
    int stock = inputInt("Stock: ");
    double price = inputDouble("Price: ");
    int supplierId = inputInt("Supplier ID: ");
    warehouse.addProduct(Product(id, name, stock, price, supplierId));
    std::cout << "Product added successfully!\n";
    pause();
}

void addSupplierUI(Warehouse &warehouse)
{
    int id;
    std::string name, contact;
    clearScreen();
    std::cout << "--- Add Supplier ---\n";
    std::cout << "Supplier ID: ";
    std::cin >> id;
    std::cout << "Name: ";
    std::cin.ignore();
    std::getline(std::cin, name);
    std::cout << "Contact: ";
    std::getline(std::cin, contact);
    warehouse.addSupplier(Supplier(id, name, contact));
    std::cout << "Supplier added successfully!\n";
    pause();
}

void addMemberUI(Warehouse &warehouse)
{
    int id;
    std::string name, role, password;
    clearScreen();
    std::cout << "--- Add Member ---\n";
    std::cout << "Member ID: ";
    std::cin >> id;
    std::cout << "Name: ";
    std::cin.ignore();
    std::getline(std::cin, name);
    std::cout << "Role (employee/customer): ";
    std::getline(std::cin, role);
    std::cout << "Password: ";
    std::getline(std::cin, password);
    warehouse.addMember(Member(id, name, role, password));
    std::cout << "Member added successfully!\n";
    pause();
}

void processOrderUI(Warehouse &warehouse)
{
    int orderId, memberId, productId, quantity;
    clearScreen();
    std::cout << "--- Process Order ---\n";
    std::cout << "Order ID: ";
    std::cin >> orderId;
    std::cout << "Member ID: ";
    std::cin >> memberId;
    Order order(orderId, memberId);
    char more = 'y';
    while (more == 'y' || more == 'Y')
    {
        std::cout << "Product ID: ";
        std::cin >> productId;
        std::cout << "Quantity: ";
        std::cin >> quantity;
        order.addItem(OrderItem(productId, quantity));
        std::cout << "Add another item? (y/n): ";
        std::cin >> more;
    }
    warehouse.processOrder(order);
    std::cout << "Order processed!\n";
    pause();
}

void showLowStockUI(Warehouse &warehouse)
{
    int threshold;
    clearScreen();
    std::cout << "--- Low Stock Products ---\n";
    std::cout << "Enter stock threshold: ";
    std::cin >> threshold;
    warehouse.showLowStock(threshold);
    pause();
}

void editProductUI(Warehouse &warehouse)
{
    clearScreen();
    std::cout << "--- Edit Product ---\n";
    int id = inputProductId("Enter Product ID to edit");
    Product *product = warehouse.getProductById(id);
    if (!product)
    {
        std::cout << "Product not found.\n";
        pause();
        return;
    }
    std::cout << "Editing Product: " << product->getName() << "\n";
    std::cout << "Leave input blank to keep current value.\n";

    std::string input;
    std::cout << "New Name [" << product->getName() << "]: ";
    std::getline(std::cin, input);
    if (!input.empty())
        product->setName(input);

    std::cout << "New Stock [" << product->getStock() << "]: ";
    std::getline(std::cin, input);
    if (!input.empty())
    {
        try
        {
            int stock = std::stoi(input);
            product->setStock(stock);
        }
        catch (...)
        {
            std::cout << "Invalid stock input. Keeping previous value.\n";
        }
    }

    std::cout << "New Price [" << product->getPrice() << "]: ";
    std::cout << "(Enter a number, e.g., 12.99. Use dot for decimals, no currency symbol)\n";
    std::getline(std::cin, input);
    if (!input.empty())
    {
        try
        {
            double price = std::stod(input);
            product->setPrice(price);
        }
        catch (...)
        {
            std::cout << "Invalid price input. Keeping previous value.\n";
        }
    }

    std::cout << "New Supplier ID [" << product->getSupplierId() << "]: ";
    std::getline(std::cin, input);
    if (!input.empty())
    {
        try
        {
            int supplierId = std::stoi(input);
            product->setSupplierId(supplierId);
        }
        catch (...)
        {
            std::cout << "Invalid supplier ID input. Keeping previous value.\n";
        }
    }

    std::cout << "Product updated successfully!\n";
    pause();
}

void editSupplierUI(Warehouse &warehouse)
{
    clearScreen();
    std::cout << "--- Edit Supplier ---\n";
    int id = inputInt("Enter Supplier ID to edit: ");
    Supplier *supplier = warehouse.getSupplierById(id);
    if (!supplier)
    {
        std::cout << "Supplier not found.\n";
        pause();
        return;
    }
    std::cout << "Editing Supplier: " << supplier->getName() << "\n";
    std::cout << "Leave input blank to keep current value.\n";

    std::string input;
    std::cout << "New Name [" << supplier->getName() << "]: ";
    std::getline(std::cin, input);
    if (!input.empty())
        supplier->setName(input);

    std::cout << "New Contact [" << supplier->getContact() << "]: ";
    std::getline(std::cin, input);
    if (!input.empty())
        supplier->setContact(input);

    std::cout << "Supplier updated successfully!\n";
    pause();
}

void editMemberUI(Warehouse &warehouse)
{
    clearScreen();
    std::cout << "--- Edit Member ---\n";
    int id = inputInt("Enter Member ID to edit: ");
    Member *member = warehouse.getMemberById(id);
    if (!member)
    {
        std::cout << "Member not found.\n";
        pause();
        return;
    }
    std::cout << "Editing Member: " << member->getName() << "\n";
    std::cout << "Leave input blank to keep current value.\n";

    std::string input;
    std::cout << "New Name [" << member->getName() << "]: ";
    std::getline(std::cin, input);
    if (!input.empty())
        member->setName(input);

    std::cout << "New Role [" << member->getRole() << "]: ";
    std::getline(std::cin, input);
    if (!input.empty())
        member->setRole(input);

    std::cout << "New Password [hidden]: ";
    std::getline(std::cin, input);
    if (!input.empty())
        member->setPassword(input);

    std::cout << "Member updated successfully!\n";
    pause();
}

// Main function (entry point)
int main()
{
    Warehouse warehouse;
    int choice;
    while (true)
    {
        clearScreen();
        showMainMenu();
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice)
        {
        case 1:
            addProductUI(warehouse);
            break;
        case 2:
            addSupplierUI(warehouse);
            break;
        case 3:
            addMemberUI(warehouse);
            break;
        case 4:
            processOrderUI(warehouse);
            break;
        case 5:
            warehouse.showAllStock();
            break;
        case 6:
            editProductUI(warehouse);
            break;
        case 7:
            editSupplierUI(warehouse);
            break;
        case 8:
            editMemberUI(warehouse);
            break;
        case 9:
            warehouse.showSupplierList();
            break;
        case 10:
            warehouse.showMemberList();
            break;
        case 11:
            warehouse.showMemberOrderCounts();
            break;
        case 12:
            std::cout << "Goodbye!\n";
            return 0;
        default:
            std::cout << "Invalid option!\n";
            pause();
            break;
        }
    }
    return 0;
}

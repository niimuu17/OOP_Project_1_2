#include <bits/stdc++.h>

// ============================================================================
// ANSI COLOR CODE DEFINITIONS FOR TERMINAL FORMATTING
// ============================================================================
#define END "\033[0m"         // Reset all formatting
#define DARK "\033[2m"        // Dim/faint text
#define BOLD "\033[1m"        // Bold text
#define ITALIC "\033[3m"      // Italic text
#define UNDERLINE "\033[4m"   // Underlined text
#define BLINK "\033[5m"       // Blinking text
#define BACK_FILL "\033[7m"   // Reverse video (background becomes foreground)
#define PITCH_BLACK "\033[8m" // Hidden text
#define BLACK "\033[30m"      // Black text
#define RED "\033[31m"        // Red text
#define GREEN "\033[32m"      // Green text
#define YELLOW "\033[33m"     // Yellow text
#define BLUE "\033[34m"       // Blue text
#define PURPLE "\033[35m"     // Purple text
#define CYAN "\033[36m"       // Cyan text

using namespace std;

bool strCompNoCap(const char *a, const char *b)
{
    int lenA = strlen(a), lenB = strlen(b);

    if(lenA == lenB)
    {
        for(int i = 0; i < lenA; ++i) 
        {
            int diff = abs(a[i] - b[i]);

            if(diff != 0 && diff != 32)
            {
                return false;
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}

// ================================================================================
// CUSTOMER CLASS DECLARATION - REPRESENTS A CUSTOMER ITEM IN THE CAFETERIA SYSTEM
// ================================================================================

class Customer;

// ============================================================================
// ITEM CLASS DEFINITION - REPRESENTS A MENU ITEM IN THE CAFETERIA SYSTEM
// ============================================================================
class Item
{
private:
    char name[50];     // Item name stored as fixed-size character array for binary file compatibility
    double price;      // Item price (non-negative)
    int quantity;      // Item quantity (non-negative)
    bool availability; // Availability status based on quantity

public:
    static int itemCount; // Static counter tracking total number of items in the system

    // Constructor with default parameters and input validation
    Item(char name[50] = "", double price = 0.0, int quantity = 0)
        : price(price < 0.0 ? 0.0 : price),      // Ensure price is non-negative
          quantity(quantity < 0 ? 0 : quantity), // Ensure quantity is non-negative
          availability(this->quantity > 0)
    { // Available if quantity > 0
        // Safely copy the name with null termination
        strncpy(this->name, name, sizeof(this->name) - 1);
        this->name[sizeof(this->name) - 1] = '\0';
    }

    // Check if the item is empty/uninitialized
    bool isEmpty() const
    {
        return name[0] == '\0' && price == 0.0 && quantity == 0;
    }

    // Display item details with colored formatting
    void display()
    {
        cout << YELLOW "Name: " END BLUE << name << END << '\n';
        cout << YELLOW "Price: " END BLUE << fixed << setprecision(2) << price << END << '\n';
        cout << YELLOW "Quantity: " END BLUE << quantity << END << '\n';
        cout << YELLOW "Status: " END << (availability ? BLUE "Available" : RED "Out of stock") << END << '\n';
    }

    // Friend function declarations allowing external access to private members
    friend int searchItem(const char *);

    friend bool changeQuantity(const int &, const int &);
    friend bool changeQuantity(const char *, const int &);

    friend bool setPrice(const int &, const double &);
    friend bool setPrice(const char *, const double &);

    friend bool addNewItem(const char *, const double &, const int &);

    friend bool discardExistingItem(int);
    friend bool discardExistingItem(const char *);

    friend void showOrder(Customer &cust);
    friend void orderItem(Customer &cust);
    friend bool cancelItem(Customer &cust);
    friend void orderFromList(Customer &cust);
    friend void showOrder(Customer &cust);
    friend void calculateBill(Customer &cust);
    friend void payment(Customer &cust);
    friend void printBill(Customer &cust);
    friend bool thanksMessage(Customer &cust);
    friend class Customer;
} item[1000]; // Global array storing up to 1000 menu items

// 82 - 145 > 500 - 1160 sajib part ;
// Customer class

class Customer
{
private:
    char name[50];
    char gender[10];
    int tableNo, seatNo;
    vector<pair<int, int>> orders;
    double billAmount;

public:
    static int customerCount;
    static bool seatBooked[20][8];
    static const int maxTables = 20;

    Customer(const char *n = "", const char *g = "", int t = -1, int s = -1)
        : tableNo(t), seatNo(s), billAmount(0.0)
    {
        strncpy(name, n, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        strncpy(gender, g, sizeof(gender) - 1);
        gender[sizeof(gender) - 1] = '\0';
    }

    void addOrder(int itemIndex, int quantity)
    {
        orders.push_back({itemIndex, quantity});
    }
    void clearOrders() { orders.clear(); }
    // friend funcction all
    friend void takeDetails(Customer &cust);
    friend bool bookSeat(Customer &cust, int t, int s);
    friend void showOrder(Customer &cust);
    friend void orderItem(Customer &cust);
    friend bool cancelItem(Customer &cust);
    friend void orderFromList(Customer &cust);
    friend void showOrder(Customer &cust);
    friend void calculateBill(Customer &cust);
    friend void payment(Customer &cust);
    friend void printBill(Customer &cust);
    friend bool thanksMessage(Customer &cust);

    // Save this customer to file1

    void saveToFile()
    {
        ofstream file("customers.bin", ios::binary | ios::app);
        if (!file.is_open())
            return;

        // Write basic info
        file.write(reinterpret_cast<char *>(this), sizeof(Customer));

        // Write order size
        int orderSize = orders.size();
        file.write(reinterpret_cast<char *>(&orderSize), sizeof(orderSize));

        // Write orders
        for (auto &p : orders)
            file.write(reinterpret_cast<char *>(&p), sizeof(p));

        file.close();
    }

    // Load all customers
    static Customer *loadAll(int &count)
    {
        ifstream file("customers.bin", ios::binary);
        if (!file.is_open())
        {
            count = 0;
            return nullptr;
        }

        // First, count how many customers
        vector<Customer> tempCustomers; // Temporary to count
        Customer tempCust;
        while (file.peek() != EOF)
        {
            file.read(reinterpret_cast<char *>(&tempCust), sizeof(Customer));
            int orderCount;
            file.read(reinterpret_cast<char *>(&orderCount), sizeof(orderCount));
            for (int i = 0; i < orderCount; i++)
            {
                pair<int, int> p;
                file.read(reinterpret_cast<char *>(&p), sizeof(p));
            }
            tempCustomers.push_back(tempCust);
        }
        file.close();

        // Create array
        count = tempCustomers.size();
        if (count == 0)
            return nullptr;

        Customer *customers = new Customer[count];
        for (int i = 0; i < count; i++)
        {
            customers[i] = tempCustomers[i];
        }

        return customers;
    }
    // Load seat booking status from file
    static void loadSeatStatus()
    {
        ifstream file("seat_status.bin", ios::binary);
        if (!file.is_open())
            return;

        file.read(reinterpret_cast<char *>(seatBooked), sizeof(seatBooked));
        file.close();
    }

    // Save seat booking status to file
    static void saveSeatStatus()
    {
        ofstream file("seat_status.bin", ios::binary | ios::trunc);
        if (!file.is_open())
            return;

        file.write(reinterpret_cast<char *>(seatBooked), sizeof(seatBooked));
        file.close();
    }
};
int Customer::customerCount = 0;
bool Customer::seatBooked[20][8] = {false};

// end customer classs

// Initialize static class member
int Item::itemCount = 0;

// ============================================================================
// CORE UTILITY FUNCTIONS FOR ITEM MANAGEMENT
// ============================================================================

/**
 * Searches for an item by name in the global item array
 * @param name: Item name to search for (C-style string)
 * @return: Index of found item, or -1 if not found
 */
int searchItem(const char *name)
{
    for (int i = 0; i < Item::itemCount; ++i)
    {
        if (strCompNoCap(name, item[i].name))
        {
            return i;
        }
    }

    return -1;
}

/**
 * Changes the quantity of an item specified by index
 * @param i: Index of the item to modify
 * @param addedQuantity: Amount to add to current quantity (can be negative to decrease)
 * @return: true if successful, false if index invalid or quantity would become negative
 */
bool changeQuantity(const int &i, const int &addedQuantity)
{
    if (i >= Item::itemCount || i < 0)
    {
        return false;
    }

    if (item[i].quantity + addedQuantity > -1)
    {
        item[i].quantity += addedQuantity;

        item[i].availability = item[i].quantity > 0;

        // Save all changes to binary file
        ofstream file("menu.bin", ios::binary | ios::trunc);

        for (int j = 0; j < Item::itemCount; ++j)
        {
            file.write(reinterpret_cast<const char *>(&item[j]), sizeof(Item));
        }

        file.close();

        return true;
    }

    return false;
}

/**
 * Changes the quantity of an item specified by name
 * @param name: Name of the item to modify
 * @param addedQuantity: Amount to add to current quantity
 * @return: true if successful, false if item not found or quantity invalid
 */
bool changeQuantity(const char *name, const int &addedQuantity)
{
    int i = searchItem(name);

    if (i == -1)
    {
        return false;
    }

    return changeQuantity(i, addedQuantity);
}

/**
 * Sets the price of an item specified by index
 * @param i: Index of the item to modify
 * @param price: New price value
 * @return: true if successful, false if index invalid
 */
bool setPrice(const int &i, const double &price)
{
    if (i >= Item::itemCount || i < 0)
    {
        return false;
    }

    if (price < 0.0)
    {
        item[i].price = 0.0;
    }
    else
    {
        item[i].price = price;
    }

    // Save all changes to binary file
    ofstream file("menu.bin", ios::binary | ios::trunc);

    for (int j = 0; j < Item::itemCount; ++j)
    {
        file.write(reinterpret_cast<const char *>(&item[j]), sizeof(Item));
    }

    file.close();

    return true;
}

/**
 * Sets the price of an item specified by name
 * @param name: Name of the item to modify
 * @param price: New price value
 * @return: true if successful, false if item not found
 */
bool setPrice(const char *name, const double &price)
{
    int i = searchItem(name);

    if (i == -1)
    {
        return false;
    }

    return setPrice(i, price);
}

/**
 * Adds a new item to the menu system
 * @param name: Name of the new item
 * @param price: Price of the new item
 * @param quantity: Initial quantity of the new item
 * @return: true if added successfully, false if item already exists
 */
bool addNewItem(const char *name, const double &price, const int &quantity)
{
    int i = searchItem(name);

    if (i != -1)
    {
        return false;
    }

    // Initialize the new item at the current count position
    strncpy(item[Item::itemCount].name, name, sizeof(item[Item::itemCount].name) - 1);
    item[Item::itemCount].name[sizeof(item[Item::itemCount].name) - 1] = '\0';
    item[Item::itemCount].price = price;
    item[Item::itemCount].quantity = (quantity < 0) ? 0 : quantity;
    item[Item::itemCount].availability = quantity > 0;

    // Save all items (including new one) to binary file
    ofstream file("menu.bin", ios::binary | ios::trunc);
    if (!file.is_open())
        return false;

    for (int j = 0; j <= Item::itemCount; ++j)
    {
        file.write(reinterpret_cast<const char *>(&item[j]), sizeof(Item));
    }

    file.close();

    ++Item::itemCount;

    return true;
}

/**
 * Removes an item from the system by index
 * @param i: Index of the item to remove
 * @return: true if removed successfully, false if index invalid
 */
bool discardExistingItem(int i)
{
    if (i >= Item::itemCount || i < 0)
    {
        return false;
    }

    // Shift all subsequent items left to fill the gap
    while (i + 1 < Item::itemCount)
    {
        item[i] = item[i + 1];
        ++i;
    }

    // Clear the last item after shifting
    strcpy(item[i].name, "");
    item[i].price = 0.0;
    item[i].quantity = 0;
    item[i].availability = false;

    --Item::itemCount;

    // Save updated item list to binary file
    ofstream file("menu.bin", ios::binary | ios::trunc);

    for (int j = 0; j < Item::itemCount; ++j)
    {
        file.write(reinterpret_cast<const char *>(&item[j]), sizeof(Item));
    }

    file.close();

    return true;
}

/**
 * Removes an item from the system by name
 * @param name: Name of the item to remove
 * @return: true if removed successfully, false if item not found
 */
bool discardExistingItem(const char *name)
{
    int i = searchItem(name);

    if (i == -1)
    {
        return false;
    }

    return discardExistingItem(i);
}

// ============================================================================
// USER INTERFACE UTILITY FUNCTIONS
// ============================================================================

/**
 * Displays a loading screen with delay for better user experience
 */
void loading(void)
{
    system("clear");
    cout << ITALIC YELLOW "Loading...\n" END;
    this_thread::sleep_for(chrono::milliseconds(800));
    system("clear");
}

/**
 * Prompts user to retry after invalid input
 * @return: true if user wants to retry, false otherwise
 */
bool tryAgain(void)
{
    system("clear");
    cout << YELLOW "Invalid input. Do you want to try again?\n" END;
    cout << ITALIC "[Press " GREEN "y/Y" END ITALIC " for yes or " RED "any other character" END ITALIC " for no]\n" END;

    char choice;
    cin >> choice;
    cin.ignore();

    switch (choice)
    {
    case 'y':
    case 'Y':
        return true;
        break;

    default:
        return false;
        break;
    }
}

// ============================================================================
// USER INTERFACE SCREENS
// ============================================================================

/**
 * Displays the complete menu card with all items
 */
void viewMenuCard(void)
{
    while (true)
    {
        loading();

        cout << BOLD UNDERLINE CYAN "Menu Card\n" END;

        // Display each item with its index and details
        for (int i = 0; i < Item::itemCount; ++i)
        {
            cout << BOLD BLACK "\nIndex: " << i << ".\n" END;

            item[i].display();
        }

        cout << '\n';

        cout << YELLOW "Press any character to go back.\n" END;

        char temp;
        if (cin >> temp)
        {
            cin.ignore();
            return;
        }
        else
        {
            if (tryAgain())
            {
                continue;
            }
        }
    }
}

// ============================================================================
// CUSTOMER FRIEND FUNCTIONS
// ============================================================================

// take customer information
bool bookSeat(Customer &cust, int t, int s);

void takeDetails(Customer &cust)
{
    while (true)
    {
        loading();
        cout << ITALIC GREEN "Please give your infromation.....\n" END;
        cout << "Enter your name: ";
        cin.getline(cust.name, 50);
        cout << "Enter your gender: ";
        char gen[20];
        scanf("%[^\n]", gen);
        cin.ignore();
        if (strcmp(gen, "male") != 0 && strcmp(gen, "female") != 0 && strcmp(gen, "Male") != 0 && strcmp(gen, "Female") != 0)
        {
            if (tryAgain())
                continue;
            else
                return;
        }
        // cin.getline(cust.gender, 10);
        strncpy(cust.gender, gen, sizeof(cust.gender) - 1);
        cust.gender[sizeof(cust.gender) - 1] = '\0';
        Customer::customerCount++;
        cout << BOLD BLUE "Wellcome !!!\n" END;
        this_thread::sleep_for(chrono::milliseconds(500));
        return;
    }
}

// show seat and ask if want to book

void showAvailableSeats(Customer &cust)
{
    while (true)
    {
        loading();
        cout << BLINK BLUE "\n--- Available Seats ---\n" END;
        for (int t = 0; t < Customer::maxTables; t++)
        {
            cout << BOLD CYAN "Table " << setw(2) << setfill('0') << t + 1 << ": " END;
            bool anySeat = false;
            for (int s = 0; s < 8; s++)
            {
                if (!Customer::seatBooked[t][s])
                {
                    cout << BOLD GREEN << s + 1 << " " END;
                    anySeat = true;
                }
                else
                {
                    cout << BOLD RED << s + 1 << " " END;
                }
            }
            if (!anySeat)
                cout << BOLD RED "Full" END;
            cout << "\n";
        }
        cout << BOLD GREEN "Do you want to book a seat (y/n)? " END;
        char choice;
        cin >> choice;
        cin.ignore();

        if (choice == 'y' || choice == 'Y')
        {
            int t, s;
            bool validInput = false;
            while (!validInput)
            {
                cout << BOLD ITALIC "Enter table number (1-20): " END;
                cin >> t;
                cout << BOLD ITALIC "Enter seat number (1-8): " END;
                cin >> s;
                if (cin.fail())
                {
                    cin.clear();
                    cout << RED "Invalid input! Please enter numbers only.\n" END;
                    continue;
                }

                if (t < 1 || t > Customer::maxTables)
                {
                    cout << RED "Invalid table number! Please choose between 1-20.\n" END;
                    continue;
                }

                if (s < 1 || s > 8)
                {
                    cout << RED "Invalid seat number! Please choose between 1-8.\n" END;
                    continue;
                }

                validInput = true;
            }

            if (bookSeat(cust, t, s))
            {
                cout << GREEN "Seat booked successfully!\n" END;
                this_thread::sleep_for(chrono::milliseconds(1000));
            }
            else
            {
                cout << RED "Seat is already booked. Please choose another seat.\n" END;
                this_thread::sleep_for(chrono::milliseconds(1500));
                if (tryAgain())
                {
                    continue;
                }
            }
        }

        else if (choice != 'n' && choice != 'N')
        {
            cout << RED "Invalid choice! Please enter 'y' or 'n'.\n" END;
            if (tryAgain())
            {
                continue;
            }
            this_thread::sleep_for(chrono::milliseconds(1500));
        }
        else
        {
            return;
        }
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}
// booking seat
bool bookSeat(Customer &cust, int t, int s)
{
    system("clear");
    if (t < 1 || t > Customer::maxTables || s < 1 || s > 8)
        return false;
    if (Customer::seatBooked[t - 1][s - 1])
        return false;
    Customer::seatBooked[t - 1][s - 1] = true;
    cust.tableNo = t;
    cust.seatNo = s;
    Customer::saveSeatStatus();
    return true;
}

// order
void orderItem(Customer &cust)
{
    system("clear");
    cout << BOLD UNDERLINE CYAN "Order Items by Name\n" END;

    if (Item::itemCount == 0)
    {
        cout << RED "No items available!\n" END;
        this_thread::sleep_for(chrono::milliseconds(1500));
        return;
    }

    // Show available items
    cout << "\nAvailable Items:\n";
    for (int i = 0; i < Item::itemCount; i++)
    {
        if (item[i].availability)
        {
            cout << GREEN << item[i].name << " - " << fixed << setprecision(2)
                 << item[i].price << " (Stock: " << item[i].quantity << ")\n" END;
        }
    }

    // Get item count
    int n;
    while (true)
    {
        cout << ITALIC "\nHow many different items? " END;
        cin >> n;
        if (cin.fail() || n < 1)
        {
            cin.clear();
            cout << RED "Give correct number!\n" END;
            continue;
        }
        cin.ignore();
        break;
    }

    // Process orders
    for (int k = 0; k < n; ++k)
    {
        char itemName[50];
        cout << ITALIC "\nEnter item name: " END;
        cin.getline(itemName, 50);

        int idx = searchItem(itemName);
        // check eta ache kina

        if (idx == -1)
        {
            cout << RED "Item not found!\n" END;
            k--;
            continue;
        }

        if (!item[idx].availability)
        {
            cout << RED "Item out of stock!\n" END;
            k--;
            continue;
        }

        // Get quantity
        int qty;
        while (true)
        {
            cout << ITALIC "Enter quantity: " END;
            cin >> qty;
            if (cin.fail() || qty < 1 || qty > item[idx].quantity)
            {
                cin.clear();
                cout << RED "Invalid quantity! Available: " << item[idx].quantity << "\n" END;
                continue;
            }
            cin.ignore();
            break;
        }

        // Add to order
        bool found = false;
        for (auto &order : cust.orders)
        {
            if (order.first == idx)
            {
                order.second += qty;
                found = true;
                break;
            }
        }
        if (!found)
            cust.orders.push_back({idx, qty});

        // Update stock
        item[idx].quantity -= qty;
        item[idx].availability = (item[idx].quantity > 0);

        cout << GREEN "Added " << item[idx].name << " x" << qty << "\n" END;
        cust.billAmount += ((item[idx].price) * qty);
        this_thread::sleep_for(chrono::milliseconds(800));
    }

    // Save changes
    ofstream file("menu.bin", ios::binary | ios::trunc);
    for (int j = 0; j < Item::itemCount; ++j)
    {
        file.write(reinterpret_cast<const char *>(&item[j]), sizeof(Item));
    }
    file.close();
    system("clear");
}

// Cancel item
bool cancelItem(Customer &cust)
{
    if (cust.orders.empty())
    {
        cout << RED "No orders to cancel!\n" END;
        this_thread::sleep_for(chrono::milliseconds(1500));
        return false;
    }

    system("clear");
    cout << BOLD UNDERLINE CYAN "Cancel Order\n" END;
    cout << "\nYour current orders:\n";

    for (int i = 0; i < cust.orders.size(); i++)
    {
        int idx = cust.orders[i].first;
        int qty = cust.orders[i].second;
        cout << i + 1 << ". " << item[idx].name << " x" << qty
             << " =  " << fixed << setprecision(2) << (item[idx].price * qty) << "\n";
    }

    //  Added input validation for order number
    int orderNum = -1;
    bool validOrderNum = false;

    //  Loop until valid order number is entered
    while (!validOrderNum)
    {
        cout << ITALIC "\nEnter the order number to cancel (0 to go back): " END;
        cin >> orderNum;

        //  Check for invalid input
        if (cin.fail())
        {
            cin.clear();
            cout << RED "Invalid input! Please enter a number.\n" END;
            continue;
        }
        // back
        if (orderNum == 0)
        {
            cout << "Returning to main menu...\n";
            this_thread::sleep_for(chrono::milliseconds(1000));
            return false;
        }

        //  Validate order number range
        if (orderNum < 1 || orderNum > cust.orders.size())
        {
            cout << RED "Invalid order number! Please choose between 1-" << cust.orders.size() << ".\n" END;
            continue;
        }

        validOrderNum = true;
    }
    cin.ignore();

    int idx = cust.orders[orderNum - 1].first;
    int qty = cust.orders[orderNum - 1].second;

    // Restore quantity
    item[idx].quantity += qty;
    item[idx].availability = item[idx].quantity > 0;

    // Remove from orders
    cust.orders.erase(cust.orders.begin() + orderNum - 1);

    cout << GREEN "Order cancelled successfully!\n" END;
    cust.billAmount -= ((item[idx].price) * qty);
    // Save menu changes
    ofstream file("menu.bin", ios::binary | ios::trunc);
    for (int j = 0; j < Item::itemCount; ++j)
    {
        file.write(reinterpret_cast<const char *>(&item[j]), sizeof(Item));
    }
    file.close();

    this_thread::sleep_for(chrono::milliseconds(1500));
    return true;
}

// order From List
void orderFromList(Customer &cust)
{
    while (true)
    {
        system("clear");
        cout << BOLD UNDERLINE CYAN "Available Items\n" END;
        if (Item::itemCount == 0)
        {
            cout << RED "No items available!\n" END;
            return;
        }
        for (int i = 0; i < Item::itemCount; i++)
        {
            cout << ITALIC "Item " END << i + 1 << ":\n";
            item[i].display();
            cout << "-------------------------\n";
        }
        int choice = 0;
        bool validChoice = false;

        // check for correct input
        while (!validChoice)
        {
            cout << ITALIC "Enter item number to order (0 to go back): " END;
            cin >> choice;

            // Check for invalid input
            if (cin.fail())
            {
                cin.clear();
                cout << RED "Invalid input! Please enter a number.\n" END;
                continue;
            }

            //  Allow user to go back
            if (choice == 0)
            {
                cout << GREEN "Returning to main menu...\n";
                this_thread::sleep_for(chrono::milliseconds(1000));
                return;
            }

            // Validate item number range
            if (choice < 1 || choice > Item::itemCount)
            {
                cout << RED "Invalid item number! Please choose between 1-" << Item::itemCount << ".\n" END;
                continue;
            }

            validChoice = true;
        }
        cin.ignore();

        int idx = choice - 1;
        if (!item[idx].availability)
        {
            cout << RED "Item not available!\n" END;
            this_thread::sleep_for(chrono::milliseconds(1000));
            continue;
        }

        // Added input validation for quantity
        int qty = 0;
        bool validQty = false;

        // Loop until valid quantity is entered
        while (!validQty)
        {
            cout << ITALIC "Enter quantity: " END;
            cin >> qty;

            //  Check for invalid input
            if (cin.fail())
            {
                cin.clear();
                cout << RED "Invalid input! Please enter a number.\n" END;
                continue;
            }

            if (qty <= 0)
            {
                cout << RED "Quantity must be positive!\n" END;
                continue;
            }

            // Check against available stock
            if (qty > item[idx].quantity)
            {
                cout << RED "Only " << item[idx].quantity << " available! Please enter less quantity.\n" END;
                continue;
            }

            validQty = true;
        }
        cin.ignore();

        // Check if item already in orders, if yes, update quantity
        bool found = false;
        for (auto &order : cust.orders)
        {
            if (order.first == idx)
            {
                order.second += qty;
                found = true;
                break;
            }
        }

        if (!found)
        {
            cust.orders.push_back({idx, qty});
        }

        item[idx].quantity -= qty;
        item[idx].availability = item[idx].quantity > 0;

        cout << GREEN "Added " << item[idx].name << " x" << qty << " to your order.\n" END;
        cust.billAmount += ((item[idx].price) * qty);

        // Save menu changes
        ofstream file("menu.bin", ios::binary | ios::trunc);
        for (int j = 0; j < Item::itemCount; ++j)
        {
            file.write(reinterpret_cast<const char *>(&item[j]), sizeof(Item));
        }
        file.close();

        // Added input validation for continue choice
        char more;
        bool validMore = false;

        while (!validMore)
        {
            cout << ITALIC "Do you want to order more? (y/n): " END;
            cin >> more;
            cin.ignore();

            if (more == 'y' || more == 'Y' || more == 'n' || more == 'N')
            {
                validMore = true;
            }
            else
            {
                cout << RED "Invalid choice! Please enter 'y' or 'n'.\n" END;
            }
        }

        if (more != 'y' && more != 'Y')
        {
            cout << GREEN "Order completed!\n" END;
            this_thread::sleep_for(chrono::milliseconds(1000));
            break;
        }
    }
}
//
void showOrder(Customer &cust)
{
    while (true)
    {
        loading();
        if (cust.orders.empty())
        {
            cout << ITALIC RED "You have no order...\n" END;
            this_thread::sleep_for(chrono::milliseconds(1000));
            return;
        }
        cout << ITALIC CYAN UNDERLINE "___This your order___ \n" END;
        int i = 1;
        for (auto &p : cust.orders)
        {
            cout << BOLD CYAN << i << " : " << GREEN << item[p.first].name << " x" << p.second << " = " << fixed << setprecision(2) << item[p.first].price * p.second << endl
                 << END;
            i++;
        }
        char temp;

        cout << YELLOW "\nPress any key to go back.. \n" END;
        if (cin >> temp)
        {
            return;
        }
        else
        {
            if (tryAgain())
                continue;
            else
                return;
        }
    }
}
// bill calculate
void calculateBill(Customer &cust)
{
    cust.billAmount = 0;
    for (auto &p : cust.orders)
    {
        cust.billAmount += item[p.first].price * p.second;
    }
    string bill;
    cout << UNDERLINE CYAN "Your total bill : " END << cust.billAmount << endl;
    this_thread::sleep_for(chrono::milliseconds(2000));
    loading();
}

// payment
void payment(Customer &cust)
{
    if (cust.billAmount == 0.0)
    {
        cout << BOLD RED "Your bill is  0.00 now \n" END;
        this_thread::sleep_for(chrono::milliseconds(1500));
        return;
    }

    system("clear");
    cout << BLINK BLUE "Choose payment method: \n" END;
    cout << BOLD CYAN "1. Cash\n2. Card\n" END;

    int choice = 0;
    bool validChoice = false;
    // check correct input
    while (!validChoice)
    {
        cout << ITALIC "Enter your choice (1-2): " END;
        cin >> choice;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED "Invalid input! Please enter 1 or 2.\n" END;
            continue;
        }

        if (choice == 1 || choice == 2)
        {
            validChoice = true;
        }
        else
        {
            cout << RED "Invalid choice! Please enter 1 for Cash or 2 for Card.\n" END;
            tryAgain();
        }
    }

    if (choice == 1)
    {
        cout << GREEN "Cash payment of  " << fixed << setprecision(2) << cust.billAmount << " successful!\n" END;
        cust.billAmount = 0;
        cust.orders.clear();
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
    else
    {
        cout << GREEN "Card payment of  " << fixed << setprecision(2) << cust.billAmount << " successful!\n" END;
        cust.billAmount = 0;
        cust.orders.clear();
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
    loading();
}
// show order

void printBill(Customer &cust)
{
    cout << BOLD "------- BILL -------\n" END;
    cout << "Customer: " << cust.name << "\n";
    cout << "Table No: " << cust.tableNo << ", Seat No: " << cust.seatNo << "\n";
    for (auto &p : cust.orders)
        cout << item[p.first].name << " x" << p.second << " = "
             << fixed << setprecision(2) << item[p.first].price * p.second << '\n';
    cout << BOLD BLACK "Total Bill: " << cust.billAmount << "\n";
    this_thread::sleep_for(chrono::milliseconds(2000));
}

bool thanksMessage(Customer &cust)
{
    if (cust.billAmount != 0)
    {
        cout << RED "Please pay your bill of " BOLD << cust.billAmount << endl
             << END;
        this_thread::sleep_for(chrono::milliseconds(1000));
        return false;
    }
    cout << GREEN "Thank you, " << cust.name << "! Please visit again.\n" END;
    this_thread::sleep_for(chrono::milliseconds(2000));
    Customer::customerCount--;
    // Free the seat after exit
    if (cust.tableNo != -1 && cust.seatNo != -1)
    {
        Customer::seatBooked[cust.tableNo - 1][cust.seatNo - 1] = false;
        Customer::saveSeatStatus();
    }

    this_thread::sleep_for(chrono::milliseconds(2000));
    return true;
}
void customerInterface(void)
{
    loading();
    Customer cust;
    cout << " " << endl;
    takeDetails(cust);

    // Customer action loop
    while (true)
    {
        loading();
        cout << BOLD UNDERLINE CYAN "Customer Menu\n" END;

        // Menu options (style unchanged, only corrected order)
        cout << BOLD PURPLE "1. " END ITALIC GREEN "Book a seat\n";
        cout << BOLD PURPLE "2. " END ITALIC GREEN "View menu card\n";
        cout << BOLD PURPLE "3. " END ITALIC GREEN "Order item\n";
        cout << BOLD PURPLE "4. " END ITALIC GREEN "Order from the menu card\n";
        cout << BOLD PURPLE "5. " END ITALIC GREEN "View orders\n";
        cout << BOLD PURPLE "6. " END ITALIC GREEN "Cancel order\n";
        cout << BOLD PURPLE "7. " END ITALIC GREEN "Calculate your bill\n";
        cout << BOLD PURPLE "8. " END ITALIC GREEN "Bill payment\n";
        cout << BOLD PURPLE "9. " END ITALIC RED "Exit\n";

        cout << BOLD GREEN " Choice your option " END ITALIC PURPLE "(1-9): ";

        char choice;
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case '1':
            showAvailableSeats(cust);
            continue;
        case '2':
            viewMenuCard();
            continue;
        case '3':
            orderItem(cust);
            continue;
        case '4':
            orderFromList(cust);
            continue;
        case '5':
            showOrder(cust);
            continue;
        case '6':
            cancelItem(cust);
            continue;
        case '7':
            calculateBill(cust);
            continue;
        case '8':
            printBill(cust);
            payment(cust);
            continue;
        case '9':
            if (thanksMessage(cust))
                return;
            else
                continue;
        default:
            if (tryAgain())
                continue;
            return;
        }
    }

    ofstream file("customers.bin", ios::binary | ios::app);
    file.write(reinterpret_cast<const char *>(&cust), sizeof(Customer));
    file.close();
    loading();
}

// sajib don't change yet it's not complete ..........

/**
 * Interface for adding new items to the menu
 */
void addNewItemInterface(void)
{
    while (true)
    {
        loading();

        cout << BOLD UNDERLINE CYAN "Add A New Item\n" END;

        char name[50];
        double price;
        int quantity;

        cout << ITALIC GREEN "Enter its name: " END;
        scanf("%[^\n]", name);

        cout << ITALIC GREEN "Enter its price: " END;
        cin >> price;

        cout << ITALIC GREEN "Enter its quantity: " END;
        cin >> quantity;

        cin.ignore();

        if (addNewItem(name, price, quantity))
        {
            cout << BLUE "Added a new item successfully." END;
            this_thread::sleep_for(chrono::milliseconds(1000));

            return;
        }
        else
        {
            cout << RED "Adding a new item failed.\nItem with the same name already exists." END;
            this_thread::sleep_for(chrono::milliseconds(2000));

            if (tryAgain())
            {
                continue;
            }

            return;
        }
    }
}

/**
 * Interface for increasing item quantities
 */
void increaseQuantityInterface(void)
{
    while (true)
    {
        loading();

        cout << BOLD UNDERLINE CYAN "Increase The Quantity Of An Existing Item\n" END;

        cout << BOLD PURPLE "\nChoose one of the following:\n" END;
        cout << BOLD BLACK "1. " END ITALIC GREEN "Enter The Name Of The Item\n" END;
        cout << BOLD BLACK "2. " END ITALIC GREEN "Enter The Index Of The Item\n" END;
        cout << BOLD BLACK "3. " END ITALIC RED "Go Back\n" END;

        cout << BLUE "\nEnter your choice (1-3): " END;

        char choice;
        cin >> choice;
        cin.ignore();

        char name[50];
        int index, quantity;

        switch (choice)
        {
        case '1':
            cout << ITALIC GREEN "Enter its name: " END;
            scanf("%[^\n]", name);
            cout << ITALIC GREEN "Enter the quantity (must be a positive integer): " END;
            cin >> quantity;

            if (quantity < 1)
            {
                cout << RED "Increasing the quantity of the item failed.";
                cout << "\nThe quantity must be positive.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));

                if (tryAgain())
                {
                    continue;
                }
                else
                {
                    return;
                }
            }

            if (!changeQuantity(name, quantity))
            {
                cout << RED "Increasing the quantity of the item failed.";
                cout << "\nItem with this name does not exist.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));

                if (tryAgain())
                {
                    continue;
                }
                else
                {
                    return;
                }
            }

            cout << BLUE "Increased the quantity successfully." END;
            this_thread::sleep_for(chrono::milliseconds(1000));

            continue;
            break;

        case '2':
            cout << ITALIC GREEN "Enter its index: " END;
            cin >> index;
            cout << ITALIC GREEN "Enter the quantity (must be a positive integer): " END;
            cin >> quantity;

            if (quantity < 1)
            {
                cout << RED "Increasing the quantity of the item failed.";
                cout << "\nThe quantity must be positive.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));

                if (tryAgain())
                {
                    continue;
                }
                else
                {
                    return;
                }
            }

            if (!changeQuantity(index, quantity))
            {
                cout << RED "Increasing the quantity of the item failed.";
                cout << "\nItem with this index does not exist.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));

                if (tryAgain())
                {
                    continue;
                }
                else
                {
                    return;
                }
            }

            cout << BLUE "Increased the quantity successfully." END;
            this_thread::sleep_for(chrono::milliseconds(1000));

            continue;
            break;

        case '3':
            return;
            break;

        default:
            if (tryAgain())
            {
                continue;
            }

            return;
            break;
        }
    }
}

/**
 * Interface for changing item prices
 */
void changePriceInterface(void)
{
    while (true)
    {
        loading();

        cout << BOLD UNDERLINE CYAN "Change The Price Of An Existing Item\n" END;

        cout << BOLD PURPLE "\nChoose one of the following:\n" END;
        cout << BOLD BLACK "1. " END ITALIC GREEN "Enter The Name Of The Item\n" END;
        cout << BOLD BLACK "2. " END ITALIC GREEN "Enter The Index Of The Item\n" END;
        cout << BOLD BLACK "3. " END ITALIC RED "Go Back\n" END;

        cout << BLUE "\nEnter your choice (1-3): " END;

        char choice;
        cin >> choice;
        cin.ignore();

        char name[50];
        int index;
        double price;

        switch (choice)
        {
        case '1':
            cout << ITALIC GREEN "Enter its name: " END;
            scanf("%[^\n]", name);
            cout << ITALIC GREEN "Enter the new price: " END;
            cin >> price;

            if (!setPrice(name, price))
            {
                cout << RED "Changing the price of the item failed.";
                cout << "\nItem with this name does not exist.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));

                if (tryAgain())
                {
                    continue;
                }
                else
                {
                    return;
                }
            }

            cout << BLUE "Changed the price successfully." END;
            this_thread::sleep_for(chrono::milliseconds(1000));

            continue;
            break;

        case '2':
            cout << ITALIC GREEN "Enter its index: " END;
            cin >> index;
            cout << ITALIC GREEN "Enter the new price: " END;
            cin >> price;

            if (!setPrice(index, price))
            {
                cout << RED "Changing the price of the item failed.";
                cout << "\nItem with this index does not exist.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));

                if (tryAgain())
                {
                    continue;
                }
                else
                {
                    return;
                }
            }

            cout << BLUE "Changed the price successfully." END;
            this_thread::sleep_for(chrono::milliseconds(1000));

            continue;
            break;

        case '3':
            return;
            break;

        default:
            if (tryAgain())
            {
                continue;
            }

            return;
            break;
        }
    }
}

/**
 * Interface for removing items from the menu
 */
void discardItemInterface(void)
{
    while (true)
    {
        loading();

        cout << BOLD UNDERLINE CYAN "Discard An Existing Item\n" END;

        cout << BOLD PURPLE "\nChoose one of the following:\n" END;
        cout << BOLD BLACK "1. " END ITALIC GREEN "Enter The Name Of The Item\n" END;
        cout << BOLD BLACK "2. " END ITALIC GREEN "Enter The Index Of The Item\n" END;
        cout << BOLD BLACK "3. " END ITALIC RED "Go Back\n" END;

        cout << BLUE "\nEnter your choice (1-3): " END;

        char choice;
        cin >> choice;
        cin.ignore();

        char name[50];
        int index;

        switch (choice)
        {
        case '1':
            cout << ITALIC GREEN "Enter its name: " END;
            scanf("%[^\n]", name);

            if (!discardExistingItem(name))
            {
                cout << RED "Discarding the item failed.";
                cout << "\nItem with this name does not exist.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));

                if (tryAgain())
                {
                    continue;
                }
                else
                {
                    return;
                }
            }

            cout << BLUE "Discarded the item successfully." END;
            this_thread::sleep_for(chrono::milliseconds(1000));

            continue;
            break;

        case '2':
            cout << ITALIC GREEN "Enter its index: " END;
            cin >> index;

            if (!discardExistingItem(index))
            {
                cout << RED "Discarding the item failed.";
                cout << "\nItem with this index does not exist.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));

                if (tryAgain())
                {
                    continue;
                }
                else
                {
                    return;
                }
            }

            cout << BLUE "Discarded the item successfully." END;
            this_thread::sleep_for(chrono::milliseconds(1000));

            continue;
            break;

        case '3':
            return;
            break;

        default:
            if (tryAgain())
            {
                continue;
            }

            return;
            break;
        }
    }
}

// ============================================================================
// MANAGER INTERFACE - PROVIDES ADMINISTRATIVE FUNCTIONS
// ============================================================================

/**
 * Main manager interface with comprehensive menu management options
 */
void managerInterface(void)
{
    while (true)
    {
        loading();

        cout << BOLD UNDERLINE CYAN "Manager Interface\n" END;

        cout << BOLD PURPLE "\nChoose one of the following:\n" END;
        cout << BOLD BLACK "1. " END ITALIC GREEN "View Menu Card\n" END;
        cout << BOLD BLACK "2. " END ITALIC GREEN "Add A New Item\n" END;
        cout << BOLD BLACK "3. " END ITALIC GREEN "Increase The Quantity Of An Existing Item\n" END;
        cout << BOLD BLACK "4. " END ITALIC GREEN "Change The Price Of An Existing Item\n" END;
        cout << BOLD BLACK "5. " END ITALIC GREEN "Discard An Existing Item\n" END;
        cout << BOLD BLACK "6. " END ITALIC RED "Go Back\n" END;

        cout << BLUE "\nEnter your choice (1 - 6): " END;

        char choice;
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case '1':
            viewMenuCard();
            continue;
            break;

        case '2':
            addNewItemInterface();
            continue;
            break;

        case '3':
            increaseQuantityInterface();
            continue;
            break;

        case '4':
            changePriceInterface();
            continue;
            break;

        case '5':
            discardItemInterface();
            continue;
            break;

        case '6':
            return;
            break;

        default:
            if (tryAgain())
            {
                continue;
            }

            return;
            break;
        }
    }
}

// ============================================================================
// MAIN MENU - USER TYPE SELECTION
// ============================================================================

/**
 * Main menu for selecting between customer and manager interfaces
 */
void mainMenu(void)
{
    while (true)
    {
        loading();

        cout << BOLD UNDERLINE CYAN "Main Menu\n" END;

        cout << BOLD PURPLE "\nChoose one of the following:\n" END;
        cout << BOLD BLACK "1. " END ITALIC GREEN "Customer\n" END;
        cout << BOLD BLACK "2. " END ITALIC GREEN "Manager\n" END;
        cout << BOLD BLACK "3. " END ITALIC RED "Go Back\n" END;

        cout << BLUE "\nEnter your choice (1-3): " END;

        char choice;
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case '1':
            customerInterface();
            continue;
            break;

        case '2':
            managerInterface();
            continue;
            break;

        case '3':
            return;
            break;

        default:
            if (tryAgain())
            {
                continue;
            }

            return;
            break;
        }
    }
}

// ============================================================================
// MAIN FUNCTION - PROGRAM ENTRY POINT
// ============================================================================

/**
 * Program entry point - initializes data and starts the main application loop
 */
int main()
{
    // Initialize file system and load existing menu data
    fstream file("menu.bin", ios::in | ios::binary);

    if (!file.is_open())
    {
        // Create new menu file if it doesn't exist
        file.open("menu.bin", ios::out | ios::binary);
        file.close();

        file.open("menu.bin", ios::in | ios::binary);

        if (!file.is_open())
        {
            cerr << "File opening failed.";
            return 0;
        }
    }

    // Load existing items from binary file into memory
    Item temp;
    while (file.read(reinterpret_cast<char *>(&temp), sizeof(Item)))
    {
        if (!temp.isEmpty())
        {
            item[Item::itemCount] = temp;
            Item::itemCount++;
        }
    }

    file.close();
    // Load customer data and seat status
    Customer::loadSeatStatus();
    int customerCount = 0;
    Customer *customers = Customer::loadAll(customerCount); // Changed from vector to array
    Customer::customerCount = customerCount;

    // Main application loop
    while (true)
    {
        loading();

        cout << BOLD BACK_FILL CYAN "Welcome to Cafeteria\n" END;

        cout << BOLD PURPLE "\nChoose one of the following:\n" END;
        cout << BOLD BLACK "1. " END ITALIC GREEN "Enter\n" END;
        cout << BOLD BLACK "2. " END ITALIC RED "Exit\n" END;

        cout << BLUE "\nEnter your choice (1 - 2): " END;

        char choice;
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case '1':
            mainMenu();
            continue;
            break;

        case '2':
            system("clear");
            cout << BOLD CYAN "Nice to have you. Have a great day!!" END;
            this_thread::sleep_for(chrono::milliseconds(1500));
            system("clear");

            return 0;
            break;

        default:
            if (tryAgain())
            {
                continue;
            }

            system("clear");
            cout << BOLD CYAN "Nice to have you. Have a great day!!" END;
            this_thread::sleep_for(chrono::milliseconds(1500));
            system("clear");

            return 0;
            break;
        }
    }
}

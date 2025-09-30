#include <bits/stdc++.h>

// ============================================================================
// ANSI COLOR CODE DEFINITIONS FOR TERMINAL FORMATTING
// ============================================================================
#define END "\033[0m"           // Reset all formatting
#define DARK "\033[2m"          // Dim/faint text
#define BOLD "\033[1m"          // Bold text
#define ITALIC "\033[3m"        // Italic text  
#define UNDERLINE "\033[4m"     // Underlined text
#define BLINK "\033[5m"         // Blinking text
#define BACK_FILL "\033[7m"     // Reverse video (background becomes foreground)
#define PITCH_BLACK "\033[8m"   // Hidden text
#define BLACK "\033[30m"        // Black text
#define RED "\033[31m"          // Red text
#define GREEN "\033[32m"        // Green text
#define YELLOW "\033[33m"       // Yellow text
#define BLUE "\033[34m"         // Blue text
#define PURPLE "\033[35m"       // Purple text
#define CYAN "\033[36m"         // Cyan text

using namespace std;

// ============================================================================
// ITEM CLASS DEFINITION - REPRESENTS A MENU ITEM IN THE CAFETERIA SYSTEM
// ============================================================================
class Item {
    char name[50];          // Item name stored as fixed-size character array for binary file compatibility
    double price;           // Item price (non-negative)
    int quantity;           // Item quantity (non-negative)
    bool availability;      // Availability status based on quantity

public:
    static int itemCount;   // Static counter tracking total number of items in the system

    // Constructor with default parameters and input validation
    Item(char name[50] = "", double price = 0.0, int quantity = 0) 
    : price(price < 0.0 ? 0.0 : price),                    // Ensure price is non-negative
    quantity(quantity < 0 ? 0 : quantity),                 // Ensure quantity is non-negative
    availability(this->quantity > 0) {                     // Available if quantity > 0
        // Safely copy the name with null termination
        strncpy(this->name, name, sizeof(this->name) - 1);
        this->name[sizeof(this->name) - 1] = '\0';
    }

    // Check if the item is empty/uninitialized
    bool isEmpty() const {
        return name[0] == '\0' && price == 0.0 && quantity == 0;
    }
    
    // Display item details with colored formatting
    void display() {
        cout << YELLOW "Name: " END BLUE << name << END << '\n';
        cout << YELLOW "Price: " END BLUE << fixed << setprecision(2) << price << END << '\n';
        cout << YELLOW "Quantity: " END BLUE << quantity << END << '\n';
        cout << YELLOW "Status: " END << (availability ? BLUE "Available" : RED "Out of stock") << END << '\n';
    }

    // Friend function declarations allowing external access to private members
    friend int searchItem(const char*);

    friend bool changeQuantity(const int&, const int&);
    friend bool changeQuantity(const char*, const int&);

    friend bool setPrice(const int&, const double&);
    friend bool setPrice(const char*, const double&);

    friend bool addNewItem(const char*, const double&, const int&);

    friend bool discardExistingItem(int);
    friend bool discardExistingItem(const char*);
} item[1000];  // Global array storing up to 1000 menu items

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
int searchItem(const char *name) {
    for(int i = 0; i < Item::itemCount; ++i) {
        if(strcmp(name, item[i].name) == 0) {
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
bool changeQuantity(const int &i, const int &addedQuantity) {
    if(i >= Item::itemCount || i < 0) {
        return false;
    }
    
    if(item[i].quantity + addedQuantity > -1) {
        item[i].quantity += addedQuantity;

        item[i].availability = item[i].quantity > 0;

        // Save all changes to binary file
        ofstream file("menu.bin", ios::binary | ios::trunc);
       
        for(int j = 0; j < Item::itemCount; ++j) {
            file.write(reinterpret_cast<const char*>(&item[j]), sizeof(Item));
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
bool changeQuantity(const char *name, const int &addedQuantity) {
    int i = searchItem(name);

    if(i == -1) {
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
bool setPrice(const int &i, const double &price) {
    if(i >= Item::itemCount || i < 0) {
        return false;
    }
    
    if(price < 0.0) {
        item[i].price = 0.0;
    }
    else {
        item[i].price = price;
    }

    // Save all changes to binary file
    ofstream file("menu.bin", ios::binary | ios::trunc);
       
    for(int j = 0; j < Item::itemCount; ++j) {
        file.write(reinterpret_cast<const char*>(&item[j]), sizeof(Item));
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
bool setPrice(const char *name, const double &price) {
    int i = searchItem(name);

    if(i == -1) {
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
bool addNewItem(const char *name, const double &price, const int &quantity) {
    int i = searchItem(name);

    if(i != -1) {
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
    if (!file.is_open()) return false;
    
    for(int j = 0; j <= Item::itemCount; ++j) {
        file.write(reinterpret_cast<const char*>(&item[j]), sizeof(Item));
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
bool discardExistingItem(int i) {
    if(i >= Item::itemCount || i < 0) {
        return false;
    }

    // Shift all subsequent items left to fill the gap
    while(i + 1 < Item::itemCount) {
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
    
    for(int j = 0; j < Item::itemCount; ++j) {
        file.write(reinterpret_cast <const char *> (&item[j]), sizeof(Item));
    }

    file.close();

    return true;
}

/**
 * Removes an item from the system by name
 * @param name: Name of the item to remove
 * @return: true if removed successfully, false if item not found
 */
bool discardExistingItem(const char *name) {
    int i = searchItem(name);

    if(i == -1) {
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
void loading(void) {
    system("clear");
    cout << ITALIC YELLOW "Loading..." END;
    this_thread::sleep_for(chrono::milliseconds(800));
    system("clear");
}

/**
 * Prompts user to retry after invalid input
 * @return: true if user wants to retry, false otherwise
 */
bool tryAgain(void) {
    system("clear");
    cout << YELLOW "Invalid input. Do you want to try again?\n" END;
    cout << ITALIC "[Press " GREEN "y/Y" END ITALIC " for yes or " RED "any other character" END ITALIC " for no]\n" END;

    char choice;
    cin >> choice;

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
void viewMenuCard(void) {
    while(true) {
        loading();
        
        cout << BOLD UNDERLINE CYAN "Menu Card\n" END;

        // Display each item with its index and details
        for(int i = 0; i < Item::itemCount; ++i) {
            cout << BOLD BLACK "\nIndex: " << i << ".\n" END;

            item[i].display();
        }

        cout << '\n';

        cout << YELLOW "Press any character to go back.\n" END;

        char temp;
        if(cin >> temp) {
            cin.ignore();
            return;
        }
        else {
            if(tryAgain()) {
                continue;
            }
        }
    }
}

/**
 * Customer interface (placeholder for future implementation)
 */
void customerInterface(void) {
    // TODO: Implement customer ordering functionality
}

/**
 * Interface for adding new items to the menu
 */
void addNewItemInterface(void) {
    while(true) {
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

        if(addNewItem(name, price, quantity)) {
            cout << BLUE "Added a new item successfully." END;
            this_thread::sleep_for(chrono::milliseconds(1000));

            return;
        }
        else {
            cout << RED "Adding a new item failed.\nItem with the same name already exists." END;
            this_thread::sleep_for(chrono::milliseconds(2000));

            if(tryAgain()) {
                continue;
            }
            
            return;
        }
        
    }
}

/**
 * Interface for increasing item quantities
 */
void increaseQuantityInterface(void) {
    while(true) {
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

            if(quantity < 1) {
                cout << RED "Increasing the quantity of the item failed.";
                cout << "\nThe quantity must be positive.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));
                
                if(tryAgain()) {
                    continue;
                }
                else {
                    return;
                }
            }

            if(!changeQuantity(name, quantity)) {
                cout << RED "Increasing the quantity of the item failed.";
                cout << "\nItem with this name does not exist.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));
                
                if(tryAgain()) {
                    continue;
                }
                else {
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

            if(quantity < 1) {
                cout << RED "Increasing the quantity of the item failed.";
                cout << "\nThe quantity must be positive.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));
                
                if(tryAgain()) {
                    continue;
                }
                else {
                    return;
                }
            }

            if(!changeQuantity(index, quantity)) {
                cout << RED "Increasing the quantity of the item failed.";
                cout << "\nItem with this index does not exist.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));
                
                if(tryAgain()) {
                    continue;
                }
                else {
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
            if(tryAgain()) {
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
void changePriceInterface(void) {
    while(true) {
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

            if(!setPrice(name, price)) {
                cout << RED "Changing the price of the item failed.";
                cout << "\nItem with this name does not exist.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));
                
                if(tryAgain()) {
                    continue;
                }
                else {
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

            if(!setPrice(index, price)) {
                cout << RED "Changing the price of the item failed.";
                cout << "\nItem with this index does not exist.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));
                
                if(tryAgain()) {
                    continue;
                }
                else {
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
            if(tryAgain()) {
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
void discardItemInterface(void) {
    while(true) {
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

            if(!discardExistingItem(name)) {
                cout << RED "Discarding the item failed.";
                cout << "\nItem with this name does not exist.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));
                
                if(tryAgain()) {
                    continue;
                }
                else {
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

            if(!discardExistingItem(index)) {
                cout << RED "Discarding the item failed.";
                cout << "\nItem with this index does not exist.\n" END;
                this_thread::sleep_for(chrono::milliseconds(2000));
                
                if(tryAgain()) {
                    continue;
                }
                else {
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
            if(tryAgain()) {
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
void managerInterface(void) {
    while(true) {
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
            if(tryAgain()) {
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
void mainMenu(void) {
    while(true) {
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
            if(tryAgain()) {
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
int main() {
    // Initialize file system and load existing menu data
    fstream file("menu.bin", ios::in | ios::binary);

    if(!file.is_open()) {
        // Create new menu file if it doesn't exist
        file.open("menu.bin", ios::out | ios::binary);
        file.close();

        file.open("menu.bin", ios::in | ios::binary);

        if(!file.is_open()) {
            cerr << "File opening failed.";
            return 0;
        }
    }

    // Load existing items from binary file into memory
    Item temp;
    while(file.read(reinterpret_cast<char*>(&temp), sizeof(Item))) {
        if(!temp.isEmpty()) {
            item[Item::itemCount] = temp;
            Item::itemCount++;
        }
    }

    file.close();
    
    // Main application loop
    while(true) {
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
            if(tryAgain()) {
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
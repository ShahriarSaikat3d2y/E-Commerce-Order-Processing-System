#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Customer Node (Linked List)
struct Customer {
    int id;
    char name[50];
    struct Customer *next;
};

// Product Node (BST)
struct Product {
    int id;
    char name[50];
    float price;
    struct Product *left, *right;
};

// Order Node (Queue + History)
struct Order {
    int orderId;
    int customerId;
    int productId;
    struct Order *next;
};

// Stack Node (for Undo)
struct StackNode {
    struct Order order;
    struct StackNode *next;
};

struct Customer *customerHead = NULL;
struct Product *productRoot = NULL;
struct Order *orderFront = NULL, *orderRear = NULL;
struct StackNode *orderStackTop = NULL;   // For undo


void pushOrder(struct Order *order) {
    struct StackNode* newNode = (struct StackNode*)malloc(sizeof(struct StackNode));
    newNode->order = *order; // copy order data
    newNode->next = orderStackTop;
    orderStackTop = newNode;
}

struct Order* popOrder() {
    if (!orderStackTop) return NULL;
    struct StackNode *temp = orderStackTop;
    orderStackTop = orderStackTop->next;
    struct Order* o = (struct Order*)malloc(sizeof(struct Order));
    *o = temp->order;
    free(temp);
    return o;
}


void AddNewCustomer(int id, char name[]) {
    struct Customer* newNode = (struct Customer*)malloc(sizeof(struct Customer));
    newNode->id = id;
    strcpy(newNode->name, name);
    newNode->next = customerHead;
    customerHead = newNode;
    printf("Customer %s added.\n", newNode->name);
}

void displayCustomers() {
    struct Customer *temp = customerHead;
    if (!temp) {
        printf("\nNo customers to display.\n");
        return;
    }
    printf("\n--- Customers ---\n");
    while (temp) {
        printf("ID: %d, Name: %s\n", temp->id, temp->name);
        temp = temp->next;
    }
    printf("\n");
}


struct Product* AddNewProduct(struct Product *root, int id, char name[], float price) {
    if (!root) {
        struct Product* newNode = (struct Product*)malloc(sizeof(struct Product));
        newNode->id = id;
        strcpy(newNode->name, name);
        newNode->price = price;
        newNode->left = newNode->right = NULL;
        printf("Product %s added.\n", newNode->name);
        return newNode;
    }
    if (id < root->id)
        root->left = AddNewProduct(root->left, id, name, price);
    else if (id > root->id)
        root->right = AddNewProduct(root->right, id, name, price);
    else
        printf("Product with ID %d already exists. Skipped.\n", id);
    return root;
}

void inorderProducts(struct Product *root) {
    if (root) {
        inorderProducts(root->left);
        printf("ID: %d, Name: %s, Price: %.2f\n", root->id, root->name, root->price);
        inorderProducts(root->right);
    }
}



// Check if customer exists
struct Customer* findCustomer(int id) {
    struct Customer *temp = customerHead;
    while (temp) {
        if (temp->id == id)
            return temp;
        temp = temp->next;
    }
    return NULL;
}


struct Product* findProduct(struct Product *root, int id) {
    if (!root) return NULL;
    if (id == root->id) return root;
    else if (id < root->id) return findProduct(root->left, id);
    else return findProduct(root->right, id);
}


void placeOrder(int orderId, int customerId, int productId) {
    if (!findCustomer(customerId)) {
        printf("Error: Customer %d does not exist. Order not placed.\n", customerId);
        return;
    }

    if (!findProduct(productRoot, productId)) {
          printf("Error: Product %d does not exist. Order not placed.\n", productId);
          return;
      }

    struct Order* newNode = (struct Order*)malloc(sizeof(struct Order));
    newNode->orderId = orderId;
    newNode->customerId = customerId;
    newNode->productId = productId;
    newNode->next = NULL;

    if (!orderRear) {
        orderFront = orderRear = newNode;
    } else {
        orderRear->next = newNode;
        orderRear = newNode;
    }

    // Push into stack for undo
    pushOrder(newNode);

    printf("Order %d placed by Customer %d for Product %d.\n", orderId, customerId, productId);
}

void displayOrders() {
    if (!orderFront) {
        printf("\nNo orders have been placed yet.\n");
        return;
    }
    printf("\n Orders \n");
    struct Order *temp = orderFront;
    while (temp) {
        printf("OrderID: %d, CustomerID: %d, ProductID: %d\n",
               temp->orderId, temp->customerId, temp->productId);
        temp = temp->next;
    }
    printf("\n");
}

// Undo last placed order
void undoLastOrder() {
    struct Order *lastOrder = popOrder();
    if (!lastOrder) {
        printf("No order to undo.\n");
        return;
    }

    // Remove from queue
    struct Order *temp = orderFront, *prev = NULL;
    while (temp) {
        if (temp->orderId == lastOrder->orderId) {
            if (prev) prev->next = temp->next;
            else orderFront = temp->next;
            if (temp == orderRear) orderRear = prev;
            free(temp);
            break;
        }
        prev = temp;
        temp = temp->next;
    }

    printf("Undid Order %d (Customer %d, Product %d)\n",
           lastOrder->orderId, lastOrder->customerId, lastOrder->productId);
    free(lastOrder);
}

// Show order history of a specific customer
void showCustomerHistory(int customerId) {
    int found = 0;
    struct Order *temp = orderFront;
    printf("\n--- Order History for Customer %d ---\n", customerId);
    while (temp) {
        if (temp->customerId == customerId) {
            printf("OrderID: %d, ProductID: %d\n", temp->orderId, temp->productId);
            found = 1;
        }
        temp = temp->next;
    }
    if (!found) printf("No orders found for this customer.\n");
    printf("\n");
}


void adminMenu() {
    int choice, id;
    char name[50];
    float price;

    while (1) {
        printf("\n Admin Menu \n");
        printf("1. Add Customer\n");
        printf("2. Display Customers\n");
        printf("3. Add Product\n");
        printf("4. Display Products\n");
        printf("5. Display Orders\n");
        printf("6. Show Customer Order History\n");
        printf("7. Back to Main Menu\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) { while(getchar()!='\n'); continue; }

        if (choice == 1) {
            printf("Enter Customer ID and Name: ");
            if (scanf("%d %49s", &id, name) == 2)
                AddNewCustomer(id, name);
        }
        else if (choice == 2) {
            displayCustomers();
        }
        else if (choice == 3) {
            printf("Enter Product ID, Name, Price: ");
            if (scanf("%d %49s %f", &id, name, &price) == 3)
                productRoot = AddNewProduct(productRoot, id, name, price);
        }
        else if (choice == 4) {
            if (!productRoot) printf("\nNo products to display.\n");
            else {
                printf("\n Products \n");
                inorderProducts(productRoot);
                printf("\n");
            }
        }
        else if (choice == 5) {
            displayOrders();
        }
        else if (choice == 6) {
            printf("Enter Customer ID: ");
            scanf("%d", &id);
            showCustomerHistory(id);
        }
        else if (choice == 7) return;
        else printf("Invalid choice.\n");
    }
}


void customerMenu() {
    int choice, orderId, custId, prodId;

    while (1) {
        printf("\n Customer Menu \n");
        printf("1. Place Order\n");
        printf("2. Display Products\n");
        printf("3. Undo Last Order\n");
        printf("4. Back to Main Menu\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) { while(getchar()!='\n'); continue; }

        if (choice == 1) {
            printf("Enter OrderID, CustomerID, ProductID: ");
            if (scanf("%d %d %d", &orderId, &custId, &prodId) == 3)
                placeOrder(orderId, custId, prodId);
        }
        else if (choice == 2) {
            if (!productRoot) printf("\nNo products to display.\n");
            else {
                printf("\n--- Products ---\n");
                inorderProducts(productRoot);
                printf("\n");
            }
        }
        else if (choice == 3) {
            undoLastOrder();
        }
        else if (choice == 4) return;
        else printf("Invalid choice.\n");
    }
}


int main() {
    int roleChoice;

    while (1) {
        printf("\n Main Menu\n");
        printf("1. Admin\n");
        printf("2. Customer\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &roleChoice) != 1) { while(getchar()!='\n'); continue; }

        if (roleChoice == 1) adminMenu();
        else if (roleChoice == 2) customerMenu();
        else if (roleChoice == 3) {
            printf("Exiting program...\n");
            break;
        }
        else printf("Invalid choice.\n");
    }
    return 0;
}

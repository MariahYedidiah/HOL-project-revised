#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function prototypes
void display_menu(int sock);

int main() {
    int sock;
    struct sockaddr_in server_address;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to bank server.\n");

    char buffer[BUFFER_SIZE];
    char username[50], password[50];

    // Receive and send login credentials
    recv(sock, buffer, sizeof(buffer), 0);  // Receive "Login Username" prompt
    printf("%s", buffer);
    scanf("%s", username);
    send(sock, username, strlen(username), 0);

    recv(sock, buffer, sizeof(buffer), 0);  // Receive "Password" prompt
    printf("%s", buffer);
    scanf("%s", password);
    send(sock, password, strlen(password), 0);

    // After authentication, display the menu
    display_menu(sock);

    close(sock);
    return 0;
}

// Function to display the menu and interact with server based on user choices
void display_menu(int sock) {
    int choice;
    char buffer[BUFFER_SIZE];
    float amount;

    while (1) {
        // Receive menu options from server
        memset(buffer, 0, BUFFER_SIZE);
        recv(sock, buffer, sizeof(buffer), 0);
        printf("%s", buffer);

        // Get the user's choice and send it to server
        printf("Enter your choice: ");
        scanf("%d", &choice);
        send(sock, &choice, sizeof(choice), 0);

        if (choice == 9) {  // Logout
            printf("Logging out...\n");
            break;
        } else if (choice == 10) {  // Exit
            printf("Exiting...\n");
            exit(0);
        }

        // Receive and print server response for each choice
        memset(buffer, 0, BUFFER_SIZE);
        recv(sock, buffer, sizeof(buffer), 0);
        printf("%s\n", buffer);
        
        // Additional inputs for choices like deposit, withdraw, etc.
        if (choice == 2 || choice == 3 || choice == 4) {  // Deposit, Withdraw, Transfer Funds
            printf("Enter amount: ");
            scanf("%f", &amount);
            send(sock, &amount, sizeof(amount), 0);

            // Receive confirmation or error message after transaction
            memset(buffer, 0, BUFFER_SIZE);
            recv(sock, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);
        } else if (choice == 6) {  // Change Password
            printf("Enter new password: ");
            scanf("%s", buffer);
            send(sock, buffer, strlen(buffer), 0);

            // Receive confirmation message after password change
            memset(buffer, 0, BUFFER_SIZE);
            recv(sock, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);
        } else if (choice == 7) {  // Add Feedback
            printf("Enter feedback: ");
            scanf(" %[^\n]", buffer);
            send(sock, buffer, strlen(buffer), 0);

            // Receive confirmation message after feedback submission
            memset(buffer, 0, BUFFER_SIZE);
            recv(sock, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);
        }
    }
}


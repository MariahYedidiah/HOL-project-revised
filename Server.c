#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include<sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include<unistd.h>
#include "headerfile.h"

#define PORT 8080

// Structure to hold customer information

static int init_status=0;
// Function prototypes
void handle_client(int client_socket);
int authenticate_user(int client_socket, Customers *customer);
void view_balance(int client_socket, Customers *customer);
void deposit_money(int client_socket, Customers *customer);
void withdraw_money(int client_socket, Customers *customer);
/*void transfer_funds(int client_socket, Customers *customer);
void apply_loan(int client_socket, Customers *customer);
void change_password(int client_socket, Customers *customer);
void add_feedback(int client_socket);
void view_transaction_history(int client_socket, Customers *customer);
void logout(int client_socket);
*/
int lock_record(int fd, int record_num, short lock_type);
void unlock_record(int fd, int record_num);
int update_balance(int cus_id, float amount);

int main() {
if(!init_status){	initialize();init_status=1;}
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Socket creation and binding
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, NOC) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Bank server is running...\n");

    while (1) {
        // Accept client connection
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }
        printf("Client connected...\n");

        // Fork a new process to handle the client
        if (fork() == 0) {
            // Child process: handle the client
            close(server_fd);  // Close the server socket in child process
            handle_client(client_socket);
            exit(0);  // End child process after handling client
        }
        // Parent process: close the client socket and continue accepting new clients
        close(client_socket);
    }
    close(server_fd);
    return 0;
}

// Function to handle client requests
void handle_client(int client_socket) {
     Customers customer;
    memset(&customer, 0, sizeof(customer));

    if (authenticate_user(client_socket, &customer)) {
        char buffer[BUFFER_SIZE];
        while (1) {
            // Send menu options to the client
            snprintf(buffer, sizeof(buffer),
                     "1. View Balance\n"
                     "2. Deposit Money\n"
                     "3. Withdraw Money\n"
                     "4. Transfer Funds\n"
                     "5. Apply for Loan\n"
                     "6. Change Password\n"
                     "7. Add Feedback\n"
                     "8. View Transaction History\n"
                     "9. Logout\n"
                     "10. Exit\n"
                     "Select an option: ");//writing to buffer
            send(client_socket, buffer, strlen(buffer), 0);

            // Receive client choice
            int choice;
            recv(client_socket, &choice, sizeof(choice), 0);

            // Perform the selected operation
            switch (choice) {
                case 1: view_balance(client_socket, &customer); break;
                case 2: deposit_money(client_socket, &customer); break;
                case 3: withdraw_money(client_socket, &customer); break;
               /* case 4: transfer_funds(client_socket, &customer); break;
                case 5: apply_loan(client_socket, &customer); break;
                case 6: change_password(client_socket, &customer); break;
                case 7: add_feedback(client_socket); break;
                case 8: view_transaction_history(client_socket, &customer); break;
                case 9: logout(client_socket); return;  // End session
                case 10: close(client_socket); exit(0); // Exit
               */ default: printf("Invalid choice\n"); break;
            }
        }
    } else {
        printf("Authentication failed for client.\n");
        close(client_socket);
    }
}

// Implement each function based on requirements (authentication, view_balance, etc.)
// Here, we can define simple placeholders or use system calls for file operations

int authenticate_user(int client_socket, Customers *customer) {

char buffer[BUFFER_SIZE];bzero(buffer,BUFFER_SIZE);
snprintf(buffer, sizeof(buffer)," Login\n Username:");
 send(client_socket, buffer, strlen(buffer), 0);
char name[50];
recv(client_socket,&name,sizeof(name),0);
bzero(buffer,BUFFER_SIZE);

snprintf(buffer, sizeof(buffer)," \nPassword :");

 send(client_socket, buffer, strlen(buffer), 0);
char password[50];
recv(client_socket,&password,sizeof(password),0);

    
     int fd = open(CUSTOMER_RECORDS, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file for reading");
        exit(EXIT_FAILURE);
    }

    Customers cus;
	for(int i=0;i<NOC;i++)
	{
	    
	  if (read(fd, &cus, sizeof(Customers)) != sizeof(Customers)) {
            perror("Failed to read cus record");
            close(fd);
            exit(EXIT_FAILURE);
        }
        if(strcmp(cus.name,name)==0 && strcmp(cus.password,password)==0)
          { *customer=cus;
              return 1;
          
        }
        
	}


    return 0;  // Return 1 if successful, 0 otherwise
}

void view_balance(int client_socket, Customers *customer) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "Your balance is: %.2f\n", customer->balance);
    send(client_socket, buffer, strlen(buffer), 0);
}
void deposit_money(int client_socket,Customers *customer)
{
  char buffer[BUFFER_SIZE];
  
  snprintf(buffer,sizeof(buffer),"\nEnter amount to be deposited\n"); 
  send(client_socket, buffer, strlen(buffer), 0);
  double deposit_amount;
            recv(client_socket, &deposit_amount, sizeof(deposit_amount), 0);
            if (update_balance(customer->cust_id, deposit_amount) == 0) {
        printf("Updated balance for cus ID %d\n", customer->cust_id);
    } else {
        printf("Failed to update balance\n");
    }

            
  
}
void withdraw_money(int client_socket,Customers *customer)
{
  char buffer[BUFFER_SIZE];
  
  snprintf(buffer,sizeof(buffer),"\nEnter amount to be withdrawn\n"); 
  send(client_socket, buffer, strlen(buffer), 0);
  double wthdrw_amount;
            recv(client_socket, &wthdrw_amount, sizeof(wthdrw_amount), 0);
            if(wthdrw_amount>customer->balance){
            char buf[777];
            snprintf(buf,sizeof(buf),"\nInsufficient balance\n");
            send(client_socket,buffer,strlen(buffer),0);
            return;
            }
            if (update_balance(customer->cust_id, -wthdrw_amount) == 0) {
        printf("Updated balance for cus ID %d\n", customer->cust_id);
    } else {
        printf("Failed to update balance\n");
    }

            
  
}
// Define additional functions for each banking operation similarly
// and use file locking (fcntl) for concurrency and data consistency
// e.g., deposit_money, withdraw_money, transfer_funds, apply_loan, etc.
///////////////////////////////////////////////////////////////////////////////
///
///
///



// Function to lock a specific record for reading or writing
int lock_record(int fd, int record_num, short lock_type) {
    struct flock lock;
    lock.l_type = lock_type;  // F_RDLCK for read, F_WRLCK for write
    lock.l_whence = SEEK_SET;
    lock.l_start = record_num * sizeof(Customers);
    lock.l_len = sizeof(Customers);
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Failed to lock record");
        return -1;
    }
    return 0;
}

// Function to unlock a specific record
void unlock_record(int fd, int record_num) {
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = record_num * sizeof(Customers);
    lock.l_len = sizeof(Customers);
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("Failed to unlock record");
    }
}

// Function to update the balance for a specific cus
int update_balance(int cus_id, float amount) {
    int fd = open(CUSTOMER_RECORDS, O_RDWR);
    if (fd == -1) {
        perror("Failed to open file for updating");
        return -1;
    }

    int record_num = cus_id - 1;
    if (lock_record(fd, record_num, F_WRLCK) == -1) {
        close(fd);
        return -1;
    }

    // Seek to the record's position and read the cus
    if (lseek(fd, record_num * sizeof(Customers), SEEK_SET) == -1) {
        perror("Failed to seek to record");
        unlock_record(fd, record_num);
        close(fd);
        return -1;
    }

    Customers cus;
    if (read(fd, &cus, sizeof(Customers)) != sizeof(Customers)) {
        perror("Failed to read record");
        unlock_record(fd, record_num);
        close(fd);
        return -1;
    }

    // Update the balance
    cus.balance += amount;
    //printf("New balance for cus ID %d: %.2f\n", cus.cus_id, cus.balance);

    // Write the updated cus back to the file
    if (lseek(fd, record_num * sizeof(Customers), SEEK_SET) == -1) {
        perror("Failed to seek to record for writing");
        unlock_record(fd, record_num);
        close(fd);
        return -1;
    }

    if (write(fd, &cus, sizeof(Customers)) != sizeof(Customers)) {
        perror("Failed to write updated record");
        unlock_record(fd, record_num);
        close(fd);
        return -1;
    }

    // Unlock the record after updating
    unlock_record(fd, record_num);
    close(fd);
    return 0;
}

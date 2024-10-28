
#ifndef generalHeader
#define generalHeader

#include<stdio.h>
#define NOC 7
#define NOE 5
#define NOM 3
#define NOT 7
#define BUFFER_SIZE 777
#define CUSTOMER_RECORDS "Customerfile.txt"
typedef enum {
    ROLE_CUSTOMER,
    ROLE_EMPLOYEE,
    ROLE_MANAGER,
    ROLE_ADMIN,
    ROLE_UNKNOWN
} UserRole;


void initialize();
extern char admin_pswd[10];extern int cur_noc,cur_nom,cur_noe;
extern char logout_msg[];
extern char exit_msg[];

/*void handle_client(int client_socket);
UserRole verify_user(const char *username, const char *password,int *);
void handle_user_session(UserRole role,int client_socket,int *);

void handle_customer_session();
void handle_employee_session();
void handle_manager_session();
void handle_admin_session();
*/
extern int All_loans_queue[NOC];
enum loanstatus{
	not_applied,
	awaiting_approval,
	approved,
	rejected
};
enum ROLE{manager,
	regular};

typedef struct {
 char  date[12];
 char  narration[20];
 double withdrawn;
 double deposit;
 double closingbalance;
}Transaction;
typedef struct {
        char  name [50] ;
        char  password[50];
        double balance;
        int cust_id;
	long int acc_no;
        enum loanstatus lns;
        char  feedback[100];
	Transaction T[NOT];//CIRCULAR QUEUE using array show last<=NOT transactions
	int front,rear;
}Customers;

typedef struct {
	char  name [50] ;
	char  password[50];
	
	int emp_id;
        int loan_q[NOC];
  	     	enum ROLE role;	
		double salary;
}Employees;


/*
typedef struct {
    unsigned char *encrypted_string;
    int length;
} EncryptedData;
*/

//extern Customers  cus[];extern Employees emp[], mng[];

//function declarations in C are  by default extern
void enQ(Transaction t,Transaction T[]);
void deQ(Transaction T);
#endif // generalHeader






#include <stdio.h>
#include "headerfile.h"
#include <unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#ifndef ADMINPSWD
#define ADMINPSWD
char admin_pswd[10];

#endif

// Check if the arrays are already defined
#ifndef CUS_DEFINED
#define CUS_DEFINED
Customers cus[NOC];
int cur_noc=-1;// Define global Customers array
#endif
/*
#ifndef EMP_DEFINED
#define EMP_DEFINED
Employees emp[NOE];
int cur_noe=-1;// Define global Employees array
#endif

#ifndef MNG_DEFINED
#define MNG_DEFINED
Employees mng[NOM];
int cur_nom=-1;// Define global Managers array
#endif
*/

void initialize()
{
	
    int fd = open(CUSTOMER_RECORDS, O_CREAT|O_WRONLY|O_EXCL, 0644);
    if (fd == -1) return;

    Customers cus;
    for (int i = 0; i < NOC; i++) {
        cus.cust_id = i + 1;
        snprintf(cus.name, sizeof(cus.name), "user%d", i + 1);
        snprintf(cus.password, sizeof(cus.password), "pass%d", i + 1);
        cus.balance = 1000.0;  // Initial balance for all cuss
    	cus.lns=not_applied;  
        // Write the cus to file
        if (write(fd, &cus, sizeof(Customers)) != sizeof(Customers)) {
            perror("Failed to write cus record");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }
    close(fd);
    printf("All records initialized successfully.\n");
    
	}
    

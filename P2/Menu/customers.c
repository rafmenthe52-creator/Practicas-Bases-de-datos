#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "customers.h"
#include "odbc.h"

#define BUFFER_LEN 512

static int ShowCustomersSubMenu(void);

/**
 * @brief Shows the Customers submenu and processes user selection.
 *
 *
 * @return selected option
 * @author Alejandro Echeverria
 */

void ShowCustomersMenu() {
    int nChoice = 0;
    char *customername;
    int customernumber;
    

    do {
        nChoice = ShowCustomersSubMenu();
        switch (nChoice) {
            case 1: {
                fprintf(stdin, "Enter the customer name to find > ");
                fscanf(stdin, "%s", customername);
                FindCustomer(customername);
            }
                break;

            case 2: {
                fprintf(stdin, "Enter the customer number to list products > ");
                fscanf(stdin, "%d", &customernumber);
                ListProducts(customernumber);
            }
                break;

            case 3: {
                fprintf(stdin, "Enter the customer number to get balance > ");
                fscanf(stdin, "%d", &customernumber);
                Balance(customernumber);
            }
                break;

            case 4: {
                printf("Bye Bye\n\n");
            }
                break;
        }
    } while (nChoice != 4);

}

/**
 * @brief prints the Customers submenu and allows to select an option.
 *
 * @return selected option
 * @author Alejandro Echeverria
 */
 int ShowCustomersSubMenu() {
    int nSelected = 0;
    char buf[16];

    do {

        printf(" (1) Find\n"
               " (2) List Products\n"
               " (3) Balance\n"
               " (4) Back\n\n");

        printf("Enter a number that corresponds to your choice > ");
        if (!fgets(buf, 16, stdin))
            /* reading input failed, give up: */
            nSelected = 0;
        else
            /* have some input, convert it to integer: */
            nSelected = atoi(buf);
        printf("\n");

        if ((nSelected < 1) || (nSelected > 4)) {
            printf("You have entered an invalid choice. Please try again\n\n\n");
        }
    } while ((nSelected < 1) || (nSelected > 4));

    return nSelected;
}

/

/**
 * @brief Opción 1: Find Customers por nombre o apellido de contacto.
 */
int FindCustomer(SQLHDBC dbc, char *name) {

    SQLHSTMT stmt = NULL;

    int ret;
    SQLRETURN ret2;

    char x[BufferLength] = "\0";
    char y[BufferLength] = "\0";
    char z[BufferLength] = "\0";
    char w[BufferLength] = "\0";

    SQLINTEGER n;
    int i = 0;

    /* Add % before and after the name */
    x[0] = '%';

    while (name[i] != '\0' && i < BufferLength - 3) {
        x[i + 1] = name[i];
        i++;
    }

    x[i + 1] = '%';
    x[i + 2] = '\0';

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", dbc, SQL_HANDLE_DBC);
        return EXIT_FAILURE;
    }

    ret = SQLPrepare(stmt, (SQLCHAR *)
        "SELECT customerNumber, customerName, contactFirstName, contactLastName "
        "FROM customers "
        "WHERE contactFirstName LIKE ? OR contactLastName LIKE ? "
        "ORDER BY customerNumber", SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Bind parameters */
    (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, BufferLength, 0, x, BufferLength, NULL);

    (void) SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, BufferLength, 0, x, BufferLength, NULL);

    /* Execute query */
    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Bind columns */
    (void) SQLBindCol(stmt, 1, SQL_C_SLONG, &n, 0, NULL);
    (void) SQLBindCol(stmt, 2, SQL_C_CHAR, y, BufferLength, NULL);
    (void) SQLBindCol(stmt, 3, SQL_C_CHAR, z, BufferLength, NULL);
    (void) SQLBindCol(stmt, 4, SQL_C_CHAR, w, BufferLength, NULL);

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%d | %s | %s | %s\n", n, y, z, w);
    }

    if (ret != SQL_NO_DATA) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLCloseCursor(stmt);

    /* Free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


/**
 * @brief Opción 2: List Products comprados por un cliente específico.
 */
int ListProducts(SQLHDBC dbc, int customerNumber) {

    SQLHSTMT stmt = NULL;

    int ret;
    SQLRETURN ret2;

    char productName[BufferLength] = "\0";
    SQLINTEGER totalUnits = 0;
    SQLINTEGER custNum = (SQLINTEGER) customerNumber;

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", dbc, SQL_HANDLE_DBC);
        return EXIT_FAILURE;
    }

    /* Prepare statement */
    ret = SQLPrepare(stmt, (SQLCHAR *)
        "SELECT p.productName, SUM(od.quantityOrdered) "
        "FROM orders o "
        "JOIN orderdetails od ON o.orderNumber = od.orderNumber "
        "JOIN products p ON od.productCode = p.productCode "
        "WHERE o.customerNumber = ? "
        "GROUP BY od.productCode, p.productName "
        "ORDER BY od.productCode", SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Bind parameter */
    ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                           SQL_C_SLONG, SQL_INTEGER,
                           0, 0, &custNum, 0, NULL);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Execute query */
    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Bind columns */
    (void) SQLBindCol(stmt, 1, SQL_C_CHAR, productName, BufferLength, NULL);
    (void) SQLBindCol(stmt, 2, SQL_C_SLONG, &totalUnits, 0, NULL);

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%s | %d\n", productName, totalUnits);
    }

    if (ret != SQL_NO_DATA) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLCloseCursor(stmt);

    /* Free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Opción 3: Balance total (pagos - compras) del cliente.
 */
int Balance(SQLHDBC dbc, int customerNumber) {

    SQLHSTMT stmt = NULL;

    int ret;
    SQLRETURN ret2;

    double balance = 0.0;
    SQLINTEGER custNum = (SQLINTEGER) customerNumber;

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", dbc, SQL_HANDLE_DBC);
        return EXIT_FAILURE;
    }

    /* Prepare statement */
    ret = SQLPrepare(stmt, (SQLCHAR *)
        "SELECT "
        "  COALESCE((SELECT SUM(amount) FROM payments WHERE customernumber = ?), 0) - "
        "  COALESCE((SELECT SUM(od.quantityordered * od.priceeach) "
        "            FROM orders o "
        "            JOIN orderdetails od ON o.ordernumber = od.ordernumber "
        "            WHERE o.customernumber = ?), 0) AS balance", SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Bind parameters */
    ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                           SQL_C_SLONG, SQL_INTEGER,
                           0, 0, &custNum, 0, NULL);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    ret = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT,
                           SQL_C_SLONG, SQL_INTEGER,
                           0, 0, &custNum, 0, NULL);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Execute query */
    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    /* Bind column */
    (void) SQLBindCol(stmt, 1, SQL_C_DOUBLE, &balance, 0, NULL);

    /* Fetch: solo devuelve 1 fila con el saldo total */
    if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("Balance = %.2f\n", balance);
    } else {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLCloseCursor(stmt);

    /* Free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
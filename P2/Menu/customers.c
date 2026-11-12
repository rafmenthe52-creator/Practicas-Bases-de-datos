#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "customers.h"
#include "odbc.h"

#define BufferLength 512

typedef struct {
    int customerNumber;
    char customerName[BufferLength];
    char contactFirstName[BufferLength];
    char contactLastName[BufferLength];
} CustomerRow;

typedef struct {
    char productName[BufferLength];
    int totalUnits;
} CustomerProductRow;

static int ShowCustomersSubMenu(void);

/**
 * @brief Solicita un nombre/apellido de contacto y lista los clientes coincidentes.
 * @param dbc Manejador de la conexión activa.
 * @author Alejandro Echeverria
 * @return EXIT_SUCCESS o EXIT_FAILURE.
 */
static int FindCustomer(SQLHDBC dbc, char *name);

/**
 * @brief Solicita el ID de un cliente y lista los productos comprados y unidades totales.
 * @param dbc Manejador de la conexión activa.
 * @author Alejandro Echeverria
 * @return EXIT_SUCCESS o EXIT_FAILURE.
 */
static int ListProducts(SQLHDBC dbc, SQLINTEGER customerNumber);

/**
 * @brief Solicita el ID de un cliente y calcula su saldo (pagos - compras).
 * @param dbc Manejador de la conexión activa.
 * @author Alejandro Echeverria
 * @return EXIT_SUCCESS o EXIT_FAILURE.
 */
static int Balance(SQLHDBC dbc, int customerNumber);

/**
 * @brief Shows the Customers submenu and processes user selection.
 * @param dbc Active database connection handle.
 * @author Alejandro Echeverria
 */
void ShowCustomersMenu(SQLHDBC dbc) {
    int nChoice = 0;
    char input[BufferLength] = "\0";
    int customernumber = 0;

    do {
        nChoice = ShowCustomersSubMenu();
        switch (nChoice) {
            case 1: {
                printf("Enter customer name > ");
                if (fgets(input, (int) sizeof(input), stdin) != NULL) {
                    input[strcspn(input, "\r\n")] = '\0';
                    (void) FindCustomer(dbc, input);
                }
            }
                break;

            case 2: {
                printf("Enter customer number > ");
                if (fgets(input, (int) sizeof(input), stdin) != NULL) {
                    customernumber = atoi(input);
                    (void) ListProducts(dbc, (SQLINTEGER) customernumber);
                }
            }
                break;

            case 3: {
                printf("Enter customer number > ");
                if (fgets(input, (int) sizeof(input), stdin) != NULL) {
                    customernumber = atoi(input);
                    (void) Balance(dbc, customernumber);
                }
            }
                break;

            case 4: {
                printf("Bye Bye\n\n");
            }
                break;

            default:
                break;
        }
    } while (nChoice != 4);
}

/**
 * @brief Prints the Customers submenu and allows to select an option.
 * @return Selected option.
 * @author Alejandro Echeverria
 */
static int ShowCustomersSubMenu(void) {
    int nSelected = 0;
    char buf[16] = "\0";

    do {
        printf(" (1) Find\n"
               " (2) List Products\n"
               " (3) Balance\n"
               " (4) Back\n\n");

        printf("Enter a number that corresponds to your choice > ");
        if (!fgets(buf, (int) sizeof(buf), stdin)) {
            nSelected = 0;
        } else {
            nSelected = atoi(buf);
        }
        printf("\n");

        if ((nSelected < 1) || (nSelected > 4)) {
            printf("You have entered an invalid choice. Please try again\n\n\n");
        }
    } while ((nSelected < 1) || (nSelected > 4));

    return nSelected;
}

/**
 * @brief Opción 1: Find Customers por nombre o apellido de contacto (con paginación).
 */
int FindCustomer(SQLHDBC dbc, char *name) {
    SQLHSTMT stmt = NULL;
    SQLRETURN ret;

    char x[BufferLength] = "\0";
    char y[BufferLength] = "\0";
    char z[BufferLength] = "\0";
    char w[BufferLength] = "\0";

    SQLINTEGER n = 0;
    int i = 0;

    CustomerRow *rows = NULL;
    CustomerRow *tmp = NULL;
    int capacity = 16;
    int count = 0;

    /* Add % before and after the name */
    x[0] = '%';
    while (name[i] != '\0' && i < BufferLength - 3) {
        x[i + 1] = name[i];
        i++;
    }
    x[i + 1] = '%';
    x[i + 2] = '\0';

    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLAllocHandle", dbc, SQL_HANDLE_DBC);
        return EXIT_FAILURE;
    }

    ret = SQLPrepare(stmt, (SQLCHAR *)
        "SELECT customerNumber, customerName, contactFirstName, contactLastName "
        "FROM customers "
        "WHERE contactFirstName ILIKE ? OR contactLastName ILIKE ? "
        "ORDER BY customerNumber", SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLPrepare", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, BufferLength, 0, x, BufferLength, NULL);
    (void) SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, BufferLength, 0, x, BufferLength, NULL);

    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLExecute", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLBindCol(stmt, 1, SQL_C_SLONG, &n, 0, NULL);
    (void) SQLBindCol(stmt, 2, SQL_C_CHAR, y, BufferLength, NULL);
    (void) SQLBindCol(stmt, 3, SQL_C_CHAR, z, BufferLength, NULL);
    (void) SQLBindCol(stmt, 4, SQL_C_CHAR, w, BufferLength, NULL);

    rows = (CustomerRow *) calloc((size_t) capacity, sizeof(CustomerRow));
    if (rows == NULL) {
        (void) SQLCloseCursor(stmt);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        if (count >= capacity) {
            capacity *= 2;
            tmp = (CustomerRow *) calloc((size_t) (capacity * 2), sizeof(CustomerRow));
            if (tmp == NULL) {
                free(rows);
                (void) SQLCloseCursor(stmt);
                (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
                return EXIT_FAILURE;
            }
           for (i = 0; i < count; i++) {
                tmp[i] = rows[i];
            }
            free(rows);
            rows = tmp;
            capacity *= 2;
        }
        rows[count].customerNumber = (int) n;
        (void) strncpy(rows[count].customerName, y, BufferLength - 1);
        rows[count].customerName[BufferLength - 1] = '\0';
        (void) strncpy(rows[count].contactFirstName, z, BufferLength - 1);
        rows[count].contactFirstName[BufferLength - 1] = '\0';
        (void) strncpy(rows[count].contactLastName, w, BufferLength - 1);
        rows[count].contactLastName[BufferLength - 1] = '\0';
        count++;
    }

    if (ret != SQL_NO_DATA) {
        odbc_extract_error("SQLFetch", stmt, SQL_HANDLE_STMT);
        free(rows);
        (void) SQLCloseCursor(stmt);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLCloseCursor(stmt);
    (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    if (count == 0) {
        printf("No customers found.\n\n");
        free(rows);
        return EXIT_SUCCESS;
    }

    for (i = 0; i < count; i++) {
        printf("%d %s %s %s\n",
               rows[i].customerNumber,
               rows[i].customerName,
               rows[i].contactFirstName,
               rows[i].contactLastName);
    }

    printf("\n");
    free(rows);
    return EXIT_SUCCESS;
}

/**
 * @brief Opción 2: List Products comprados por un cliente específico (con paginación).
 */
static int ListProducts(SQLHDBC dbc, SQLINTEGER customernumber) {
    SQLHSTMT stmt = NULL;
    SQLRETURN ret;

    char y[BufferLength] = "\0";
    char nav[16] = "\0";
    SQLINTEGER z = 0;

    CustomerProductRow *rows = NULL;
    CustomerProductRow *tmp = NULL;
    int capacity = 16;
    int count = 0;
    int page = 0;
    int start = 0;
    int end = 0;
    int i = 0;

    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLAllocHandle", dbc, SQL_HANDLE_DBC);
        return EXIT_FAILURE;
    }

    ret = SQLPrepare(stmt,
        (SQLCHAR *)
        "SELECT p.productName, SUM(od.quantityOrdered) "
        "FROM products p "
        "JOIN orderdetails od ON p.productCode = od.productCode "
        "JOIN orders o ON od.orderNumber = o.orderNumber "
        "WHERE o.customerNumber = ? "
        "GROUP BY p.productCode, p.productName "
        "ORDER BY p.productCode",
        SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLPrepare", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
        SQL_C_SLONG, SQL_INTEGER, 0, 0,
        &customernumber, 0, NULL);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLBindParameter", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLExecute", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLBindCol(stmt, 1, SQL_C_CHAR, y, (SQLLEN) sizeof(y), NULL);
    (void) SQLBindCol(stmt, 2, SQL_C_SLONG, &z, 0, NULL);

    rows = (CustomerProductRow *) calloc((size_t) capacity, sizeof(CustomerProductRow));
    if (rows == NULL) {
        (void) SQLCloseCursor(stmt);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        if (count >= capacity) {
            tmp = (CustomerProductRow *) calloc((size_t) (capacity * 2), sizeof(CustomerProductRow));
            if (tmp == NULL) {
                free(rows);
                (void) SQLCloseCursor(stmt);
                (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
                return EXIT_FAILURE;
            }
            for (i = 0; i < count; i++) {
                tmp[i] = rows[i];
            }
            free(rows);
            rows = tmp;
            capacity *= 2;
        }
        (void) strncpy(rows[count].productName, y, BufferLength - 1);
        rows[count].productName[BufferLength - 1] = '\0';
        rows[count].totalUnits = (int) z;
        count++;
    }

    if (ret != SQL_NO_DATA) {
        odbc_extract_error("SQLFetch", stmt, SQL_HANDLE_STMT);
        free(rows);
        (void) SQLCloseCursor(stmt);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLCloseCursor(stmt);
    (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    if (count == 0) {
        printf("No products found for this customer.\n\n");
        free(rows);
        return EXIT_SUCCESS;
    }

    if (count <= 10) {
        for (i = 0; i < count; i++) {
            printf("%s\t%d\n", rows[i].productName, rows[i].totalUnits);
        }
        printf("\n");
        free(rows);
        return EXIT_SUCCESS;
    }

    for (;;) {
        start = page * 10;
        end = start + 10;
        if (end > count) {
            end = count;
        }

        for (i = start; i < end; i++) {
            printf("%s\t%d\n", rows[i].productName, rows[i].totalUnits);
        }

        printf("\nPage %d/%d ", page + 1, (count + 9) / 10);
        if (page > 0) {
            printf("[< Previous] ");
        }
        if (end < count) {
            printf("[> Next] ");
        }
        printf("[q Exit] > ");

        if (fgets(nav, (int) sizeof(nav), stdin) == NULL) {
            break;
        }

        if (nav[0] == '>' && end < count) {
            page++;
        } else if (nav[0] == '<' && page > 0) {
            page--;
        } else {
            break;
        }
        printf("\n");
    }

    printf("\n");
    free(rows);
    return EXIT_SUCCESS;
}

/**
 * @brief Opción 3: Balance total (pagos - compras) del cliente.
 */
int Balance(SQLHDBC dbc, int customerNumber) {
    SQLHSTMT stmt = NULL;
    SQLRETURN ret;

    double balance = 0.0;
    SQLINTEGER custNum = (SQLINTEGER) customerNumber;

    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLAllocHandle", dbc, SQL_HANDLE_DBC);
        return EXIT_FAILURE;
    }

    ret = SQLPrepare(stmt, (SQLCHAR *)
        "SELECT "
        "  COALESCE((SELECT SUM(amount) FROM payments WHERE customernumber = ?), 0) - "
        "  COALESCE((SELECT SUM(od.quantityordered * od.priceeach) "
        "            FROM orders o "
        "            JOIN orderdetails od ON o.ordernumber = od.ordernumber "
        "            WHERE o.customernumber = ?), 0) AS balance", SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLPrepare", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &custNum, 0, NULL);
    (void) SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &custNum, 0, NULL);

    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLExecute", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLBindCol(stmt, 1, SQL_C_DOUBLE, &balance, 0, NULL);

    if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("Balance = %.2f\n\n", balance);
    } else {
        odbc_extract_error("SQLFetch", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLCloseCursor(stmt);
    if (!SQL_SUCCEEDED(SQLFreeHandle(SQL_HANDLE_STMT, stmt))) {
        odbc_extract_error("SQLFreeHandle", stmt, SQL_HANDLE_STMT);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

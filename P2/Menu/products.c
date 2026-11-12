#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "products.h"
#include "odbc.h"

#define BufferLength 512

static int ShowProductsSubMenu(void);

/**
 * @brief Shows the Products submenu and processes user selection.
 * @param dbc Active database connection handle.
 * @author Alejandro Echeverria
 */
void ShowProductsMenu(SQLHDBC dbc) {
    int nChoice = 0;
    char productCode[BufferLength] = "\0";
    char productName[BufferLength] = "\0";

    do {
        nChoice = ShowProductsSubMenu();
        switch (nChoice) {
            case 1: {
                printf("Enter the product code to check stock > ");
                if (fgets(productCode, (int) sizeof(productCode), stdin)) {
                    productCode[strcspn(productCode, "\r\n")] = '\0';
                    (void) ProductStock(dbc, productCode);
                }
            }
                break;

            case 2: {
                printf("Enter the product name to find > ");
                if (fgets(productName, (int) sizeof(productName), stdin)) {
                    productName[strcspn(productName, "\r\n")] = '\0';
                    (void) ProductFind(dbc, productName);
                }
            }
                break;

            case 3: {
                printf("Bye Bye\n\n");
            }
                break;

            default:
                break;
        }
    } while (nChoice != 3);
}

/**
 * @brief Prints the Products submenu and allows to select an option.
 * @return Selected option.
 * @author Alejandro Echeverria
 */
static int ShowProductsSubMenu(void) {
    int nSelected = 0;
    char buf[16] = "\0";

    do {
        printf(" (1) Stock\n"
               " (2) Find\n"
               " (3) Back\n\n");

        printf("Enter a number that corresponds to your choice > ");
        if (!fgets(buf, (int) sizeof(buf), stdin)) {
            nSelected = 0;
        } else {
            nSelected = atoi(buf);
        }
        printf("\n");

        if ((nSelected < 1) || (nSelected > 3)) {
            printf("You have entered an invalid choice. Please try again\n\n\n");
        }
    } while ((nSelected < 1) || (nSelected > 3));

    return nSelected;
}

/**
 * @brief Consulta y muestra el stock disponible de un producto.
 */
int ProductStock(SQLHDBC dbc, char *productCode) {
    SQLHSTMT stmt = NULL;
    int ret;
    SQLRETURN ret2;
    SQLINTEGER stock = 0;

    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", dbc, SQL_HANDLE_DBC);
        return EXIT_FAILURE;
    }

    ret = SQLPrepare(stmt, (SQLCHAR *)
        "SELECT quantityinstock "
        "FROM products "
        "WHERE productcode = ?", SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                           SQL_C_CHAR, SQL_VARCHAR,
                           strlen(productCode), 0,
                           productCode, 0, NULL);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLBindCol(stmt, 1, SQL_C_SLONG, &stock, 0, NULL);

    if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%d\n\n", stock);
    } else {
        printf("Product not found\n\n");
    }

    (void) SQLCloseCursor(stmt);

    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Busca y lista todos los productos cuyo nombre contenga la subcadena dada.
 */
int ProductFind(SQLHDBC dbc, char *name) {
    SQLHSTMT stmt = NULL;
    int ret;
    SQLRETURN ret2;

    char x[BufferLength] = "\0";
    char code[BufferLength] = "\0";
    char productName[BufferLength] = "\0";
    int i = 0;

    x[0] = '%';
    while (name[i] != '\0' && i < BufferLength - 3) {
        x[i + 1] = name[i];
        i++;
    }
    x[i + 1] = '%';
    x[i + 2] = '\0';

    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", dbc, SQL_HANDLE_DBC);
        return EXIT_FAILURE;
    }

    ret = SQLPrepare(stmt, (SQLCHAR *)
        "SELECT productcode, productname "
        "FROM products "
        "WHERE productname LIKE ? "
        "ORDER BY productcode", SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                            SQL_C_CHAR, SQL_VARCHAR,
                            BufferLength, 0, x, BufferLength, NULL);

    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLBindCol(stmt, 1, SQL_C_CHAR, code, BufferLength, NULL);
    (void) SQLBindCol(stmt, 2, SQL_C_CHAR, productName, BufferLength, NULL);

    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%s\t%s\n", code, productName);
    }

    if (ret != SQL_NO_DATA) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return EXIT_FAILURE;
    }

    (void) SQLCloseCursor(stmt);

    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return EXIT_FAILURE;
    }

    printf("\n");
    return EXIT_SUCCESS;
}
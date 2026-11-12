#ifndef PRODUCTS_H
#define PRODUCTS_H

#include <sql.h>
#include <sqlext.h>

/**
 * @brief Muestra el submenú de Products y procesa la selección del usuario.
 * @param dbc Manejador de la conexión activa a la base de datos.
 * @author Alejandro Echeverria
 */
void ShowProductsMenu(SQLHDBC dbc);

/**
 * @brief Consulta y muestra el stock disponible de un producto por su productcode.
 * @param dbc Manejador de la conexión activa a la base de datos.
 * @param productCode Código identificador del producto.
 * @author Alejandro Echeverria
 * @return EXIT_SUCCESS si la consulta finaliza correctamente, EXIT_FAILURE en caso de error.
 */
int ProductStock(SQLHDBC dbc, char *productCode);

/**
 * @brief Busca y lista todos los productos cuyo nombre contenga la subcadena indicada.
 * @param dbc Manejador de la conexión activa a la base de datos.
 * @param name Subcadena a buscar en el nombre del producto.
 * @author Alejandro Echeverria
 * @return EXIT_SUCCESS si la consulta finaliza correctamente, EXIT_FAILURE en caso de error.
 */
int ProductFind(SQLHDBC dbc, char *name);

#endif


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


#endif


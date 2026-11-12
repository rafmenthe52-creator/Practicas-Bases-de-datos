#ifndef ORDERS_H
#define ORDERS_H

#include <sql.h>
#include <sqlext.h>

/**
 * @brief Muestra el submenú de Orders y procesa la selección del usuario.
 * @param dbc Manejador de la conexión activa a la base de datos.
 * @author Rafael Velasco Wagner
 */
void ShowOrdersMenu(SQLHDBC dbc);


#endif


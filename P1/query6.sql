SELECT
    od1.productcode AS producto_1,
    od2.productcode AS producto_2,
    COUNT(*) AS numero_carros
FROM orderdetails AS od1
JOIN orderdetails AS od2
    ON od1.ordernumber = od2.ordernumber
   AND od1.productcode < od2.productcode
GROUP BY
    od1.productcode,
    od2.productcode
HAVING COUNT(*) > 1
ORDER BY
    numero_carros DESC;
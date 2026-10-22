SELECT
    c.customernumber,
    c.customername,
    SUM(p.amount) AS total_pagado
FROM payments AS p
JOIN customers AS c
    ON p.customernumber = c.customernumber
WHERE EXISTS (
    SELECT *
    FROM orders AS o
    JOIN orderdetails AS od
        ON o.ordernumber = od.ordernumber
    JOIN products AS pr
        ON od.productcode = pr.productcode
    WHERE o.customernumber = c.customernumber
      AND pr.productname = '1940 Ford Pickup Truck'
)
GROUP BY
    c.customernumber,
    c.customername
ORDER BY
    total_pagado DESC;
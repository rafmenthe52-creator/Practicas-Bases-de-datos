select p.productline, avg(o.shippeddate - o.orderdate) as tiempo_medio_dias
from orders as o
	join orderdetails od on o.ordernumber = od.ordernumber
	join products p on p.productcode = od.productcode 
where o.shippeddate is not null 
group by p.productline 
order by p.productline
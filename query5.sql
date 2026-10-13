select o.country, count(o.officecode) as oficinas_sin_ventas
from offices o
	where o.officecode not in (
		select distinct e.officecode
		from employees as e
			join customers c on e.employeenumber = c.salesrepemployeenumber 
			join orders ord on c.customernumber = ord.customernumber 
			WHERE EXTRACT(YEAR FROM ord.orderdate) = '2003'
	)
group by o.country
order by oficinas_sin_ventas desc;

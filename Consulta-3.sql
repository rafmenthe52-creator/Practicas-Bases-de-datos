select e.employeenumber, e.lastname
from employees e
	join employees m on e.reportsto = m.employeenumber 
	join employees d on m.reportsto = d.employeenumber 
where d.reportsto is null
group by e.employeenumber;
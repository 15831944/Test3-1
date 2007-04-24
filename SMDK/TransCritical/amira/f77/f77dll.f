	function dll_double_square_f77 (value)



	double precision dll_double_square_f77, value
c
	dll_double_square_f77 = 5*value**2
	return
	end



	function dllarray (arr)

	dimension  arr(10)
	do 10, i = 1, 10
 10	   arr(i) = i
	dllarray = 42.0

	return 
	end
 

	   

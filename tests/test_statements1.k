--testing statements
-----------------------

--variables
-----------------------
--x : var char --not working. what are the types available?
--y1 : var REFERENCE --not working. what is a reference in cpp?
--y2 : var REFERENCE --not working. what is a reference in cpp?

--if test
-----------------------
--if 1 = 1 x = 2 else x = 3 end --working

--case test
-----------------------
--select x in case 5 test end --not working
select x in case 1 : x = x + 1 case 2 : x = x + 2 case 3 : x = x + 2 else x = x + 3 end

--caselabel test
-----------------------
--5 --untested
--5 .. 5 --untested

--while test
-----------------------
--while 1 = 1 do x = 1; putchar('a') end --used to work
--while 1 = 1 x = 1; putchar('a') end --used to work

--until test
-----------------------
--do x = 1 until x = 4  --working

--for test
-----------------------
--for x in TYPE do x = 5; end --working
--for x in TYPE x = 5; end  --working

--handler test
-----------------------
--try x = 1 ; handle y1 y2 : x = 3; end  --working
--try x = 1 ; handle y1 y2 y3 : x = 3; end  --working
--try x = 1 ; handle y1 , y2 y3 : x = 3; end  --comma not working

--raise test
-----------------------
--raise y --segFault

--procedure call test
-----------------------
--y --working

--assignment test
-----------------------
--x = 5;  --working with declaration check commented 


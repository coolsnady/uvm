print('test_types begin')
var a1 = 123
var a2 = 4.56
var a3 = "abc"
var a4 = true
var a5 = [a1, a2, a3, a4]

pprint(a1, a2, a3, a4, a5)

local a={name=1,b=print,c={a=1,b="hi"}}
b = tostring(a)
c = tojsonstring(a)
print('b='..b)
pprint('c='..c)

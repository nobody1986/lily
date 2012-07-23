<html>
<head>
<title>Lily Sanity Test</title>
</head>
<body>
<@lily
method manyargs (integer addA, integer addB, integer addC, integer addD,
integer addE, integer addF):integer {
    return addA
}

manyargs(1,2,3,4,5,6)

# Test very basic assignments.
str a = "a"
integer b = 1
number c = 1.0
integer a_b = 1

# Test utf-8 labels and strs.
str ustr = "á"
str h3llö = "hello"

# Test assigning different types to an object.
object o
o = 1
o = 1.1
o = 1

# Random function call.
print("Hello")

# Declaration list check.
str d, e, f
str g

# Test ast merging, specifically for binary ops.
integer math

math = 1 + 2 + 3
math = 4 - 5
math = 50

# Test comparisons.
integer compa, compb, compc, compd, compe
compa = "1" >= "1"
compb = "1" > "1"
compc = "1" < "1"
compd = "1" <= "1"
compe = "1" == "1"

integer aa, ab

aa = 1
ab = 2

# No { after the first : means each if/elif/else gets a single line.
# These single-line ifs do not nest.
if aa == 1:
    aa = ab
elif aa == 2:
    aa = 3
elif aa == 3:
    aa = 4
else:
    aa = 4

# In Lily, braces are only needed for the start and end of an if. This is a
# multi-line if test.
if aa == 1: {
    aa = 2
    aa = 3
elif aa = 2:
    aa = 4
    aa = 5
else:
    aa = 6
    aa = 7
}

if aa == 1: {
    if aa == 1: {
        if aa == 1: {
            if aa == 1: {
if aa == 1: {
    if aa == 1: {
        if aa == 1: {
            if aa == 1: {
if aa == 1: {
    if aa == 1: {
        if aa == 1: {
            if aa == 1: {
                aa = 2
            }
        }
    }
}
            }
        }
    }
}
            }
        }
    }
}

method t(integer a):str {
    return "str"
}

method t2(str a):str {
    return a
}

print(t(1))
# method t uses the same storage to return the value each time. These three
# calls make sure that the storage isn't ref'd each time. If that happened,
# there would be more refs than values (memory leak).
t(1)
t(1)
t(1)
t2("str")

method add (integer addA, integer addB):integer {
    return addA + addB
}

add(1+1, 1+1)
integer addcheck = add(1, 1) + 10

integer foldcheck = add(1+1>1+1, 1)

@>
</body>
</html>

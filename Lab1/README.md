# Lab Task -1

**Task : Write a C program to generate a set of Ndistinct random integers. N is an input from the user.** <br />

**LCG**: Xn=(aXn-1+b) mod c <br />

**Rules**: X1=2, X2=(aX1+b) mod c, X3=(aX2+b) mod c <br />

1. Own method: XOR + bit-shift/any other <br />
2. LCG Method: Xn=(aXn-1+b) mod c <br />
3. Lagged Fibonacci method: Xi=Xi-p\*Xi-q (p, q) are basically called lags <br />

### Criteria:

1. Numbers should not exhibit easily recognizable patterns. Some patterns are:<br />
   a. Falling within same range [0-10] or [0-100] or etc..<br />
   b. Having same number of digits<br />
   c. Having repetitions or large period<br />
   d. Should not blindly use rand() function. Should explain how they are generating the random numbers/strings. <br />

**Task :Write a C program to generate a set of N distinct random strings.** <br />

### Grading Criteria:

1. Characters must be printable on screen.<br />
2. Should not repeat strings or characters too many times.<br />

**_Task: Implement a stack that will store Integers_**

### Grading Criteria:

1. Show the basic operations: push, pop, display <br />
2. Test for 5 to 10 integers. <br />

### Additional Useful Notes

Include a program that checks that the output really does consist of distinct values.

### Libraries of relevance

stdio.h
stdlib.h
time.h

### Library functions

rand(), srand(seed), time(),scanf()

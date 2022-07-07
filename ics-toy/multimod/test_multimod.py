import os
import random

f = True
for i in range(100000):
    a = random.randint(0, 2**63-1)
    b = random.randint(0, 2**63-1)
    m = random.randint(0, 2**63-1)

    REF = a * b % m
    DUT1 = int(os.popen("./multimod-64 {} {} {}".format(a, b, m)).read())

    if REF != DUT1 or REF != DUT2:
        print("False a={} b={} m={} REF={} DUT1={}".format(
            a, b, m, REF, DUT1))
        f = False

if f:
    print("No Error")

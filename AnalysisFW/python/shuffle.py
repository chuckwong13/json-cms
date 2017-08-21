import random
with open('prueba.txt','r') as source:
    data = [ (random.random(), line) for line in source ]
    data.sort()
    with open('empty.txt','w') as target:
        for _, line in data:
                target.write( line )

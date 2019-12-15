#!/usr/bin/python

import random

# Parity Dataset (for recurrent networks)

n=10
seq = 20


for j in range(seq):
 parity = True

 for i in range(n):

  inp =	random.random() > 0.5
  
  if inp:
    parity = not parity
    
  print int(inp)
  print int(parity)
  
 print "#"
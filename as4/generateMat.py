from math import *
import random
f = open('mat.t','w')
f.write('dl 1 0 0 1 1 1\n')
f.write('dl 0 1 0 1 1 1\n')
f.write('dl 0 -1 0 1 1 1\n')
f.write('kr .9 .9 .9\n')
f.write('ks 1 1 1\n')
f.write('sp 100\n')
f.write('ka .2 .2 .2\n')
z=0
y=-4
while z > -8:
	z -=1
	y+=1
	for x in range(-4,4,2):
		f.write('kd '+str(random.random())+' '+str(random.random())+' '+str(random.random())+'\n')
		f.write("translate "+str(x)+" "+str(y)+ " " + str(z)+" \n")
		f.write('sph .5\n')
		f.write('kd '+str(random.random())+' '+str(random.random())+' '+str(random.random())+'\n')
		f.write("translate "+str(x)+" "+str(-y)+ " " + str(z)+" \n")
		f.write('sph .5\n')		
f.write('kd '+str(random.random())+' '+str(random.random())+' '+str(random.random())+'\n')
f.write('translate 0 0 -16\n')
f.write('sph 8\n')
f.write('cleartrans\n')
f.write('translate 10 0 10\n')
f.write('rotate 0 .67 0\n')
f.write('camera\n')
f.write('print mat.png\n')
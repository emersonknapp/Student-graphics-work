from math import *
import random
 
def uniquer(seq, idfun=None): # http://code.activestate.com/recipes/52560/
	# Generates a unique list of elements from seq
	# maintaining their original order
	if idfun is None:
		def idfun(x): return x
	seen = {}
	result = []
	for item in seq:
		marker = idfun(item)
		if marker in seen: continue
		seen[marker] = 1
		result.append(item)
	return result
 
def x(theta,alpha):
	return int(round(alpha*theta*cos(theta)/(2*pi)))
 
def y(theta,alpha):
	return int(round(alpha*theta*sin(theta)/(2*pi)))
 
def delta(alpha, theta, omega):
	return (2*pi*omega)/sqrt(pow(alpha,2)*(1+pow(theta,2)))
 
def dist(a,b):
	return sqrt(pow(a[0]-b[0],2)+pow(a[1]-b[1],2))
 
def rasterspiral(TL,BR,F):
	# Distance between spiral loops
#	alphas=1/sqrt(2)
	alphas=.4
	# Distance along spiral at each step
	omegas=1/sqrt(2)
	# Starting theta
	thetas=0
 
	# Define top left pixel and bottom right pixel coordinates
	(tlx,tly)=TL
	(brx,bry)=BR
	# Spiral focus coordinates (must lie within rectangle above)
	(xf,yf)=F
 
	# Find which corner is farthest from the spiral focus
	corners=[[tlx,tly],[tlx,bry],[brx,bry],[brx,tlx]]
	distances=[dist([xf,yf],i) for i in corners]
	farthestind=max(xrange(len(distances)), key=distances.__getitem__)
	farthest=corners[farthestind]
 
	# Initialise the spiral at F
	xc,yc=xf,yf
	results=[(xc,yc)]
 
	# Keep spiralling until we've capture the farthest corner
	while [xc,yc]!=farthest:
		# Update the step in angle based on current angle etc.
		deltanew=delta(alphas,thetas,omegas)
		thetas=thetas+deltanew
		# Calculate the next coordinates
		xc,yc=xf+x(thetas,alphas),yf+y(thetas,alphas)
		# If these coordinates lie within the defined rectangle
		# then store them
		if tlx<=xc<=brx and tly<=yc<=bry:
			results.append((xc,yc))
 
	# Now eliminate duplicate copies of points on the spiral
	results=uniquer(results)
	return results
 
# Example of use:
f = open('spiral.t','w')
f.write('dl 0 0 -1 1 1 1\n')
f.write('dl 1 0 0 1 1 1\n')
f.write('kr .9 .9 .9\n')
f.write('ks 1 1 1\n')
f.write('sp 100\n')
f.write('ka .2 .2 .2\n')
test=rasterspiral((-4,-4),(4,4),(0,0))
z = -1
for xy in test[1:]:
	f.write('kd '+str(random.random())+' '+str(random.random())+' '+str(random.random())+'\n')
	f.write('translate '+str(xy[0])+' '+str(xy[1])+' '+str(z)+'\n')
	f.write('sph .5\n')
	z -= 1
f.write('kd '+str(random.random())+' '+str(random.random())+' '+str(random.random())+'\n')
f.write('translate -8 0 -12\n')
f.write('sph 5\n')
f.write('kd '+str(random.random())+' '+str(random.random())+' '+str(random.random())+'\n')
f.write('translate 0 8 -12\n')
f.write('sph 5\n')
f.write('kd '+str(random.random())+' '+str(random.random())+' '+str(random.random())+'\n')
f.write('translate 0 0 -60\n')
f.write('sph 10\n')
f.write('cleartrans\n')
f.write('translate 3 0 10\n')
f.write('rotate 0 .2 0\n')
f.write('camera\n')
f.write('print spiral.png\n')
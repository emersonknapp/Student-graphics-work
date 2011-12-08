#messing around with textures

pl 0 -5-10 1 1 1 100
#dl 0 0 -1 1 1 1 100
sp 100

translate 0 -0 25
#rotate .3 0 0
cam

ct

#rotate .3 0 0
#back wall
v -10 10 -20
v 10 10 -20
v -10 -10 -20
v 10 -10 -20

#forward corners
v -10 10 -10
v 10 10 -10
v -10 -10 -10
v 10 -10 -10

vt 0 0
vt 1 0
vt 1 1
vt 0 1

tex textures/BlackMarble.png

#back wall
kd 0 0 0
ks 0 0 0
kr 0 0 0
f 3/1 2/3 1/4
f 3/1 4/2 2/3

ctex

#top wall
kd .1 .2 .7
ks .1 .1 .1
kr 0 0 0
f 1 6 5
f 1 2 6

#left wall
kd .6 .2 .4
ks .1 .1 .1
f 7 1 5
f 7 3 1

#right wall
kd .4 .4 .1
ks .1 .1 .1
f 4 6 2
f 4 8 6

#bottom wall

tex textures/world-map.png
kd 0 0 0
ks 0 0 0
f 7/1 4/3 3/4
f 7/1 8/2 4/3

ctex

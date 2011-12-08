#two refractive spheres over a checkerboard texture

tex textures/checkerboard.png

pl 0 0 10 1 1 1 100

translate 0 0 20
#rotate -.3 0 0
cam

ct
v -10 -10 -10
v 10 -10 -10
v 10 10 -10
v -10 10 -10

v -2 -2 -9
v 2 -2 -9
v 2 2 -9
v -2 2 -9

#glass slab
v -4 -4 -9
v 4 -4 -9
v 4 4 -9
v -4 4 -9
v -4 -4 -8
v 4 -4 -8
v 4 4 -8
v -4 4 -8

vt 0 0
vt 1 0
vt 1 1
vt 0 1

kd 0 0 0
ks 0 0 0
f 1/1 3/3 4/4
f 1/1 2/2 3/3

ctex

#kd .5 .3 .1
#ks .1 .1 .1
#f 5 7 8
#f 5 6 7

kd 0 0 0
ks 1 1 1
#ri 1.0
#translate 0 0 3
#s 3

#ri 2.0
#translate 0 0 0
#s 4

#ri 1.6
#translate 0 0 8
#s 1

kd 0 0 0
ks 1 1 1
sp 50
kr 0 0 0
ri 1.5
translate -5 -5 -3
s 2.5

kd 0 0 0
ks 1 1 1
sp 50
kr 0 0 0
ri 2.0
translate 5 -5 -5
s 2.5

#faces of glass slab
kd 0 0 0
ks 1 1 1
sp 50
kr 0 0 0
ri 1.5

translate 0 0 4 
#rotate .5 -.5 0

f 12 11 10 9

f 13 14 15 16

#f 13 15 16
#f 13 14 15

#sides of glass slab
#f 14 11 15
#f 14 10 11

#f 15 12 16
#f 15 11 12

#f 16 9 13
#f 16 12 9

#f 13 10 14
#f 13 9 10

ct

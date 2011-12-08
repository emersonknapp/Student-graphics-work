#two refractive spheres over a checkerboard texture

tex textures/checkerboard.png

pl 0 0 5 1 1 1 100

translate 0 0 20
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

kd .1 .1 .1
ks .1 .1 .1
ri 1.3
translate 0 0 3
s 3

ri 1.6
translate 0 0 -5
s 5

#ri 1.6
#translate 0 0 8
#s 1

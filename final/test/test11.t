#room with a glass slab

pl 0 0 10 1 1 1 100

translate 0 0 20
rotate 0 0 0
cam

ct
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

#glass slab
v -4 -4 -9
v 4 -4 -9
v 4 4 -9
v -4 4 -9
v -4 -4 -8
v 4 -4 -8
v 4 4 -8
v -4 4 -8

#back wall
kd .3 .6 .4
ks .1 .1 .1
#kr 1 1 1
f 3 2 1
f 3 4 2

#top wall
kd .1 .2 .7
ks .1 .1 .1
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
kd .5 .5 .5
ks .1 .1 .1
f 7 4 3
f 7 8 4

#faces of glass slab
kd 0 0 0
ks 1 1 1
sp 50
kr 0 0 0
ri 1.5

translate 0 0 0
rotate -.5 .5 0

f 9 10 11 12

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

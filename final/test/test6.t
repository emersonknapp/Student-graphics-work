pl 0 5 -16 .5 .5 .5 500

#dl 0 0 -1 1 1 1
sp 100

translate 0 0 20
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


#back wall
kd .3 .6 .4
ks .1 .1 .1
f 3 2 1
f 3 4 2

#top wall
kd .6 .3 .4
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
kd .1 .2 .7
ks .1 .1 .1
f 7 4 3
f 7 8 4

kd 0 0 0
ks 1 1 1
kr .9 .9 .9
translate -3 -7 -16
s 3

kd 0 0 0
ks 1 1 1
sp 50
kr 0 0 0
ri 2.0
translate 5 -7.5 -12
s 2.5

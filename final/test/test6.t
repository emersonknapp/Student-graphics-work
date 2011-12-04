pl 0 0 -10 .5 .5 .5
#dl 0 0 -1 .1 .15 .1
sp 100
ka .1 .1 .1

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
ks .3 .6 .4
f 3 2 1
f 3 4 2

#top wall
kd .6 .3 .4
ks .6 .3 .4
f 1 6 5
f 1 2 6


#left wall
kd .3 .1 .2
ks .3 .1 .2
f 7 1 5
f 7 3 1

#right wall
kd .2 .2 .0
ks .2 .2 .0
f 4 6 2
f 4 8 6

#bottom wall
kd .0 .2 .6
ks .0 .2 .6
f 7 4 3
f 7 8 4

kd 1 1 1
translate -5 0 -15
s 3

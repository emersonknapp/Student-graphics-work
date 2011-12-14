pl 0 9.8 -25 .4 .4 .4 500

#dl 0 0 -1 1 1 1
sp 100

translate 0 0 20
#rotate 0 3.14 0
cam

ct
#back wall

v -10 10 -40
v 10 10 -40
v -10 -10 -40
v 10 -10 -40

#forward corners
v -10 10 -10
v 10 10 -10
v -10 -10 -10
v 10 -10 -10


#back wall

kd .8 .8 .8
ks .05 .05 .05
f 3 2 1
f 3 4 2


#top wall
kd .8 .8 .8
ks .05 .05 .05
f 1 6 5
f 1 2 6

#left wall
kd .9 .3 .3
ks .05 .05 .05
f 7 1 5
f 7 3 1
#f 7 5 3 1


#right wall
kd .3 .3 .9
ks .05 .05 .05
f 4 6 2
f 4 8 6

#bottom wall
kd .8 .8 .8
ks .05 .05 .05
f 7 4 3
f 7 8 4

kd 0 0 0
ks 1 1 1
kr .9 .9 .9
#scale 1.3 1 1
translate -4.2 -6.3 -30
s 3.7

kd 0 0 0
ks 1 1 1
sp 100
kr 0 0 0
ri 1.8
scale 1 1 1
translate 5 -6.6 -19
s 3.4

ct


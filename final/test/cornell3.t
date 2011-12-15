#pl 0 9.8 -25 .4 .4 .4 2000

#dl 0 0 -1 1 1 1
sp 100

translate 0 0 20
#rotate 0 3.14 0
cam

ct

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




#left wall
kd .7 .15 .15
ks .05 .05 .05
f 7 1 5
f 7 3 1
#f 7 5 3 1


#right wall
kd .15 .15 .7
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
v -3 10 -28
v 3 10 -28
v 3 10 -22
v -3 10 -22
ri 0
tl 9 10 12 .4 .4 .4 1000
tl 10 11 12 .4 .4 .4 1000
#pl 0 0 -25 .5 .5 .5 500
#top wall
kd .8 .8 .8
ks .05 .05 .05
f 1 9 12
f 1 2 9
f 9 2 10
f 2 6 10
f 10 6 11
f 11 6 5
f 12 11 5
f 1 12 5




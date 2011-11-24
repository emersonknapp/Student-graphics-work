dl 0 0 -1 1 1 1

#translate 0 3 4
#rotate -.4 0 0 
translate 0 0 8
cam

kd .4 0 0
ks .5 0 0
kr 0 0 0
sp 100

ct
v -15 0 -15
v 15 0 -15
v -15 -1 15
v 15 -1 15

v -15 15 -15
v 15 15 -15

f 3 2 1
f 3 4 2

kd 0 .4 .4
ks 0 .4 .4
f 1 2 6
f 6 5 1

ct
translate 0 1.5 -2
ks .6 .6 .6
kd .1 .1 .1
kr .8 .8 .8
ri 1.33
s 1

ct
translate -.5 0 -1
kd 1 0 1
ks .6 .6 .6
kr .2 .2 .2
ri 0
s .5

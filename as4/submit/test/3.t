kd .3 .2 .2
ka .2 .2 .2
kr .9 .9 .9
ks 1 1 1
sp 50
tri 10 10 -8 -10 -10 -8 10 -10 -8
tri -10 10 -8 -10 -10 -8 10 10 -8
kd .8 .8 .2
translate 2 0 -7
sph 1
kd 1 .2 .3
translate -2 0 -6
sph 2
kd 0 0 1
translate 0 7 -5
sph 3
dl 0 0 -1 .2 .2 .2
pl 0 -5 -5 1 1 1
cleartrans
translate 0 -30 8
rotate 1.2 0 0 
camera
print images/img3.png
dl .57735027 -.57735207 -.57735207 1 1 1
dl .57735207 .57735207 -.57735207 0 0 1
ka .1 .1 .1
kd 1 0 1
ks 1 1 1
sp 50
translate 0 0 -20
sph 3
kd 1 1 0
translate -2 2 -15
sph 1
kd 0 1 1
translate -2 -2 -15
sph 1
cleartrans
kd .1 .1 .1
kr 1 1 1
tri 5 5 -17 1 4 -20 6 -1 -20
camera
print final.png

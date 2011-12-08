pl 0 9 -10 1 1 1 100
#pl 0 -9 -10 1 1 1 100

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

#checkerboard floor
v -5 -10 -20
v 0 -10 -20
v 5 -10 -20

v -10 -10 -17.5
v -5 -10 -17.5
v 0 -10 -17.5
v 5 -10 -17.5
v 10 -10 -17.5

v -10 -10 -15
v -5 -10 -15
v 0 -10 -15
v 5 -10 -15
v 10 -10 -15

v -10 -10 -12.5
v -5 -10 -12.5
v 0 -10 -12.5
v 5 -10 -12.5
v 10 -10 -12.5

v -5 -10 -10
v 0 -10 -10
v 5 -10 -10

#table leg
v -3 -10 -14.5
v -2.5 -10 -14.5
v -2.5 -10 -15
v -3 -10 -15

v -3 -5 -14.5
v -2.5 -5 -14.5
v -2.5 -5 -15
v -3 -5 -15

#table top
v -7 -5 -10.5
v 1.5 -5 -10.5
v 1.5 -5 -19
v -7 -5 -19

v -7 -4.75 -10.5
v 1.5 -4.75 -10.5
v 1.5 -4.75 -19
v -7 -4.75 -19

#back green wall
kd .3 .6 .4
ks .1 .1 .1
f 3 2 1
f 3 4 2

#pink ceiling
kd .6 .3 .4
ks .1 .1 .1
f 1 6 5
f 1 2 6


#left blue wall
kd .1 .2 .7
ks .1 .1 .1
f 7 1 5
f 7 3 1

#right yellow wall
kd .4 .4 .1
ks .1 .1 .1
f 4 6 2
f 4 8 6

#checkerboard floor
#grey checkers
kd .1 .1 .1
ks .1 .1 .1
f 12 9 3
f 12 13 9

f 22 18 17
f 22 23 18

f 18 14 13
f 18 19 14

f 27 24 23
f 27 28 24

f 14 11 10
f 14 15 11

f 24 20 19
f 24 25 20

f 20 16 15
f 20 21 16

f 29 26 25
f 29 8 26

#white checkers
kd 1 1 1
#ks .1 .1 .1
f 17 13 12
f 17 18 13

f 7 23 22
f 7 27 23

f 13 10 9
f 13 14 10

f 23 19 18
f 23 24 19

f 19 15 14
f 19 20 15

f 28 25 24
f 28 29 25

f 15 4 11
f 15 16 4

f 25 21 20
f 25 26 21

#table leg
kd .3 .2 .1
ks .1 .1 .1

f 30 35 34
f 30 31 35

f 31 36 35
f 31 32 36

f 32 37 36
f 32 33 37

f 33 34 37
f 33 31 34

#table top
f 38 40 41
f 38 39 40

f 38 43 42
f 38 39 43

f 39 44 43
f 39 40 44

f 40 45 44
f 40 41 45

f 41 42 45
f 41 38 42

f 42 44 45
f 42 43 44

#mirror ball
kd 0 0 0
ks 1 1 1
sp 100
kr .9 .9 .9
translate -2.75 -1.75 -14.75
s 3

#translucent ball
#kd 0 0 0
#ks 0 0 0
#kr 0 0 0
#ri 1.33
#translate -2.75 -3 -14.75
#s 2

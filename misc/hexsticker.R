# define colors
fill_color<-"#FFFFFF" #"#000000"
border_color<-"#FFD636"

# hex sticker library
library(hexSticker)

# public domain image source:
imgurl<-"../img/Abstract-Banana.png"

# 'print' sticker
sticker(imgurl, package="bnn", p_size=10, s_x=1.01, s_y=.65, s_width=.6,
        filename="../img/bnn-hexsticker.png",h_fill = fill_color, h_color = border_color,p_color="black")
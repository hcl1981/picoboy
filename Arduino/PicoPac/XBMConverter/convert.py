# Importing Image from PIL package
from PIL import Image
 
# creating a image object
im = Image.open(r"wnr_logo.jpg")
f = open("output.txt", "w")
px = im.load()

f.write("unsigned int name[64][128] = {\n")

for j in range(64):
    f.write("  {")
    for i in range(128):
        if((px[i, j][0]+px[i, j][1]+px[i, j][2])>382):
            f.write("1")
        else:
            f.write("0");
        if(i < 127):
            f.write(",")
    if(j < 63):
        f.write("},\n")
    else:
        f.write("}\n")
f.write("};\n")
f.close()
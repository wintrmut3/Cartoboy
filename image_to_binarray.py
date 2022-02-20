import shutil
import PIL
from PIL import Image
import numpy as np
import requests

output_size  = (48, 48)

def RequestImage():
    pk =  'pk.eyJ1Ijoid2ludGVybXV0MyIsImEiOiJja3pvYXB5NGMyN25qMnVwYXFxY2pvNWRsIn0.X2yHM1WgBhkGhB5jQ8-UHA'
    lat = 43.66076346584001
    # lon = -79.39647823208459
    43.660500454074636, -79.39695061729677
    zoom = 14
    bearing = -19


    url = f'https://api.mapbox.com/styles/v1/wintermut3/ckzugijcn001s15pnwaw3bdie/static/{lon},{lat},{zoom},{bearing}/{300}x{300}?access_token={pk}'
    # https://api.mapbox.com/styles/v1/mapbox/light-v10/static/-79.395,43.655,10/300x300?access_token=pk.eyJ1Ijoid2ludGVybXV0MyIsImEiOiJja3pvYXB5NGMyN25qMnVwYXFxY2pvNWRsIn0.X2yHM1WgBhkGhB5jQ8-UHA
    # https://api.mapbox.com/styles/v1/wintermut3/ckzugijcn001s15pnwaw3bdie.html?title=copy&access_token=pk.eyJ1Ijoid2ludGVybXV0MyIsImEiOiJja3pvYXB5NGMyN25qMnVwYXFxY2pvNWRsIn0.X2yHM1WgBhkGhB5jQ8-UHA&zoomwheel=true&fresh=true#11.1/47.5768/8.3201
    # print (url)

    r = requests.get(url, stream=True)
    if r.status_code == 200:
        with open("map_input.png", 'wb') as f:
            r.raw.decode_content = True
            shutil.copyfileobj(r.raw, f)  


def WriteOutImage():
    img = Image.open('map_input.png')
    img = img.convert ('RGB')
    arrOut = np.zeros(output_size);

    for x in range (img.width):
        for y in range (img.height):
            tpx = img.getpixel((x,y))
            R,G,B  = tpx

            px = (R+G+B)/(3*255.)
            

            # draw dark if 1
            binar = 0 if px > 0.1 else 1 
            # arrOut[y,x] = binar
            img.putpixel((x,y), (binar*255,binar*255,binar*255, 255))
            # print(px,arrOut)
            # print(px)
    
    img = img.resize(output_size, resample=PIL.Image.BILINEAR)
    for x in range (img.width):
        for y in range (img.height):
            tpx = img.getpixel((x,y))
            R,G,B  = tpx

            px = (R+G+B)/(3*255.)
            

            # draw dark if 1
            binar = 0 if px > 0.1 else 1 
            binar = 1 if binar == 0 else 0
            # arrOut[y,x] = binar
            img.putpixel((x,y), (binar*255,binar*255,binar*255, 255))
            arrOut[y,x] = binar
            # print(px,arrOut)
            # print(px)
    

    img.save('img_preview.png')
    print(arrOut)
    return arrOut


def generateGCodeFromImg(dat):
    gc = ''
    c = ''

    # print(dat.shape)
    for y in range(dat.shape[0]):
        # print(dat[y,:])
        gc+=generateGCodeRow(dat[y, :], dir = y%2) + "T"    
    return gc

def generateGCodeRow(row, dir):
    ret = ''
    i = 0
    sgn = 'R'

    # if revdir, do it backwards.
    if dir == 1: 
        row = row[::-1]
        sgn = 'L'
    
    
    if(row[i]==0): ret+='U'
    else: ret += 'D'

    while i < len(row):
        # if(row[i]==1):
        #     ret += 'RTLB'
        # else:
        #     ret += 'URD'
        # i+=1
        ssame = seekSame(row, i)
        ret += sgn*ssame
        # ret += f'{sgn}{str(ssame).zfill(2)}'
        i += ssame
        if(i == len(row)): return ret
        #prefix the next
        if(row[i]==0): ret+='U'
        else: ret += 'D'
    # ret += 'U' + 'L'*output_size[0];
    return ret

def seekSame(row, idx):
    z = 0
    same = row[idx]
    # print(row)
    # print(f'seeking @ {idx} = {row[idx]}')
    for i in range(idx, len(row)):
        # print(f'seeking @ {i} = {row[i]}')
        if row[i] == same:
            z+=1
        else:
            return z 
    return z

RequestImage()
binarized = WriteOutImage()
gcode = generateGCodeFromImg(binarized)
print(gcode)


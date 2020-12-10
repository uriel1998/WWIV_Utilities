#!/usr/bin/python3

import captcha 
import random # importing the random module

from captcha.audio import AudioCaptcha
from captcha.image import ImageCaptcha

audio = AudioCaptcha(voicedir='/home/steven/documents/programming/wwiv_bbs/wwiv_helpers/captcha/data/')
image = ImageCaptcha(fonts=['/home/steven/documents/programming/wwiv_bbs/wwiv_helpers/captcha/data/Inconsolata-Regular.ttf', '/home/steven/documents/programming/wwiv_bbs/wwiv_helpers/captcha/data/Inconsolata-Regular.ttf'])

tnum = random.randint(1000,99999)


data = audio.generate(str(tnum))
audio.write(str(tnum), 'out.wav')

data = image.generate(str(tnum))
image.write(str(tnum), 'out.png')

print(str(tnum))

#https://github.com/lepture/captcha
#chafa --colors=none -s 70x20  --clear --fill -all-stipple-braille-ascii-space-extra-inverted --invert --symbols -all-stipple-braille+ascii+space-extra-inverted out.png | sed 's/ /./g'

#chafa --colors=16 -s 78x24  --clear --fill -all-stipple-braille+ascii+space-extra-inverted --invert --symbols -all-stipple-braille+ascii+space-extra-inverted out.png
#chafa --colors=16 -s 78x24  --clear --symbols -all-stipple-braille+ascii+space-extra-inverted out.png
#chafa --colors=none --dither=diffusion --clear --symbols -s 80x24 -all-stipple-braille+ascii+space-extra-inverted out.png

# add in some random pipe code colors maybe?

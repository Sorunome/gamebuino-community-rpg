#!/bin/python3
import struct
PTROFFSET = 29568 + 42 + 42

tracks = [
	{
		'name':'101_starship',
		'patternset':[
			[0x8005,0x8101,0x410,0x2FC,0x210,0x2FC,0x210,0x4FC,0x20C,0x4FC,0x210,0x4FC,0x218,0x2FC,0x420,0x2FC,0x220,0x4FC,0x220,0x2FC,0x418,0x2FC,0x220,0x2FC,0x220,0x428,0x000],
			[0x8005,0x8101,0x12C,0x5FC,0x12C,0x1FC,0x12C,0x3FC,0x140,0x3FC,0x12C,0x5FC,0x12C,0x1FC,0x12C,0x3FC,0x140,0x3FC,0x0000],
			[0x8005,0x8101,0xC5C,0x250,0x258,0x85C,0x870,0x0000],
			[0x8005,0x8101,0xC40,0x23C,0x240,0x63C,0x640,0x448,0xC50,0x23C,0x240,0x648,0x650,0x458,0x0000],
			[0x8005,0x8101,0x106C,0x664,0x65C,0x458,0x0000],
			[0x8005,0x8101,0x13C,0x5FC,0x13C,0x1FC,0x13C,0x3FC,0x11C,0x3FC,0x13C,0x5FC,0x13C,0x1FC,0x13C,0x3FC,0x11C,0x3FC,0x0000],
			[0x8005,0x8101,0xC5C,0x258,0x264,0x86C,0x870,0x0000],
			[0x8005,0x8101,0x12C,0x5FC,0x12C,0x1FC,0x12C,0x3FC,0x140,0x3FC,0x13C,0x5FC,0x13C,0x1FC,0x13C,0x3FC,0x11C,0x3FC,0x0000],
			[0x8005,0x8101,0x106C,0x664,0x66C,0x458,0x0000],
			[0x8005,0x8101,0x13C,0x5FC,0x13C,0x1FC,0x13C,0x3FC,0x120,0x3FC,0x134,0x5FC,0x134,0x1FC,0x134,0x3FC,0x118,0x3FC,0x0000],
			[0x8005,0x8101,0xC5C,0x258,0x250,0x864,0x86C,0x0000],
			[0x8005,0x8101,0x12C,0x5FC,0x12C,0x1FC,0x12C,0x3FC,0x140,0x3FC,0x134,0x5FC,0x134,0x1FC,0x13C,0x3FC,0x1EC,0x3FC,0x0000],
			[0x8005,0x8101,0x870,0x46C,0x470,0x878,0x45C,0x48C,0x0000],
			[0x8005,0x8101,0x140,0x3FC,0x140,0x3FC,0x13C,0x3FC,0x13C,0x3FC,0x148,0x3FC,0x148,0x3FC,0x15C,0x3FC,0x13C,0x3FC,0x0000],
			[0x8005,0x8101,0x640,0x650,0x45C,0x648,0x658,0x464,0x0000],
			[0x8005,0x8101,0x410,0x2FC,0x210,0x8FC,0x418,0x2FC,0x23C,0x8FC,0x0000],
			[0x8005,0x8101,0x65C,0x66C,0x478,0x664,0x66C,0x458,0x0000],
			[0x8005,0x8101,0x42C,0x2FC,0x22C,0x8FC,0x434,0x2FC,0x234,0x8FC,0x0000],
			[0x8005,0x8101,0x65C,0x66C,0x478,0x66C,0x678,0x480,0x0000],
			[0x8005,0x8101,0x42C,0x2FC,0x22C,0x8FC,0x440,0x2FC,0x240,0x8FC,0x0000],
			[0x8241,0x8005,0x25C,0x264,0x440,0x848,0x240,0x250,0x458,0x85C,0x0000],
			[0x8241,0x8005,0x15C,0x5FC,0x15C,0x1FC,0x15C,0x7FC,0x140,0x5FC,0x140,0x1FC,0x140,0x7FC,0x0000],
			[0x8241,0x8005,0x28C,0x288,0x280,0x278,0x870,0x0000],
			[0x8241,0x8005,0x15C,0x1FC,0x158,0x1FC,0x150,0x1FC,0x148,0x1FC,0x840,0x0000]
		],
		'tracks':[
			[0,2,4,6,8,2,4,10,12,14,16,14,18,14,16,0xFFFF,0,0,0,0,0],
			[3,1,5,7,9,1,5,11,13,15,17,15,19,15,17,0xFFFF,0,0,0,0,0]
		]
	}
]


i = 0
sound = []
for t in tracks:
	if 'name' in t:
		print('Creating Track',t['name'],'('+str(i)+')...')
	else:
		print('Creating Track '+str(i)+'...')
	ptoffset = PTROFFSET + (len(t['patternset'])*2)
	mksound = t['tracks'][0]
	mksound += t['tracks'][1]
	for p in t['patternset']:
		mksound.append(ptoffset)
		ptoffset += len(p)*2
	for p in t['patternset']:
		mksound += p
	elements = len(mksound)
	if elements > 512:
		print('ERROR: track is too big, skipping...')
	else:
		while elements < 512:
			mksound.append(0)
			elements+=1
		sound += mksound
		print('Done')
	i += 1


sound_real = []
for s in sound:
	reg_val_msb, reg_val_lsb = struct.pack('<H',s)
	sound_real.append(reg_val_msb)
	sound_real.append(reg_val_lsb)
with open('SOUND.DAT','wb') as f:
	f.write(bytes(sound_real))

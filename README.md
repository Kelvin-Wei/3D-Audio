# 3D Audio

A head-related transfer function (HRTF) is a response that characterizes how an ear receives a sound from a point in space. Base on this, this program transfer stereo audio tracks to a source of sound pass around the listener.

### Dependencies

- Python2.7
- Numpy
- wave

### Arguments

This program has following arguments:

- `--input` / `-i`: input audio file
- `--output` / `-o`: output audio file
- `--rate` / `-r`: angle speed of rotation, the larger the value, the faster it rotates. Default value is `12`, 60 degree per second
- `--level` / `-l`: vertical angle of the source of sound. Default level is 0.
- `--clockwise` / `-c`: no argument required, if selected, the source of sound will rotate clockwise

So in the folder `test`, the `3d_output.wav` is generated with following command:

```bash
python headset.py -i doom-trap_of_clouds.wav -o 3d_output.wav -r 12 -l 0 -c
```
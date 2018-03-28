# im-spinner

try linearizing the 2d FFT by slicing through the centre with a line. rotate the line to get a rhodes-like effect maybe?

## results

- eehhhhhhhh

- 2d FFT gets too slow here, especially to use with video at a reasonable frame-rate

- instead of taking a linear slice of the frequencies from the 2d image, maybe take a slice from the original pixel image and then just do 1d-to-1d transforms on that. should be plenty fast.

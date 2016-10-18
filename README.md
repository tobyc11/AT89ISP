# AT89ISP
Arduino based AT89S52 programmer

## Compile

Use Arduino IDE to compile the arduino sketch under `AT89Interface` folder

The application relies on Qt and qhexedit2

```
cmake [This repository] -DCMAKE_PREFIX_PATH=[Path to Qt];[Path to qhexedit2]
make
```

## Wiring

![Wiring](/Sketch.png)
